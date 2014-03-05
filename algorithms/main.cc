#include "heaps.h"

#include "instrumented.h"

#include "min_window.h"


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

  
  return 0;
}
