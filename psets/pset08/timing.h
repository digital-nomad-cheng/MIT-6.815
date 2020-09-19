#ifndef _TIMING_H_
#define _TIMING_H_

using namespace Halide;

#define N_TIMES 10

/**
 * Millisecond-precision timer function
 * \return Clock value in milliseconds
 *
 * This routine implements a timer with millisecond precision.  In order to
 * obtain timing at high resolution, platform-specific functions are needed:
 *
 *    - On Windows systems, the GetSystemTime function is used.
 *    - On Mac and POSIX systems, the gettimeofday function is used.
 *
 * Preprocessor symbols are checked in attempt to detect whether the platform
 * is POSIX or Windows or Mac and defines millisecond_timer() accordingly.
 */
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
unsigned long millisecond_timer(void) {
    static SYSTEMTIME t;
    GetSystemTime(&t);
    return (unsigned long)((unsigned long)t.wMilliseconds
            + 1000*((unsigned long)t.wSecond
            + 60*((unsigned long)t.wMinute
            + 60*((unsigned long)t.wHour
            + 24*(unsigned long)t.wDay))));

#elif defined(_APPLE_) || defined(__APPLE__) || \
    defined(APPLE)   || defined(_APPLE)    || defined(__APPLE) || \
defined(unix)    || defined(__unix__)  || defined(__unix)
#include <unistd.h>
#include <sys/time.h>
unsigned long millisecond_timer(void) {
    struct timeval t;
    gettimeofday(&t, NULL);
    return (unsigned long)(t.tv_usec/1000 + t.tv_sec*1000);
}
#else
unsigned long millisecond_timer(void) {
    std::cout << "Warning: no timer implementation available" << std::endl;
    return 0;
}
#endif


/**
 * Print the runtime and throughout and return the runtime.
 */
float profile(Func myFunc, int w, int h) {
    myFunc.compile_jit();

    unsigned long s = millisecond_timer();
    for (int i=0; i<N_TIMES; i++) {
        myFunc.realize(w,h);
    }
    float total_time = float(millisecond_timer()-s);

    float mpixels = float(w*h)/1e6;
    std::cout << "  - runtime: " << total_time/N_TIMES << " ms\n"
        << "  - throughput: " << (mpixels*N_TIMES)/(total_time/1000) << " megapixels/sec" << std::endl;

    return total_time/N_TIMES;
}


/**
 * Print the runtime and throughout and return the runtime.
 */
float profile(Func myFunc, int w, int h, int c) {
    myFunc.compile_jit();

    unsigned long s = millisecond_timer();
    for (int i=0; i<N_TIMES; i++) {
        myFunc.realize(w,h,c);
    }
    float total_time = float(millisecond_timer()-s);

    float mpixels = float(w*h)/1e6;
    std::cout << " runtime " << total_time/N_TIMES << " ms "
        << " throughput " << (mpixels*N_TIMES)/(total_time/1000) << " megapixels/sec" << std::endl;

    return total_time/N_TIMES;
}

#endif // _TIMING_H_
