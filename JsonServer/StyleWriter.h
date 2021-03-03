#ifndef __JSONLEARN_STYLEWRITER_H__
#define __JSONLEARN_STYLEWRITER_H__


#include"noncopyable.h"
#include"FastWriter.h"
#include<string>
#include<cassert>

namespace json
{

template <typename WriterStream>
class StyleWriter : public noncopyable
{
public:
    StyleWriter(WriterStream& ws, const std::string& indent)
        :   indentDepth_(0),
            indent_(indent),
            ws_(ws),
            fw_(ws)

    {}

    bool Null()
    {
        fw_.Null();
        keepIndent();
        return true;
    }

    bool Bool(bool b)
    {
        fw_.Bool(b);
        keepIndent();
        return true;
    }

    bool Int32(int32_t i32)
    {
        fw_.Int32(i32);
        keepIndent();
        return true;
    }

    bool Int64(int64_t i64)
    {
        fw_.Int64(i64);
        keepIndent();
        return true;
    }

    bool Double(double d)
    {
        fw_.Double(d);
        keepIndent();
        return true;
    }

    bool String(const std::string &str)
    {
        fw_.String(str);
        keepIndent();
        return true;
    }

    bool StartArray()
    {
        fw_.StartArray();
        incrIndent();
        return true;
    }

    bool EndArray()
    {
        decrIndent();
        fw_.EndArray();
        return true;
    }

    bool StartObject()
    {
        fw_.StartObject();
        incrIndent();
        return true;
    }

    bool EndObject()
    {
        decrIndent();
        fw_.EndObject();
        return true;
    }

    bool Key(const std::string& key)
    {
        keepIndent();
        fw_.Key(key);
        return true;
    }

private:
    void incrIndent()
    {
        ++indentDepth_;
        keepIndent();
    }

    void decrIndent()
    {
        assert(indentDepth_ > 0);
        --indentDepth_;
        keepIndent();
    }

    void keepIndent()
    {
        if(indentDepth_ > 0){
            putNewLine();
            putIndent();
        }
    }

    void putIndent()
    {
        for(auto i = 0; i < indentDepth_; ++i)
            ws_.put(indent_);
    }

    void putNewLine()
    {
        ws_.put('\n');
    }

private:
    int indentDepth_;
    std::string indent_;
    WriterStream& ws_;
    FastWriter<WriterStream> fw_;
};

}


#endif