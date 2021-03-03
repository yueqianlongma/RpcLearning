

#ifndef __RPCSERVER_PROCEDURE_H__
#define __RPCSERVER_PROCEDURE_H__

#include<RpcServer/common.h>
#include<vector>
#include<RpcServer/Exception.h>
#include<RpcServer/RpcError.h>

namespace rpc
{

template <typename CallBack>
class Procedure
{
public:
    Procedure(const CallBack& callBack,  std::vector<std::string>& paramNames,
                     std::vector<json::ValueType>& paramTypes)
        :   callBack_(callBack)
    {
        assert(paramNames.size() == paramTypes.size());
        for(int i = 0; i < paramNames.size(); ++i){
            params_.emplace_back(paramNames[i], paramTypes[i]);
        }
    }

    // procedure call
    void invoke(json::Value& request, const RpcDoneCallback& done);
    // procedure notify
    void invoke(json::Value& request);

private:
    typedef struct Param
    {
        Param(std::string& paramName, json::ValueType paramType)
            :   paramName_(paramName),
                paramType_(paramType)
        {}

        std::string paramName_;
        json::ValueType paramType_;
    }Param;


    void validateRequest(json::Value& request) const;
    bool validateGeneric(json::Value& request) const;

private:
    CallBack callBack_;
    std::vector<Param> params_;                                                                                                           
};


typedef std::function<void(json::Value&, const RpcDoneCallback&)> ProcedureReturnCallback;
typedef Procedure<ProcedureReturnCallback> ProcedureReturn;

typedef std::function<void(json::Value&)> ProcedureNotifyCallback;
typedef Procedure<ProcedureNotifyCallback> ProcedureNotify;

template class Procedure<ProcedureReturnCallback>;
template class Procedure<ProcedureNotifyCallback>;

template <>
void Procedure<ProcedureReturnCallback>::validateRequest(json::Value& request) const
{
    switch (request.getType()) {
        case json::ValueType::TYPE_OBJECT:
        case json::ValueType::TYPE_ARRAY:
            if (!validateGeneric(request))
                throw RequestException(RPC_INVALID_PARAMS,
                                       request["id"],
                                       "params name or type mismatch");
            break;
        default:
            throw RequestException(
                    RPC_INVALID_REQUEST,
                    request["id"],
                    "params type must be object or array");
    }
}


template <>
void Procedure<ProcedureNotifyCallback>::validateRequest(json::Value& request) const
{
    switch (request.getType()) {
        case json::ValueType::TYPE_OBJECT:
        case json::ValueType::TYPE_ARRAY:
            if (!validateGeneric(request))
                throw NotifyException(RPC_INVALID_PARAMS,
                                      "params name or type mismatch");
            break;
        default:
            throw NotifyException(RPC_INVALID_REQUEST,
                                  "params type must be object or array");
    }
}


template <typename Func>
bool Procedure<Func>::validateGeneric(json::Value& request) const
{
    auto params_exits = request.hasMember("params");
    if(params_exits == false)
    {
        return params_.empty();
    }

    auto& params = request["params"];
    if(params.getSize() != params_.size())
        return false;
    
    switch(params.getType()){
        case json::ValueType::TYPE_ARRAY:
            for(size_t i = 0; i < params_.size(); ++i)
                if(params[i].getType() != params_[i].paramType_)
                    return false;
            break;
        case json::ValueType::TYPE_OBJECT:
            for(auto& p : params_){
                if(!params.hasMember(p.paramName_))
                    return false;
                if(params[p.paramName_].getType() != p.paramType_)
                    return false;
            }
            break;
        default:
            return false;
    }
    return true;
}



template <>
void Procedure<ProcedureReturnCallback>::invoke(json::Value& request,
                                                const RpcDoneCallback& done)
{
    validateRequest(request);
    callBack_(request, done);
}


template <>
void Procedure<ProcedureNotifyCallback>::invoke(json::Value& request)
{
    validateRequest(request);
    callBack_(request);
}

}






#endif

