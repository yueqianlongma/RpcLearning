#ifndef __JSONLEARN_READER_H__
#define __JSONLEARN_READER_H__

#include"noncopyable.h"
#include"ReadStream.h"
#include"Value.h"
#include<limits>
#include <cassert>
#include <cmath>
#include <string>
#include <vector>
#include <variant>
#include <memory>
#include <exception>
#include<iostream>
using std::unique_ptr;
using std::shared_ptr;

namespace json
{

#define ERROR_MAP(XX)\
    XX(PARSE_OK, "ok") \
    XX(PARSE_EXPECT_VALUE, "expect value")\
    XX(PARSE_INVALID_VALUE, "invalid value")\
    XX(PARSE_ROOT_NOT_SINGULAR, "root not singular")\
    XX(PARSE_NUMBER_TOO_BIG, "number too big")\
    XX(PARSE_MISS_QUOTATION_MARK, "miss quotation mark")\
    XX(PARSE_INVALID_STRING_ESCAPE, "invalid string escape")\
    XX(PARSE_INVALID_STRING_CHAR, "invalid string char")\
    XX(PARSE_INVALID_UNICODE_HEX, "invalid unicode hex")\
    XX(PARSE_INVALID_UNICODE_SURROGATE, "invalid unicode surrogate")\
    XX(PARSE_MISS_COMMA_OR_SQUARE_BRACKET, "miss comma or sqare bracket")\
    XX(PARSE_MISS_KEY, "miss key")\
    XX(PARSE_MISS_COLON, "miss colon")\
    XX(PARSE_MISS_COMMA_OR_CURLY_BRACKET, "miss comma or curly bracket")\

enum ParseError{
#define GEN_ERRNO(e, s) e,
    ERROR_MAP(GEN_ERRNO)
#undef GEN_ERRNO(e, s)
};

inline const char* parseErrorStr(ParseError err)
{
    const static char* tab[] = {
#define GEN_STRERR(e, s) s,
    ERROR_MAP(GEN_STRERR)
#undef GEN_STRERR
    };
    assert(err >= 0 && err < sizeof(tab) / sizeof(tab[0]));
    return tab[err];
}

#undef ERROR_MAP


template <typename ReadStream>
class Reader : public noncopyable 
{                                
public:
    typedef shared_ptr<Value> ValuePtr;
public:
    static ParseError parse(ReadStream& is, Value& value)
    { 
        auto tempValue = std::make_shared<Value>();
        ParseError ret = PARSE_OK;
        parseWhitespace(is);

        if((ret = parseValue(is, tempValue)) != PARSE_OK){
            value = std::move(*tempValue);
            return ret; 
        }
        parseWhitespace(is);
        if (is.hasNext())
            return PARSE_ROOT_NOT_SINGULAR;
        value = std::move(*tempValue);
        return PARSE_OK;
    }

private:
    static bool isDigit(char ch)
    {
        return ch >= '0' && ch <= '9';
    }

    static bool isDigit19(char ch)
    {
        return ch >= '1' && ch <= '9';
    }

    static void parseWhitespace(ReadStream& is) 
    {
        while (is.hasNext())
        {
            char ch = is.front();
            if(ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n')
                is.pop();
            else break;
        }
        
    }

    static ParseError parseLiteral(ReadStream& is, ValuePtr value, const char* literal, ValueType type)
    {
        char c = *literal;

        is.expect(*literal++);
        while (*literal != '\0' && *literal == is.front()) {
            literal++;
            is.pop();
        }
        if(*literal != '\0')
            return PARSE_EXPECT_VALUE;

        if(type == ValueType::TYPE_NULL)
            value->setNull();
        else if(type == ValueType::TYPE_BOOL){
            if(c == 'f')
                value->setBool(false);
            else    
                value->setBool(true);
        }
        return PARSE_OK;
    }


    static ParseError parseNumber(ReadStream& is, ValuePtr value)
    {
        auto start = is.getIter();
        if(is.front() == '-') is.pop();
        if(is.front() == '0') is.pop();
        else {
            if(!isDigit19(is.front()))  return PARSE_INVALID_VALUE;
            while(isDigit(is.front()))  is.pop();
        }
        auto expectType = ValueType::TYPE_NULL;

        if (is.front() == '.') {
            expectType = ValueType::TYPE_DOUBLE;
            is.pop();
            if (!isDigit(is.front()))
                return PARSE_INVALID_VALUE;
            is.pop();
            while (isDigit(is.front()))
                is.pop();
        }
        if (is.front() == 'e' || is.front() == 'E') {
            expectType = ValueType::TYPE_DOUBLE;
            is.pop();
            if (is.front() == '+' || is.front() == '-')
                is.pop();
            if (!isDigit(is.front()))
                return PARSE_INVALID_VALUE;
            is.pop();
            while (isDigit(is.front()))
                is.pop();
        }

        // int64 or int32 ?
        if (is.front() == 'i') {
            is.pop();
            if (expectType == ValueType::TYPE_DOUBLE)
                return PARSE_INVALID_VALUE;
            switch (is.next())
            {
                case '3':
                    if (is.next() != '2')
                        return PARSE_INVALID_VALUE;
                    expectType = ValueType::TYPE_INT32;
                    break;
                case '6':
                    if (is.next() != '4')
                        return PARSE_INVALID_VALUE;
                    expectType = ValueType::TYPE_INT64;
                    break;
                default:
                    return PARSE_INVALID_VALUE;
            }
        }


        auto end = is.getIter();
        if (start == end)
            return PARSE_INVALID_VALUE;
        // std::stod() && std::stoi() are bad ideas,
        // because new string buffer is needed
        //
        try{
            std::size_t idx;
            if (expectType == ValueType::TYPE_DOUBLE) {
                double d = __gnu_cxx::__stoa(&std::strtod, "stod", &*start, &idx);
                assert(start + idx == end);
                value->setDouble(d);
            }
            else {
                int64_t i64 = __gnu_cxx::__stoa(&std::strtol, "stol", &*start, &idx, 10);
                if (expectType == ValueType::TYPE_INT64){
                    value->setInt64(i64);
                }
                else if (expectType == ValueType::TYPE_INT32){
                    if (i64 > std::numeric_limits<int32_t>::max() ||
                        i64 < std::numeric_limits<int32_t>::min()) {
                        return PARSE_NUMBER_TOO_BIG;
                    }
                    value->setInt32(static_cast<int32_t>(i64));
                }
                else{
                    return PARSE_INVALID_VALUE;
                }
            }
        }catch (...){
            if(expectType == ValueType::TYPE_INT32) 
                value->setInt32(0);
            else if(expectType == ValueType::TYPE_INT64) 
                value->setInt64(0);
            else value->setDouble(0.0);
            return PARSE_NUMBER_TOO_BIG;
        }
        return PARSE_OK;
    }

    static ParseError parseHex4(ReadStream& is, unsigned* u) {
        *u = 0;
        for (int i = 0; i < 4; i++) {
            char ch = is.next();
            *u <<= 4;
            if      (ch >= '0' && ch <= '9')  *u |= ch - '0';
            else if (ch >= 'A' && ch <= 'F')  *u |= ch - ('A' - 10);
            else if (ch >= 'a' && ch <= 'f')  *u |= ch - ('a' - 10);
            else return PARSE_INVALID_UNICODE_HEX;
        }
        return PARSE_OK;
    }

    static void encodeUtf8(std::string& buffer, unsigned u)
    {
        if (u <= 0x7F) 
            buffer.push_back(u & 0xFF);
        else if (u <= 0x7FF) {
            buffer.push_back(0xC0 | ((u >> 6) & 0xFF));
            buffer.push_back(0x80 | ( u       & 0x3F));
        }
        else if (u <= 0xFFFF) {
            buffer.push_back(0xE0 | ((u >> 12) & 0xFF));
            buffer.push_back(0x80 | ((u >>  6) & 0x3F));
            buffer.push_back(0x80 | ( u        & 0x3F));
        }
        else {
            assert(u <= 0x10FFFF);
            buffer.push_back(0xF0 | ((u >> 18) & 0xFF));
            buffer.push_back(0x80 | ((u >> 12) & 0x3F));
            buffer.push_back(0x80 | ((u >>  6) & 0x3F));
            buffer.push_back(0x80 | ( u        & 0x3F));
        }
    }

    static ParseError parseString(ReadStream& is, ValuePtr value)
    {
        is.expect('"');
        std::string buffer;
        while (is.hasNext()) {
            switch (char ch = is.next()) {
                case '"':
                    value->setString(std::move(buffer));
                    return PARSE_OK;
                case '\\':
                    switch (is.next()) {
                        case '"':  buffer.push_back('"');  break;
                        case '\\': buffer.push_back('\\'); break;
                        case '/':  buffer.push_back('/');  break;
                        case 'b':  buffer.push_back('\b'); break;
                        case 'f':  buffer.push_back('\f'); break;
                        case 'n':  buffer.push_back('\n'); break;
                        case 'r':  buffer.push_back('\r'); break;
                        case 't':  buffer.push_back('\t'); break;
                        case 'u': {
                            // unicode stuff from Milo's tutorial
                            unsigned u = 0;
                            if(parseHex4(is, &u) != PARSE_OK)   
                                return PARSE_INVALID_UNICODE_HEX;
                            if (u >= 0xD800 && u <= 0xDBFF) {
                                if (is.next() != '\\')
                                    return PARSE_INVALID_UNICODE_SURROGATE;
                                if (is.next() != 'u')
                                    return PARSE_INVALID_UNICODE_SURROGATE;

                                unsigned u2 = 0;
                                if(parseHex4(is, &u2) != PARSE_OK)  
                                    return PARSE_INVALID_UNICODE_HEX;
                                if (u2 < 0xDC00 || u2 > 0xDFFF)
                                    return PARSE_INVALID_UNICODE_SURROGATE;
                                u = 0x10000 + (u - 0xD800) * 0x400 + (u2 - 0xDC00);
                            }
                            encodeUtf8(buffer, u);
                            break;
                        }
                        default: return PARSE_INVALID_STRING_ESCAPE;
                    }
                    break;
                case '\0':
                    return PARSE_MISS_QUOTATION_MARK;
                default: 
                    if ((unsigned char)ch < 0x20)
                        return PARSE_INVALID_STRING_CHAR;
                    buffer.push_back(ch);
            }
        }
        return PARSE_OK;
    }

    static ParseError parseArray(ReadStream& is, ValuePtr value)
    {
        value->setArray();
        is.expect('[');
        parseWhitespace(is);
        if (is.front() == ']') {
            is.pop();
            return PARSE_OK;
        }

        ParseError ret = PARSE_OK;
        while (true) {
            auto temp = std::make_shared<Value>();
            if((ret = parseValue(is, temp)) != PARSE_OK)   break; 
            value->addValue(std::move(*temp));

            parseWhitespace(is);
            switch (is.next()) {
                case ',':
                    parseWhitespace(is);
                    break;
                case ']':
                    return PARSE_OK;
                default:
                    ret =  PARSE_MISS_COMMA_OR_SQUARE_BRACKET;
            }
        }
        return ret;
    }


    static ParseError parseObject(ReadStream& is, ValuePtr object)
    {
        object->setObject();
        is.expect('{');
        parseWhitespace(is);
        if (is.front() == '}') {
            is.pop();
            return PARSE_OK;
        }

        ParseError ret = PARSE_OK;
        while (true) {
            if (is.front() != '"')
                return PARSE_MISS_KEY;

            auto key = std::make_shared<Value>();
            if((ret = parseString(is, key)) != PARSE_OK)
                break;
            // parse ':'
            parseWhitespace(is);
            if (is.next() != ':')
                return PARSE_MISS_COLON;
            parseWhitespace(is);

            // go on
            auto value = std::make_shared<Value>();
            if((ret = parseValue(is, value)) != PARSE_OK)
                break;
            
            object->addMember(std::move(*key), std::move(*value));
            
            parseWhitespace(is);
            if(is.front() == ','){
                is.pop();
                parseWhitespace(is);
            }
            else if(is.front() == '}'){
                is.pop();
                ret = PARSE_OK;
                break;
            }
            else{
                ret = PARSE_MISS_COMMA_OR_CURLY_BRACKET;
                break;
            }
        }
        return ret;
    }

    static ParseError parseValue(ReadStream& is, ValuePtr value)
    {
        switch(is.front()){
            case 'n': return parseLiteral(is, value, "null",  ValueType::TYPE_NULL);
            case 't': return parseLiteral(is, value, "true",  ValueType::TYPE_BOOL);
            case 'f': return parseLiteral(is, value, "false", ValueType::TYPE_BOOL);
            case '"': return parseString(is, value);
            case '[': return parseArray(is, value);
            case '{': return parseObject(is, value);
            default:  return parseNumber(is, value);
        }
    }
};

}


#endif
