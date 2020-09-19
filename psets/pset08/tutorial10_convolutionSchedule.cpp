// Halide tutorial lesson 10.

// This lesson illustrates scheduling for convolution using reduction
//
// Among othe things it runs different schedules with varying tile widths
// to illustrate the effect of tile widths on performance.
//
// This tutorial will take a long time to run. It is better to run it as the
// following command:
//
// ./tutorial10_convolutionSchedule > output.txt
//
// You can then open output.txt to see the performance of all the schedules

#include <iostream>
#include <cmath>
#include <Halide.h>
#include <halide_image_io.h>
#include "timing.h"

using std::cout;
using std::endl;

using namespace Halide;
using namespace Halide::Tools;


void boxBlur(Halide::Buffer<uint8_t> input, int indexOfSchedule, int tileX=128, int tileY=128) {
    // we will compute a 5x5 box blur
    int vectorWidth=4;
    int kernel_width=5;

    // Declaration of Vars and Funcs
    Var x("x"), y("y"), c("c");
    Var xi("xi"), yi("yi"), xo("xo"), yo("yo");

    Func blur_x("blur_x");
    Func blur_y("blur_y");
    Func blur  ("blur");

    // declare and define a clamping function that restricts x,y to the image size
    // boring but necessary
    Func clamped("clamped");
    clamped(x, y, c) = cast<float>(input(clamp(x, 0, input.width()-1),
                                         clamp(y, 0, input.height()-1), c));

    // Since we're using separable convolution, each stage is a 1D sum.
    // First, we'll define the horizontal blur.

    // Note how the reduction domain below is 1D
    RDom rx(0, kernel_width, "rx");
    blur_x(x,y,c) = 0.0f;
    blur_x(x,y,c) += clamped(x+rx.x-kernel_width/2, y, c);

    Func clampedBlurx("clampedBlurx");
    clampedBlurx(x, y, c) = blur_x(clamp(x, 0, input.width()-1),
                              clamp(y, 0, input.height()-1), c);



    // the first coordinate of a reduction domain is always called x.
    // This might be a little ocnfusing here, because we want to use the 1D
    // reduction domain for a sumamtion over y.
    // Not the ry.x below in the y coordinate calculation.
    RDom ry(0, kernel_width, "ry");
    blur_y(x,y,c) = 0.0f;
    blur_y(x,y,c) += clampedBlurx(x, y+ry.x-kernel_width/2, c);

    // Finally we normalize
    blur(x,y,c) = cast<uint8_t>(blur_y(x,y,c)/(kernel_width*kernel_width));

    //schedule attempt
    cout << "Schedule " << indexOfSchedule;

    switch (indexOfSchedule)
    {
        case 0:
            cout << ": default schedule";
            // our first schedule is the default schedule.
            // Everything gets inlined.
            // This reduces to a brute force 2D non-spearable blur.
            break;

        case 1:
            cout << ": root first stage";
            // To get a more reasonable schedule for non-tiny filter sizes and
            // avoid redudant calculation, we schedule as root the stages that are
            // consumed by a stencil consumer, i.e. a consumer that will use each
            // produced value multiple times.
            // In our case, this means scheduling teh first stage as root.
            // We will compute the values of blur_x for the whole image before moving on
            // to the next stage
            clampedBlurx.compute_root();
            break;

        case 2:
            cout << ": tile " << tileX << "x" << tileY << "+interleave";
            // We next seek to strike a balance between the lack of redundancy
            // of root and the locality of inline. For this, we schedule the last stage as tile
            // and compute a tile of teh first stage as needed just before computing
            // the corresponding tiel of teh consumer. If the tiles are small enough,
            // they will stay in cache.

            blur.tile(x, y, xo, yo, xi, yi, tileX, tileY);

            // tile is a shorthand for split+reorder
            // it will generate teh following nested loops:
            // for yo:
            //    for xo:
            //        for yi:
            //            for xi:

            // specify that the prducer stage should be scheduled at the xo granularity
            // of the consumer. It will produce a tile of blur_x before the cmputation
            // of a tile of the final blur

            clampedBlurx.compute_at(blur, xo);
            break;

        case 3:
            cout << ": tile " << tileX << "x" << tileY << "+parallel";
            // Same tiled and interleaved schedule, but computed in parallel over rows of tiles
            // simply add the .parallel command
            blur.tile(x, y, xo, yo, xi, yi, tileX, tileY).parallel(yo);
            // blur_x 'scheduling commands are unchanged, but it inherits blur's parallelism
            // because it is computed at the level of blur's xo, which is inside the yo
            // parallel loop
            clampedBlurx.compute_at(blur, xo);
            break;

        case 4:
            cout << ": tile " << tileX << "x" << tileY << "+vector";
            // same as above except that we are using vectorization instead of parallelism
            blur.tile(x, y, xo, yo, xi, yi, tileX, tileY).vectorize(xi, vectorWidth);
            clampedBlurx.compute_at(blur, xo);
            break;

        case 5:
            cout << ": tile " << tileX << "x" << tileY << "+parallel+vector";
            // same as above except that we are using both parallelism and vectorization
            blur.tile(x, y, xo, yo, xi, yi, tileX, tileY).parallel(yo).vectorize(xi, vectorWidth);
            clampedBlurx.compute_at(blur, xo);
            break;

        case 6:
            cout << ": tile " << tileX << "x" << tileY << "+parallel+vector without interleaving";
            // FInally we check that parallelism without locality is not enough.
            // We still perform computation in tiles but do not interleave blur_x and blur
            // anymore. This means that we compute all the tiles of blur_x before computing
            // any final tile.
            // Tiling is still useful to extract parallelism and also it allows us to keep
            // everything but interleaving equal compared to schedule 3.
            blur.tile(x, y, xo, yo, xi, yi, tileX, tileY).parallel(yo).vectorize(xi, vectorWidth);
            // we now need to specify blur_x's tiling and paralelism because it is
            // not schedule at the granularity of blur's tiles anymore.
            clampedBlurx.compute_root().tile(x, y, xo, yo, xi, yi, tileX, tileY).parallel(yo);
            break;

        default:
            break;
    }

    // compile
    blur.compile_jit();

    profile(blur, input.width(), input.height(), input.channels());
}


int main(int argc, char** argv)
{
    Halide::Buffer<uint8_t> im = load_image("../Input/ante1-1.png");

    for (int i=0; i<7; i++) {
        if (i<2) {
            boxBlur(im, i);
        }
        else {
            int min_tile = 20;
            int max_tile = 400;
            for (int tileY=min_tile; tileY<=max_tile; tileY+=2*min_tile) {
                for (int tileX=min_tile; tileX<=max_tile; tileX+=2*min_tile) {
                    boxBlur(im, i, tileX, tileY);
                }
            }
        }
        cout << endl;
    }

    return EXIT_SUCCESS;
}
