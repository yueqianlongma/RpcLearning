

#ifndef __RPCSERVER_STUB_CLIENTSTUBGEN_H__
#define __RPCSERVER_STUB_CLIENTSTUBGEN_H__


#include<RpcServer/Stub/StubGen.h>
#include<RpcServer/common.h>

namespace rpc
{

class ClientStubGen: StubGen 
{
public:
    ClientStubGen(const json::Value& value)
        :   StubGen(value)
    {}

    std::string genClient();

private:
    std::string genMacroName();
    std::string genClientName();
    std::string genProcedureReturns();
    std::string genProcedureNotifys();
    std::string genArg(const json::Value& params, bool hasLast);
    std::string genMember(const json::Value& params);

};

}







#endif
