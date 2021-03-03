

#include<RpcServer/Stub/StubGen.h>
#include<RpcServer/Stub/ClientStubGen.h>
#include<RpcServer/common.h>
#include<string>
#include<RpcServer/Exception.h>
#include<iostream>
#include<fstream>
using namespace std;
using namespace json;
using namespace rpc;

int main()
{
    string file("/home/mjl/RpcLearning/RpcServer/testStub/Echo.json");
    cout<<file<<endl;
    Value json;
    FileReadStream fs(file);
    auto err = Reader<FileReadStream>::parse(fs, json);
    assert(err == PARSE_OK);

    // StringWriteStream ws;
    // StyleWriter<StringWriteStream> styleWriter(ws, "  ");
    // json.writeTo(styleWriter);
    // cout<<ws.get()<<endl;

    try{
        ClientStubGen clientGen(json);
        string clientFile = clientGen.genClient();
        cout<<clientFile<<endl;
    }catch(StubException& e){
        cout<<e.what()<<endl;
    }


    return 0;
}
