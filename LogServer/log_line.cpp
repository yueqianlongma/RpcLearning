#include"log_line.hpp"
#include<cstring>
#include<chrono>
#include<thread>

namespace
{

    uint64_t timestamp_now()
    {
        using namespace std::chrono;
        system_clock::time_point tp = system_clock::now();
        return duration_cast<microseconds>(tp.time_since_epoch()).count();
    }
     /* I want [2016-10-13 00:01:23.528514] */
    void format_timestamp(std::ostream & os, uint64_t timestamp)
    {
		std::time_t time_t = timestamp / 1000000;
		auto gmtime = std::gmtime(&time_t);
		char buffer[32];
		strftime(buffer, 32, "%Y-%m-%d %T.", gmtime);
		char microseconds[7];
		sprintf(microseconds, "%06llu", timestamp % 1000000);
		os << '[' << buffer << microseconds << ']';
    }

    std::thread::id this_thread_id()
    {
        static thread_local const std::thread::id id = std::this_thread::get_id();
        return id;
    }    

    template < typename T, typename Tuple >
    struct TupleIndex;

    template < typename T,typename ... Types >
    struct TupleIndex < T, std::tuple < T, Types... > > 
    {
		static const std::size_t value = 0;
    };

    template < typename T, typename U, typename ... Types >
    struct TupleIndex < T, std::tuple < U, Types... > > 
    {
		static const std::size_t value = 1 + TupleIndex < T, std::tuple < Types... > >::value;
    };
}

namespace logserver
{
    typedef std::tuple < char, uint32_t, uint64_t, int32_t, int64_t, double, LogLine::stringWrapper, char* > SupportedTypes;

    const char* to_string(LogLevel level)
    {
        switch (level)
        {
        case LogLevel::TRACE:
			return "TRACE";
		case LogLevel::DEBUG:
			return "DEBUG";
		case LogLevel::INFO:
			return "INFO";
        case LogLevel::WARN:
			return "WARN";
        case LogLevel::ERROR:
			return "ERROR";
        case LogLevel::FATAL:
			return "FATAL";
		}
		return "XXXX";
    }

    LogLine::LogLine(LogLevel level, const char* file, const char* function, uint32_t line)
        : m_buffer_used(0), m_buffer_size(256)
    {
        encode<uint64_t>(timestamp_now());
        encode<std::thread::id>(this_thread_id());
        encode<stringWrapper>(stringWrapper(file));
        encode<stringWrapper>(stringWrapper(function));
        encode<uint32_t>(line);
        encode<LogLevel>(level);
    }

    void LogLine::stringify(std::ostream& os)
    {
        char* ptr = !m_heap_buffer ? m_stack_buffer : m_heap_buffer.get();
        char* end = ptr + m_buffer_used;

        uint64_t timestamp = *reinterpret_cast<uint64_t*>(ptr); ptr += sizeof(uint64_t);
        std::thread::id threadid = *reinterpret_cast<std::thread::id*>(ptr); ptr += sizeof(std::thread::id);
        stringWrapper file = *reinterpret_cast<stringWrapper*>(ptr); ptr += sizeof(stringWrapper);
        stringWrapper function = *reinterpret_cast<stringWrapper*>(ptr); ptr += sizeof(stringWrapper);
        uint32_t line = *reinterpret_cast<uint32_t*>(ptr); ptr += sizeof(uint32_t);
        LogLevel level = *reinterpret_cast<LogLevel*>(ptr); ptr += sizeof(LogLevel);

        format_timestamp(os, timestamp);
        os<<'['<<to_string(level)<<']';
        os<<'['<<threadid<<']';
        os<<'['<<file.pstr<<':'<<function.pstr<<':'<<line<<']';

        stringify(os, ptr, end);

        os << std::endl;
    }

    template <typename Arg>
    char* decode(std::ostream& os, char* begin, Arg* nummary)
    {
        Arg arg = *reinterpret_cast<Arg*>(begin); begin += sizeof(Arg);
        os<<arg;
        return begin;
    }

    char* decode(std::ostream& os, char* begin, LogLine::stringWrapper* nummary)
    {
        LogLine::stringWrapper str = *reinterpret_cast<LogLine::stringWrapper*>(begin); begin += sizeof(LogLine::stringWrapper);
        os<<str.pstr;
        return begin;
    }

    char* decode(std::ostream& os, char* begin, char** nummary)
    {
        for(; *begin != '\0'; ++begin)
            os<<*begin;
        return ++begin;
    }

    void LogLine::stringify(std::ostream& os, char* begin, char* end)
    {
        while(begin != end)
        {
            int type_id = *reinterpret_cast<uint8_t*>(begin); begin += sizeof(uint8_t);
            switch (type_id)
            {
            case 0:
                begin = decode(os, begin, static_cast<std::tuple_element<0, SupportedTypes>::type*>(nullptr));
                break;
            case 1:
                begin = decode(os, begin, static_cast<std::tuple_element<1, SupportedTypes>::type*>(nullptr));
                break;
            case 2:
                begin = decode(os, begin, static_cast<std::tuple_element<2, SupportedTypes>::type*>(nullptr));
                break;
            case 3:
                begin = decode(os, begin, static_cast<std::tuple_element<3, SupportedTypes>::type*>(nullptr));
                break;
            case 4:
                begin = decode(os, begin, static_cast<std::tuple_element<4, SupportedTypes>::type*>(nullptr));
                break;
            case 5:
                begin = decode(os, begin, static_cast<std::tuple_element<5, SupportedTypes>::type*>(nullptr));
                break;
            case 6:
                begin = decode(os, begin, static_cast<std::tuple_element<6, SupportedTypes>::type*>(nullptr));
                break;
            case 7:
                begin = decode(os, begin, static_cast<std::tuple_element<7, SupportedTypes>::type*>(nullptr));
                break;
            }
        }
    }

    LogLine& LogLine::operator<<(char arg)
    {
        encode<char>(arg, TupleIndex<char, SupportedTypes>::value);
        return *this;
    }

    LogLine& LogLine::operator<<(uint32_t arg)
    {
        encode<uint32_t>(arg, TupleIndex<uint32_t, SupportedTypes>::value);
        return *this;
    }

    LogLine& LogLine::operator<<(int32_t arg)
    {
        encode<int32_t>(arg, TupleIndex<int32_t, SupportedTypes>::value);
        return *this;
    }

    LogLine& LogLine::operator<<(uint64_t arg)
    {
        encode<uint64_t>(arg, TupleIndex<uint64_t, SupportedTypes>::value);
        return *this;
    }

    LogLine& LogLine::operator<<(int64_t arg)
    {
        encode<int64_t>(arg, TupleIndex<int64_t, SupportedTypes>::value);
        return *this;
    }

    LogLine& LogLine::operator<<(double arg)
    {
        encode<double>(arg, TupleIndex<double, SupportedTypes>::value);
        return *this;
    }

    LogLine& LogLine::operator<<(std::string& arg)
    {
        encode_c_str(arg.c_str(), arg.length());
        return *this;
    }

    LogLine& LogLine::operator<<(const char* arg)
    {
        encode(arg);
        return *this;
    }


    char* LogLine::buffer() 
    {
        return m_heap_buffer ? &m_heap_buffer.get()[m_buffer_used] : &m_stack_buffer[m_buffer_used];
    }

    void LogLine::resize_if_need(size_t need_size)
    {
        size_t required_size = m_buffer_used + need_size;
        if(required_size <= m_buffer_size)
            return;
        if(m_heap_buffer)
        {
            m_buffer_size = std::max(static_cast<size_t>(2 * m_buffer_size), required_size);
            std::unique_ptr<char[]> new_heap_buffer(new char[m_buffer_size]);
            memcpy(new_heap_buffer.get(), m_heap_buffer.get(), m_buffer_used);
            m_heap_buffer.swap(new_heap_buffer);
        }
        else
        {
            m_buffer_size = std::max(static_cast<size_t>(512), required_size);
            m_heap_buffer.reset(new char[m_buffer_size]);
            memcpy(m_heap_buffer.get(), m_stack_buffer, m_buffer_used);
        }
    }

    void LogLine::encode(stringWrapper arg)
    {
		encode<stringWrapper>(arg, TupleIndex <stringWrapper, SupportedTypes >::value);
    }

    void LogLine::encode(const char* arg)
    {
        if(arg)
            encode_c_str(arg, strlen(arg));
    }

    void LogLine::encode_c_str(const char* str, size_t length)
    {
        if(length == 0) return;
        resize_if_need(1 + length + 1);
        char* ptr = buffer();
        auto type_id = TupleIndex<char*, SupportedTypes>::value;
        *reinterpret_cast<uint8_t*>(ptr++) = type_id;
        memcpy(ptr, str, length + 1); //将\0也复制进去
        m_buffer_used += 1 + length + 1;
    }


}