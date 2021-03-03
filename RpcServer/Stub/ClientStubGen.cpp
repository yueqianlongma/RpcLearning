

#include"ClientStubGen.h"
#include<RpcServer/common.h>
using namespace rpc;


namespace
{



std::string clientTemplate(const std::string& macroName,
                    const std::string& clientName,
                    const std::string& procedureReturns,
                    const std::string& procedureNotifys)
{

    std::string str = R"(
        #ifndef __RPCSERVER_[macroName]_H__
        #define __RPCSERVER_[macroName]_H__

        #include<RpcServer/common.h>
        #include<RpcServer/Client/BaseClient.h>

        class [clientName]
        {
        public:
            [clientName](net::EventLoop* loop, const net::InetAddress& serverAddr)
                :   client_(loop, serverAddr),
                    latch_(1)
            {
                client_.setConnectionCallback([&](const net::TcpConnectionPtr conn){
                    if(conn->connected()){
                        info(\"tcp connected\");
                        connetcion_ = conn;
                        latch_.countDown();
                    }else{
                        info(\"tcp disconnected\");
                    }
                });
            }

            ~[clientName]() = default;

            void start()
            {
                client_.start();
            }

            void wait()
            {
                latch_.wait();
            }

            [procedureReturns]

            [procedureNotifys]

        private:
            net::CountDownLatch latch_;
            net::TcpConnectionPtr connetcion_;
            rpc::BaseClient client_;
        };

        #endif

        )";

    replaceAll(str, "[macroName]",  macroName);
    replaceAll(str, "[clientName]", clientName);
    replaceAll(str, "[procedureReturns]", procedureReturns);
    replaceAll(str, "[procedureNotifys]", procedureNotifys);
    return str;
}

std::string procedureReturnTemplate(const std::string& serviceName,
                            const std::string& methodName,
                            const std::string& methodArg,
                            const std::string& paramsMember)
{

    std::string str = R"(
        void [methodName]([methodArg] const rpc::ResponseCallback& cb)
        {
            rpc::Value params(json::ValueType::TYPE_OBJECT);
            [paramsMember]

            rpc::Value json(json::ValueType::TYPE_OBJECT);
            json.addMember("jsonrpc", "2.0");
            json.addMember("method", "[serviceName].[methodName]");
            json.addMember("params", params);

            assert(connetcion_ != nullptr);
            client_.sendCall(connetcion_, json, cb);
        }
    )";

    replaceAll(str, "[serviceName]",  serviceName);
    replaceAll(str, "[methodName]",   methodName);
    replaceAll(str, "[methodArg]",    methodArg);
    replaceAll(str, "[paramsMember]", paramsMember);
    return str;
}

std::string procedureNotifyTemplate(const std::string& serviceName,
                            const std::string& methodName,
                            const std::string& methodArg,
                            const std::string& paramsMember)
{

    std::string str = R"(
        void [methodName]([methodArg])
        {
            rpc::Value params(json::ValueType::TYPE_OBJECT);
            [paramsMember]

            rpc::Value json(json::ValueType::TYPE_OBJECT);
            json.addMember("jsonrpc", "2.0");
            json.addMember("method", "[serviceName].[methodName]");
            json.addMember("params", params);

            assert(connetcion_ != nullptr);
            client_.sendNotify(connetcion_, json);
        }
    )";

    replaceAll(str, "[serviceName]",  serviceName);
    replaceAll(str, "[methodName]",   methodName);
    replaceAll(str, "[methodArg]",    methodArg);
    replaceAll(str, "[paramsMember]", paramsMember);
    return str;
}

std::string methodArgTemplate(const json::ValueType type,
                        const std::string argName)
{

    std::string str = R"( [argType] [argName]    )";
    auto argType = [=](){
        switch (type) {
            case json::ValueType::TYPE_INT32:
                return "int32_t";
            case json::ValueType::TYPE_INT64:
                return "int64_t";
            case json::ValueType::TYPE_DOUBLE:
                return "double";
            case json::ValueType::TYPE_BOOL:
                return "bool";
            case json::ValueType::TYPE_STRING:
                return "std::string";
            case json::ValueType::TYPE_OBJECT:
            case json::ValueType::TYPE_ARRAY:
                return "json::Value";
            default:
                assert(false && "bad arg type");
                return "bad type";
        }
    }();
    replaceAll(str, "[argType]",  argType);
    replaceAll(str, "[argName]",  argName);   
    return str; 
}

std::string methodMemberTemplate(const std::string& argName)
{
    std::string str = R"(
        params.addMember("[argName]", [argName]);
    )";

    replaceAll(str, "[argName]",  argName);
    return str;
}

}




std::string ClientStubGen::genClient()
{
    auto macroName = genMacroName();
    auto clientName = genClientName();
    auto procedureReturns = genProcedureReturns();
    auto procedureNotifys = genProcedureNotifys();
    return clientTemplate(macroName,
                clientName,
                procedureReturns,
                procedureNotifys);
}

std::string ClientStubGen::genMacroName()
{
    return serviceInfo_.name_ + "CLIENT";
}

std::string ClientStubGen::genClientName()
{
    return serviceInfo_.name_ + "Client";
}

std::string ClientStubGen::genProcedureReturns()
{
    std::string returnFuncs;
    for(auto& func : serviceInfo_.rpcReturns_){
        auto methodName = func.name_;
        auto methodArg = genArg(func.params_, true);
        auto paramsMember = genMember(func.params_);
        auto notify = procedureReturnTemplate(serviceInfo_.name_,
                            methodName,
                            methodArg,
                            paramsMember);
        returnFuncs.append(notify);
        returnFuncs.append("\n");
    }
    return returnFuncs;
}

std::string ClientStubGen::genProcedureNotifys()
{
    std::string notifyFuncs;
    for(auto& func : serviceInfo_.rpcNotifys_){
        auto methodName = func.name_;
        auto methodArg = genArg(func.params_, false);
        auto paramsMember = genMember(func.params_);
        auto notify = procedureNotifyTemplate(serviceInfo_.name_,
                            methodName,
                            methodArg,
                            paramsMember);
        notifyFuncs.append(notify);
        notifyFuncs.append("\n");
    }
    return notifyFuncs;
}


//函数的参数 Type name
std::string ClientStubGen::genArg(const json::Value& params, bool hasLast)
{
    std::string args;
    bool bk = false;
    for(auto& p : params.getObject()){
        std::string arg = methodArgTemplate(p.second.getType(), p.first.getString());
        if(!bk){
            bk = true;
        }else{
            args.append(", ");
        }
        args.append(arg);
    }
    if(hasLast)
        args.append(", ");
    return args;
}

//添加params
std::string ClientStubGen::genMember(const json::Value& params)
{
    std::string members;
    for(auto& p : params.getObject()){
        std::string member = methodMemberTemplate(p.first.getString());
        members.append(member);
        members.append("\n");
    }
    return members;
}



