#ifndef _A8_HPP_
#define _A8_HPP_

#include <iostream>
#include <cmath>
#include <Halide.h>
#include <halide_image_io.h>
#include <halide_image.h>

using std::string;
using std::map;
using std::cout;
using std::cerr;
using std::endl;
using std::vector;

using Halide::Tools::Image;
using Halide::Func;
using Halide::Tools::save_image;
using Halide::Tools::load_image;
using Halide::Tools::load_and_convert_image;
using Halide::Tools::convert_and_save_image;

void apply_auto_schedule(Func F);
void apply_my_schedule(Func F);

Func HarrisCorners(const Image<float> & input,
    int schedule_index = -1,
    vector<int> schedule_parameters = vector<int>(),
    float sigmaG      = 1.0f,
    float factorSigma = 4.0f,
    float truncate    = 3.0f,
    float k           = 0.15f,
    float thresh      = 0.0f,
    int maxiDiam      = 7
);

// C++ reference implementation of the Harris Corner detector
Image<float> maximum_filter(const Image<float>& input, int maxiDiam = 7);
Image<float> normalize_image(const Image<float> &input); // debug tool for visualization
Image<float> sobelFilter_x(const Image<float> &input);
Image<float> sobelFilter_y(const Image<float> &input);
Image<float> gaussian_blur(const Image<float> & input, float sigma, float truncate = 3.0f);
Image<float> computeLuminance(const Image<float> &input);
Image<float> HarrisCornersRef(const Image<float> & input,
    float sigmaG      = 1.0f,
    float factorSigma = 4.0f,
    float truncate    = 3.0f,
    float k           = 0.15f,
    float thresh      = 0.0f,
    int maxiDiam      = 7
); // main function

void autoschedule_harris(const Image<float> &input);

#endif // _A8_H_
