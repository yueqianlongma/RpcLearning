

#ifndef __NETSERVER_INET_ADDRESS_H__
#define __NETSERVER_INET_ADDRESS_H__

#include<netinet/in.h>
#include<string>

namespace net
{

class InetAddress
{
public:
    explicit InetAddress(uint16_t port = 0, bool loopback = false);
    InetAddress(const std::string& ip,  uint16_t port);

    void setAddress(const sockaddr_in& addr)
    {   addr_ = addr; }

    const sockaddr* getSockaddr()   const
    {   return reinterpret_cast<const sockaddr*>(&addr_);   }

    socklen_t getSocklen()    const
    {   return sizeof(addr_);   }

    std::string toIp()      const;
    uint16_t    toPort()    const;
    std::string toIpPort()  const;

private:
    sockaddr_in     addr_;
};


}

#endif