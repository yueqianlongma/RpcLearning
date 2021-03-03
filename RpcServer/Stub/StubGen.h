

#ifndef __RPCSERVER_STUBGEN_H__
#define __RPCSERVER_STUBGEN_H__

#include<string>
#include<vector>
#include<RpcServer/common.h>

namespace rpc
{


class StubGen
{
public:
    StubGen(const json::Value& proto);

protected:
    typedef struct RpcReturn{
        RpcReturn(std::string name, const json::Value& params, const json::Value& returns)
            :   name_(name),
                params_(params),
                returns_(returns)
        {}

        std::string name_;
        json::Value params_;
        json::Value returns_;

    }RpcReturn;

    typedef struct RpcNotify{
        RpcNotify(std::string name, const json::Value& params)
            :   name_(name),
                params_(params)
        {}

        std::string name_;
        json::Value params_;
    }RpcNotify;

    typedef struct ServiceInfo{
        std::string name_;
        std::vector<RpcReturn> rpcReturns_;
        std::vector<RpcNotify> rpcNotifys_;
    }ServiceInfo;

protected:
    ServiceInfo serviceInfo_;

private:
    void parseRpc(const json::Value& rpc);
    void validateParams(json::Value& params);
    void validateReturns(json::Value& returns);
};


inline void replaceAll(std::string& str, const std::string& from, const std::string& to)
{
    size_t pos;
    while((pos = str.find(from)) != std::string::npos){
        str.replace(pos, from.size(), to);
    }
}


}









#endif