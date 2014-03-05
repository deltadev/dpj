#ifndef _MIN_WINDOW_H_
#define _MIN_WINDOW_H_


#include <iostream>
#include <vector>
#include <array>

#include "instrumented.h"

void time_min_window_slow();
void time_min_window_fixed_buf();
void time_min_window_vector_buf();
void time_min_window_alloc();


#endif /* _MIN_WINDOW_H_ */
