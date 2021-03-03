#ifndef __LOG_SERVER__
#define __LOG_SERVER__

#include "log_line.hpp"
#include<functional>
#include<memory>
#include<ostream>
#include<fstream>
#include<thread>
#include<mutex>
#include<condition_variable>
#include<queue>

namespace logserver
{
    class LogServer
    {
    public:
        LogServer() = default;
        
        void logIn(LogLine& arg)
        {
            logOut(arg);
        }

        void setLogOut(std::function<void(LogLine&)> arg)
        {
            logOut = arg;
        }

        ~LogServer() = default;
    private:
        std::function<void(LogLine&)> logOut;
    };


    class FileWriter
    {
    public:
        FileWriter(const std::string& log_directory, 
                            const std::string& log_file_name, 
                                uint32_t log_file_roll_size_mb)
            : m_log_file_roll_size_bytes(log_file_roll_size_mb * 1024 * 1024)
            , m_file_name(log_directory + log_file_name)
        {
            roll_file();
        }

        void write(LogLine& arg)
        {
            auto pos = m_os->tellp();
            arg.stringify(*m_os);
            m_bytes_written += m_os->tellp() - pos;
            
            if(m_bytes_written >= m_log_file_roll_size_bytes)
                roll_file();
        }
            

    private:
        void roll_file()
        {
            if(m_os)
            {
                m_os->flush();
                m_os->close();
            }
            m_bytes_written = 0;
            m_os.reset(new std::ofstream());

            std::string log_file_name = m_file_name;
            log_file_name.append(".").append(std::to_string(++m_file_number)).append(".txt");
            m_os->open(log_file_name, std::ofstream::out | std::ofstream::trunc);
        }

    private:
        uint32_t m_file_number = {0};
        uint32_t m_bytes_written = {0};
        const uint32_t m_log_file_roll_size_bytes;
        const std::string m_file_name;
        std::unique_ptr<std::ofstream> m_os;
    };

    class AsyncWriter
    {
    public:
        AsyncWriter(const std::string& log_directory, 
                        const std::string& log_file_name, 
                            uint32_t log_file_roll_size_mb)
        {
            log_.reset(new FileWriter(log_directory, log_file_name, log_file_roll_size_mb));
            thread_.reset(new std::thread(&AsyncWriter::writer, this));
        }

        void push(LogLine& arg)
        {
            std::lock_guard<std::mutex> lck(mutex_);
            buffer_.emplace(std::move(arg));
            if(buffer_.size() == 1)
                cv_.notify_one();
        }

        void writer()
        {
            LogLine line;
            while(!state_)
            {
                {
                    std::unique_lock<std::mutex> lck(mutex_);
                    cv_.wait(lck, [=](){
                        return !(this->buffer_.empty() && !this->state_);
                    });
                    if(state_)  return;
                    line = std::move(buffer_.front());
                    buffer_.pop();
                }
                log_->write(line);
            }
            while(!buffer_.empty())
            {
                log_->write(buffer_.front());
                buffer_.pop();
            }
        }

        ~AsyncWriter()
        {
            state_ = true;
            cv_.notify_one();
            thread_->join();
        }
        
    private:
        bool state_ = {false};
        std::queue<LogLine> buffer_;
        std::mutex mutex_;
        std::condition_variable cv_;
        std::unique_ptr<std::thread> thread_;
        std::unique_ptr<FileWriter> log_;
    };
    

    std::unique_ptr<LogServer> logserver_ptr;
    std::unique_ptr<FileWriter> filewriter_ptr;
    std::unique_ptr<AsyncWriter> asyncwriter_ptr;
    void LogTerminalInitSync()
    {
        logserver_ptr.reset(new LogServer());
        logserver_ptr->setLogOut([](LogLine& arg){
            arg.stringify(std::cout);
        });
    }

    void LogFileInitSync(const std::string& directory, const std::string& file, uint32_t roll_size_mb)
    {
        logserver_ptr.reset(new LogServer());
        filewriter_ptr.reset(new FileWriter(directory, file, roll_size_mb));
        logserver_ptr->setLogOut([](LogLine& arg){
            if(filewriter_ptr)
                filewriter_ptr->write(arg);
        });
    }

    void LogFileInitAsync(const std::string& directory, const std::string& file, uint32_t roll_size_mb)
    {
        logserver_ptr.reset(new LogServer());
        asyncwriter_ptr.reset(new AsyncWriter(directory, file, roll_size_mb));
        logserver_ptr->setLogOut([](LogLine& arg){
            if(asyncwriter_ptr)
                asyncwriter_ptr->push(arg);
        });
        
    }

    struct LogWrapper
    {
        void operator=(LogLine &arg)
        {
            if(logserver_ptr)
                logserver_ptr->logIn(arg);
        }
    };

}


#define LOG(LEVEL) logserver::LogWrapper() = logserver::LogLine(LEVEL, __FILE__, __func__, __LINE__)
#define LOG_TRACE  LOG(logserver::LogLevel::TRACE)

#define LOG_DEBUG  LOG(logserver::LogLevel::DEBUG)

#define LOG_INFO  LOG(logserver::LogLevel::INFO)

#define LOG_WARN  LOG(logserver::LogLevel::WARN)

#define LOG_ERROR  LOG(logserver::LogLevel::ERROR)

#define LOG_FATAL  LOG(logserver::LogLevel::FATAL)


#endif