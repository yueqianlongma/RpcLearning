#include"BaseClient.h"
#include"RpcServer/common.h"
#include"RpcServer/Exception.h"
using namespace rpc;


namespace
{
    
    json::Value& findValue(json::Value& cur, const char* key, json::ValueType type)
    {
        if(!cur.hasMember(key))
            throw rpc::ResponseException("key not exist");
        auto& val = cur[key];
        if(val.getType() != type)
            throw rpc::ResponseException("bad type");
        return val;
    }

    json::Value& findValue(json::Value& cur, const char* key, json::ValueType type, int32_t id)
    {
        try{
            return findValue(cur, key, type);
        }catch(ResponseException& e){
            throw ResponseException(e.what(), id);
        }
    }
}


BaseClient::BaseClient(EventLoop* loop, const InetAddress& serverAddress)
    :   id_(0),
        baseClient_(new TcpClient(loop, serverAddress))
{
    baseClient_->setMessageCallback(std::bind(&BaseClient::onMessage, this, _1, _2));
}

void BaseClient::start()
{
    baseClient_->start();
}

void BaseClient::setConnectionCallback(const ConnectionCallback& cb)
{
    baseClient_->setConnectionCallback(cb);
}

TcpConnectionPtr BaseClient::getTcpConnionPtr()
{
    return baseClient_->getTcpConnection();
}

void BaseClient::sendCall(const TcpConnectionPtr& conn, 
                                    Value& call, 
                                        const ResponseCallback& cb)
{
    call.addMember("id", id_);
    idToResponse_[id_++] = cb;
    sendJson(conn, call);
}

void BaseClient::sendNotify(const TcpConnectionPtr& conn, Value& notify)
{
    sendJson(conn, notify);
}

void BaseClient::sendJson(const TcpConnectionPtr& conn, Value& value)
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

void BaseClient::onMessage(const TcpConnectionPtr& conn, Buffer& buffer)
{
    try{
        handleMessage(buffer);
    }catch(ResponseException& e){
        error("response error: %s", e.what());
        if (e.hasId()) {
            // fixme: should we?
            idToResponse_.erase(e.getId());
        }
    }
}

void BaseClient::handleMessage(Buffer& buffer)
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
            throw ResponseException("invalid message length");
        }

        auto bodyLen = static_cast<int32_t>(header.getInt32());
        if(bodyLen >= kMaxMessageLen){
            throw ResponseException("message is too long");
        }

        if(buffer.readableBytes() < headerLen + bodyLen)
            break;
        buffer.retrieve(headerLen);
        auto json = buffer.retrieveAllAsString();
        handleResponse(json);
    }
}

void BaseClient::handleResponse(std::string& json)
{
    Value responses;
    StringReadStream is(json);
    auto err = Reader<StringReadStream>::parse(is, responses);
    if(err != json::PARSE_OK)
        throw ResponseException("parse error");
    
    auto type = responses.getType();
    if(type == ValueType::TYPE_OBJECT){
        handleResonseSingle(responses);
    }else if(type == ValueType::TYPE_ARRAY){
        size_t n = responses.getSize();
        if(n == 0)
            throw ResponseException("batch reponse is empty");
        for(size_t i = 0; i < n; ++i)
            handleResonseSingle(responses[i]);
    }else{
        throw ResponseException("reponse should be json object or array");
    }
}

void BaseClient::handleResonseSingle(Value& response)
{
    validateResponse(response);
    auto id = response["id"].getInt32();

    auto it = idToResponse_.find(id);
    if(it == idToResponse_.end()){
        warn("response %d not found in stub", id);
        return;
    }

    if(response.hasMember("result")){
        it->second(response["result"], false);
    }
    else{
        auto err = response["error"];
        it->second(err, true);
    }
    idToResponse_.erase(it);
}

/**
 *  rpc 2.0 响应对象格式
 *      jsonrpc： 指定JSON-RPC协议版本的字符串，必须准确写为“2.0”
 *      result :  该成员在成功时必须包含。当调用方法引起错误时必须不包含该成员。
 *      error  :  该成员在失败是必须包含。当没有引起错误的时必须不包含该成员。
 *      id     :  该成员必须包含。该成员值必须于请求对象中的id成员值一致。
 * */

void BaseClient::validateResponse(Value& response)
{
    if(response.getType() != ValueType::TYPE_OBJECT)
        throw ResponseException("reponse should be json object or array");
    
    if(response.getSize() != 3)
        throw ResponseException("response should have exactly 3 field"
                                        "(jsonrpc, error/result, id)");

    auto id = findValue(response, "id", json::ValueType::TYPE_INT32).getInt32();
    auto version = findValue(response, "jsonrpc", json::ValueType::TYPE_STRING, id).getString();

    if(version != "2.0")
        throw ResponseException("unknown json rpc version", id);
    
    auto resultIn = response.hasMember("result");
    auto errorIn  = response.hasMember("error");
    if(resultIn && errorIn)
        throw ResponseException("result ans error can not include both");
    if(!resultIn && !errorIn)
        throw ResponseException("error and result must contain one");
    if(resultIn)
        return;
    findValue(response, "error", json::ValueType::TYPE_OBJECT, id);
}