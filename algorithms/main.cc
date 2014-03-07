#include "heaps.h"

#include <vector>
#include <random>


#include "instrumented.h"

#include "min_window.h"

#include "cq.hh"

#include "prettyprint.hpp"

int main(int argc, char *argv[])
{
  using std::begin; using std::end;
  
  instrumented_base::initialize(0);
  
  do_it();
  
  instrumented_base::print();
  
  
  std::cout << std::endl;
  
  
//  instrumented_base::initialize(0);
//
//  time_min_window_slow();
//  
//  instrumented_base::print();
//
//  std::cout << std::endl;
//  
//  instrumented_base::initialize(0);
//  
//  time_min_window_alloc();
//  
//  instrumented_base::print();
  using std::begin;
  using std::end;
  using indexed_int = std::pair<int, int>;

  
  
  int const n = 10;
  int const w = 3;

  std::vector<int> ints(n);
  std::generate(begin(ints), end(ints), [n]() { return rand() % n; });
  
  std::cout << ints << '\n';
  
  
  std::vector<indexed_int> output;
  cq<indexed_int, w> c;
  int idx = 0;
  for (auto i : ints)
  {
    if (c.front().second < idx)
      output.push_back(c.pop_front());
    
    while (i < c.back().first)
      c.pop_back();

    std::cout << "\n\n***queue " << idx << " ***\n";
    c.push_back({i, idx++});
    

    for (auto el : c)
      std::cout << el << '\n';
    
    std::cout << "***output***\n";
    for (auto el : output)
      std::cout << el << '\n';
    
    std::cout << "************\n";
  }

  cq<int, 4> ct;
  std::array<int, 7> data{{8,4,1,3,9,6,2}};
  
  for (auto el : data)
    ct.push_back(el);
  
  for (auto el : ct)
    std::cout << el << std::endl;
  std::cout << '\n';
  
  auto front = ct.pop_front();
  
  for (auto el : ct)
    std::cout << el << std::endl;
  std::cout << '\n';

  ct.push_back(10);
  ct.push_back(11);

  
  for (auto el : ct)
    std::cout << el << std::endl;
  std::cout << '\n';

  front = ct.pop_front();
  auto back = ct.pop_back();
  for (auto el : data)
    ct.push_back(el);
  back = ct.pop_back();
  
  for (auto el : ct)
    std::cout << el << std::endl;
  std::cout << '\n';
  

  
  return 0;
}
