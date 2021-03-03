#ifndef __LOG_LINE__
#define __LOG_LINE__

#include<memory>
#include<string>
#include<iostream>

namespace logserver
{
    //?，解释这些level
    enum class LogLevel : uint8_t
    {
        TRACE,
        DEBUG,
        INFO,
        WARN,
        ERROR,
        FATAL
    };
    
    class LogLine
    {
    public:
        LogLine() = default;    //notice;
        LogLine(LogLevel level, const char* file, const char* function, uint32_t line);

        void stringify(std::ostream& os);

        LogLine(LogLine&&) = default;
        LogLine& operator=(LogLine&&) = default;

        LogLine& operator<<(char);
        LogLine& operator<<(uint32_t);
        LogLine& operator<<(int32_t);
        LogLine& operator<<(uint64_t);
        LogLine& operator<<(int64_t);
        LogLine& operator<<(double);
        LogLine& operator<<(std::string&);
        LogLine& operator<<(const char*);

        template <size_t N>
        LogLine& operator<<(const char (&arg) [N])
        {
            // encode_c_str(arg, N);
            encode(stringWrapper(arg));
        }

        struct stringWrapper
        {
            explicit stringWrapper(const char* arg)
                : pstr(arg) {}
            const char* pstr;
        };

        void showTest()
        {
            stringify(std::cout);
        }

    private:
        char* buffer();
        void resize_if_need(size_t);

        template <typename Arg>
        void encode(Arg);
        template <typename Arg>
        void encode(Arg, uint8_t);
        void encode(stringWrapper);
        void encode(const char*);
        void encode_c_str(const char*, size_t);

        void stringify(std::ostream& os, char* start, char* end);

    private:
        size_t m_buffer_used;
        size_t m_buffer_size;
        std::unique_ptr<char[]> m_heap_buffer;
        char m_stack_buffer[256];
    };

    template <typename Arg>
    void LogLine::encode(Arg arg)
    {
        *reinterpret_cast<Arg*>(buffer()) = arg;
        m_buffer_used += sizeof(Arg);
    }

    template <typename Arg>
    void LogLine::encode(Arg arg, uint8_t type_id)
    {
        size_t need_size = sizeof(Arg) + sizeof(uint8_t);
        resize_if_need(need_size);
        encode<uint8_t>(type_id);
        encode<Arg>(arg);
    }
}

#endif