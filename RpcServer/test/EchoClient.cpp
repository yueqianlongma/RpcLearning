#include<RpcServer/Client/BaseClient.h>
#include<RpcServer/common.h>
#include<string>
#include<unistd.h>

class EchoClient
{
public:
    EchoClient(net::EventLoop* loop, const net::InetAddress& serverAddr)
        :   client_(loop, serverAddr),
            latch_(1)
    {
        client_.setConnectionCallback([&](const net::TcpConnectionPtr conn){
            if(conn->connected()){
                info("tcp connected");
                connetcion_ = conn;
                latch_.countDown();
            }else{
                info("tcp disconnected");
            }
        });
    }

    ~EchoClient() = default;

    void start()
    {
        client_.start();
    }

    void wait()
    {
        latch_.wait();
    }

    void Echo(std::string message, const rpc::ResponseCallback& cb)
    {
        rpc::Value params(json::ValueType::TYPE_OBJECT);
        params.addMember("message", message);

        rpc::Value json(json::ValueType::TYPE_OBJECT);
        json.addMember("jsonrpc", "2.0");
        json.addMember("method", "EchoServer.Echo");
        json.addMember("params", params);

        client_.sendCall(connetcion_, json, cb);
    }

    void Notify(std::string message)
    {
        rpc::Value params(json::ValueType::TYPE_OBJECT);
        params.addMember("message", message);

        rpc::Value json(json::ValueType::TYPE_OBJECT);
        json.addMember("jsonrpc", "2.0");
        json.addMember("method", "EchoServer.Notify");
        json.addMember("params", params);

        client_.sendNotify(connetcion_, json);
    }


private:
    net::CountDownLatch latch_;
    net::TcpConnectionPtr connetcion_;
    rpc::BaseClient client_;
};


int main(int argc, char** argv)
{
    net::EventLoop* loop;
    net::CountDownLatch latch_(1);
    std::thread threadLoop([&](){
        net::EventLoop loop_;
        loop = &loop_;
        latch_.countDown();
        loop_.loop();
    });
    latch_.wait();


    net::InetAddress serverAddr("127.0.0.1", 6666);
    EchoClient client(loop, serverAddr);
    client.start();
    client.wait();


    std::string message;
    while(std::getline(std::cin, message))
    {
        client.Echo(message, [](json::Value value, bool isError){
            if(!isError){
                std::cout<<"response: "<<value.getString()<<std::endl;
            }else{
                std::cout<<"response: "<<value["message"].getString()<<" : "
                         <<value["data"].getString()<<std::endl;
            }
        });
        // sleep(2);
        client.Notify(message);
    }

    return 0;
}