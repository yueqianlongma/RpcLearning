

#include<TcpClient.h>
#include<EventLoop.h>
#include<Callbacks.h>
#include<InetAddress.h>
#include<Log.h>
#include<TcpConnection.h>
#include<iostream>
using namespace net;
using namespace std;

class EchoClient : noncopyable
{
public:
    EchoClient(EventLoop *baseLoop, InetAddress& addr)
        :   baseLoop_(baseLoop),
            client_(baseLoop, addr)
    {
        client_.setConnectionCallback(std::bind(&EchoClient::connectionCallBack, this, _1));
        client_.setWriteCompleteCallback(std::bind(&EchoClient::writeCompleteCallBack, this, _1));
        client_.setMessageCallback(std::bind(&EchoClient::messageCallBack, this, _1, _2));
    }

    void start()
    {
        client_.start();
    }

    void connectionCallBack(const TcpConnectionPtr& conn)
    {
        info("连接建立 %s", conn->name().c_str());
        if (conn->connected()) {
            auto th = std::thread([conn](){
                string str;
                while(getline(cin, str)){
                    conn->send(str);
                }
                trace("break while");
                conn->shutdown();
            });
            th.detach();
        }
        else {
            baseLoop_->quit();
        }
    }

    void writeCompleteCallBack(const TcpConnectionPtr& conn)
    {
        info("发送完毕");
    }

    void messageCallBack(const TcpConnectionPtr& conn, Buffer& buff)
    {
        cout<<conn->name()<<" 收到数据：-------》 "<<buff.retrieveAllAsString()<<endl;
    }

private: 
    EventLoop *baseLoop_;
    TcpClient client_;
};



int main(int argc, char** argv)
{
    EventLoop loop;
    InetAddress addr("127.0.0.1", 7777);
    EchoClient client(&loop, addr);
    client.start();
    loop.loop();
    return 0;
}