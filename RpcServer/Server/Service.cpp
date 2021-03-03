
#include"Service.h"
#include<RpcServer/common.h>
#include<RpcServer/Exception.h>
#include<RpcServer/RpcError.h>
using namespace rpc;




void Service::callProcedureReturn(const std::string& methodName,
                                     json::Value& request,
                                     const RpcDoneCallback& done)
{
    auto it = procedureReturn_.find(methodName);
    if (it == procedureReturn_.end()) {
        throw RequestException(RPC_METHOD_NOT_FOUND,
                               request["id"],
                               "method not found");
    }
    it->second->invoke(request, done);
};

void Service::callProcedureNotify(const std::string& methodName, json::Value& request)
{
    auto it = procedureNotfiy_.find(methodName);
    if (it == procedureNotfiy_.end()) {
        throw NotifyException(RPC_METHOD_NOT_FOUND,
                              "method not found");
    }
    it->second->invoke(request);
};