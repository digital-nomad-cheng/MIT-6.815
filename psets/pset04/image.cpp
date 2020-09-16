/* -----------------------------------------------------------------
 * File:    Image.cpp
 * Created: 2015-08-29
 * -----------------------------------------------------------------
 * 
 * The functions students need to implement are at the top
 * of this file.
 * 
 * ---------------------------------------------------------------*/

#include "image.hpp"

using namespace std;


/*********************************************************************
 *                    DO NOT EDIT BELOW THIS LINE                    *
 *********************************************************************/


int Image::debugWriteNumber = 0;

Image::Image(int x, int y, int z, const std::string &name_) {
    initialize_image_metadata(x,y,z,name_);
    long long size_of_data = 1;
    for (int k = 0; k < dimensions(); k++) {
        size_of_data *= dim_values[k];
    }
    image_data = std::vector<float>(size_of_data, 0);
}

void Image::initialize_image_metadata(int x, int y, int z,  const std::string &name_) {
    dim_values[0] = 0;
    dim_values[1] = 0;
    dim_values[2] = 0;
    stride_[0] = 0;
    stride_[1] = 0;
    stride_[2] = 0;

    dims = 0;
    long long size_of_data = 1;
    if ( x < 0 )
        throw NegativeDimensionException();
    if ( y< 0)
        throw NegativeDimensionException();
    if (z < 0 )
        throw NegativeDimensionException();

    image_name = name_;


    dims++;
    dim_values[0] = x;
    size_of_data *= x;
    stride_[0] = 1;
    if (y > 0 ) {
        dims++;
        dim_values[1] = y;
        size_of_data *= y;
        stride_[1] = x;
    } else {
        return;
    }

    if (z>0)  {
        dims++;
        dim_values[2] =z;
        size_of_data *= z;
        stride_[2] = x*y;
    } else {
        return;
    }

}

Image::Image(const std::string & filename) {
    std::vector<unsigned char> uint8_image;
    unsigned int height_;
    unsigned int width_;
    unsigned int channels_ = 4;
    unsigned int outputchannels_ = 3; // Throw away transparency
    unsigned err = lodepng::decode(uint8_image, width_, height_, filename.c_str()); // In column major order with packed color values
    if(err == 48) {
        throw FileNotFoundException();
    }

    image_data = std::vector<float>(height_*width_*outputchannels_,0);

    for (unsigned int x= 0; x < width_; x++) {
        for (unsigned int y = 0; y < height_; y++) {
            for (unsigned int c = 0; c < outputchannels_; c++) {
                image_data[x+y*width_+c*width_*height_] = uint8_to_float(uint8_image[c + x*channels_ + y*channels_*width_]);
            }
        }
    }

    initialize_image_metadata(width_, height_, outputchannels_, filename);

}

Image::~Image() { } // Nothing to clean up

void Image::write(const std::string &filename) const {
    if (channels() != 1 && channels() != 3 && channels() != 4)
        throw ChannelException();
    int png_channels = 4;
    std::vector<unsigned char> uint8_image(height()*width()*png_channels, 255);
    int c;
    for (int x= 0; x < width(); x++) {
        for (int y = 0; y < height(); y++) {
            for (c = 0; c < channels(); c++) {
                uint8_image[c + x*png_channels + y*png_channels*width()] = float_to_uint8(image_data[x+y*width()+c*width()*height()]);
            }
            for ( ; c < 3; c++) { // Only executes when there is one channel

                uint8_image[c + x*png_channels + y*png_channels*width()] = float_to_uint8(image_data[x+y*width()+0*width()*height()]);
            }
        }
    }
    lodepng::encode(filename.c_str(), uint8_image, width(), height());
}

void Image::debug_write() const {
    std::ostringstream ss;
    ss << "../Output/" <<  debugWriteNumber << ".png";
    std::string filename = ss.str();
    write(filename);
    debugWriteNumber++;

}

float Image::uint8_to_float(const unsigned char &in) {
    return ((float) in)/(255.0f);
}

unsigned char Image::float_to_uint8(const float &in) {
    float out = in;
    if (out < 0)
        out = 0;
    if (out > 1)
        out = 1;
    return (unsigned char) (255.0f*out);

}

// --------------------- Begin of pset01 ----------------------------------
long long Image::number_of_elements() const
{
    return dim_values[0] * dim_values[1] * dim_values[2];
}

float Image::min() const {
    float minf = FLT_MAX;
    for (int i = 0; i < number_of_elements(); i++) {
        minf = std::min(minf, (*this)(i));
    }
    return minf;
}

float Image::max() const {
    float maxf = -FLT_MAX;
    for (int i = 0; i < number_of_elements(); i++) {
        maxf = std::max(maxf, (*this)(i));
    }
    return maxf;
}

const float & Image::operator()(int x) const
{
    // Linear accessor to the image data
    if (x >= 0 && x < this->width()*this->height()*this->channels()) {
        return image_data[x];
    } else {
        throw OutOfBoundsException();
    }
    
}

const float & Image::operator()(int x, int y) const
{
    // Accessor to the image data at channel 0
    if (x >= 0 && x < this->width() && y >= 0 && y < this->height()) {
        return image_data[this->width() * y + x];
    } else {
        throw OutOfBoundsException();
    }
}

const float & Image::operator()(int x, int y, int z) const
{
    // Accessor to the image data at channel z
    if (x >= 0 && x < this->width() && y >= 0 && y < this->height() && z >= 0 && z < this->channels()) {
        return image_data[this->width()*this->height()*z + this->width() * y + x];
    } else {
        throw OutOfBoundsException();
    }
     
}

float & Image::operator()(int x) {
    // Linear setter to the image data
    if (x >= 0 && x < this->width()*this->height()*this->channels()) {
        return image_data[x];
    } else {
        throw OutOfBoundsException();
    }
}

float & Image::operator()(int x, int y) {
    // Setter to the image data at channel 0
    if (x >= 0 && x < this->width() && y >= 0 && y < this->height()) {
        return image_data[this->width() * y + x];
    } else {
        throw OutOfBoundsException();
    }

}
float & Image::operator()(int x, int y, int z)
{
    // Setter to the image data at channel z
    if (x >= 0 && x < this->width() && y >= 0 && y < this->height() && z >= 0 && z < this->channels()) {
        return image_data[this->width()*this->height()*z + this->width() * y + x];
    } else {
        throw OutOfBoundsException();
    }
}

void compareDimensions(const Image & im1, const Image & im2) 
{
    if (im1.width() != im2.width() || im1.height() != im2.height() || im1.channels() != im2.channels()) {
        throw MismatchedDimensionsException();
    }
    return;
}

// Image/Image element-wise operations
Image operator+ (const Image & im1, const Image & im2)
{
    compareDimensions(im1, im2);
    Image output(im1.width(), im1.height(), im1.channels());
    for (int c = 0; c < im1.channels(); c++) {
        for (int h = 0; h < im1.height(); h++) {
            for (int w = 0; w < im1.width(); w++) {
                output(w, h, c) = im1(w, h, c) + im2(w, h, c);
            } 
        }
    }
    return output;
}
Image operator- (const Image & im1, const Image & im2)
{
    compareDimensions(im1, im2);
    Image output(im1.width(), im1.height(), im1.channels());
    for (int c = 0; c < im1.channels(); c++) {
        for (int h = 0; h < im1.height(); h++) {
            for (int w = 0; w < im1.width(); w++) {
                output(w, h, c) = im1(w, h, c) - im2(w, h, c);
            } 
        }
    }
    return output;
}
Image operator* (const Image & im1, const Image & im2)
{
    compareDimensions(im1, im2);
    Image output(im1.width(), im1.height(), im1.channels());
    for (int c = 0; c < im1.channels(); c++) {
        for (int h = 0; h < im1.height(); h++) {
            for (int w = 0; w < im1.width(); w++) {
                output(w, h, c) = im1(w, h, c) * im2(w, h, c);
            } 
        }
    }
    return output;
}
Image operator/ (const Image & im1, const Image & im2)
{
    compareDimensions(im1, im2);
    Image output(im1.width(), im1.height(), im1.channels());
    for (int c = 0; c < im1.channels(); c++) {
        for (int h = 0; h < im1.height(); h++) {
            for (int w = 0; w < im1.width(); w++) {
                if (im2(w, h, c) == 0.0) {
                    throw DivideByZeroException();
                }
                output(w, h, c) = im1(w, h, c) / im2(w, h, c);
            } 
        }
    }
    return output;
}

// Image/scalar operations 
Image operator+ (const Image & im1, const float & c)
{
    Image output(im1.width(), im1.height(), im1.channels());
    for (int c = 0; c < im1.channels(); c++) {
        for (int h = 0; h < im1.height(); h++) {
            for (int w = 0; w < im1.width(); w++) {
                output(w, h, c) = im1(w, h, c) + c;
            } 
        }
    }
    return output;

}
Image operator- (const Image & im1, const float & c)
{
    Image output(im1.width(), im1.height(), im1.channels());
    for (int c = 0; c < im1.channels(); c++) {
        for (int h = 0; h < im1.height(); h++) {
            for (int w = 0; w < im1.width(); w++) {
                output(w, h, c) = im1(w, h, c) - c;
            } 
        }
    }
    return output;
}
Image operator* (const Image & im1, const float & c)
{
    Image output(im1.width(), im1.height(), im1.channels());
    for (int c = 0; c < im1.channels(); c++) {
        for (int h = 0; h < im1.height(); h++) {
            for (int w = 0; w < im1.width(); w++) {
                output(w, h, c) = im1(w, h, c) * c;
            } 
        }
    }
    return output;
}
Image operator/ (const Image & im1, const float & c)
{
    if (c == 0.0) {
        throw DivideByZeroException();
    }
    Image output(im1.width(), im1.height(), im1.channels());
    for (int c = 0; c < im1.channels(); c++) {
        for (int h = 0; h < im1.height(); h++) {
            for (int w = 0; w < im1.width(); w++) {
                output(w, h, c) = im1(w, h, c) / c;
            } 
        }
    }
    return output;
}

// scalar/Image operations 
Image operator+ (const float & c, const Image & im1)
{
    Image output(im1.width(), im1.height(), im1.channels());
    for (int c = 0; c < im1.channels(); c++) {
        for (int h = 0; h < im1.height(); h++) {
            for (int w = 0; w < im1.width(); w++) {
                output(w, h, c) = c + im1(w, h, c);
            } 
        }
    }
    return output;
}
Image operator- (const float & c, const Image & im1)
{
    Image output(im1.width(), im1.height(), im1.channels());
    for (int c = 0; c < im1.channels(); c++) {
        for (int h = 0; h < im1.height(); h++) {
            for (int w = 0; w < im1.width(); w++) {
                output(w, h, c) = c - im1(w, h, c);
            } 
        }
    }
    return output;

}
Image operator* (const float & c, const Image & im1)
{
    Image output(im1.width(), im1.height(), im1.channels());
    for (int c = 0; c < im1.channels(); c++) {
        for (int h = 0; h < im1.height(); h++) {
            for (int w = 0; w < im1.width(); w++) {
                output(w, h, c) = c * im1(w, h, c);
            } 
        }
    }
    return output;
}
Image operator/ (const float & c, const Image & im1)
{
    Image output(im1.width(), im1.height(), im1.channels());
    for (int c = 0; c < im1.channels(); c++) {
        for (int h = 0; h < im1.height(); h++) {
            for (int w = 0; w < im1.width(); w++) {
                if (im1(w, h, c) == 0.0) {
                    throw DivideByZeroException();
                }
                output(w, h, c) = c / im1(w, h, c);
            } 
        }
    }
    return output;
}
// ---------------------- End of pset01 -----------------------------------

// ---------------------- Begin of pse02 -----------------------------------
float Image::smartAccessor(int x, int y, int z, bool clamp) const
{
    if (clamp) {
        if (x < 0) {
            x = 0;
        }
        if (x >= width()) {
            x = width() - 1;
        }
        if (y < 0) {
            y = 0;
        }
        if (y >= height()) {
            y = height() - 1;
        }
        return image_data[this->width()*this->height()*z + this->width() * y + x];
    } else {
        if (x < 0 || x >= width() || y < 0 || y >= height()) {
            return 0.0;
        }
        return image_data[this->width()*this->height()*z + this->width() * y + x];
    }
    return image_data[this->width()*this->height()*z + this->width() * y + x];
}

// ----------------------- End of pset02 -----------------------------------
