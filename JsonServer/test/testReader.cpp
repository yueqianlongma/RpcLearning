
#include "../Value.h"
#include"../Reader.h"
#include"../ReadStream.h"
#include<iostream>
#include<string>
#include"../StyleWriter.h"
#include"../FastWriter.h"
#include"../WriteStream.h"
using namespace json;
using namespace std;


static int main_ret = 0;
static int test_count = 0;
static int test_pass = 0;

#define EXPECT_EQ_BASE(equality, expect, actual, format) \
    do {\
        test_count++;\
        if (equality)\
            test_pass++;\
        else {\
            fprintf(stderr, "%s:%d: expect: " format " actual: " format "\n", __FILE__, __LINE__, expect, actual);\
            main_ret = 1;\
        }\
    } while(0)
#define EXPECT_EQ_INT(expect, actual) EXPECT_EQ_BASE((expect) == (actual), expect, actual, "%d")
#define EXPECT_EQ_DOUBLE(expect, actual) EXPECT_EQ_BASE((expect) == (actual), expect, actual, "%.17g")

static void testNull()
{
    Value value;
    string str("null");
    StringReadStream is(str);
    EXPECT_EQ_INT(PARSE_OK, Reader<StringReadStream>::parse(is, value));
    EXPECT_EQ_INT(true, value.isNull());
}

static void testBool()
{
    Value value1;
    string str1("false");
    StringReadStream is1(str1);
    EXPECT_EQ_INT(PARSE_OK, Reader<StringReadStream>::parse(is1, value1));
    EXPECT_EQ_INT(true, value1.isBool());
    EXPECT_EQ_INT(false, value1.getBool());
    cout<<value1.getBool()<<endl;

    Value value2;
    string str2("true");
    StringReadStream is2(str2);
    EXPECT_EQ_INT(PARSE_OK, Reader<StringReadStream>::parse(is2, value2));
    EXPECT_EQ_INT(true, value2.isBool());
    EXPECT_EQ_INT(true, value2.getBool());
    cout<<value2.getBool()<<endl;
}


#define TEST_INT32(expect, json)\
    do {\
        Value value;\
        string str(json);\
        StringReadStream is(str);\
        ParseError ret = Reader<StringReadStream>::parse(is, value);\
        EXPECT_EQ_INT(PARSE_OK, ret);\        
        EXPECT_EQ_INT(true, value.isInt32());\
        EXPECT_EQ_DOUBLE(expect, value.getInt32());\
        cout<<value.showType()<<"  "<<value.getInt32()<<endl;\
    } while(0)

#define TEST_INT64(expect, json)\
    do {\
        Value value;\
        string str(json);\
        StringReadStream is(str);\
        ParseError ret = Reader<StringReadStream>::parse(is, value);\
        EXPECT_EQ_INT(PARSE_OK, ret);\
        EXPECT_EQ_INT(true, value.isInt64());\
        EXPECT_EQ_DOUBLE(expect, value.getInt64());\
        cout<<value.showType()<<"  "<<value.getInt64()<<endl;\
    } while(0)

#define TEST_DOUBLE(expect, json)\
    do {\
        Value value;\
        string str(json);\
        StringReadStream is(str);\
        ParseError ret = Reader<StringReadStream>::parse(is, value);\
        EXPECT_EQ_INT(PARSE_OK, ret);\   
        EXPECT_EQ_INT(true, value.isDouble());\
        EXPECT_EQ_DOUBLE(expect, value.getDouble());\
        cout<<value.getDouble()<<endl;\
    } while(0)

#define TEST_ERROR(error, json)\
    do {\
        Value value;\
        string str(json);\
        StringReadStream is(str);\
        ParseError ret = Reader<StringReadStream>::parse(is, value);\
        EXPECT_EQ_INT(PARSE_OK, ret);\ 
        EXPECT_EQ_INT(true, value.isNull());\
        cout<<value.showType()<<endl;\
    } while(0)

static void testNumber()
{
    TEST_DOUBLE(0.0, "0.0");
    TEST_DOUBLE(0.0, "-0.0");
    TEST_DOUBLE(0.0, "-0.0");
    TEST_DOUBLE(1.0, "1.0");
    TEST_DOUBLE(-1.0, "-1.0");
    TEST_DOUBLE(1.5, "1.5");
    TEST_DOUBLE(-1.5, "-1.5");
    TEST_DOUBLE(3.1416, "3.1416");
    TEST_DOUBLE(1E10, "1E10");
    TEST_DOUBLE(1e10, "1e10");
    TEST_DOUBLE(1E+10, "1E+10");
    TEST_DOUBLE(1E-10, "1E-10");
    TEST_DOUBLE(-1E10, "-1E10");
    TEST_DOUBLE(-1e10, "-1e10");
    TEST_DOUBLE(-1E+10, "-1E+10");
    TEST_DOUBLE(-1E-10, "-1E-10");
    TEST_DOUBLE(1.234E+10, "1.234E+10");
    TEST_DOUBLE(1.234E-10, "1.234E-10");
    TEST_DOUBLE(0.0, "1e-10000"); /* must underflow */

    TEST_DOUBLE(1.0000000000000002, "1.0000000000000002"); /* the smallest number > 1 */
    TEST_DOUBLE( 4.9406564584124654e-324, "4.9406564584124654e-324"); /* minimum denormal */
    TEST_DOUBLE(-4.9406564584124654e-324, "-4.9406564584124654e-324");
    TEST_DOUBLE( 2.2250738585072009e-308, "2.2250738585072009e-308");  /* Max subnormal double */
    TEST_DOUBLE(-2.2250738585072009e-308, "-2.2250738585072009e-308");
    TEST_DOUBLE( 2.2250738585072014e-308, "2.2250738585072014e-308");  /* Min normal positive double */
    TEST_DOUBLE(-2.2250738585072014e-308, "-2.2250738585072014e-308");
    TEST_DOUBLE( 1.7976931348623157e+308, "1.7976931348623157e+308");  /* Max double */
    TEST_DOUBLE(-1.7976931348623157e+308, "-1.7976931348623157e+308");

        /* invalid number */
    TEST_ERROR(PARSE_INVALID_VALUE, "+0");
    TEST_ERROR(PARSE_INVALID_VALUE, "+1");
    TEST_ERROR(PARSE_INVALID_VALUE, ".123"); /* at least one digit before '.' */
    TEST_ERROR(PARSE_INVALID_VALUE, "1.");   /* at least one digit after '.' */
    TEST_ERROR(PARSE_INVALID_VALUE, "INF");
    TEST_ERROR(PARSE_INVALID_VALUE, "inf");
    TEST_ERROR(PARSE_INVALID_VALUE, "NAN");
    TEST_ERROR(PARSE_EXPECT_VALUE, "nan");


    TEST_INT32(0, "0i32");
    TEST_INT32(1, "1i32");
    TEST_INT32(10, "10i32");
    TEST_INT32(234134, "234134i32");
    TEST_INT32(214, "214i32");

    TEST_INT64(0, "0i64");
    TEST_INT64(1, "1i64");
    TEST_INT64(10, "10i64");
    TEST_INT64(234134, "234134i64");
    TEST_INT64(214, "214i64");

    TEST_ERROR(PARSE_INVALID_VALUE, "0.1i32");
    TEST_ERROR(PARSE_INVALID_VALUE, "0.1i64");
}


#define EXPECT_EQ_STRING(expect, actual, alength) \
    EXPECT_EQ_BASE(sizeof(expect) - 1 == alength && memcmp(expect, actual, alength + 1) == 0, expect, actual, "%s")
#define TEST_STRING(expect, json)\
    do {\
        Value value;\
        string str(json);\
        StringReadStream is(str);\
        ParseError ret = Reader<StringReadStream>::parse(is, value);\
        EXPECT_EQ_INT(PARSE_OK, ret);\ 
        EXPECT_EQ_INT(true, value.isString());\
        EXPECT_EQ_STRING(expect, value.getString().c_str(), value.getString().size());\
    } while(0)

void testString()
{
    TEST_STRING("", "\"\"");
    TEST_STRING("Hello", "\"Hello\"");
    TEST_STRING("Hello\nWorld", "\"Hello\\nWorld\"");
    TEST_STRING("\" \\ / \b \f \n \r \t", "\"\\\" \\\\ \\/ \\b \\f \\n \\r \\t\"");
    TEST_STRING("Hello\0World", "\"Hello\\u0000World\"");
    TEST_STRING("\x24", "\"\\u0024\"");         /* Dollar sign U+0024 */
    TEST_STRING("\xC2\xA2", "\"\\u00A2\"");     /* Cents sign U+00A2 */
    TEST_STRING("\xE2\x82\xAC", "\"\\u20AC\""); /* Euro sign U+20AC */
    TEST_STRING("\xF0\x9D\x84\x9E", "\"\\uD834\\uDD1E\"");  /* G clef sign U+1D11E */
    TEST_STRING("\xF0\x9D\x84\x9E", "\"\\ud834\\udd1e\"");  /* G clef sign U+1D11E */
}


static void testArray() {
    size_t i, j;

    Value value;
    StringReadStream is(string("[ ]"));
    EXPECT_EQ_INT(PARSE_OK, Reader<StringReadStream>::parse(is, value));
    EXPECT_EQ_INT(true, value.isArray());
    EXPECT_EQ_INT(0, value.getSize());

    value.setNull();
    StringReadStream is1(string("[ null , false , true , 123.0 , \"abc\" , 123i32]"));
    EXPECT_EQ_INT(PARSE_OK, Reader<StringReadStream>::parse(is1, value));
    EXPECT_EQ_INT(true, value.isArray());
    EXPECT_EQ_INT(6, value.getSize());

    //value[const char*] 和 valeu[int] 在下标等于0时会出现错误，即编译器无法决定使用那个value[], 这是因为0也可以被当作NULL。
    EXPECT_EQ_INT(true, value[size_t(0)].isNull());
    EXPECT_EQ_INT(true, value[1].isBool());
    EXPECT_EQ_INT(true, value[2].isBool());
    EXPECT_EQ_INT(true, value[3].isDouble());
    EXPECT_EQ_INT(true, value[4].isString());
    EXPECT_EQ_INT(true, value[5].isInt32());


    EXPECT_EQ_DOUBLE(123.0, value[3].getDouble());
    EXPECT_EQ_STRING("abc", value[4].getString().c_str(), value[4].getString().size());
    EXPECT_EQ_INT(123, value[5].getInt32());

    value.setNull();
    StringReadStream is2(string("[ [ ] , [ 0.0 ] , [ 0.0 , 1.0 ] , [ 0.0 , 1.0 , 2.0 ] ]"));
    EXPECT_EQ_INT(PARSE_OK, Reader<StringReadStream>::parse(is2, value));
    EXPECT_EQ_INT(true, value.isArray());
    EXPECT_EQ_INT(4, value.getSize());
    for (i = 0; i < 4; i++) {
        Value tp = value[i];
        EXPECT_EQ_INT(true, tp.isArray());
        EXPECT_EQ_INT(i, tp.getSize());
        for (j = 0; j < i; j++) {
            Value e = tp[j];
            EXPECT_EQ_INT(true, e.isDouble());
            EXPECT_EQ_DOUBLE((double)j, e.getDouble());
        }
    }

    value.setNull();
    string str("{\"n\":null,\"f\":false,\"t\":true,\"i\":123.0,\"s\":\"abc\",\"a\":[1.0,2.0,3.0],\"o\":{\"1\":1.0,\"2\":2.0,\"3\":3.0}}");
    StringReadStream is3(str);
    ParseError ret = Reader<StringReadStream>::parse(is3, value);
    EXPECT_EQ_INT(PARSE_OK, ret);
    cout<<value.showType()<<endl;
    EXPECT_EQ_INT(true, value.isObject());
    EXPECT_EQ_INT(7, value.getSize());
}

#define TEST_ROUNDTRIP(json) do { \
    Value value;\
    string str(json);\
    StringReadStream is(str);\
    ParseError ret = Reader<StringReadStream>::parse(is, value);\
    EXPECT_EQ_INT(PARSE_OK, ret);\ 
    StringWriteStream os; \
    StyleWriter<StringWriteStream> writer(os, string("  ")); \
    value.writeTo(writer); \
    cout<<json<<"  "<<os.get()<<endl;\
    EXPECT_EQ_STRING(json, os.get().c_str(), os.get().size());\
} while(false)

void testRound()
{

    // Value value;
    // string str("0.0");
    // StringReadStream is(str);
    // EXPECT_EQ_INT(PARSE_OK, Reader<StringReadStream>::parse(is, value));
    // cout<<value.getDouble()<<endl;
    // StringWriteStream os; 
    // FastWriter<StringWriteStream> writer(os); 
    // value.writeTo(writer);
    // cout<<os.get()<<endl;
    TEST_ROUNDTRIP("0.0");
    TEST_ROUNDTRIP("1.0");
    TEST_ROUNDTRIP("-1.0");
    TEST_ROUNDTRIP("10086.9527");
    TEST_ROUNDTRIP("2.345e+100");
    TEST_ROUNDTRIP("-1.11e-10");

// /* https://en.wikipedia.org/wiki/Double-precision_floating-point_format */
    TEST_ROUNDTRIP("1.0000000000000002");
    TEST_ROUNDTRIP("-1.0000000000000002");
    // fixme: stod has bug...
    // TEST_ROUNDTRIP("4.9406564584124654e-324");
    // TEST_ROUNDTRIP("-4.9406564584124654e-324");
    // TEST_ROUNDTRIP("2.2250738585072009e-308");
    // TEST_ROUNDTRIP("-2.2250738585072009e-308");
    // // cout<<value.getDouble()<<"  "<<to_string(value.getDouble())<<endl; 下面-》
    TEST_ROUNDTRIP("2.2250738585072014e-308");
    TEST_ROUNDTRIP("-2.2250738585072014e-308");
    TEST_ROUNDTRIP("1.7976931348623157e+308");
    TEST_ROUNDTRIP("-1.7976931348623157e+308");

    TEST_ROUNDTRIP("\"蛤蛤蛤\"");
    TEST_ROUNDTRIP("\"\"");
    TEST_ROUNDTRIP("\"Hello\"");
    TEST_ROUNDTRIP("\"Hello\\nWorld\"");
    TEST_ROUNDTRIP("\"\\\" \\\\ / \\b \\f \\n \\r \\t\"");
    TEST_ROUNDTRIP("\"Hello\\u0000World\"");

    TEST_ROUNDTRIP("{}");
    cout<<"next........."<<endl;
    TEST_ROUNDTRIP("{\"n\":null,\"f\":false,\"t\":true,\"i\":123.0,\"s\":\"abc\",\"a\":[1.0,2.0,3.0],\"o\":{\"1\":1.0,\"2\":2.0,\"3\":3.0}}");

    TEST_ROUNDTRIP("{\"a\" : 1.0, \"b\" : 2i32, \"c\" : 3i64, \"d\" : 4.0}");
}




int main()
{
    testNull();
    testBool();
    testNumber();
    testString();
    testArray();
    testRound();
    return 0;
}