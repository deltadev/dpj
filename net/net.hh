#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <unistd.h>

#include <string>
#include <vector>
namespace dpj { namespace net
{  
  inline
  std::string ntop(int domain, sockaddr* addr)
  {
    std::vector<char> p;
    switch (domain)
    {
    case AF_INET:
      p.resize(INET_ADDRSTRLEN);
      inet_ntop(domain, &((sockaddr_in*)addr)->sin_addr, p.data(), (socklen_t)p.size());
      break;
    case AF_INET6:
      p.resize(INET6_ADDRSTRLEN);
      inet_ntop(domain, &((sockaddr_in6*)addr)->sin6_addr, p.data(), (socklen_t)p.size());
      break;
    default:
      throw std::runtime_error("ntop: could not convert address.");
    }
    return p.data();
  }

  inline
  std::string ntop(int domain, sockaddr_storage* addr)
  {
    return ntop(domain, (sockaddr*)addr);
  }

  
//  struct unix_socket_t
//  {
//    int fd;
//    std::string path;
//    unix_socket_t(std::string path) : path{path}
//    { 
//      fd = socket(AF_UNIX, SOCK_STREAM, 0);
//    }
//    ~unix_socket_t()
//    {
//      close(fd);
//      int r = unlink(path.c_str());
//      if (r == -1)
//        std::cerr << "WARNING: unlink failed in ~unix_socket_t\n";
//    }
//  };
}
}
