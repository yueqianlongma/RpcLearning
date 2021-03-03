
#include"TcpServer.h"
#include"Log.h"
#include"EventLoop.h"
#include"EventLoopThread.h"
#include"EventLoopThreadPool.h"
#include"Acceptor.h"
#include"TcpConnection.h"
using namespace net;


TcpServer::TcpServer(EventLoop* loop, const InetAddress& local)
    :   baseLoop_(loop),
        local_(local),
        acceptor_(new Acceptor(loop, local)),
        threadPool_(new EventLoopThreadPool(loop)),
        started_(false),
        threadInitCallback_(defaultThreadInitCallback),
        connectionCallback_(defaultConnectionCallback),
        messageCallback_(defaultMessageCallback)
{
    trace("create TcpServer() %s", local.toIpPort().c_str());
    acceptor_->setNewConnectionCallback(std::bind(&TcpServer::newConnection, this, _1, _2, _3));
}

TcpServer::~TcpServer()
{
    trace("destroyed TcpServer() %s", local_.toIpPort().c_str());
}

void TcpServer::setNumThread(size_t threadNums)
{
    assert(threadNums >= 0);
    threadPool_->setThreadNums(threadNums);
}

void TcpServer::start()
{
    baseLoop_->assertInLoopThread();
    assert(!started_);

    threadPool_->start();

    assert(!acceptor_->listening());
    baseLoop_->runInLoop(std::bind(&Acceptor::listen, acceptor_.get()));  

    started_ = true;  
}



void TcpServer::newConnection(int connfd, const InetAddress& local, const InetAddress& peer)
{
    trace("TcpServer::newConnection()");
    baseLoop_->assertInLoopThread();
    EventLoop* ioLoop = threadPool_->getNextLoop();
    auto conn = std::make_shared<TcpConnection>(ioLoop, connfd, local, peer);
    connections_.insert(conn);
    conn->setConnectionCallback(connectionCallback_);
    conn->setMessageCallback(messageCallback_);
    conn->setWriteCompleteCallback(writeCompleteCallback_);
    conn->setCloseCallBack(std::bind(&TcpServer::closeConnection, this, _1));
    // enable and tie channel, create net connection
    
    ioLoop->runInLoop(std::bind(&TcpConnection::connectEstablished, conn));
}

void TcpServer::closeConnection(const TcpConnectionPtr& conn)
{
    baseLoop_->runInLoop(std::bind(&TcpServer::closeConnectionInLoop, this, conn));    
}

void TcpServer::closeConnectionInLoop(const TcpConnectionPtr& conn)
{
    baseLoop_->assertInLoopThread();
    assert(!connections_.empty());
    connections_.erase(conn);
    baseLoop_->queueInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
}