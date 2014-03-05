#include "instrumented.h"
#include <algorithm>

unsigned instrumented_base::counts[];
const char* instrumented_base::counter_names[number_ops] = {"n", "copy", "assign", "destruct", "default", "equal", "less", "construct"};
void instrumented_base::initialize(size_t m) {
    std::fill(counts, counts + number_ops, 0.0);
    counts[n] = unsigned(m);
}

void instrumented_base::print()
{
  for (int i = 0; i < instrumented_base::number_ops; ++i) {
    std::cout << std::setw(20) << instrumented_base::counter_names[i]
    << ' ' << instrumented_base::counts[i] << '\n';
  }
}