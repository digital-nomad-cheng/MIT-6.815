/**
 * Instructions.
 *
 * This file contains a number of empty function stubs with the problem
 * statements. You need to fill in all details. These problems
 * are designed as extensions to the tutorials in this Pset. Please run
 * each tutorial and go through the code and comments. You are welcome
 * to discuss details on Piazza if you do not understand the tutorials.
 *
 * The main function is in a9_main.cpp. You can modify the contents for
 * your debugging purposes but restore the original contents.
 *
 * Live long and propser.
 */

#include <a9.hpp>

using namespace Halide;


Func HarrisCorners(const Image<float> &input,
    int schedule_index,
    vector<int> schedule_parameters,
    float sigmaG,
    float factorSigma,
    float truncate,
    float k,
    float thresh,
    int maxiDiam
    ) 
{

    // --------- HANDOUT  PS09 ------------------------------
    // Re-implement the Harris Corner detector using Halide. By fusing
    // different stages of the processing pipeline and properly scheduling your
    // opertations, you should be able to get a drastic speed-up over the
    // reference implementation given in 'reference_implementation.cpp'.
    //
    // Make sure you debug your intermediate Func (by
    // realizing them over some domain for example). You can use the
    // normalize_image helper function to remap the pixel value to the [0,1]
    // range, valid for display.
    //
    // Start by building a correct implementation of the algorithm,
    // Then walk your way up to a fast implementation by trying
    // various schedules.
    //
    // Return the output Func, properly scheduled. Chek the reference
    // implementation to see how the input parameters are used.
    //
    // You can use scheule_index to switch between different schedules and
    // compare them, this will come in handy when you implement an autotuner
    // (6.865). You can also pass schedule parameters as a vector of ints.
    

    // --------- Algorithm ----------------------------------

}

