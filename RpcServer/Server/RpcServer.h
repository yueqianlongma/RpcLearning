

#ifndef __RPCSERVER_PRCSERVER_H__
#define __RPCSERVER_PRCSERVER_H__



#include<RpcServer/common.h>
#include<RpcServer/Server/BaseServer.h>
#include<memory>
#include<RpcServer/Server/Service.h>
#include<map>


namespace rpc
{


class RpcServer: public BaseServer
{
public:
    RpcServer(EventLoop* loop, const InetAddress& serverAddr)
        :   BaseServer(loop, serverAddr)
    {}
    ~RpcServer() = default;
    
    // used by user stub
    void addService(const std::string& serviceName, Service* service);

    void handleRequest(const std::string& json, const RpcDoneCallback& done) override;

private:
    void handleRequestBatch(Value& requests, const RpcDoneCallback& done);
    void handleRequestSingle(Value& requst, const RpcDoneCallback& done);
    void handleNotifySingle(Value& notify);

    void validateRequest(Value& request);
    void validateNotify(Value& notify);

private:
    typedef std::unique_ptr<Service> RpcServeicPtr;
    typedef std::map<std::string, RpcServeicPtr> ServiceList;

    ServiceList services_;
};


}













#endif