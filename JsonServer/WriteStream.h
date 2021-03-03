#ifndef __JSONLEARN_WRITESTREAM_H__
#define __JSONLEARN_WRITESTREAM_H__

#include"noncopyable.h"
#include<sstream>
#include<fstream>
#include<memory>

namespace json
{

// class WriteStream : public noncopyable
// {
// public:
//     virtual void put(char c) = 0;
//     virtual void put(std::string&) = 0;
//     virtual std::string get() const = 0;
// };



class FileWriteStream : public noncopyable
{
public:
    FileWriteStream(const std::string& fileName)
        : m_os(new std::ofstream())
    {
        m_os->open(fileName, std::ofstream::out | std::ofstream::trunc);
    }

    void put(char c) 
    {
        (*m_os)<<c;
    }

    void put(const std::string& str) 
    {
        (*m_os)<<str;
    }
    

private:
    std::unique_ptr<std::ofstream> m_os;
};



class StringWriteStream : public noncopyable
{
public:
    
    void put(char c) 
    { 
        ss<<c;
    }

    void put(const std::string& str) 
    {
        ss<<str;
    }

    std::string get() const 
    {
        return ss.str();
    }

private:
    std::stringstream ss;
};



}

#endif