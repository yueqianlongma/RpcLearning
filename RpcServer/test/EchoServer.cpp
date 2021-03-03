

#include<RpcServer/Server/RpcServer.h>
#include<RpcServer/common.h>
#include<string>
#include<RpcServer/Server/Service.h>
#include<RpcServer/Server/Procedure.h>
using std::placeholders::_1;
using std::placeholders::_2;

class EchoServiceStub : public net::noncopyable
{
public:
    explicit EchoServiceStub(rpc::RpcServer& rpcServer)
    {
        rpc::Service* serviceInfo = new rpc::Service();
        std::vector<json::ValueType> paramType;
        paramType.push_back(json::ValueType::TYPE_STRING);

        std::vector<std::string> paramName;
        paramName.emplace_back("message");
        
        serviceInfo->addProcedureReturn("Echo", 
                new rpc::ProcedureReturn(std::bind(&EchoServiceStub::EchoStub, this, _1, _2),
                                                        paramName, paramType));
        serviceInfo->addProcedureNotify("Notify", 
                new rpc::ProcedureNotify(std::bind(&EchoServiceStub::EchoNotify, this, _1), 
                                                        paramName, paramType));

        rpcServer.addService("EchoServer", serviceInfo);
    }

    void EchoStub(json::Value& request, const rpc::RpcDoneCallback& done)
    {
        auto& params = request["params"];
        auto message = params["message"].getString();
        Echo(message, rpc::UserDoneCallback(request, done));
    }

    void EchoNotify(json::Value& request)
    {
        auto& params = request["params"];
        auto message = params["message"].getString();
        Notify(message);
    }

    virtual void Echo(std::string message, const rpc::UserDoneCallback& done)  = 0;

    virtual void Notify(std::string message) = 0;
    
};

class EchoService: EchoServiceStub
{
public: 
    explicit EchoService(rpc::RpcServer& rpcServer)
        :   EchoServiceStub(rpcServer)
    {}
    
    void Echo(std::string message, const rpc::UserDoneCallback& done) override
    {
        done(json::Value(message));
    }

    void Notify(std::string message) override
    {
        info("Notify: %s", message.c_str());
    }
};

int main()
{
    net::EventLoop loop;
    net::InetAddress serverAddr("127.0.0.1", 6666);
    rpc::RpcServer rpcServer(&loop, serverAddr);

    EchoService server(rpcServer);
    rpcServer.start();
    loop.loop();
}