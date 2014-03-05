#include "heaps.h"

#include <iostream>

#include <vector>
#include <algorithm>
#include <iomanip>

#include <random>

#include "prettyprint.hpp"
#include "instrumented.h"

using std::begin; using std::end;

void do_it()
{
  std::mt19937 rng;
  std::uniform_int_distribution<> udist{0, 10};
  
  using int_t = instrumented<int>;
  
  std::vector<int_t> ints(20);
  std::generate(begin(ints), end(ints), [&rng, &udist]() { return instrumented<int>(udist(rng)); });
  

  std::cout << ints << '\n';

  std::make_heap(begin(ints), end(ints));
  
  std::cout << ints << "\n\n";
  
}
