
/* --------------------------------------------------------------------------
 * File:    align.hpp
 * Created: 2020-09-10
 * --------------------------------------------------------------------------
 * 
 * 
 * 
 * ------------------------------------------------------------------------*/



#ifndef __align__hpp
#define __align__hpp

#include <iostream>
#include <cmath>


#include "image.hpp"
#include "basic_image_manipulation.hpp"

using namespace std;

Image denoiseSeq(const vector<Image> &imgs);
Image logSNR(const vector<Image> &imSeq, float scale=1.0/20.0);
vector<int> align(const Image &im1, const Image &im2, int maxOffset=20);
Image alignAndDenoise(const vector<Image> &imSeq, int maxOffset=20);
Image split(const Image &sergeyImg);
Image sergeyRGB(const Image &sergeyImg, int maxOffset=20);
Image roll(const Image &im, int xRoll, int yRoll); 
 
#endif