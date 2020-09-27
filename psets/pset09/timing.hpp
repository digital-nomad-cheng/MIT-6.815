#ifndef _TIMING_HPP_
#define _TIMING_HPP_

#include <Halide.h>
using namespace Halide;

#define N_TIMES 1

unsigned long millisecond_timer(void);


/**
 * Print the runtime and throughout and return the runtime.
 */
float profile(Func myFunc, int w, int h);


/**
 * Print the runtime and throughout and return the runtime.
 */
float profile(Func myFunc, int w, int h, int c);

#endif // _TIMING_HPP_