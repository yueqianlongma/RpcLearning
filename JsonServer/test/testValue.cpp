#include "../Value.h"
#include<iostream>
#include<string>
using namespace json;
using namespace std;

void testNull()
{
    Value value;
    cout<<"isNull: "<<value.isNull()<<endl;
}

void testInt32()
{
    int32_t cur = 100;
    Value value(cur);
    cout<<"isInt32: "<<value.isInt32()<<endl;
    cout<<"int32: "<<value.getInt32()<<endl;
}

void testInt64()
{
    int64_t cur = 100;
    Value value(cur);
    cout<<"isInt64: "<<value.isInt64()<<endl;
    cout<<"int64: "<<value.getInt64()<<endl;
}

void testDouble()
{
    double cur = 3.1415926;
    Value value(cur);
    cout<<"isDouble: "<<value.isDouble()<<endl;
    cout<<"double: "<<value.getDouble()<<endl;
}

void testString()
{
    string str("test string");
    Value value(str);
    cout<<"isStr: "<<value.isString()<<endl;
    cout<<"str: "<<value.getString()<<endl;

    Value v2("test string!");
    cout<<"isStr: "<<value.isString()<<endl;
    cout<<"str: "<<value.getString()<<endl;
}

void testArray()
{
    Value array(ValueType::TYPE_ARRAY);
    cout<<"isArray: "<<array.isArray()<<endl;

    string str("test array");
    Value value(str);
    array.addValue(value);

    array.addValue(str);
    array.addValue("test array");

    cout<<"array size: "<<array.getSize()<<endl;
    for(auto& tp : array.getArray())
        cout<<tp.getString()<<" ";
    cout<<endl;
}

void testObject()
{
    Value object(ValueType::TYPE_OBJECT);
    cout<<"isObject: "<<object.isObject()<<endl;

    object.addMember(string("object1"), Value("test object"));

    string str("object2");
    object.addMember(Value(str), "test_object");

    object.addMember(Value("object3"), Value("test_object"));

    cout<<"object size: "<<object.getSize()<<endl;

    for(auto& tp : object.getObject())
        cout<<tp.first.getString()<<" "<<tp.second.getString()<<endl;

    cout<<"find : "<<object.hasMember(Value("object1"))<<endl;
}

int main(int argc, char** argv)
{
    testNull();
    testInt32();
    testInt64();
    testDouble();
    testArray();
    testObject();

    return 0;
}