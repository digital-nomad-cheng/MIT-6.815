/**
 * Instructions.
 *
 * This file contains a number of empty function stubs with the problem
 * statements. You need to fill in all details. These problems
 * are designed as extensions to the tutorials in this Pset. Please run
 * each tutorial and go through the code and comments. You are welcome
 * to discuss details on Piazza if you do not understand the tutorials.
 *
 * The main function is in a8_main.cpp. You can modify the contents for
 * your debugging purposes but restore the original contents.
 *
 * Live long and propser.
 */

#include <a8.hpp>

using namespace Halide;

// This applies a compute_root() schedule to all the Func's that are consumed
// by the calling Func. DO NOT EDIT IT.
void apply_auto_schedule(Func F) {

}

// ---------------------------------- PART 1 -----------------------------------
// For all Halide Func's you write in Part 1, you do not need to worry about
// schedule. However, things will be faster if you just use:
// apply_auto_schedule(final_Result_Func);

Image<uint8_t> SmoothGradNormalized(void) {
    // // --------- HANDOUT  PS08 ------------------------------
    // Use Halide to compute a 512x512 smooth gradient where pixel (x,y) is
    // equal to x+y divided by 1024.
    //
    // HINT: tutorial 1 has a very similar function except that it does not
    // normalize by 1024
    //
    // OUPUT: a single channel output image of size 512x512 (uint8_t)
    //
    // SCHEDULE: use compute_root() on all the Func's you create (i.e.
    // use apply_auto_schedule(Func f).


}

Image<uint8_t> WavyRGB(void) {
    // // --------- HANDOUT  PS08 ------------------------------
    // Use a Halide Func to compute a wavy RGB image obtained
    // by the formula: output(x,y,c) = (1-c)*cos(x)*cos(y)
    // Schedule: use compute_root() on all the Func's you create.
    //
    // HINT: you need one more dimension than before
    //
    // OUTPUT 512x512 RGB image (uint8_t)
    //
    // SCHEDULE: use compute_root() on all the Func's you create (i.e.
    // use apply_auto_schedule(Func f).
    

}

Image<uint8_t> Luminance(Image<uint8_t> input) {
    // // --------- HANDOUT  PS08 ------------------------------
    // Use Halide to compute a 1-channel luminance image. Use the formula
    // L = 0.3R+0.6G+0.1B
    //
    // INPUT: RGB image
    //
    // OUTPUT: single channel luminance image the same size as the input image
    //
    // SCHEDULE: use compute_root() on all the Func's you create (i.e.
    // use apply_auto_schedule(Func f).
   

}

Image<uint8_t> Sobel(Image<uint8_t> input) {
    // // --------- HANDOUT  PS08 ------------------------------
    // Use Halide to apply a Sobel filter and return the gradient magnitude.
    // Use the following kernels to compute the horizontal and vertical gradients
    //
    //            | -1 0 1 |             | -1 -2 -1 |
    // x kernel = | -2 0 2 |, y kernel = |  0  0  0 |
    //            | -1 0 1 |             |  1  2  1 |
    //
    // INPUT: single channel luminance image
    //
    // OUTPUT: single channel luminance image the same size as the input image
    // OUTPUT: single channel image where each pixel is the magnitude of Sobel x and y filters
    //
    // SCHEDULE: use compute_root() on all the Func's you create (i.e.
    // use apply_auto_schedule(Func f).

}

// Example code for the equivalent .cpp loop questions
Image<uint8_t> boxSchedule1(Image<uint8_t> input) {

}

Image<uint8_t> boxSchedule5(Image<uint8_t> input) {
    // // --------- HANDOUT  PS08 ------------------------------
    // Write the cpp nested loops corresponding to the 3x3 box schedule 5 in tutorial 6
    // print the order of evaluation. 
    
}

Image<uint8_t> boxSchedule6(Image<uint8_t> input) {
    // // --------- HANDOUT  PS08 ------------------------------
    // Write the cpp nested loops corresponding to the 3x3 box schedule 6 in tutorial 6
    // print the order of evaluation. 
    // Each time you perform a computation of blur_x or
    // blur_y, put a triplet with the name of the Func ("blur_x" or "blur_y") and
    // the output coordinates x and y. For example, when you compute (50,75) print:
    // blur_x 50 75 ( with exactly one space between each) in a new line.
    //
    // Do not print anything else using cout within the current function.
    

}



Image<uint8_t> boxSchedule7(Image<uint8_t> input) {
    // // --------- HANDOUT  PS08 ------------------------------
    // Write the cpp nested loops corresponding to the 3x3 box schedule 7 in tutorial 6
    // print the order of evaluation. 
    // Each time you perform a computation of blur_x or
    // blur_y, put a triplet with the name of the Func ("blur_x" or "blur_y") and
    // the output coordinates x and y. For example, when you compute (50,75) print:
    // blur_x 50 75 ( with exactly one space between each) in a new line.
    //
    // Do not print anything else using cout within the current function.
    
        // // --------- HANDOUT  PS08 ------------------------------
    // Write the cpp nested loops corresponding to the 3x3 box schedule 7 in tutorial 6
    // print the order of evaluation. 
    // Each time you perform a computation of blur_x or
    // blur_y, put a triplet with the name of the Func ("blur_x" or "blur_y") and
    // the output coordinates x and y. For example, when you compute (50,75) print:
    // blur_x 50 75 ( with exactly one space between each) in a new line.
    //
    // Do not print anything else using cout within the current function.
    

}


// ---------------------------------- PART 2 -----------------------------------
// For all the problems in Part 2, yo are expected to write schedules. You will
// score points for faster schedules. You should also report the machine used
// for testing in the form accompanying this Pset. Each problem is supposed to
// return a Halide Func that computes the result in uint8_t. All intermediate
// operations should be performed in float for Gaussian and unsharp mask.
//
// First get the algorithm right and make sure it is generating the
// correct output. Use apply_auto_schedule() during this phase. Once you
// are confident of your result, remove apply_auto_schedule() and write
// your own schedule. You can set the macro AUTO_SCHEDULE to 0 or 1 to easily
// alternate between the default and updated schedules.
//
// Hint: your schedule should beat apply_auto_schedule()


Func LocalMax(Image<uint8_t> input, int window) {
    // // --------- HANDOUT  PS08 ------------------------------
    // Compute the local maximum in a window of given size centered at the pixel.
    // Return white color if it's a local maximum and black otherwise.
    // Run your function with autoschedule as you write the algorithm, then
    // write your own (better) schedule.
    //
    // Handle boundary conditions by clamping at boundaries (see tutorials)
    //
    // INPUT: single channel luminance image
    // OUTPUT: Halide Func that computes the localMax

    // output Func

}


Func Gaussian(Image<uint8_t> input, float sigma, float truncate) {
    // // --------- HANDOUT  PS08 ------------------------------
    // Perform 2D gaussian blur. use what you learnt in previous Pset about
    // fast Gaussian blur algorithm. The Gaussians have variance sigma and the
    // kernels are truncated at trunc*sigma on both sides. We give you the
    // kernel.
    //
    // Use the sum() sugar (see tutorials)
    // First generate the kernel as a Halide Func
    // Handle boundary conditions
    //
    // INPUT: single channel luminance image
    //
    // OUTPUT: Func that computes Gaussian blur
    

}

Func UnsharpMask(Image<uint8_t> input, float sigma, float truncate, float strength) {
    // // --------- HANDOUT  PS08 ------------------------------
    // Implement Unsharp Mask. Use a Gaussian of standard deviation sigma to extract
    // a lowpassed version of the image. Subtract that lowpassed version from the
    // original image to produce a highpassed version of the image and then add the
    // highpassed version back to it strength times.
    //
    // Handle boundary conditions
    // Use the Gaussian above for blur
    // You may want to write a new Gaussian with a new schedule because it may be
    // possible to use a different schedule for Gaussian blur such that it works
    // better with subsequent stages
    //
    // INPUT: single channel luminance image
    // OUTPUt: Halide Func that computes the localMax
    //
    // return Func("f");


}
