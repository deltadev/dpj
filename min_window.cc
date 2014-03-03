#include <iostream>
#include <vector>
#include <array>
#include <random>

using std::begin; using std::end; using std::cout; using std::cerr;


#include "prettyprint.hpp"
#include "timer.hh"

template<typename InputIt, typename OutputIt, typename Buffer>
void min_window(InputIt first1, InputIt last1, OutputIt first2, Buffer& buf)
{
  auto const k = buf.size();
  for (std::size_t i = k; first1 != last1; ++first1, ++first2, ++i)
  {
    buf[i % k] = *first1;
    std::size_t r = 1;
    while (*first1 < buf[(i - r) % k])
    {
      buf[(i - r) % k] = *first1;
      ++r;
    }
    *first2 = buf[(i + 1) % k];
  }
}

template<typename InputIt, typename OutputIt>
void min_window_slow(InputIt first1, InputIt last1, OutputIt first2, std::size_t k)
{
  typedef typename std::iterator_traits<InputIt>::value_type value_type;
  first2 += k - 1;
  for (auto seg_end = first1 + k; seg_end != last1; ++first1, ++first2, ++seg_end)
  {
    auto min = std::numeric_limits<value_type>::max();
    
    for (auto it = first1; it != seg_end; ++it)
    {
      if (*it < min)
        min = *it;
    }
    *first2 = min;
  }
}



int main(int argc, const char * argv[])
{
  std::mt19937 rng;
  std::uniform_int_distribution<> range{0, 50};

  const std::size_t k = 31;
  const int N = 100;
  const int T = 1000000;
  std::vector<int> input(N);
  std::generate(begin(input), end(input), std::bind(range, rng));

  auto b = input.cbegin();
  auto e = input.cend();

  {
    std::vector<int> output(input.size());
    dpj::timer_ms t{"slow min_window"};
    for (int i = 0; i < T; ++i)
    {
      min_window_slow(b, e, begin(output), k);
    }
    t.stop();
    std::cout << output << '\n';
  }
  {
    std::vector<int> output(input.size());
    std::array<int, k> buf{{0}};
    dpj::timer_ms t{"min_window, fixed buf"};
    for (int i = 0; i < T; ++i)
    {
      min_window(b, e, begin(output), buf);
    }
    t.stop();
    std::cout << output << '\n';
  }
  {
    std::vector<int> output(input.size());
    std::vector<int> buf(k);
    dpj::timer_ms t{"min_window, vector buf"};
    for (int i = 0; i < T; ++i)
    {
      min_window(b, e, begin(output), buf);
    }
    t.stop();
    std::cout << output << '\n';
  }

  
  
  return 0;
}

