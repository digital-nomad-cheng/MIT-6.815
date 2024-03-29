/* -----------------------------------------------------------------
 * File:    a1.h
 * Created: 2015-09-15
 * -----------------------------------------------------------------
 * 
 * Assignment 01
 * 
 * ---------------------------------------------------------------*/


#ifndef __a1__h
#define __a1__h

#include <cmath>
#include <iostream>

#include "image.hpp"

using namespace std;

// Brightness and contrast
Image brightness(const Image &im, float factor);
Image contrast(const Image &im, float factor, float midpoint = 0.5);

// Colorspace problems
static const float weight_init[3] = {0.299, 0.587, 0.114};
Image color2gray(const Image &im,
    const std::vector<float> &weights = std::vector<float>(weight_init, weight_init+3)
);

std::vector<Image> lumiChromi(const Image &im);

Image brightnessContrastLumi(const Image &im, float brightF, float contrastF, float midpoint = 0.3);

Image rgb2yuv(const Image &im);

Image yuv2rgb(const Image &im);

Image saturate(const Image &im, float k);

Image add_black_dot(const Image &im);

std::vector<Image> spanish(const Image &im);

// White Balance
Image grayworld(const Image & in);

std::vector<float> get_mean_per_channel(const Image & im);

#endif