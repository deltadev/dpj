#include <iostream>
#include <vector>
#include <array>
#include <random>

using std::begin; using std::end; using std::cout; using std::cerr;


#include "prettyprint.hpp"
#include "timer.hh"
namespace tmp {
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
}

template<std::size_t size, typename T>
class circ_buff
{
public:
  static std::size_t const k = size;
  template<typename It>
  circ_buff(It first, It last)
  {
    for (int i = 0; first != last; ++i, ++first)
    {
      buf[i] = std::make_pair(*first, i);
      ++count;
    }
  }
  void add(T x)
  {
    std::size_t pop_count = 0;
    while (x < back())
    {
      pop_front();
      ++pop_count;
    }
    buf[(base() + count) % size] = {x, base()};
  }
  T operator[](std::size_t i) const { return buf[(base() + i) % size].first; }
  T front() { return buf[base()].first; }
  void pop_front() { ++idx; --count;}


  
private:
  std::array<std::pair<T, std::size_t>, size> buf;
  std::size_t base() const { return idx % size; };

  T& back() {return buf[base() + count == 0 ? size - 1 : base() + count - 1].first; }
  std::size_t idx = 0;
  std::size_t count = 0;
};

template<typename U>
void print(U const& a)
{
  for (int i = 0; i < a.k; ++i) {
    std::cout << a[i] << ' ';
  }
  std::cout << std::endl;
}

int main(int argc, const char * argv[])
{
  std::mt19937 rng;
  //std::uniform_int_distribution<> range{0, 500};
  //std::uniform_real_distribution<> range{0, 64};
  std::exponential_distribution<> range{.05};
  
  const std::size_t k = 10;
  const int N = 10;
  const int T = 10000;
  
  using val_t = int;
  using data_t = std::array<val_t, N>;
  std::vector<data_t> inputs(T);
  for (auto& input : inputs)
    std::generate(begin(input), end(input), std::bind(range, rng));


  circ_buff<k, int> cb{inputs.front().begin(), inputs.front().end()};
  
  
  for (int i = 1; i < 10; ++i)
  {
    print(cb);
    cb.add(i);
  }
  
  if (false) {
  {
    std::vector<val_t> output(N);
    dpj::timer_ms t{"slow min_window"};
    for (auto const& input : inputs)
    {
      tmp::min_window_slow(k, begin(input), end(input), begin(output));
    }
    t.stop();
  }
  {
    std::vector<val_t> output(N);
    std::array<val_t, 1024> buf{{0}};
    dpj::timer_ms t{"min_window, fixed buf"};
    for (auto const& input : inputs)
    {
      tmp::min_window(k, begin(input), end(input), begin(output), begin(buf));
    }
    t.stop();
  }
  {
    std::vector<val_t> output(N);
    std::vector<val_t> buf(1024);
    dpj::timer_ms t{"min_window, vector buf"};
    for (auto const& input : inputs)
    {
      tmp::min_window(k, begin(input), end(input), begin(output), begin(buf));
    }
    t.stop();
  }
  {
    std::vector<val_t> output(N);
    dpj::timer_ms t{"min_window, alloc array buf"};
    for (auto const& input : inputs)
    {
      tmp::min_window_alloc<k>(begin(input), end(input), begin(output));
    }
    t.stop();
  }

  
  }
  
  
  return 0;
}

