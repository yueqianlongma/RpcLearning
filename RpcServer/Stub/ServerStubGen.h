
#ifndef __RPCSERVER_SERVERSTUBGEN_H__
#define __RPCSERVER_SERVERSTUBGEN_H__

#include<RpcServer/Stub/StubGen.h>

namespace rpc
{

class ServerStubGen: public StubGen 
{
public:
    ServerStubGen(json::Value json);

    std::string genServer();

private:

    std::string genMacroStubName();

    std::string genServerStubName();

    std::string genServerName();

    std::string genProcedureBind();
    std::string genProcedureBindReturn();
    std::string genProcedureBindNotify();

    std::string genProcedureReturnsStub();

    std::string genProcedureNotifysStub();

    std::string genProcedureReturns();

    std::string genProcedureNotifys();

    std::string genParseMethodArgArray(const json::Value& params, bool hasLast);
    std::string genParseMethodArgObject(const json::Value& params, bool hasLast);
    std::string genMethodArg(const json::Value& params, bool hasLast);

};

}





#endif