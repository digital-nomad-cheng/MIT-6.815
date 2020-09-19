// Halide tutorial lesson 9.

// This lesson illustrates convolution using reduction



#include <iostream>
#include <cmath>
#include <Halide.h>
#include <halide_image_io.h>
#include "timing.h"

using std::cout;
using std::endl;
using std::to_string;

using namespace Halide;
using namespace Halide::Tools;

void boxBlur(Halide::Buffer<uint8_t> input, int indexOfBlur=0)
{
    int kernel_width=5;

    Func blur("blur");
    Var x("x"), y("y"), c("c"); //declare domain variables

    // declare and define a clamping function that restricts x,y to the image size
    // boring but necessary
    Func clamped("clamped");
    clamped(x, y, c) = cast<float>(
            input(clamp(x, 0, input.width()-1), clamp(y, 0, input.height()-1), c));

    Func superBlur("superBlur");

    RDom r(0, kernel_width, 0, kernel_width, "r");

    // each of the convolution algorithms will generate the pseudo code in an HTML
    // file. This should help you understand the concepts in this tutorial. You will
    // eventually be using these ideas to implement Gaussian blur.

    switch(indexOfBlur)
    {
        case 0:
            cout << "Box blur as a simple reduction " << endl;

            // first version of our box blur. Simple but not efficient.
            // Our reduction domain will cover the footprint of the kernel
            // It corresponds to the bounds of the inner loop you would write
            // in C++ for the output of each pixel
            // next we initialize the convolution sum at each pixel to zero
            // Finally, the update equation describes what happens at each reduction location
            // It will be called for each value in the reduction domain r.x, r.y and for each
            // free variable triplet x, y, c
            // in this case, the reduction domain r.x and r.y get added to the free variables
            // x and y but it doesn't need to be the case in general for reductions.

            blur(x,y,c) = 0.0f;
            blur(x,y,c) += clamped(x+r.x-kernel_width/2, y+r.y-kernel_width/2, c);

            // The reduction loops are always outside the free variable loops to
            // ensure unambiguous semantic.
            // This can be a problem for convolutions, because the above order
            // has very poor locality. For each iteration of rx, ry, we need to access
            // data all over the image.
            // Unfortunately, Halide's .reorder command does not allow you to swap the
            // order of reduction and free variables because it could change the semantic
            // (for example if we use reductions over time to perform a simulation such
            //    as a game of life. If we put the loop over x y outside that of time,
            // the results would be very different)
            // Fortunately there is a trick that fixes things in cases like convolution
            // where the calculations at different free variable locations are independent.
            break;

        case 1:
            cout << "Box blur with the helper/inline trick " << endl;
            // We now introduce the helper / inline trick to get a better loop order
            // We will add one pointiwise stage after the reduction. The reduction will get
            // be scheduled with the default: inline. As a result, Halide will inline the
            // whole reduction for each triplet x, y, c. We will see that it all simplifies
            // into what we want: the outer loops will be on x,y, c and the reduction variable
            // will, in practice, end up as inner loops
            blur(x,y,c) = 0.0f;
            blur(x,y,c) += clamped(x+r.x-kernel_width/2, y+r.y-kernel_width/2, c);

            // But we now add an extra stage that consumes the output of our reduction
            superBlur(x,y,c) = blur(x,y,c);

            // just so that the realize() code uses the actual result superBlur
            blur = superBlur;

            // That's it. Simply adding this stage and letting it scheduled as inline (the default)
            // wil trick Halide into putting free variable loops outside of teh reduction loops.
            // Let's see why.
            // As usual, everything starts with the consumer. Halide schedules superBlur with loops
            // for the free variables x, y, c.
            // Then it inlines blur inside the innermost loop of superBlur. At this point, x, y, c
            // are known and have a single value each. Inlining means that we dump the code needed to
            // compute the producer for all the values needed by the consumer. Here we just need value(s)
            // for a single triplet x,y,c.  Halide generates the blur code for domain that covers only
            // this triplet. In a sense, from the perspective of blur, teh reduction variables are still
            // the outer loop and there are inner loops for x, y, and c but they are restricted to a single
            // iteration each. Let's loop at teh equivalent Python code to make this clearer
            break;

        case 2:
            cout << "Box blur with the *sum* syntactic sugar " << endl;

            // The helper/inline trick and convolutions are so common that
            // there is a sugar called 'sum' sum takes care of teh
            // initialization (to 0), the extra inilined stage and the update.
            // The syntax then becomes

            blur(x,y,c) = sum(clamped(
                        x+r.x-kernel_width/2,
                        y+r.y-kernel_width/2, c));
            break;

        default: break;
    }

    std::string file = "Output/tut9_" + blur.name() + "_" + to_string(indexOfBlur)+".html";

    blur.compile_jit();

    Buffer<float> buffer(type_of<float>(), input.width(), input.height(), input.channels());
    blur.compile_to_lowered_stmt(file, {buffer}, HTML);

    profile(blur, input.width(), input.height(), input.channels());
}

int main(int argc, char** argv)
{
    Halide::Buffer<uint8_t> im = load_image("../Input/ante1-1.png");

    for (int i=0; i<3; i++) {
        boxBlur(im, i);
    }

    return EXIT_SUCCESS;
}
