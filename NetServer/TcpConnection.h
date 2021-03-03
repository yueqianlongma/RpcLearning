
#ifndef __TCP_CONNECTION_H__
#define __TCP_CONNECTION_H__

#include"noncopyable.h"
#include"Buffer.h"
#include"Callbacks.h"
#include"InetAddress.h"
#include"Channel.h"
#include<functional>

namespace net
{

class TcpConnection: public noncopyable,
                        public std::enable_shared_from_this<TcpConnection>
{
public:
    TcpConnection(EventLoop* loop, int sockfd,
                  const InetAddress& local,
                  const InetAddress& peer);
    ~TcpConnection();

    void setConnectionCallback(const ConnectionCallback& cb)
    { connectionCallback_ = cb; }
    void setMessageCallback(const MessageCallback& cb)
    { messageCallback_ = cb; }
    void setWriteCompleteCallback(const WriteCompleteCallback& cb)
    { writeCompleteCallback_ = cb; }
    void setHighWaterMarkCallback(const HighWaterMarkCallback& cb, size_t mark)
    { highWaterMarkCallback_ = cb; highWaterMark_ = mark; }
    void setCloseCallBack(const CloseCallback& cb)
    { closeCallback_ = cb; }

    // TcpServerSingle create connection
    void connectEstablished();

    bool connected() const;
    bool disconnected() const;

    const InetAddress& local() const
    { return local_; }
    const InetAddress& peer() const
    { return peer_; }

    std::string name() const
    { return local_.toIpPort() + " -> " + peer_.toIpPort(); }

    // I/O operations are thread safe
    void send(std::string data);
    void send(const char* data, size_t len);
    void send(Buffer& buffer);
    void shutdown();
    void forceClose();

    void stopRead();
    void startRead();
    bool isReading() // not thread safe
    { return channel_.isReading(); };

    const Buffer& inputBuffer() const { return inputBuffer_; }
    const Buffer& outputBuffer() const { return outputBuffer_; }

    void connectDestroyed(); 

private:
    void handleRead();
    void handleWrite();
    void handleClose();
    void handleError();

    void sendInLoop(const char* data, size_t len);
    void sendInLoop(const std::string& message);
    void shutdownInLoop();
    void forceCloseInLoop();

    int stateAtomicGetAndSet(int newState);

private:
    EventLoop* loop_;
    const int sockfd_;
    Channel channel_;
    int state_;
    InetAddress local_;
    InetAddress peer_;
    Buffer inputBuffer_;
    Buffer outputBuffer_;
    size_t highWaterMark_;

    ConnectionCallback      connectionCallback_;
    MessageCallback         messageCallback_;
    WriteCompleteCallback   writeCompleteCallback_;
    HighWaterMarkCallback   highWaterMarkCallback_;
    CloseCallback           closeCallback_;

};

}

#endif