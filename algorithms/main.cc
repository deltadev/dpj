#include "heaps.h"

#include <vector>
#include <deque>
#include <random>

#include "algo.hh"

#include "instrumented.h"

#include "cq.hh"
#include "timer.hh"

#include "prettyprint.hpp"


#include <random>

using std::begin; using std::end;

void printf_test_cq();


using int_t = int;

std::vector<int_t> test_dequeue_func(std::vector<int_t> input, int const w, int const num_tests);
std::vector<int_t> test_queue_func(std::vector<int_t> input, int const w, int const num_tests);
std::vector<int_t> test_simple(std::vector<int_t> input, int const w, int const num_tests);


int main(int argc, char *argv[])
{
  
  //printf_test_cq();
  
  std::mt19937 rng{1};//(static_cast<int>(clock()));
  
  int const n = 100;
  int const w = 50;
  
  std::cout << "testing sequences of length: " << n << " and window: " << w << '\n';
  int const num_tests = 1000000;
  
  std::uniform_int_distribution<> runif{0, 64};
  
  std::vector<int_t> ints(n);
  std::generate(begin(ints), end(ints), [&]() { return int_t{runif(rng)}; });
  
  //instrumented_base::initialize(0);
  
  auto output1 = test_dequeue_func(ints, w, num_tests);
  auto output = test_queue_func(ints, w, num_tests);
  
  if (output != output1)
    std::cerr << "Error: output mismatch!\n";
  
  output1 = test_simple(ints, w, num_tests);
  
  if (output != output1)
    std::cerr << "Error: output mismatch!\n";
  //instrumented_base::print();
  
  return 0;
}

std::vector<int_t> test_simple(std::vector<int_t> input,
                               int const w, int const num_tests)
{
  std::vector<int_t> output(input.size());
  
  dpj::timer_ms t{"min_window_simple"};
  
  for (int i = 0; i < num_tests; ++i)
  {
    min_window_simple(w, begin(input), end(input), begin(output));
  }
  
  t.stop();
  
  return output;
}
std::vector<int_t> test_queue_func(std::vector<int_t> input,
                                   int const w, int const num_tests)
{
  cq<std::pair<int_t, int_t>, 6> c;
  dpj::timer_ms t{"min_window circ_queue func"};
  
  std::vector<int_t> output(input.size());
  
  for (int ii = 0; ii < num_tests; ++ii)
  {
    min_window_queue(c, w, begin(input), end(input), begin(output));
    c.clear();
  }
  t.stop();
  
  return output;
}

std::vector<int_t> test_dequeue_func(std::vector<int_t> input,
                                     int const w, int const num_tests)
{
  std::deque<std::pair<int_t, int_t>> c;
  dpj::timer_ms t{"min_window dequeue func"};
  
  std::vector<int_t> output(input.size());
  
  for (int ii = 0; ii < num_tests; ++ii)
  {
    min_window_queue(c, w, begin(input), end(input), begin(output));
    c.clear();
  }
  t.stop();
  
  return output;
}

void printf_test_cq()
{
  cq<int, 4> ct;
  std::array<int, 7> data{{8,4,1,3,9,6,2}};
  
  for (auto el : data)
    ct.push_back(el);
  
  std::cout << ct << '\n';
  
  
  ct.pop_front();
  
  std::cout << ct << '\n';
  
  
  ct.push_back(10);
  ct.push_back(11);
  
  std::cout << ct << '\n';
  
  ct.pop_front();
  
  std::cout << ct << '\n';
  
  ct.pop_back();
  for (auto el : data)
    ct.push_back(el);
  
  std::cout << ct << '\n';
  
  ct.pop_back();
  
  std::cout << ct << '\n';
}