
#ifndef __RPCSERVER_BASESERVER_H__
#define __RPCSERVER_BASESERVER_H__

#include<RpcServer/common.h>
#include<RpcServer/Exception.h>

namespace rpc
{

class BaseServer: public noncopyable 
{
public:
    BaseServer(EventLoop* loop, const InetAddress& serverAddr);
    
    void start();

    virtual void handleRequest(const std::string& json, const RpcDoneCallback& done) = 0;

protected:
    Value wrapException(RequestException& e);
    void sendJson(const TcpConnectionPtr& conn, Value& value);
    void onMessage(const TcpConnectionPtr& conn, Buffer& buffer);
    void handleMessage(const TcpConnectionPtr& conn, Buffer& buffer);

private:
    TcpServer server_;
};


}


#endif