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

using Halide::Tools::Image;
using Halide::Func;
using Halide::Tools::save_image;
using Halide::Tools::load_image;

void apply_auto_schedule(Func F);

Image<uint8_t> SmoothGradNormalized(void);
Image<uint8_t> WavyRGB(void);
Image<uint8_t> Luminance(Image<uint8_t> input);
Image<uint8_t> Sobel(Image<uint8_t> input);

Image<uint8_t> boxSchedule1(Image<uint8_t> input);
Image<uint8_t> boxSchedule5(Image<uint8_t> input);
Image<uint8_t> boxSchedule6(Image<uint8_t> input);
Image<uint8_t> boxSchedule7(Image<uint8_t> input);

Image<uint8_t> LocalMax_cpp(Image<uint8_t> input, int window=5);
Func LocalMax(Image<uint8_t> lumi, int window=5);

Func Gaussian(Image<uint8_t> input, float sigma, float truncate=3.0f);
Image<uint8_t> Gaussian_cpp(Image<uint8_t> input, float sigma, float truncate = 3.0f);

Func UnsharpMask(Image<uint8_t> input, float sigma, float truncate=3.0f, float strength=1.0f);

#endif // _A8_H_
