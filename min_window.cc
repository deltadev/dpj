#include <iostream>
#include <vector>
#include <array>
#include <random>

using std::begin; using std::end; using std::cout; using std::cerr;


#include "prettyprint.hpp"
#include "timer.hh"

template<std::size_t const k, typename InputIt, typename OutputIt>
void min_window_alloc(InputIt first1, InputIt const last1, OutputIt first2)
{
  typedef typename std::iterator_traits<InputIt>::value_type value_type;
  std::array<value_type, k> buf{{0}};
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

template<typename InputIt, typename OutputIt, typename BufferIt>
void min_window(std::size_t const k, InputIt first1, InputIt const last1, OutputIt first2, BufferIt buf)
{
  for (std::size_t i = k; first1 != last1; ++first1, ++first2, ++i)
  {
    *(buf + (i % k)) = *first1;
    std::size_t r = 1;
    while (*first1 < *(buf + (i - r) % k))
    {
      *(buf + (i - r) % k) = *first1;
      ++r;
    }
    *first2 = *(buf + (i + 1) % k);
  }
}

template<typename InputIt, typename OutputIt>
void min_window_slow(std::size_t k, InputIt first1, InputIt const last1, OutputIt first2)
{
  typedef typename std::iterator_traits<InputIt>::value_type value_type;
  first2 = first2 + (k - 1);
  for (auto seg_end = first1 + k; (seg_end - 1) != last1; ++first1, ++first2, ++seg_end)
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
  //std::uniform_int_distribution<> range{0, 500};
  //std::uniform_real_distribution<> range{0, 64};
  std::exponential_distribution<> range{.05};
  
  const std::size_t k = 60;
  const int N = 100;
  const int T = 1000000;
  
  using val_t = double;
  using data_t = std::array<val_t, N>;
  std::vector<data_t> inputs(T);
  for (auto& input : inputs)
    std::generate(begin(input), end(input), std::bind(range, rng));

  {
    std::vector<val_t> output(N);
    dpj::timer_ms t{"slow min_window"};
    for (auto const& input : inputs)
    {
      min_window_slow(k, begin(input), end(input), begin(output));
    }
    t.stop();
  }
  {
    std::vector<val_t> output(N);
    std::array<val_t, 1024> buf{{0}};
    dpj::timer_ms t{"min_window, fixed buf"};
    for (auto const& input : inputs)
    {
      min_window(k, begin(input), end(input), begin(output), begin(buf));
    }
    t.stop();
  }
  {
    std::vector<val_t> output(N);
    std::vector<val_t> buf(1024);
    dpj::timer_ms t{"min_window, vector buf"};
    for (auto const& input : inputs)
    {
      min_window(k, begin(input), end(input), begin(output), begin(buf));
    }
    t.stop();
  }
  {
    std::vector<val_t> output(N);
    dpj::timer_ms t{"min_window, alloc array buf"};
    for (auto const& input : inputs)
    {
      min_window_alloc<k>(begin(input), end(input), begin(output));
    }
    t.stop();
  }

  
  
  return 0;
}

