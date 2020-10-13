/* -----------------------------------------------------------------
 * File:    basic_image_manipulation.cpp
 * Created: 2020-09-10
 * -----------------------------------------------------------------
 * 
 * Assignment 03
 * We define and implement the functions we declared in basic_image_manipulation.hpp
 * 
 * ---------------------------------------------------------------*/

// This command imports all the content of basic_image_manipulation.hpp, thus declaring our functions.
#include "basic_image_manipulation.hpp"

// We use the namespace "std" so that we can write the command "cout" instead
// of the verbose version "std::cout". 
using namespace std;

Image brightness(const Image &im, float factor)
{
    Image output = Image(im.width(), im.height(), im.channels());
    // image are stored using w -> h -> c order 
    // these three for loops increase memory coherence
    for (int c = 0; c < im.channels(); c++) {
        for (int h = 0; h < im.height(); h++) {
            for (int w = 0; w < im.width(); w++) {
                output(w, h, c) = im(w, h, c) * factor;
            }
        }
    }
    return output;
}

Image contrast(const Image &im, float factor, float midpoint)
{
    // Iout = factor * (Iin - midpoint) + midpoint;
    Image output = Image(im.width(), im.height(), im.channels());
    // image are stored using w -> h -> c order 
    // these three for loops increase memory coherence
    for (int c = 0; c < im.channels(); c++) {
        for (int h = 0; h < im.height(); h++) {
            for (int w = 0; w < im.width(); w++) {
                output(w, h, c) = (im(w, h, c) - midpoint) * factor + midpoint;
            }
        }
    }
    return output;
}

Image color2gray(const Image &im, const std::vector<float> &weights)
{
    Image output = Image(im.width(), im.height(), 1);
    // image are stored using w -> h -> c order 
    // these three for loops increase memory coherence
    for (int h = 0; h < im.height(); h++) {
        for (int w = 0; w < im.width(); w++) {
            output(w, h) = im(w, h, 0) * weights[0] + im(w, h, 1) * weights[1] +  im(w, h, 2) * weights[2];
        }
    }
    return output;
}

std::vector<Image> lumiChromi(const Image &im)
{
    std::vector<Image> result;
    Image grayscale = color2gray(im);
    Image chrom = Image(im.width(), im.height(), im.channels());
    for (int h = 0; h < im.height(); h++) {
        for (int w = 0; w < im.width(); w++) {
            chrom(w, h, 0) = im(w, h, 0) / grayscale(w, h);
            chrom(w, h, 1) = im(w, h, 1) / grayscale(w, h);
            chrom(w, h, 2) = im(w, h, 2) / grayscale(w, h);
        }
    }
    result.push_back(grayscale);
    result.push_back(chrom);
    return result;
}

Image lumiChromi2rgb(const vector<Image> & lc) {
    // luminance is lc[0]
    // chrominance is lc[1]

    // Create chrominance images
    // We copy the input as starting point for the chrominance
    Image im = Image(lc[1].width(), lc[1].height(), lc[1].channels()); 
    for (int c = 0 ; c < im.channels(); c++ )
    for (int y = 0 ; y < im.height(); y++) 
    for (int x = 0 ; x < im.width(); x++)
    {
        im(x,y,c) = lc[1](x,y,c) * lc[0](x,y);
    }

    return im;
}

Image brightnessContrastLumi(const Image &im, float brightF, float contrastF, float midpoint)
{
    std::vector<Image> images = lumiChromi(im);
    Image lumi = images[0];
    Image chrom = images[1];
    lumi = brightness(lumi, brightF);
    lumi = contrast(lumi, contrastF, midpoint);

    Image result = Image(im.width(), im.height(), im.channels());
    for (int h = 0; h < im.height(); h++) {
        for (int w = 0; w < im.width(); w++) {
            result(w, h, 0) = chrom(w, h, 0) * lumi(w, h);
            result(w, h, 1) = chrom(w, h, 1) * lumi(w, h);
            result(w, h, 2) = chrom(w, h, 2) * lumi(w, h);
        }
    }
    return result;
}

Image rgb2yuv(const Image &im)
{
    Image output(im.width(), im.height(), im.channels());
    for (int h = 0; h < im.height(); h++) {
        for (int w = 0; w < im.width(); w++) {
            output(w, h, 0) = 0.299 * im(w, h, 0) + 0.587 * im(w, h, 1) + 0.114 * im(w, h, 2);
            output(w, h, 1) = -0.147 * im(w, h, 0) + (-0.289) * im(w, h, 1) + 0.436 * im(w, h, 2);
            output(w, h, 2) = 0.615 * im(w, h, 0) + (-0.515) * im(w, h, 1) + (-0.100) * im(w, h, 2);
        }
    }
    return output;

}

Image yuv2rgb(const Image &im)
{
    Image output(im.width(), im.height(), im.channels());
    for (int h = 0; h < im.height(); h++) {
        for (int w = 0; w < im.width(); w++) {
            output(w, h, 0) = 1 * im(w, h, 0) + 1.14 * im(w, h, 2);
            output(w, h, 1) = 1 * im(w, h, 0) + (-0.395) * im(w, h, 1) + (-0.581) * im(w, h, 2);
            output(w, h, 2) = 1 * im(w, h, 0) + 2.032 * im(w, h, 1);
        }
    }
    return output;
}

Image saturate(const Image &im, float k)
{
    Image yuv = rgb2yuv(im);
    for (int h = 0; h < im.height(); h++) {
        for (int w = 0; w < im.width(); w++) {
            yuv(w, h, 1) *= k;
            yuv(w, h, 2) *= k;
        }
    }
    return yuv2rgb(yuv);
}

Image add_black_dot(const Image &im)
{
    Image output(im.width(), im.height(), im.channels());

    for (int h = 0; h < im.height(); h++) {
        for (int w = 0; w < im.width(); w++) {
            output(w, h, 0) = im(w, h, 0);
            output(w, h, 1) = im(w, h, 1);
            output(w, h, 2) = im(w, h, 2);
        }
    }
    for (int i = -1; i < 2; i++) {
        for (int j = -1; j < 2; j++) {
            output(floor(im.width()/2)+i, floor(im.height()/2)+j, 0) = 0;
            output(floor(im.width()/2)+i, floor(im.height()/2)+j, 1) = 0;
            output(floor(im.width()/2)+i, floor(im.height()/2)+j, 2) = 0;
        }
    }
    output(floor(im.width()/2), floor(im.height()/2), 0) = 0;
    output(floor(im.width()/2), floor(im.height()/2), 1) = 0;
    output(floor(im.width()/2), floor(im.height()/2), 2) = 0;
    return output;
}

std::vector<Image> spanish(const Image &im)
{
    Image yuv = rgb2yuv(im);
    Image output(im.width(), im.height(), im.channels());
    for (int h = 0; h < yuv.height(); h++) {
        for (int w = 0; w < yuv.width(); w++) {
            output(w, h, 0) = 0.5;
            output(w, h, 1) = -yuv(w, h, 1);
            output(w, h, 2) = -yuv(w, h, 2);
        }
    }
    Image color = yuv2rgb(output);
    Image grayscale = color2gray(im);

    std::vector<Image> results;
    results.push_back(color);
    results.push_back(grayscale);
    return results;
}

Image grayworld(const Image & in)
{
    std::vector<float> means = get_mean_per_channel(in);
    float r = means[0];
    float g = means[1];
    float b = means[2];
    float red_alpha = g / r;
    float blue_alpha = b / r;
    Image output(in.width(), in.height(), in.channels());
    for (int h = 0; h < in.height(); h++) {
        for (int w = 0; w < in.width(); w++) {
            output(w, h, 0) = in(w, h, 0) * red_alpha;
            output(w, h, 1) = in(w, h, 1);
            output(w, h, 2) = in(w, h, 2) * blue_alpha;
        }
    }
    return output;
}

std::vector<float> get_mean_per_channel(const Image & im)
{
    std::vector<float> means;
    means.resize(3);
    int image_size = im.width() * im.height();
    for (int c = 0; c < im.channels(); c++) {
        for (int h = 0; h < im.height(); h++) {
            for (int w = 0; w < im.width(); w++) {
                means[c] = means[c] + im(w, h, c) / image_size;
            }
        }
    }
    return means;
}


/* ------------------------- pset05 ---------------------- */
Image scaleNN(const Image &im, float factor)
{
    int new_height = floor(im.height() * factor);
    int new_width = floor(im.width() * factor);
    Image output(new_width, new_height, im.channels());

    for (int c = 0; c < output.channels(); c++) {
        for (int h = 0; h < output.height(); h++) {
            for (int w = 0; w < output.width(); w++) {
                output(w, h, c) = im.smartAccessor(round(w / factor), round(h / factor), c);
            }
        }
    }
    return output;
}

Image scaleLin(const Image &im, float factor)
{
    int new_height = floor(im.height() * factor);
    int new_width = floor(im.width() * factor);
    Image output(new_width, new_height, im.channels());

    for (int c = 0; c < output.channels(); c++) {
        for (int h = 0; h < output.height(); h++) {
            for (int w = 0; w < output.width(); w++) {
                if ((w / factor - round(w / factor)) == 0.0 && (h / factor - round(h / factor) == 0.0)) {
                     output(w, h, c) = im.smartAccessor(w / factor, h / factor, c);
                } else {
                    output(w, h, c) = interpolateLin(im, w / factor, h / factor, c);
                }
            }
        }
    }
    return output;
}

float interpolateLin(const Image &im, float x, float y, int z, bool clamp)
{
    int floor_x = floor(x);
    int floor_y = floor(y);
    int ceil_x = ceil(x);
    int ceil_y = ceil(y);

    float value;
    if (ceil_x == floor_x) {
        value = im.smartAccessor(ceil_x, floor_y, z)*(ceil_y - y) + im.smartAccessor(ceil_x, ceil_y, z)*(y - floor_y);
    } else if (ceil_y == floor_y) {
        value = im.smartAccessor(floor_x, floor_y, z)*(ceil_x - x) + im.smartAccessor(ceil_x, ceil_y, z)*(x - floor_x);
    } else {
        float top = im.smartAccessor(floor_x, floor_y, z)*(ceil_x - x) + im.smartAccessor(ceil_x, floor_y, z)*(x - floor_x);
        float bottom = im.smartAccessor(floor_x, ceil_y, z)*(ceil_x - x) + im.smartAccessor(ceil_x, ceil_y, z)*(x - floor_x);
        value = top * (ceil_y - y) + bottom * (y - floor_y);
    }

    return value;
}

Image rotate(const Image &im, float theta)
{
    // Bug
    Image output = Image(im.width(), im.height(), im.channels());

    int center_x = im.width() / 2;
    int center_y = im.height() / 2;


    for (int c = 0; c < im.channels(); c++) {
        for (int h = 0; h < im.height(); h++) {
            for (int w = 0; w < im.width(); w++) {
            float orig_w = (static_cast<float>(w) - center_x) * cos(theta) +
             (center_y - static_cast<float>(h)) * sin(theta) + center_x;
            float orig_h = center_y - (-(static_cast<float>(w) - center_x) * sin(theta) +
                        (center_y - static_cast<float>(h)) * cos(theta));
            output(w, h, c) = interpolateLin(im, orig_w, orig_h, c);
            }
        }
    }
    return output;
}