/* -----------------------------------------------------------------
 * File:    filtering.h
 * Created: 2015-09-22
 * -----------------------------------------------------------------
 * 
 * Image convolution and filtering
 * 
 * ---------------------------------------------------------------*/


#ifndef __filtering__hpp
#define __filtering__hpp

#include <cmath>
#include <iostream>

#include "basic_image_manipulation.hpp"
#include "image.hpp"

using namespace std;

class Filter {
    
public:
    //Constructor
    Filter(const vector<float> &fData, int fWidth, int fHeight);
    Filter(int fWidth, int fHeight); // kernel with all zero

    // Destructor. Because there is no explicit memory management here, this doesn't do anything
    ~Filter();
    
    // function to convolve your filter with an image
    Image convolve(const Image &im, bool clamp=true);
    
    // Accessors of the filter values
    const float & operator()(int x, int y) const;
    float & operator()(int x, int y);
    
    
    
// The following are functions and variables that are not accessible from outside the class
private:
    std::vector<float> kernel;
    int width;
    int height;
    
};

// Box Blurring
Image boxBlur(const Image &im, int k, bool clamp=true);
Image boxBlur_filterClass(const Image &im, int k, bool clamp=true);

// Gradient Filter
Image gradientMagnitude(const Image &im, bool clamp=true);

// Gaussian Blurring
vector<float> gauss1DFilterValues(float sigma, float truncate);
vector<float> gauss2DFilterValues(float sigma, float truncate);
Image gaussianBlur_horizontal(const Image &im, float sigma, float truncate=3.0, bool clamp=true);
Image gaussianBlur_separable(const Image &im, float sigma, float truncate=3.0, bool clamp=true);
Image gaussianBlur_2D(const Image &im, float sigma, float truncate=3.0, bool clamp=true);

// Sharpen an Image
Image unsharpMask(const Image &im, float sigma, float truncate=3.0, float strength=1.0, bool clamp=true);

// Bilaterial Filtering
Image bilateral(const Image &im, float sigmaRange=0.1, float sigmaDomain=1.0, float truncateDomain=3.0, bool clamp=true);
Image bilaYUV(const Image &im, float sigmaRange=0.1, float sigmaY=1.0, float sigmaUV=4.0, float truncateDomain=3.0, bool clamp=true);

// Return impulse image of size kxkx1
Image impulseImg(int k);
 
#endif
