#include "min_window.h"


#include <random>

using std::begin; using std::end; using std::cout; using std::cerr;

#include "algo.hh"
#include "prettyprint.hpp"
#include "timer.hh"

namespace
{
  const std::size_t k = 10;
  const int N = 100;
  const int T = 10000;
  
  using val_t = instrumented<int>;
  using data_t = std::array<val_t, N>;
  
  std::vector<data_t> data(int T)
  {
    std::mt19937 rng;
    //std::uniform_int_distribution<> range{0, 500};
    std::uniform_real_distribution<> udist{0, 64};
    //std::exponential_distribution<> range{.05};
    
    std::vector<data_t> inputs(T);
    for (auto& input : inputs)
      std::generate(begin(input), end(input), [&rng, &udist]() {
        return instrumented<int>(udist(rng)); });
    
    return inputs;
  }
}

void time_min_window_slow()
{
  
  auto inputs = data(T);
  std::vector<val_t> output(N);
  dpj::timer_ms t{"min_window_slow"};
  for (auto const& input : inputs)
  {
    min_window_slow(k, begin(input), end(input), begin(output));
  }
  t.stop();
}

void time_min_window_fixed_buf() {
  auto inputs = data(T);
  std::vector<val_t> output(N);
  std::array<val_t, 1024> buf{{instrumented<int>{0}}};
  dpj::timer_ms t{"min_window, fixed buf"};
  for (auto const& input : inputs)
  {
    min_window(k, begin(input), end(input), begin(output), begin(buf));
  }
  t.stop();
}
void time_min_window_vector_buf()
{
  auto inputs = data(T);
  std::vector<val_t> output(N);
  std::vector<val_t> buf(1024);
  dpj::timer_ms t{"min_window, vector buf"};
  for (auto const& input : inputs)
  {
    min_window(k, begin(input), end(input), begin(output), begin(buf));
  }
  t.stop();
}

void time_min_window_alloc()
{
  auto inputs = data(T);
  std::vector<val_t> output(N);
  dpj::timer_ms t{"min_window, alloc array buf"};
  for (auto const& input : inputs)
  {
    min_window_alloc<k>(begin(input), end(input), begin(output));
  }
  t.stop();
}



