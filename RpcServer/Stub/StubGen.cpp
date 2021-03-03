

#include"StubGen.h"
#include<RpcServer/Exception.h>
using namespace rpc;

namespace
{

void expect(bool result, const char* errMsg)
{
    if (!result) {
        throw StubException(errMsg);
    }
}

}


StubGen::StubGen(const json::Value& proto)
{
    expect(proto.isObject(), "expect object");
    expect(proto.getSize() == 2, "expect 'name' and 'rpc' fields in object");
    expect(proto.hasMember("name"), "expect name");
    expect(proto["name"].isString(), "expect name is string type");
    expect(proto.hasMember("rpc"), "expect rpc");
    expect(proto["rpc"].isArray(), "expect rpc is array type");

    serviceInfo_.name_ = proto["name"].getString();
    auto& rpc = proto["rpc"];
    parseRpc(rpc);
}

void StubGen::parseRpc(const json::Value& rpc)
{
    expect(rpc.getSize() > 0, "expect rpc's size > 0 ");
    for(auto& func : rpc.getArray()){
        expect(func.isObject(), "expect func is object type");
        expect(func.hasMember("name"), "expect func has name member");
        expect(func["name"].isString(), "expect func: name is string type");
        expect(func.hasMember("params"), "expect func has params member");
        expect(func["params"].isObject(), "expect func: params is object type");

        auto params = func["params"];
        validateParams(params);

        if(func.hasMember("returns")){
            auto returns = func["returns"];
            validateReturns(returns);
            RpcReturn r(func["name"].getString(), params, returns);
            serviceInfo_.rpcReturns_.push_back(r);
        }else{
            RpcNotify r(func["name"].getString(), params);
            serviceInfo_.rpcNotifys_.push_back(r);
        }
    }
}

void StubGen::validateParams(json::Value& params)
{
    std::set<std::string> paramsSet;
    for(auto& p : params.getObject()){
        auto key = p.first.getString();
        expect(paramsSet.find(key) == paramsSet.end(), "duplicate param name");

        paramsSet.insert(key);
        switch (p.second.getType()) {
            case json::ValueType::TYPE_NULL:
                expect(false, "bad param type");
                break;
            default:
                break;
        }
    }
}

void StubGen::validateReturns(json::Value& returns)
{
    switch (returns.getType()) {
        case json::ValueType::TYPE_NULL:
        case json::ValueType::TYPE_ARRAY:
            expect(false, "bad returns type");
            break;
        case json::ValueType::TYPE_OBJECT:
            validateParams(returns);
            break;
        default:
            break;
    }
}



