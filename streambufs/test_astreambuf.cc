#include <iostream>

#include <vector>

#include "astreambuf.hh"


int main(int argc, char *argv[])
{

  std::string s0{"2 3 4 5 6"};
  std::string s1{" 7 8 9 10 11"};
  float a, b;
  std::vector<char> buf{begin(s0), end(s0)};
  dpj::astreambuf sb{buf.data(), buf.data() + buf.size()};
  std::istream is{&sb};
  while (is >> a >> b)
  {
    sb.mark_unget();
    std::cout << a << ' ' << b << '\n';
  }
  is.clear();
  
  auto n = sb.fill(s1.data(), s1.data() + s1.size());
  while (is >> a >> b)
  {
    sb.mark_unget();
    std::cout << a << ' ' << b << '\n';
  }
  is.clear();
  
  if (n > 0)
    n = sb.fill(s1.data() + n, s1.data() + s1.size());

  while (is >> a >> b)
  {
    sb.mark_unget();
    std::cout << a << ' ' << b << '\n';
  }
  
  return 0;
}
