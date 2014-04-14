#include <iostream>
#include <sstream>

#include <vector>

#include <random>

#include "array_streambuf.hh"

struct point_t
{
  float x, y, z;

  friend
  std::ostream& operator<<(std::ostream& os, point_t const& p)
  { return os << p.x << ' ' << p.y << ' ' << p.z; }
  
  friend
  std::istream& operator>>(std::istream& is, point_t& p)
  { return is >> p.x >> p.y >> p.z; }
  
  friend
  void println(point_t const& p, std::ostream& os = std::cout)
  { os << p << '\n'; }
  
  friend
  std::string to_string(point_t const& p)
  {
    std::ostringstream oss;
    oss << p;
    return oss.str();
  }
  
  friend
  bool operator==(point_t const& a, point_t const& b)
  {
    float e = 0.01;
    return std::abs(a.x - b.x) < e && std::abs(a.y - b.y) < e && std::abs(a.z - b.z) < e;
  }
};


int main(int argc, char *argv[])
{
  std::mt19937 rng;
  std::uniform_real_distribution<float> x{0, 100};
  std::vector<point_t> points(10000);
  
  std::generate(begin(points), end(points),
                [&]() -> point_t { return {x(rng), x(rng), x(rng)}; });
  
  std::vector<char> bytes;
  for (auto const& p: points)
  {
    auto str = to_string(p);
    str.append("\n");
    std::copy(begin(str), end(str), std::back_inserter(bytes));
  }
  
  std::uniform_int_distribution<long> read_amount{16, 32};
  
  dpj::array_streambuf sb;
  std::vector<point_t> points2;

  // Reads a random number of bytes into the streambuf and then extracts.
  //
  auto it = begin(bytes);
  while (it != end(bytes))
  {
    auto n = read_amount(rng);
    n = std::min(n, sb.avail());
    auto e = std::min(it + n, end(bytes));
    std::copy(it, e, sb.pptr());
    it = e;
    sb.bump_egptr(n);
    
    std::istream is{&sb};
    point_t p;
    sb.mark_unget();
    while (is >> p)
    {
      if (!is.eof())
      {
        points2.push_back(p);
        sb.mark_unget();
      }
    }
  }
  
  std::cout << "test passed: " << std::boolalpha << (points == points2) << '\n';
  
  return 0;
}
