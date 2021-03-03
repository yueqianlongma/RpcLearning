

#include"ServerStubGen.h"
using namespace rpc;

namespace
{

std::string serverTemplate(const std::string& macroStubName,
                    const std::string& serverStubName,
                    const std::string& serverName,
                    const std::string& procedureBind,
                    const std::string& procedureReturnsStub,
                    const std::string& procedureNotifysStub,
                    const std::string& procedureReturns,
                    const std::string& procedureNotifys)
{
    //Echo
    //EchoServerStub
    //EchoServer
    std::string str = R"(
        #ifndef __RPCSERVER_[macroStubName]_H__
        #define __RPCSERVER_[macroStubName]_H__

        #include<RpcServer/Server/RpcServer.h>
        #include<RpcServer/common.h>
        #include<string>
        #include<RpcServer/Server/Service.h>
        #include<RpcServer/Server/Procedure.h>
        using std::placeholders::_1;
        using std::placeholders::_2;

        class [serverStubName] : public net::noncopyable
        {
        public:
            explicit [serverStubName](rpc::RpcServer& rpcServer)
            {
                rpc::Service* serviceInfo = new rpc::Service();

                [procedureBind]

                rpcServer.addService("[serverName]", serviceInfo);
            }

           [procedureReturnsStub]

           [procedureNotifysStub]

           [procedureReturns]

           [procedureNotifys]
            
        };
    )";

    replaceAll(str, "[macroStubName]",          macroStubName);
    replaceAll(str, "[serverStubName]",         serverStubName);
    replaceAll(str, "[serverName]",             serverName);
    replaceAll(str, "[procedureBind]",          procedureBind);
    replaceAll(str, "[procedureReturnsStub]",   procedureReturnsStub);
    replaceAll(str, "[procedureNotifysStub]",   procedureNotifysStub);
    replaceAll(str, "[procedureReturns]",       procedureReturns);
    replaceAll(str, "[procedureNotifys]",       procedureNotifys);
    return str;
}

std::string procedureBindNameTypeTemplateReturn(
                                const json::ValueType type,
                                const std::string& argName)
{
    std::string str = R"( 
        paramType.push_back([paramType]);
        paramName.push_back([paramName];
    )";
    auto paramType = [=](){
        switch (type) {
            case json::ValueType::TYPE_INT32:
                return "json::ValueType::TYPE_INT32";
            case json::ValueType::TYPE_INT64:
                return "json::ValueType::TYPE_INT64";
            case json::ValueType::TYPE_DOUBLE:
                return "json::ValueType::TYPE_DOUBLE";
            case json::ValueType::TYPE_BOOL:
                return "json::ValueType::TYPE_BOOL";
            case json::ValueType::TYPE_STRING:
                return "json::ValueType::TYPE_STRING";
            // case json::ValueType::TYPE_OBJECT:  暂不支持
            // case json::ValueType::TYPE_ARRAY:
            //     return "json::Value";
            default:
                assert(false && "bad arg type");
                return "bad type";
        }
    }();
    replaceAll(str, "[paramType]",paramType);
    replaceAll(str, "[argName]",  argName);   
    return str;     
}  


std::string procedureBindNameTypeTemplateBind(
                                const json::ValueType type,
                                const std::string& argName)
{
    std::string str = R"( 
        paramTypeN.push_back([paramType]);
        paramNameN.push_back([paramName];
    )";
    auto paramType = [=](){
        switch (type) {
            case json::ValueType::TYPE_INT32:
                return "json::ValueType::TYPE_INT32";
            case json::ValueType::TYPE_INT64:
                return "json::ValueType::TYPE_INT64";
            case json::ValueType::TYPE_DOUBLE:
                return "json::ValueType::TYPE_DOUBLE";
            case json::ValueType::TYPE_BOOL:
                return "json::ValueType::TYPE_BOOL";
            case json::ValueType::TYPE_STRING:
                return "json::ValueType::TYPE_STRING";
            // case json::ValueType::TYPE_OBJECT:  暂不支持
            // case json::ValueType::TYPE_ARRAY:
            //     return "json::Value";
            default:
                assert(false && "bad arg type");
                return "bad type";
        }
    }();
    replaceAll(str, "[paramType]",paramType);
    replaceAll(str, "[argName]",  argName);   
    return str;     
}                    


std::string procedureBindReturnTemplate(
                                const std::string& methodName,
                                const std::string& methodNameStub,
                                const std::string& TypeName,
                                const std::string& serverStubName)
{
    std::string str = R"(
        [TypeName]
        serviceInfo->addProcedureReturn([methodName], 
                                            new rpc::ProcedureReturn(
                                                std::bind(&[serverStubName]:[methodNameStub], this, _1, _2),
                                                paramName,
                                                paramType)
                                            );
    )";

    replaceAll(str, "[methodName]",        methodName);
    replaceAll(str, "[methodNameStub]",     methodNameStub);
    replaceAll(str, "[TypeName]",         TypeName);
    replaceAll(str, "[serverStubName]",    serverStubName);
}

std::string procedureBindNotifyTemplate(
                                const std::string& methodName,
                                const std::string& methodNameStub,
                                const std::string& TypeName,
                                const std::string& serverStubName)
{
    std::string str = R"(
        [TypeName]
        serviceInfo->addProcedureReturn([methodName], 
                                            new rpc::ProcedureReturn(
                                                std::bind(&[serverStubName]:[methodNameStub], this, _1, _2),
                                                paramNameN,
                                                paramTypeN)
                                            );
    )";

    replaceAll(str, "[methodName]",        methodName);
    replaceAll(str, "[methodNameStub]",     methodNameStub);
    replaceAll(str, "[TypeName]",         TypeName);
    replaceAll(str, "[serverStubName]",    serverStubName);
}

// std::string procedureBindTemplate(const std::string& serverStubName)
// {
//     std::string str = R"(
//         for(auto& p : serviceInfo_.rpcReturns_){
            // std::vector<json::ValueType> paramType;
            // std::vector<std::string> paramName;
//             for(auto& name_type : p.params_){
//                 paramType.push_back(name_type.second.getType());
//                 paramName.push_back(name_type.first.getString());
//             }
//             serviceInfo->addProcedureReturn(p.name_, 
//                                             new rpc::ProcedureReturn(
//                                                 std::bind(&[serverStubName]:p.name_+"Stub", this, _1, _2),
//                                                 paramName,
//                                                 paramType)
//                                             );
//         }

//         for(auto& p : serviceInfo_.rpcNotifys_){
//             std::vector<json::ValueType> paramType;
//             std::vector<std::string> paramName;
//             for(auto& name_type : p.params_){
//                 paramType.push_back(name_type.second.getType());
//                 paramName.push_back(name_type.first.getString());
//             }
//             serviceInfo->addProcedureReturn(p.name_, 
//                                             new rpc::ProcedureReturn(
//                                                 std::bind(&[serverStubName]:[p.name_+"Stub"], this, _1),
//                                                 paramName,
//                                                 paramType)
//                                             );
//         }
//     )";

//     replaceAll(str, "[serverStubName]",         serverStubName);
//     return str;
// }

std::string procedureReturnsStubTemplate(const std::string& methodStubName,
                                    const std::string& methodName,
                                    const std::string& methodArg)
{
    std::string str = R"(
        void [methodStubName](json::Value& request, const rpc::ResponseCallback& cb)
        {
            auto& params = request["params"];
            auto& params = request["params"];
            if(request.isArray()){
                [methodName]([methodArg] rpc::UserDoneCallback(request, done));
            }else{
                [methodName]([methodArg] rpc::UserDoneCallback(request, done));
            }
        }
    )";  

    replaceAll(str, "[methodStubName]",     methodStubName);
    replaceAll(str, "[methodName]",         methodName);
    replaceAll(str, "[methodArg]",          methodArg);
    return str;
}

std::string procedureNotifyStubTemplate(const std::string& methodStubName,
                                    const std::string& methodName,
                                    const std::string& methodArg)
{
    std::string str = R"(
        void [methodStubName](json::Value& request)
        {
            auto& params = request["params"];
            if(request.isArray()){
                [methodName]([methodArg]);
            }else{
                [methodName]([methodArg]);
            }
        }
    )";  

    replaceAll(str, "[methodStubName]",     methodStubName);
    replaceAll(str, "[methodName]",         methodName);
    replaceAll(str, "[methodArg]",          methodArg);
    return str;
}


std::string procedureReturnsTemplate(const std::string& methodName,
                                    const std::string& methodArg)
{
    std::string str = R"(
        virtual void [methodName]([methodArg] const rpc::UserDoneCallback& done) = 0;
    )";
    replaceAll(str, "[methodName]",         methodName);
    replaceAll(str, "[methodArg]",          methodArg);
    return str;
}

std::string procedureNotifysTemplate(const std::string& methodName, const std::string& methodArg)
{
    std::string str = R"(
        virtual void [methodName]([methodArg]) = 0;
    )";
    replaceAll(str, "[methodName]",         methodName);
    replaceAll(str, "[methodArg]",          methodArg);
    return str;
}


std::string methodArgTemplate(
                        const json::ValueType type,
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

std::string parseMethodArgTemplate(
                                const std::string& index,
                                json::ValueType type)
{
    std::string str = R"(params[[index]][method];)";
    std::string method = [=](){
        switch (type) {
            case json::ValueType::TYPE_BOOL:
                return ".getBool()";
            case json::ValueType::TYPE_INT32:
                return ".getInt32()";
            case json::ValueType::TYPE_INT64:
                return ".getInt64()";
            case json::ValueType::TYPE_DOUBLE:
                return ".getDouble()";
            case json::ValueType::TYPE_STRING:
                return ".getString()";
            case json::ValueType::TYPE_OBJECT:
            case json::ValueType::TYPE_ARRAY:
                return "";//todo
            default:
                assert(false && "bad value type");
                return "bad type";
        }
    }();
    replaceAll(str, "[index]", index);
    replaceAll(str, "[method]", method);
    return str;
}

}


ServerStubGen::ServerStubGen(json::Value json)
    :   StubGen(json)
{}


std::string ServerStubGen::genServer()
{
    auto macroStubName = genMacroStubName();
    auto serverStubName = genServerStubName();
    auto serverName = genServerName();
    auto procedureBind = genProcedureBind();
    auto procedureReturnsStub = genProcedureReturnsStub();
    auto procedureNotifysStub = genProcedureNotifysStub();
    auto procedureReturns = genProcedureReturns();
    auto procedureNotifys = genProcedureNotifys();

    return serverTemplate(macroStubName,
                        serverStubName,
                        serverName,
                        procedureBind,
                        procedureReturnsStub,
                        procedureNotifysStub,
                        procedureReturns,
                        procedureNotifys);
}

std::string ServerStubGen::genMacroStubName()
{
    return serviceInfo_.name_ + "SERVERSTUB";
}

std::string ServerStubGen::genServerStubName()
{
    return serviceInfo_.name_ + "ServerStub";
}

std::string ServerStubGen::genServerName()
{
    return serviceInfo_.name_;
}


std::string ServerStubGen::genProcedureBind()
{
    return genProcedureBindReturn() + genProcedureBindNotify();
}


std::string ServerStubGen::genProcedureBindReturn()
{
    auto serverStubName = genServerStubName();
    std::string returnBindFuncs;
    bool bk = false;
    for(auto& func : serviceInfo_.rpcReturns_){
        std::string TypeName;
        auto methodName = func.name_;
        auto methodNameStub = methodName + "Stub";
        if(!bk){
            TypeName = R"( 
                    vector<json::ValueType> paramType;
                    std::vector<std::string> paramName;
            )";
            for(auto& p : func.params_.getObject()){
                TypeName += procedureBindNameTypeTemplateReturn(p.second.getType(), p.first.getString());
            }
            returnBindFuncs.append(procedureBindReturnTemplate(methodName,
                                                                methodNameStub,
                                                                TypeName,
                                                                serverStubName));
            bk = true;
        }else{
            TypeName = R"(
                paramType.clear();
                paramName.clear();
            )";
            for(auto& p : func.params_.getObject()){
                TypeName += procedureBindNameTypeTemplateReturn(p.second.getType(), p.first.getString());
            }
            returnBindFuncs.append(procedureBindReturnTemplate(methodName,
                                                                methodNameStub,
                                                                TypeName,
                                                                serverStubName));
        }
    }

    return returnBindFuncs;
}


std::string ServerStubGen::genProcedureBindNotify()
{
    auto serverStubName = genServerStubName();
    std::string notifyBindFuncs;
    bool bk = false;
    for(auto& func : serviceInfo_.rpcNotifys_){
        std::string TypeName;
        auto methodName = func.name_;
        auto methodNameStub = methodName + "Stub";
        if(!bk){
            TypeName = R"( 
                    vector<json::ValueType> paramTypeN;
                    std::vector<std::string> paramNameN;
            )";
            for(auto& p : func.params_.getObject()){
                TypeName += procedureBindNameTypeTemplateBind(p.second.getType(), p.first.getString());
            }
            notifyBindFuncs.append(procedureBindNotifyTemplate(methodName,
                                                                methodNameStub,
                                                                TypeName,
                                                                serverStubName));
            bk = true;
        }else{
            TypeName = R"(
                paramTypeN.clear();
                paramNameN.clear();
            )";
            for(auto& p : func.params_.getObject()){
                TypeName += procedureBindNameTypeTemplateBind(p.second.getType(), p.first.getString());
            }
            notifyBindFuncs.append(procedureBindNotifyTemplate(methodName,
                                                                methodNameStub,
                                                                TypeName,
                                                                serverStubName));
        }
    }
    return notifyBindFuncs;
}

// std::string ServerStubGen::genProcedureBindNotify()
// {
//     return procedureBindTemplate(genServerStubName());
// }

std::string ServerStubGen::genProcedureReturnsStub()
{
    std::string returnFuncs;
    for(auto& func : serviceInfo_.rpcReturns_){
        auto methodStubName = func.name_ + "Stub";
        auto methodName = func.name_;
        std::string methodArg;
        if(func.params_.isArray())
            methodArg = genParseMethodArgArray(func.params_, true);
        else
            methodArg = genParseMethodArgObject(func.params_, true);
        auto one = procedureReturnsStubTemplate(methodStubName,
                                        methodName,
                                        methodArg);
        returnFuncs.append(one).append("\n");
    }
    return returnFuncs;
}

std::string ServerStubGen::genProcedureNotifysStub()
{
    std::string notifyFuncs;
    for(auto& func : serviceInfo_.rpcNotifys_){
        auto methodStubName = func.name_ + "Stub";
        auto methodName = func.name_;
        std::string methodArg;
        if(func.params_.isArray())
            methodArg = genParseMethodArgArray(func.params_, false);
        else
            methodArg = genParseMethodArgObject(func.params_, false);
        auto one = procedureNotifyStubTemplate(methodStubName,
                                        methodName,
                                        methodArg);
        notifyFuncs.append(one).append("\n");
    }
    return notifyFuncs;
}

std::string ServerStubGen::genProcedureReturns()
{
    std::string returnFuncs;
    for(auto& func : serviceInfo_.rpcReturns_){
        auto methodName = func.name_;
        std::string methodArg = genMethodArg(func.params_, true);
        auto one = procedureReturnsTemplate(methodName,
                                                methodArg);
        returnFuncs.append(one).append("\n");
    }
    return returnFuncs;
}

std::string ServerStubGen::genProcedureNotifys()
{
    std::string notifyFuncs;
    for(auto& func : serviceInfo_.rpcNotifys_){
        auto methodName = func.name_;
        std::string methodArg = genMethodArg(func.params_, false);
        auto one = procedureReturnsTemplate(methodName,
                                                methodArg);
        notifyFuncs.append(one).append("\n");
    }
    return notifyFuncs; 
}


std::string ServerStubGen::genParseMethodArgArray(const json::Value& params, bool hasLast)
{
    std::string args;
    bool bk = false;
    int index_ = 0;
    for(auto& p : params.getObject()){
        std::string arg = parseMethodArgTemplate(std::to_string(index_++), p.second.getType());
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

std::string ServerStubGen::genParseMethodArgObject(const json::Value& params, bool hasLast)
{
    std::string args;
    bool bk = false;
    int index_ = 0;
    for(auto& p : params.getObject()){
        std::string arg = parseMethodArgTemplate(p.first.getString(), p.second.getType());
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

std::string ServerStubGen::genMethodArg(const json::Value& params, bool hasLast)
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
