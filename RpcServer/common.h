#ifndef __RPCSERVER_COMMON_H__
#define __RPCSERVER_COMMON_H__

#include<NetServer/CountDownLatch.h>
#include<NetServer/Callbacks.h>
#include<NetServer/noncopyable.h>
#include<NetServer/EventLoop.h>
#include<NetServer/TcpClient.h>
#include<NetServer/TcpConnection.h>
#include<NetServer/TcpServer.h>
#include<NetServer/InetAddress.h>
#include<JsonServer/FastWriter.h>
#include<JsonServer/StyleWriter.h>
#include<JsonServer/Reader.h>
#include<JsonServer/ReadStream.h>
#include<JsonServer/WriteStream.h>
#include<JsonServer/Value.h>
#include<NetServer/Log.h>


namespace rpc
{
    using std::placeholders::_1;
    using std::placeholders::_2;
    using std::placeholders::_3;

    using json::Value;
    using json::StringReadStream;
    using json::StringWriteStream;
    using json::FileReadStream;
    using json::FastWriter;
    using json::Reader;
    using json::StyleWriter;
    using json::ValueType;


    using net::noncopyable;
    using net::InetAddress;
    using net::EventLoop;
    using net::TcpConnection;
    using net::TcpServer;
    using net::TcpClient;
    using net::TcpConnectionPtr;
    using net::ConnectionCallback;
    using net::MessageCallback;
    using net::CloseCallback;
    using net::HighWaterMarkCallback;
    using net::Buffer;


    typedef std::function<void(json::Value response)> RpcDoneCallback;


    class UserDoneCallback
    {
    public:
        UserDoneCallback(json::Value &request,
                        const RpcDoneCallback &callback)
                : request_(request),
                callback_(callback)
        {}


        void operator()(json::Value &&result) const
        {
            json::Value response(json::ValueType::TYPE_OBJECT);
            response.addMember("jsonrpc", "2.0");
            response.addMember("id", request_["id"]);
            response.addMember("result", result);
            callback_(response);
        }

    private:
        mutable json::Value request_;
        RpcDoneCallback callback_;
    };
    
}


#endif