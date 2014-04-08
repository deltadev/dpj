#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <string>
#include <vector>
namespace net
{

  inline
  std::string ntop(int family, sockaddr* addr)
  {
    std::vector<char> p;
    switch (family)
    {
    case AF_INET:
      p.resize(INET_ADDRSTRLEN);
      inet_ntop(family, &((sockaddr_in*)addr)->sin_addr, p.data(), (socklen_t)p.size());
      break;
    case AF_INET6:
      p.resize(INET6_ADDRSTRLEN);
      inet_ntop(family, &((sockaddr_in6*)addr)->sin6_addr, p.data(), (socklen_t)p.size());
      break;
    default:
      return "Problem converting to network address.";
    }
    return p.data();
  }

  std::string ntop(int family, sockaddr_storage* addr)
  {
    return ntop(family, (sockaddr*)addr);
  }
}
