
#include"Acceptor.h"
#include"EventLoop.h"
#include"Log.h"
#include<unistd.h>
#include<assert.h>
#include <fcntl.h>
using namespace net;

namespace
{

int createSocket()
{
    int ret = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
    if(ret == -1)
        fatal("Acceptor::socket()");
    return ret;
}

}


Acceptor::Acceptor(EventLoop* loop, const InetAddress& local)
            :   listening_(false),
                loop_(loop),
                acceptFd_(createSocket()),
                acceptChannel_(loop_, acceptFd_),
                local_(local),
                idleFd_(::open("/dev/null", O_RDONLY | O_CLOEXEC))
{
    int on = 1;
    int ret = ::setsockopt(acceptFd_, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    if(ret == -1)
        fatal("Acceptor::setsockopt() SO_REUSEADDR");

    ret = ::setsockopt(acceptFd_, SOL_SOCKET, SO_REUSEPORT, &on, sizeof(on));
    if(ret == -1)
        fatal("Acceptor::setsockopt() SO_REUSEPORT");
    
    ret = ::bind(acceptFd_, local_.getSockaddr(), local_.getSocklen());
    if(ret == -1)
        fatal("Acceptor::bind() %s", local.toIpPort().c_str());

    acceptChannel_.setReadCallback([this](){
        handleRead();
    });
}

Acceptor::~Acceptor()
{
    ::close(acceptFd_);
    ::close(idleFd_);
}

void Acceptor::listen()
{
    loop_->assertInLoopThread();
    int ret = ::listen(acceptFd_, SOMAXCONN);
    if(ret == -1)
        fatal("Acceptor::listen()");
    
    listening_ = true;
    acceptChannel_.enableRead();
}

void Acceptor::handleRead()
{
    loop_->assertInLoopThread();

    sockaddr_in addr;
    socklen_t len = sizeof(addr);

    void* any = &addr;
    int sockfd = ::accept4(acceptFd_, static_cast<sockaddr*>(any), &len, 
                                    SOCK_NONBLOCK | SOCK_CLOEXEC);
    // if(sockfd == -1){
    //     int saveErrno = errno;
    //     error("Acceptor::accept4()");
    //     switch(saveErrno){                          //notice
    //         case ECONNABORTED:
    //         case EMFILE:
    //             break;
    //         default:
    //             fatal("unexcepted accept4() error");
    //     }
    // }
    if(sockfd >= 0){
        if(newConnectionCallback_){
            InetAddress peer;
            peer.setAddress(addr);
            newConnectionCallback_(sockfd, local_, peer);
        }
        else ::close(sockfd);
    }
    else {
        error("in Acceptor::handleRead");
        //优雅关闭
        if(errno == EMFILE){
            ::close(idleFd_);
            idleFd_ = ::accept(acceptFd_, NULL, NULL);
            ::close(idleFd_);
            idleFd_ = ::open("/dev/null", O_RDONLY | O_CLOEXEC);
        }
    }

}
