#include "heaps.h"

#include "instrumented.h"

#include "min_window.h"

#include "cq.hh"

int main(int argc, char *argv[])
{
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
//
//  std::cout << std::endl;
//  
//  
//  instrumented_base::initialize(0);
//  
//  time_min_window_alloc();
//  
//  instrumented_base::print();

  cq c;
  
  c.push_back(1);
  c.push_back(2);
  c.push_back(1);
  int popped = c.pop_front();
  
  c.push_back(4);
  c.push_back(5);
  c.push_back(6);
  c.push_back(7);
  
  popped = c.pop_front();
  
  c.push_back(8);
  c.push_back(9);
  c.push_back(10);

  auto b = c.begin();
  auto e = c.end();
  
  for (auto el : c)
  {
    std::cout << el << '\n';
  }
  
  return 0;
}
