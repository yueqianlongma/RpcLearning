

#ifndef __RPCSERVER_SERVICE_H__
#define __RPCSERVER_SERVICE_H__


#include<RpcServer/common.h>
#include<RpcServer/Server/Procedure.h>


namespace rpc
{

class Service : noncopyable
{
public:
    Service() = default;

    void addProcedureReturn(const std::string& methodName, ProcedureReturn* p)
    {
        assert(procedureReturn_.find(methodName) == procedureReturn_.end());
        procedureReturn_.emplace(methodName, p);
    }

    void addProcedureNotify(const std::string& methodName, ProcedureNotify *p)
    {
        assert(procedureNotfiy_.find(methodName) == procedureNotfiy_.end());
        procedureNotfiy_.emplace(methodName, p);
    }

    void callProcedureReturn(const std::string& methodName,
                             json::Value& request,
                             const RpcDoneCallback& done);
    void callProcedureNotify(const std::string& methodName, json::Value& request);

private:
    typedef std::unique_ptr<ProcedureReturn> ProcedureReturnPtr;
    typedef std::unique_ptr<ProcedureNotify> ProcedureNotifyPtr;
    typedef std::map<std::string, ProcedureReturnPtr> ProcedureReturnList;
    typedef std::map<std::string, ProcedureNotifyPtr> ProcedureNotifyList;

private:

    ProcedureReturnList procedureReturn_;
    ProcedureNotifyList procedureNotfiy_;

};

}






#endif
