


#include"BaseServer.h"
#include<RpcServer/common.h>
#include<RpcServer/Exception.h>
#include<RpcServer/RpcError.h>
#include<string>
using namespace rpc;


namespace
{
    const size_t kHighWatermark = 65536;
    const size_t kMaxMessageLen = 100 * 1024 * 1024;
}


BaseServer::BaseServer(EventLoop* loop, const InetAddress& serverAddr)
    :   server_(loop, serverAddr)
{
    server_.setMessageCallback(std::bind(&BaseServer::onMessage, this, _1, _2));
}

void BaseServer::start()
{
    server_.start();
}

/*
*   rpc 2.0 错误对象格式
*      jsonrpc： 指定JSON-RPC协议版本的字符串，必须准确写为“2.0”
*      error  :  该成员在失败是必须包含。当没有引起错误的时必须不包含该成员。
*                code   :   使用数值表示该异常的错误类型
*                message:   对该错误的简单描述字符串。该描述应尽量限定在简短的一句话
*                dta    :   包含关于错误附加信息的基本类型活结构化类型。该成员可以忽略。
*                           该成员值有服务端定义（例如详细的错误信息，嵌套的错误）
*   code 定义：
*       -32700 Parse error语法解析错误      服务端接收到无效的json。该错误发送于服务器尝试解析json文本
*       -32600 Invalid Request无效请求      发送的jons不是一个有效的请求对象
*       -32601 Method not fount找不到方法   该方法不存在或无效
*       -32602 Invalid params无效的参数     无效的方法参数
*       -32603 Internal error内部错误       JSON-RPC内部错误
*       -32000 to -32099 Server error服务端错误 预留用于自定义的服务器错误
*/

Value BaseServer::wrapException(RequestException& e)
{
    Value value(json::ValueType::TYPE_OBJECT);
    value.addMember("code", e.err().asCode());
    value.addMember("message", e.err().asString());
    value.addMember("data", e.detail());

    Value response(json::ValueType::TYPE_OBJECT);
    response.addMember("jsonrpc", "2.0");
    response.addMember("error", value);
    return response;
}

void BaseServer::sendJson(const TcpConnectionPtr& conn, Value& value)
{
    StringWriteStream os;
    FastWriter<StringWriteStream> fw(os);
    value.writeTo(fw);
    auto message = std::to_string(os.get().size() + 2).append("i32")
            .append("\r\n")
            .append(os.get())
            .append("\r\n");
    conn->send(message);
}

void BaseServer::onMessage(const TcpConnectionPtr& conn, Buffer& buffer)
{
    try{
        handleMessage(conn, buffer);
    }catch(RequestException& e){
        Value value = wrapException(e);
        sendJson(conn, value);
        conn->shutdown();

        warn("BaseServer::onMessage() %s request error: %s  detail: %s",
             conn->peer().toIpPort().c_str(), e.what(), e.detail());
    }catch (NotifyException& e){
        warn("BaseServer::onMessage() %s notify error: %s  detail: %s",
             conn->peer().toIpPort().c_str(), e.what(), e.detail());
    }
}

void BaseServer::handleMessage(const TcpConnectionPtr& conn, Buffer& buffer)
{
    while(true){
        const char* crlf = buffer.findCRLF();
        if(crlf == nullptr) break;
        
        size_t headerLen = crlf - buffer.peek() + 2;
        Value header;
        StringReadStream is(std::string(buffer.peek(), headerLen));
        auto err = Reader<StringReadStream>::parse(is, header);
        if(err != json::PARSE_OK || !header.isInt32() || header.getInt32() <= 0){
            buffer.retrieve(headerLen);
            throw RequestException(RPC_INVALID_REQUEST, "invalid message length");
        }

        auto bodyLen = static_cast<int32_t>(header.getInt32());
        if(bodyLen >= kMaxMessageLen){
            throw RequestException(RPC_INVALID_REQUEST, "message is too long");
        }

        if(buffer.readableBytes() < headerLen + bodyLen)
            break;
        buffer.retrieve(headerLen);
        auto json = buffer.retrieveAsString(bodyLen);
        // auto json = buffer.retrieveAllAsString();        //这个地方是一个bug, 如果两条数据同时到达的话，就会出现问题
        handleRequest(json, [conn, this](Value response){
            if (!response.isNull()) {
                sendJson(conn, response);
                trace("BaseServer::handleMessage() %s request success",
                      conn->peer().toIpPort().c_str());
            }
            else {
                trace("BaseServer::handleMessage() %s notify success",
                      conn->peer().toIpPort().c_str());
            }
        });
    }
}