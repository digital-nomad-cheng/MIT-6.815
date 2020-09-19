#include "a8.hpp"

// Reference implementation for local max
Image<uint8_t> LocalMax_cpp(Image<uint8_t> input, int window) {
    int height = input.height(); 
    int width = input.width(); 

    Image<uint8_t> output(width,height);

    for (int y = 0; y < height; ++y)
    for (int x = 0; x < width; ++x) {

        // compute local max
        uint8_t maxi = 0;
        for (int wy = -window/2; wy < window-window/2; ++wy)
        for (int wx = -window/2; wx < window-window/2; ++wx) {
            if( x+wx < 0 || x+wx >= width ||
               y+wy < 0 || y+wy >= height )
            {
                continue;
            }
            if(input(x+wx, y+wy)>maxi) {
                maxi = input(x+wx, y+wy);
            }
        }

        // Is a local max?
        if(input(x, y) == maxi) {
            output(x, y) = 255;
        }
    }

    return output;
}

// Reference implementation for separable Gaussian
Image<uint8_t> Gaussian_cpp(Image<uint8_t> input, float sigma, float truncate) {
    int radius = sigma * truncate;
    int fwidth = 2 * radius + 1;

    // Precompute the kernel
    Image<float> kernel1D(fwidth);
    float s = 0.0f;
    for (int x = 0; x < fwidth; ++x) {
        kernel1D(x) = exp(-((x-radius)*(x-radius))/(2.0f*sigma*sigma));
        s += kernel1D(x);
    }
    // normalize it
    for (int x = 0; x < fwidth; ++x) {
        kernel1D(x) /= s;
    }

    int height = input.height(); 
    int width = input.width(); 
    Image<float> blur_x(width,height);
    Image<uint8_t> output(width,height);

    // Blur horizontal
    for (int y = 0; y < height; ++y) 
    for (int x = 0; x < width; ++x) {
        for (int i = 0; i < fwidth; ++i) {
            if(x+i < 0 || x+i >= width) {
                continue;
            }
            blur_x(x,y) += static_cast<float>(input(x+i-radius,y))*kernel1D(i);
        }
    }

    // Blur vertical
    for (int y = 0; y < height; ++y) 
    for (int x = 0; x < width; ++x) {
        float accum = 0.0f;
        for (int i = 0; i < fwidth; ++i) {
            if(y+i < 0 || y+i >= height) {
                continue;
            }
            accum += blur_x(x, y+i-radius)*kernel1D(i);
        }
        output(x,y) = static_cast<uint8_t>(accum);
    }

    return output;
}
