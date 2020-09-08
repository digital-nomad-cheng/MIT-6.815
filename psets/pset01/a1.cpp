/* -----------------------------------------------------------------
 * File:    a1.cpp
 * Created: 2015-09-09
 * -----------------------------------------------------------------
 * 
 * Implementation file for PS01
 *
 * We define and implement the functions we declared in a1.h
 * 
 * ---------------------------------------------------------------*/

// This command imports all the content of a1.h, thus declaring our functions.
#include "a1.h"

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