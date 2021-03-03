
#include"Log.h"
#include"EventLoop.h"
#include"TcpConnection.h"
#include"TcpClient.h"
#include"Timer.h"
#include"Callbacks.h"
using namespace net;



TcpClient::TcpClient(EventLoop* loop, const InetAddress& peer)
    :   loop_(loop),
        connected_(false),
        peer_(peer),
        retryTimer_(nullptr),
        connector_(new Connector(loop, peer)),
        connectionCallback_(defaultConnectionCallback),
        messageCallback_(defaultMessageCallback),
        countDown_(1)
{
    connector_->setNewConnectionCallback(std::bind(&TcpClient::newConnection, this, _1, _2, _3));
}

TcpClient::~TcpClient()
{
    if(connection_ && !connection_->disconnected())
        connection_->forceClose();
    if(retryTimer_ != nullptr)
        loop_->cancelTimer(retryTimer_);
}

void TcpClient::start()
{
    // loop_->assertInLoopThread();
    loop_->runInLoop([this](){
        connector_->start();
    });
    // connector_->start();
    retryTimer_ = loop_->runEvery(Second(3), [this]{ retry(); });
}

TcpConnectionPtr TcpClient::getTcpConnection()
{
    countDown_.wait();
    return connection_;
}

void TcpClient::retry()
{
    loop_->assertInLoopThread();
    if(connected_)
        return;
    warn("TcpClient()::retry() reconnect %s...", peer_.toIpPort().c_str());
    connector_.reset(new Connector(loop_, peer_));
    connector_->setNewConnectionCallback(std::bind(&TcpClient::newConnection, this, _1, _2, _3));
    connector_->start();
}

void TcpClient::newConnection(int connfd, const InetAddress& local, const InetAddress& peer)
{
    loop_->assertInLoopThread();
    loop_->cancelTimer(retryTimer_);
    retryTimer_ = nullptr;
    connected_ = true;

    connection_ = std::make_shared<TcpConnection>(loop_, connfd, local, peer);
    connection_->setMessageCallback(messageCallback_);
    connection_->setWriteCompleteCallback(writeCompleteCallback_);
    connection_->setCloseCallBack(std::bind(&TcpClient::closeConnection, this, _1));

    countDown_.countDown();
    connection_->setConnectionCallback(connectionCallback_);
    connection_->connectEstablished();
}

void TcpClient::closeConnection(const TcpConnectionPtr& conn)
{
    loop_->assertInLoopThread();
    assert(connection_ != nullptr);

    connection_.reset();
    loop_->queueInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
    // connectionCallback_(conn);
}

