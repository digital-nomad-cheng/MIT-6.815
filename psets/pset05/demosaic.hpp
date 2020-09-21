/* --------------------------------------------------------------------------
 * File:    demosaic.hpp
 * Created: 2020-09-10
 * --------------------------------------------------------------------------
 * 
 * 
 * 
 * ------------------------------------------------------------------------*/


#ifndef __demosaic__hpp__
#define __demosaic__hpp__


#include <iostream>
#include <math.h>

#include "image.hpp"
#include "basic_image_manipulation.hpp"

using namespace std;

Image basicGreen(const Image &raw, int offset=0);
Image basicRorB(const Image &raw, int offsetX, int offsetY);
Image basicDemosaic(const Image &raw, int offsetGreen=0, int offsetRedX=1, int offsetRedY=1, int offsetBlueX=0, int offsetBlueY=0);
Image edgeBasedGreen(const Image &raw, int offset=0);
Image edgeBasedGreenDemosaic(const Image &raw, int offsetGreen=0, int offsetRedX=1, int offsetRedY=1, int offsetBlueX=0, int offsetBlueY=0);
Image greenBasedRorB(const Image &raw, Image &green, int offsetX, int offsetY);
Image improvedDemosaic(const Image &raw, int offsetGreen=0, int offsetRedX=1, int offsetRedY=1, int offsetBlueX=0, int offsetBlueY=0);

 
#endif