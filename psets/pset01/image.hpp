#ifndef __IMAGE_HPP__
#define __IMAGE_HPP__

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <cfloat>
#include <cmath>

#include "image_exception.hpp"
#include "lodepng.h"

class Image 
{
public:
	// Constructor to initialize an image of size width_*height_*channels_
    // If height_ and channels_ are zero, the image will be one dimensional
    // If channels_ is zero, the image will be two dimensional
    Image(int width_, int height_ = 0, int channels_ = 0,  const std::string &name="");

    // Constructor to create an image from a file. The file needs to be in the PNG format
    Image(const std::string & filename);

    // Destructor. Because there is no explicit memory management here, this doesn't do anything
    ~Image();

    // Returns the images name, should you specify one
    const std::string & name() const { return image_name; }

    // The number of dimensions in the image (1, 2 or 3)
    int dimensions() const { return dims; }

    // The distance between adjacent values in image_data in a given dimension where
    // width is dimension 0, height is dimension 1 and channel is dimension 2
    int stride(int dim) const { return stride_[dim]; }

    int extent(int dim) const { return dim_values[dim]; } // Size of dimension
    int width()    const { return dim_values[0]; } // Extent of dimension 0
    int height()   const { return dim_values[1]; } // Extent of dimension 1
    int channels() const { return dim_values[2]; } // Extent of dimension 2

    // Write an image to a file. 
    void write(const std::string & filename) const;
    void debug_write() const; // Writes image to Output directory with automatically chosen name


    // --------- HANDOUT  PS01 ------------------------------
    // The total number of elements. Should be equal to width()*height()*channels()
    // That is, a 200x100x3 image has 60000 pixels not 20000 pixels
    long long number_of_elements() const;

    // Accessors for the pixel values
    const float & operator()(int x) const;
    const float & operator()(int x, int y) const;
    const float & operator()(int x, int y, int z) const;
    
    // Setters for the pixel values. A reference to the value in image_data is returned
    float & operator()(int x);
    float & operator()(int x, int y);
    float & operator()(int x, int y, int z);
    // ------------------------------------------------------

    static int debugWriteNumber; // Image number for debug write

private:
	unsigned int dims;
	unsigned int dim_values[3];
	unsigned stride_[3]; 
	std::string image_name; // The filename it read from a file

	std::vector<float> image_data; 

	// Common code shared between contrustors
	// This does not allocate the image; it only initializes image metadata -
    // image name, width, height, number of channels and number of pixels
    void initialize_image_metadata(int x, int y, int z, const std::string &name_);

	// Helper functions for reading and writing
	static float uint8_to_float(const unsigned char &in); // [0-255] -> [0-1]
	static unsigned char float_to_uint8(const float &in); // [0-1] -> [0-255]

};

// --------- HANDOUT  PS01 ------------------------------
void compareDimensions(const Image & im1, const Image & im2);

// Image/Image element-wise operations
Image operator+ (const Image & im1, const Image & im2);
Image operator- (const Image & im1, const Image & im2);
Image operator* (const Image & im1, const Image & im2);
Image operator/ (const Image & im1, const Image & im2);

// Image/scalar operations 
Image operator+ (const Image & im1, const float & c);
Image operator- (const Image & im1, const float & c);
Image operator* (const Image & im1, const float & c);
Image operator/ (const Image & im1, const float & c);

// scalar/Image operations 
Image operator+ (const float & c, const Image & im1);
Image operator- (const float & c, const Image & im1);
Image operator* (const float & c, const Image & im1);
Image operator/ (const float & c, const Image & im1);
// ------------------------------------------------------

#endif