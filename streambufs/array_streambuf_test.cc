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
  { return is >> p.x >> p.y >> p.z >> std::ws; }
  
  friend
  void println(point_t const& p, std::ostream& os = std::cout)
  { os << p << '\n'; }
  
  friend
  bool operator==(point_t const& a, point_t const& b)
  {
    float e = 0.01;
    return std::abs(a.x - b.x) < e && std::abs(a.y - b.y) < e && std::abs(a.z - b.z) < e;
  }
};

// Reads up to n bytes.
//

std::mt19937 rng{42};

int main(int argc, char *argv[])
{
  // Firstly we generate a stream of points.
  //
  std::uniform_real_distribution<float> x{0, 100};
  std::vector<point_t> points(20);
  
  std::generate(begin(points), end(points),
                [&]() -> point_t { return {x(rng), x(rng), x(rng)}; });
  
  std::vector<char> bytes;
  std::stringstream ss;
  for (auto const& p: points) ss << p << '\n';

  std::string str{ss.str()};
  std::copy(begin(str), end(str), std::back_inserter(bytes));
  
  // This function is a mock input stream that will deliver us data.
  //
  auto read_some = [&bytes](char* data, long n) -> long
  {
    static auto it = begin(bytes);
    
    if (it == end(bytes))
      return -1;

    std::uniform_int_distribution<long> read_amount{0, 64};
    auto avail = std::min<long>(read_amount(rng), n);

    auto tmp = it;
    it = std::min(tmp + avail, end(bytes));

    std::copy(tmp, it, data);

    return avail;
  };
  
  dpj::array_streambuf sb;
  std::vector<point_t> points2;

  // Reads a random number of bytes into the streambuf and then extracts.
  //
  while (true)
  {
    auto n = read_some(sb.pptr(), sb.avail());
    
    if (n < 0)
      break;
    
    // We update the the streambufs egptr().
    //
    sb.bump_egptr(n);
    
    // Now we contruct an istream to extract from.
    //
    std::istream is{&sb};
    point_t p;


    while (true)
    {
      auto state_a = (is >> p).fail();
      auto state_b = (is >> std::ws).fail();
      if (state_a && state_b)
        break;
      
      points2.push_back(p);
      std::cerr << "parsed: " << p << '\n';
      sb.mark_unget();
    }

  }
  
  std::cout << "test passed: " << std::boolalpha << (points == points2) << '\n';
  
  for (auto p: points) println(p);
  std::cout << "\n\n";
  
  for (auto p: points2) println(p);
  std::cout << "\n\n";

  
  return 0;
}
