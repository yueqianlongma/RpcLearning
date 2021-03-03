


#include<string>
#include<RpcServer/RpcError.h>
#include<RpcServer/Exception.h>
#include<RpcServer/common.h>
#include<RpcServer/Server/RpcServer.h>
#include<mutex>
using namespace rpc;


namespace 
{

bool isNotify(json::Value& value)
{
    return !value.hasMember("id");
}

bool hasParams(json::Value& value)
{
    return value.hasMember("params");
}

json::Value& findValue(json::Value& cur, const char* key, json::ValueType type)
{
    if(!cur.hasMember(key)) 
        throw rpc::RequestException(rpc::RPC_INVALID_REQUEST, 
                                        std::string(key)
                                        .append(" key not exist").c_str());
    auto& val = cur[key];
    if(val.getType() != type)
        throw rpc::RequestException(rpc::RPC_INVALID_REQUEST, 
                                        std::string(key)
                                        .append(" bad type").c_str());
    return val;
}

// 一个很好的结构设计
// a thread safe batch response container
// use shared_ptr to manage json value
class ThreadSafeBatchResponse
{
public:
    explicit
    ThreadSafeBatchResponse(const RpcDoneCallback& done):
            data_(std::make_shared<ThreadSafeData>(done))
    {}

    void addResponse(json::Value response)
    {
        std::unique_lock<std::mutex> lock(data_->mutex);
        data_->responses.addValue(response);
    }

private:
    struct ThreadSafeData
    {
        explicit
        ThreadSafeData(const RpcDoneCallback& done_):
                responses(json::ValueType::TYPE_ARRAY),
                done(done_)
        {}

        ~ThreadSafeData()
        {
            // last reference to data is destructing, so notify RPC server we are done
            done(responses);
        }

        std::mutex mutex;
        json::Value responses;
        RpcDoneCallback done;
    };

    typedef std::shared_ptr<ThreadSafeData> DataPtr;
    DataPtr data_;
};

}

void RpcServer::addService(const std::string& serviceName, Service *service)
{
    assert(services_.find(serviceName) == services_.end());
    services_.emplace(serviceName, service);
}


void RpcServer::handleRequest(const std::string& json, const RpcDoneCallback& done)
{
    Value value;
    StringReadStream is(json);
    auto err = Reader<StringReadStream>::parse(is, value);
    if(err != json::PARSE_OK)
        throw RequestException(RPC_PARSE_ERROR, json::parseErrorStr(err));

    switch(value.getType()){
        case json::ValueType::TYPE_OBJECT:
            if(isNotify(value))
                handleNotifySingle(value);
            else    
                handleRequestSingle(value, done);
            break;
        case json::ValueType::TYPE_ARRAY:
            handleRequestBatch(value, done);
            break;
        default:
            throw RequestException(RPC_INVALID_REQUEST, "request should be json object or array");
    }
}   


void RpcServer::handleRequestBatch(Value& requests, const RpcDoneCallback& done)
{
    size_t num = requests.getSize();
    if (num == 0)
        throw RequestException(RPC_INVALID_REQUEST, "batch request is empty");

    ThreadSafeBatchResponse responses(done);
    try {
        size_t n = requests.getSize();
        for (size_t i = 0; i < n; i++) {

            auto& request = requests[i];

            if (!request.isObject()) {
                throw RequestException(RPC_INVALID_REQUEST, "request should be json object");
            }

            if (isNotify(request)) {
                handleNotifySingle(request);
            }
            else {
                handleRequestSingle(request, [=](json::Value response) mutable {
                    responses.addResponse(response);
                });
            }
        }
    }
    catch (RequestException &e) {
        auto response = wrapException(e);
        responses.addResponse(response);
    }
    catch (NotifyException &e) {
        // todo: print something here
    }

}

void RpcServer::handleRequestSingle(Value& request, const RpcDoneCallback& done)
{
    validateRequest(request);

    auto& id = request["id"];
    auto serviceMethod = request["method"].getString();
    auto pos = serviceMethod.find('.');
    if(pos == 0 || pos == std::string::npos)
        throw RequestException(RPC_INVALID_REQUEST, id, "missing service name in method");

    auto serviceName = serviceMethod.substr(0, pos);
    auto it = services_.find(serviceName);
    if (it == services_.end())
        throw RequestException(RPC_METHOD_NOT_FOUND, id, "service not found");     

    if(pos == serviceMethod.length() - 1)
        throw RequestException(RPC_INVALID_REQUEST, id, "missing method name in method");
    auto methodName = serviceMethod.substr(pos + 1);
    if(methodName.length() == 0)
        throw RequestException(RPC_INVALID_REQUEST, id, "missing method name in method");
    
    auto& service = it->second;
    service->callProcedureReturn(methodName, request, done);       
}

void RpcServer::handleNotifySingle(Value& notify)
{
    validateNotify(notify);

    auto serviceMethod = notify["method"].getString();
    auto pos = serviceMethod.find('.');
    if(pos == 0 || pos == std::string::npos)
        throw NotifyException(RPC_INVALID_REQUEST, "missing service name in method");

    auto serviceName = serviceMethod.substr(0, pos);
    auto it = services_.find(serviceName);
    if (it == services_.end())
        throw RequestException(RPC_METHOD_NOT_FOUND, "service not found");   
    
    if(pos == serviceMethod.length() - 1)
        throw NotifyException(RPC_INVALID_REQUEST, "missing method name in method");    
    auto methodName = serviceMethod.substr(pos + 1);
    if(methodName.length() == 0)
        throw NotifyException(RPC_INVALID_REQUEST, "missing method name in method");

    auto& service = it->second;
    service->callProcedureNotify(methodName, notify);    
}


/**
 *  rpc 2.0 请求对象格式
 *      jsonrpc： 指定JSON-RPC协议版本的字符串，必须准确写为“2.0”
 *      method ： 包含所要调用方法名称的字符串，以rpc开头的方法名，
 *                          用英文句号（U+002E or ASCII 46）
 *                          连接的为预留给rpc内部的方法名及扩展名，且不能在其他地方使用   
 *      params :  调用方法所需要的结构化参数值，该成员参数可以被省略。
 *                  这里params不能省略
 *      id     :  已建立客户端的唯一标识id，值必须包含一个字符串、数值或NULL空值。
 *                  如果不包含该成员则被认定为是一个通知。      
 *                这里我们将id类型置为int32    
 */

void RpcServer::validateRequest(Value& request)
{
    auto& id = findValue(request, "id", json::ValueType::TYPE_INT32);
    auto version = findValue(request, "jsonrpc", json::ValueType::TYPE_STRING).getString();
    if(version != "2.0")
        throw RequestException(RPC_INVALID_REQUEST, id, "jsonrpc version is unknown/unsupported");   
    
    auto method = findValue(request, "method", json::ValueType::TYPE_STRING).getString();
    if(method == "rpc.")
        throw RequestException(RPC_METHOD_NOT_FOUND, id, "method name is internal use");
    
    // jsonrpc, method, id, params
    size_t nMembers = 3u + hasParams(request);
    if (request.getSize() != nMembers)
        throw RequestException(RPC_INVALID_REQUEST, id, "unexpected field");
}

void RpcServer::validateNotify(Value& notify)
{
    auto version = findValue(notify, "jsonrpc", json::ValueType::TYPE_STRING).getString();
    if(version != "2.0")
        throw NotifyException(RPC_INVALID_REQUEST, "unknown json rpc version");
    
    auto method = findValue(notify, "method", json::ValueType::TYPE_STRING).getString();
    if(method == "rpc.")
        throw NotifyException(RPC_METHOD_NOT_FOUND, "method name is internal use");
    
    //version  method  param
    size_t nMember = 2u + hasParams(notify);
    if(notify.getSize() != nMember)
        throw NotifyException(RPC_INVALID_REQUEST, "unexpected field");
}