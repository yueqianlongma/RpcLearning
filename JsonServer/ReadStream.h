#ifndef __JSONLEARN_READSTREAM_H__
#define __JSONLEARN_READSTREAM_H__

#include"noncopyable.h"
#include<fstream>
#include<memory>
#include<vector>
#include<cstring>
#include<cassert>
#include<iostream>

namespace json
{


// class ReadStream : public noncopyable
// {
// public:
//     virtual bool hasNext() = 0;
//     virtual char front() = 0; 
//     virtual void pop() = 0;
//     virtual void expect(char) = 0;
//     virtual char next() = 0;
// };



class FileReadStream
{
public:
    using Iterator = std::vector<char>::iterator;

public:
    FileReadStream(const std::string& fileName)
        : m_is(new std::ifstream())
    {
        m_is->open(fileName, std::fstream::in);
        readStream();
    }

    bool hasNext()  
    {
        return it != buffer_.end();
    }

    char front() 
    {
        return hasNext() ? *it : '\0';
    }

    void pop() 
    {
        ++it;
    }

    char next() 
    {
        if (hasNext()) {
            char ch = *it;
            it++;
            return ch;
        }
        return '\0';
    };

    bool expect(char ch) 
    {
        assert(front() == ch);
        pop();
    }

    const Iterator getIter() const
    { return it; }

private:
    void readStream()
    {
        char buf[65536];
        bzero(buf, 1024);
        while (!m_is->eof())
        {
            m_is->read(buf, sizeof(buf));
            buffer_.insert(buffer_.end(), buf, buf + strlen(buf));
        }
        it = buffer_.begin();
    }
    
private:
    std::vector<char> buffer_;  
    Iterator it;  
    std::unique_ptr<std::ifstream> m_is;
};




class StringReadStream 
{
public:
    using Iterator = std::string::iterator;

public:
    
    StringReadStream(const std::string& json)
        :   json_(json),
            it(json_.begin())
    {}

    bool hasNext() 
    {
        return it != json_.end();
    }

    char front()  
    {
        return hasNext() ? *it : '\0';
    }

    void pop()  
    {
        ++it;
    }

    char next()
    {
        if (hasNext()) {
            char ch = *it;
            it++;
            return ch;
        }
        return '\0';
    }

    bool expect(char ch)  
    {
        assert(front() == ch);
        pop();
    }

    Iterator getIter() const
    {
        return it;
    }

private:
    std::string json_;
    Iterator it;
};

}



#endif
