#ifndef __JSONLEARN_WRITER_H__
#define __JSONLEARN_WRITER_H__

#include"noncopyable.h"
#include"Value.h"
#include<cstring>
#include<string>
#include<vector>

namespace json
{

template <typename WriterStream>   
class FastWriter : public noncopyable
{
public:
    explicit FastWriter(WriterStream& ws)
        :   seeValue_(false), ws_(ws)
    {}

    bool Null()
    {
        prefix(ValueType::TYPE_NULL);
        ws_.put("null");
        return true;
    }

    bool Bool(bool b)
    {
        prefix(ValueType::TYPE_BOOL);
        ws_.put(b ? "true" : "false");
        return true;
    }

    bool Int32(int32_t i32)
    {
        prefix(ValueType::TYPE_INT32);
        ws_.put(std::to_string(i32) + "i32");
        return true;
    }

    bool Int64(int64_t i64)
    {
        prefix(ValueType::TYPE_INT64);
        ws_.put(std::to_string(i64)+"i64");
        return true;
    }

    bool Double(double d)
    {
        prefix(ValueType::TYPE_DOUBLE);
        ws_.put(std::to_string(d));
        return true;
    }

    bool String(const std::string &str)
    {
        prefix(ValueType::TYPE_STRING);
        ws_.put('"');
        for(auto c : str){
            auto u = static_cast<unsigned char>(c);
            switch (u)
            {
            case '\"':  ws_.put("\\\""); break;
            case '\\':  ws_.put("\\\\"); break;
            case '\b':  ws_.put("\\b"); break;
            case '\f':  ws_.put("\\f"); break;
            case '\n':  ws_.put("\\n"); break;
            case '\r':  ws_.put("\\r"); break;
            case '\t':  ws_.put("\\t"); break;
            default:
                if(u < 0x20){
                    char buf[7];
                    snprintf(buf, 7, "\\u%04x", u);
                    ws_.put(buf);
                }
                else ws_.put(c);
                break;
            }
        }
        ws_.put('"');
        return true;
    }

    bool StartArray()
    {
        prefix(ValueType::TYPE_ARRAY);
        stack_.emplace_back(true);
        ws_.put('[');
        return true;
    }

    bool EndArray()
    {
        assert(!stack_.empty());
        assert(stack_.back().inArray_);
        stack_.pop_back();
        ws_.put(']');
        return true;
    }

    bool StartObject()
    {
        prefix(ValueType::TYPE_OBJECT);
        stack_.emplace_back(false);
        ws_.put('{');
        return true;
    }

    bool EndObject()
    {
        assert(!stack_.empty());
        assert(!stack_.back().inArray_);
        stack_.pop_back();
        ws_.put('}');
        return true;
    }

    bool Key(const std::string& key)
    {
        prefix(ValueType::TYPE_STRING);
        ws_.put('"');
        ws_.put(key);
        ws_.put('"');
        return true;
    }

private:
    void prefix(ValueType type)
    {
        if(seeValue_)
            assert(!stack_.empty() && "root not singular");
        else seeValue_ = true;

        if(stack_.empty())  return;

        auto& top = stack_.back();
        if(top.inArray_){
            if(top.valueCount > 0)
                ws_.put(',');
        }else{
            if(top.valueCount % 2 == 1)
                ws_.put(':');
            else{
                assert(type == ValueType::TYPE_STRING && "miss quotation mark");
                if(top.valueCount > 0)
                    ws_.put(',');
            }
        }
        ++top.valueCount;
    }

private:
    typedef struct Level{
        explicit Level(bool inArray):
                inArray_(inArray), valueCount(0)
        {}
        bool inArray_; // in array or object
        int valueCount;
    }Level;

private:
    bool seeValue_;
    std::vector<Level>  stack_;
    WriterStream& ws_;
};


} // namespace json



#endif