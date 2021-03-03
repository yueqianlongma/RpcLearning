
#ifndef __RECSERVER_EXCEPTION_H__
#define __RECSERVER_EXCEPTION_H__


#include <exception>
#include<RpcServer/RpcError.h>
#include<RpcServer/common.h>
#include<memory>

namespace rpc
{


class NotifyException: public std::exception
{
public:
    explicit NotifyException(RpcError err, const char* detail):
            err_(err),
            detail_(detail)
    {}

    const char* what() const noexcept
    {
        return err_.asString();
    }
    RpcError err() const
    {
        return err_;
    }
    const char* detail() const
    {
        return detail_;
    }

private:
    RpcError err_;
    const char* detail_;
};



class RequestException: public std::exception
{
public:
    RequestException(RpcError err, json::Value& id, const char* detail):
            err_(err),
            id_(new json::Value(id)),
            detail_(detail)
    {}
    explicit RequestException(RpcError err, const char* detail):
            err_(err),
            id_(new json::Value(json::ValueType::TYPE_NULL)),
            detail_(detail)
    {}

    const char* what() const noexcept
    {
        return err_.asString();
    }
    RpcError err() const
    {
        return err_;
    }
    json::Value& id()
    {
        return *id_;
    }
    const char* detail() const
    {
        return detail_;
    }

private:
    RpcError err_;
    std::unique_ptr<json::Value> id_;
    const char* detail_;
};



class ResponseException: public std::exception
{
public: 
    ResponseException(const char* msg)
        :   msg_(msg),
            hasId_(false),
            id_(-1)
    {}

    ResponseException(const char* msg, const int32_t id)
        :   msg_(msg),
            hasId_(true),
            id_(id)
    {}

    const char* what() const noexcept override
    {
        return msg_;
    }

    bool hasId()    const
    {
        return hasId_;
    }

    int32_t getId() const
    {
        return id_;
    }

private:
    const char* msg_;
    const bool  hasId_;
    const int32_t id_;
};


class StubException: std::exception
{
public:
    explicit StubException(const char* msg):
            msg_(msg)
    {}

    const char* what() const noexcept
    {
        return msg_;
    }

private:
    const char* msg_;
};

}




#endif
