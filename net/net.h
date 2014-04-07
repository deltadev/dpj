#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <string>

namespace net {
 /// gets a string network address from a sockaddr.
  std::string ntop(int family, sockaddr* addr);
  std::string ntop(int family, sockaddr_storage* addr);
}
