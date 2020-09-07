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
    for (int c = 0; c <= im.channels(); c++) {
        for (int h = 0; h <= im.height(); h++) {
            for (int w = 0; w <= im.width(); w++) {
                output(w, h, c) = im(w, h, c) * factor;
            }
        }
    }
    return output;
}