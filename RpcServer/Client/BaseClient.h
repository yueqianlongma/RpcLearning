
#ifndef __RPCSERVER_BASECLIENT_H__
#define __RPCSERVER_BASECLIENT_H__

#include<RpcServer/common.h>
#include<memory>
#include<map>

namespace rpc
{

using ResponseCallback = std::function<void(Value&,
                                    bool isError)>;

class BaseClient: public noncopyable
{
public:
    BaseClient(EventLoop* loop, const InetAddress& serverAddress);

    void start();

    void setConnectionCallback(const ConnectionCallback& cb);

    TcpConnectionPtr getTcpConnionPtr();
    
    void sendCall(const TcpConnectionPtr& conn, Value& call,
                                                     const ResponseCallback& cb);

    void sendNotify(const TcpConnectionPtr& conn, Value& notify);

private:
    void sendJson(const TcpConnectionPtr& conn, Value& value);
    void onMessage(const TcpConnectionPtr& conn, Buffer& buffer);
    void handleMessage(Buffer& buffer);
    void handleResponse(std::string& json);
    void handleResonseSingle(Value& response);
    void validateResponse(Value& response);

private:
    using IdToResponse = std::map<int64_t, ResponseCallback>;
    const size_t kMaxMessageLen = 65536;

private:
    int32_t id_;
    IdToResponse idToResponse_;
    std::unique_ptr<TcpClient> baseClient_;
};

}




#endif
