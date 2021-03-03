
#ifndef __SJONLEARN_VALUE_H__
#define __SJONLEARN_VALUE_H__

#include<vector>
#include<map>
#include<string>
#include<cstring>
#include<cassert>
#include<atomic>

namespace json
{

enum class ValueType {
    TYPE_NULL,
    TYPE_BOOL,
    TYPE_INT32,
    TYPE_INT64,
    TYPE_DOUBLE,
    TYPE_STRING,
    TYPE_ARRAY,
    TYPE_OBJECT,
};


class Value
{
public:
    explicit Value(ValueType type = ValueType::TYPE_NULL)
        :   type_(type),
            array_(nullptr)
    {
        switch (type_)
        {
        case ValueType::TYPE_NULL:
        case ValueType::TYPE_BOOL:
        case ValueType::TYPE_INT32:
        case ValueType::TYPE_INT64:
        case ValueType::TYPE_DOUBLE:    break;
        case ValueType::TYPE_STRING:
            str_ = new JsonStr();       break;
        case ValueType::TYPE_ARRAY:
            array_ = new JsonArray();   break;
        case ValueType::TYPE_OBJECT:
            object_ = new JsonObject(); break;
        default: assert(false && "bad value type");
        }
    }

    explicit Value(bool b)
        :   type_(ValueType::TYPE_BOOL),
            b_(b)
    {}

    explicit Value(int32_t i32)
        :   type_(ValueType::TYPE_INT32),
            i32_(i32)
    {}

    explicit Value(int64_t i64)
        :   type_(ValueType::TYPE_INT64),
            i64_(i64)
    {}

    explicit Value(double d)
        :   type_(ValueType::TYPE_DOUBLE),
            d_(d)
    {}

    explicit Value(const std::string& str)
        :   type_(ValueType::TYPE_STRING),
            str_(new JsonStr(str.begin(), str.end()))
    {}

    explicit Value(const char* str)
        :   type_(ValueType::TYPE_STRING),
            str_(new JsonStr(str, str + strlen(str)))
    {}    

    Value(const char* str, size_t len)
        :   type_(ValueType::TYPE_STRING),
            str_(new JsonStr(str, str + len))
    {}

    Value(const Value& rhs)
        :   type_(rhs.type_),
            array_(rhs.array_)
    {
        switch (type_)
        {
        case ValueType::TYPE_NULL:
        case ValueType::TYPE_BOOL:
        case ValueType::TYPE_INT32:
        case ValueType::TYPE_INT64:
        case ValueType::TYPE_DOUBLE:    break;
        case ValueType::TYPE_STRING:
            str_->incrAndGet();         break;
        case ValueType::TYPE_ARRAY:
            array_->incrAndGet();       break;
        case ValueType::TYPE_OBJECT:
            object_->incrAndGet();      break;
        default: assert(false && "bad value type");
        }
    }

    Value(Value&& rhs)
        :   type_(rhs.type_),
            array_(rhs.array_)
    {
        rhs.type_ = ValueType::TYPE_NULL;
        rhs.array_ = nullptr;
    }

    Value& operator=(Value& rhs)
    {
        assert(this != &rhs);
        this->~Value();
        type_ = rhs.type_;
        array_ = rhs.array_;
        switch (type_)
        {
        case ValueType::TYPE_NULL:
        case ValueType::TYPE_BOOL:
        case ValueType::TYPE_INT32:
        case ValueType::TYPE_INT64:
        case ValueType::TYPE_DOUBLE:    break;
        case ValueType::TYPE_STRING:
            str_->incrAndGet();         break;
        case ValueType::TYPE_ARRAY:
            array_->incrAndGet();       break;
        case ValueType::TYPE_OBJECT:
            object_->incrAndGet();      break;
        default: assert(false && "bad value type");
        }
        return *this;
    }

    Value& operator=(Value&& rhs)
    {
        assert(this != &rhs);
        this->~Value();
        type_ = rhs.type_;
        array_ = rhs.array_;
        rhs.type_ = ValueType::TYPE_NULL;
        rhs.array_ = nullptr;
        return *this;
    }

    ~Value()
    {
        switch (type_)
        {
        case ValueType::TYPE_NULL:
        case ValueType::TYPE_BOOL:
        case ValueType::TYPE_INT32:
        case ValueType::TYPE_INT64:
        case ValueType::TYPE_DOUBLE:    
            break;
        case ValueType::TYPE_STRING:
            if (str_->decrAndGet() == 0)
                delete str_;
            break;
        case ValueType::TYPE_ARRAY:
            if (array_->decrAndGet() == 0)
                delete array_;
            break;
        case ValueType::TYPE_OBJECT:
            if (object_->decrAndGet() == 0)
                delete object_;
            break;
        default: assert(false && "bad value type");
        }
    }
    
    //is
    ValueType getType() const { return type_; }
    
    size_t getSize() const
    {
        if (type_ == ValueType::TYPE_ARRAY)
            return array_->data_.size();
        else if (type_ == ValueType::TYPE_OBJECT)
            return object_->data_.size();
        return 1; 
    }

    bool isNull()   const { return type_ == ValueType::TYPE_NULL; }
    bool isBool()   const { return type_ == ValueType::TYPE_BOOL; }
    bool isInt32()  const { return type_ == ValueType::TYPE_INT32; }
    bool isInt64()  const { return type_ == ValueType::TYPE_INT64 || type_ == ValueType::TYPE_INT32; }
    bool isDouble() const { return type_ == ValueType::TYPE_DOUBLE; }
    bool isString() const { return type_ == ValueType::TYPE_STRING; }
    bool isArray()  const { return type_ == ValueType::TYPE_ARRAY; }
    bool isObject() const { return type_ == ValueType::TYPE_OBJECT; }

    std::string showType() const
    {
        if(isNull())        return "null";
        else if(isBool())   return "bool";
        else if(isInt32())  return "int32";
        else if(isInt64())  return "int64";
        else if(isDouble()) return "double";
        else if(isString()) return "string";
        else if(isArray())  return "array";
        else if(isObject()) return "object";
    }

    //get
    bool getBool()  const
    {
        assert(type_ == ValueType::TYPE_BOOL);
        return b_;
    }

    int32_t getInt32()  const
    {
        assert(type_ == ValueType::TYPE_INT32);
        return i32_;
    }

    int64_t getInt64()  const
    {
        assert(type_ == ValueType::TYPE_INT64 || type_ == ValueType::TYPE_INT32);
        return i64_;
    }

    double getDouble()  const
    {
        assert(type_ == ValueType::TYPE_DOUBLE);
        return d_;
    }

    std::string getString()  const
    {
        assert(type_ == ValueType::TYPE_STRING);
        return std::string(str_->data_.begin(), str_->data_.end());
    }

    const std::vector<Value>& getArray()  const
    {
        assert(type_ == ValueType::TYPE_ARRAY);
        return array_->data_;
    }

    const std::map<Value, Value>& getObject()  const
    {
        assert(type_ == ValueType::TYPE_OBJECT);
        return object_->data_;
    }
    
    //set
    Value& setNull()
    {
        this->~Value();
        return *new (this) Value(ValueType::TYPE_NULL);
    }

    Value& setBool(bool b)
    {
        this->~Value();
        return *new (this) Value(b);
    }

    Value& setInt32(int32_t i32)
    {
        this->~Value();
        return *new (this) Value(i32);
    }

    Value& setInt64(int64_t i64)
    {
        this->~Value();
        return *new (this) Value(i64);
    }

    Value& setDouble(double d)
    {
        this->~Value();
        return *new (this) Value(d);
    }

    Value& setString(std::string s)
    {
        this->~Value();
        return *new (this) Value(s);
    }

    Value& setArray()
    {
        this->~Value();
        return *new (this) Value(ValueType::TYPE_ARRAY);
    }

    Value& setObject()
    {
        this->~Value();
        return *new (this) Value(ValueType::TYPE_OBJECT);
    }


    //array
    Value& operator[](size_t i)
    {
        assert(type_ == ValueType::TYPE_ARRAY);
        return array_->data_[i];
    }

    const Value& operator[](size_t i)   const
    {
        assert(type_ == ValueType::TYPE_ARRAY);
        return array_->data_[i];
    }

    template <typename... Args>
    void addValue(Args&&... args)
    {
        assert(type_ == ValueType::TYPE_ARRAY);
        array_->data_.emplace_back(std::forward<Args>(args)...);
    }

    //object
    /*
    *   map用来比较用的， 比较使用的是key，所以为TYPE_STRING类型
    */
    bool operator<(const Value& rhs)    const
    {
        return getString() < rhs.getString();
    }

    bool hasMember(const std::string& key) const
    {
        return hasMember(Value(key));
    }

    bool hasMember(const char* key) const
    {
        return hasMember(Value(key));
    }

    bool hasMember(const Value& key)    const
    {
        return object_->data_.find(key) != object_->data_.end();
    }

    Value& operator[](const char* key)
    {
        return this->operator[](std::string(key)); 
    }

    const Value& operator[](const char* key) const
    {
        return this->operator[](std::string(key)); 
    }

    Value& operator[](const std::string& key) 
    {
        assert(type_ == ValueType::TYPE_OBJECT);
        Value key_(key);
        assert(hasMember(key_));
        return object_->data_[key_];
    }

    const Value& operator[](const std::string& key) const
    {
        assert(type_ == ValueType::TYPE_OBJECT);
        Value key_(key);
        assert(hasMember(key_));
        return object_->data_[key_]; 
    }

    template <typename... Args>
    void addMember(const std::string& key, Args&&... args)
    {
        assert(type_ == ValueType::TYPE_OBJECT);
        addMember(std::move(Value(key)), std::forward<Args>(args)...);
    }

    template <typename... Args>
    void addMember(Value&& key, Args&&... args)
    {
        assert(type_ == ValueType::TYPE_OBJECT);
        assert(!hasMember(key));
        object_->data_.emplace(std::move(key), std::forward<Args>(args)...);
    }

    template <typename Write>
    bool writeTo(Write& write) const;

private:
    template <typename T>
    struct Json
    {
        template <typename... Args>
        explicit Json(Args&&... args)
            :   data_(std::forward<Args>(args)...),
                refCount(1)
        {}
        
        ~Json()
        {assert(refCount == 0);}

        int incrAndGet()
        {
            assert(refCount > 0);
            return ++refCount;
        }

        int decrAndGet()
        {
            assert(refCount > 0);
            return --refCount;
        }

        std::atomic_int refCount;
        T data_;
    };

    using JsonStr = Json<std::vector<char>>;
    using JsonArray = Json<std::vector<Value>>;
    using JsonObject = Json<std::map<Value, Value>>;


private:
    union
    {
        bool         b_;
        int32_t      i32_;
        int64_t      i64_;
        double       d_;
        JsonStr*     str_;
        JsonArray*   array_;
        JsonObject*  object_;
    };

    ValueType type_;

};


#define CALL(expr) do { if (!(expr)) return false; } while(false)

template <typename Write>
inline bool Value::writeTo(Write& write) const
{
    switch (type_)
    {
        case ValueType::TYPE_NULL:
            CALL(write.Null());
            break;
        case ValueType::TYPE_BOOL:
            CALL(write.Bool(b_));
            break;
        case ValueType::TYPE_INT32:
            CALL(write.Int32(i32_));
            break;
        case ValueType::TYPE_INT64:
            CALL(write.Int64(i64_));
            break;
        case ValueType::TYPE_DOUBLE:
            CALL(write.Double(d_));
            break;
        case ValueType::TYPE_STRING:
            CALL(write.String(getString()));
            break;
        case ValueType::TYPE_ARRAY:
            CALL(write.StartArray());
            for (auto& val: getArray()) {
                CALL(val.writeTo(write));
            }
            CALL(write.EndArray());
            break;
        case ValueType::TYPE_OBJECT:
            CALL(write.StartObject());
            for (auto& member: getObject()) {
                write.Key(member.first.getString());
                CALL(member.second.writeTo(write));
            }
            CALL(write.EndObject());
            break;
        default:
            assert(false && "bad type");
    }
    return true;
}

#undef CALL

}

#endif
