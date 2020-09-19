// Halide tutorial lesson 6.

// This lessons demonstrates the scheduling of a two-stage pipeline
// We will see how to schedule one (earlier) producer stage with respect to
// its consumer.

// We will implement a separable 3x3 box blur and explore different schedules

// Compile: g++ tutorial06*.cpp -g -I /Users/vincent/Libs/Halide/tools/ -L /usr/local/lib/ -lHalide `libpng-config --cflags --ldflags` -ljpeg -o tutorial06 -std=c++11
#include <iostream>
#include <cmath>
#include <Halide.h>
#include <halide_image_io.h>
#include "timing.h"

using std::cout;
using std::endl;


int main(int argc, char** argv)
{
    //load the input, convert to single channel and turn into Halide Halide::Buffer
    Halide::Buffer<uint8_t> im = Halide::Tools::load_image("../Input/ante1-1.png");

    int width = im.width();
    int height = im.height();

    ////Declarations of Halide variable and function names

    Halide::Var x("x"), y("y"); //declare domain variables
    Halide::Var xo("xo"), yo("yo"), xi("xi"), yi("yi");// Declare the inner tile variables

    // 3 channel to 1 channel conversion: we should take luminance
    // but here we only want to test schedules, so it does not matter.
    Halide::Func input("input");
    input(x,y) = cast<float>(im(x,y,0));

    float refTime = 0.0f;

    //////// SCHEDULE 1 : ROOT ////////
    {
        Halide::Func blur_x("blur_x"); //declare the horizontal blur function
        Halide::Func blur_y("blur_y"); //declare the vertical blur function
        blur_x(x,y) = cast<float>(input(x,y)+input(x+1,y)+input(x+2,y))/3.0f;
        blur_y(x,y) = (blur_x(x,y)+blur_x(x,y+1)+blur_x(x,y+2))/3.0f;

        // C++ style schedule: root. We compute the first stage (blur_x)
        // on the entire image before computing the second stage.
        //blur_y.compute_root();
        //blur_x.compute_root();

        blur_y.split(x, xo, xi, 2);
        blur_x.compute_at(blur_y, y);

        // Debug Halide::HTML
        Halide::Buffer<uint8_t> b(type_of<uint8_t>(),im.height(),im.width());
        blur_y.compile_to_lowered_stmt("./Output/LocaLM.html", {b}, Halide::HTML);

        // this schedule has bad locality because the data produced by blur_x
        // are long ejected from the cache by the time blur_y needs them
        // it also doesn"t have any parallelism
        // But it doesn"t introduce any extra computation

        // Finally compile and run.
        // Note that subtract two to the height and width to avoid boundary issues
        cout << "\nSchedule 1, ROOT:\n";
        float t = profile(blur_y, width-2, height-2);
        refTime = t;
    }


    //////// SCHEDULE 2 : INLINE ////////
    {
        Halide::Func blur_x("blur_x"); //declare the horizontal blur function
        Halide::Func blur_y("blur_y"); //declare the vertical blur function
        blur_x(x,y) = cast<float>(input(x,y)+input(x+1,y)+input(x+2,y))/3.0f;
        blur_y(x,y) = (blur_x(x,y)+blur_x(x,y+1)+blur_x(x,y+2))/3.0f;

        // In this schedule, we compute values for blur_x each time they are
        // required by blur_y (inline). This means excellent locality between
        // producer and consumer, since blur_x values are produced as needed and
        // directly consumed by blur_y.
        // However, this introduces significant redundant computation since each
        // blur_x value is recomputed 3 times, once for each blur_y computation
        // that needs it

        blur_y.compute_root();
        blur_x.compute_inline();

        // inline is the default schedule, however. This makes it easy to express
        // long expressions as chains of Halide::Funcs without paying a performance price
        // in general, inline is good when the dependency is a single pixel
        // (no branching factor that would introduce redundant computation)
        cout << "\nSchedule 2, INLINE:\n";
        float t = profile(blur_y, width-2, height-2);
        cout << "  - speedup vs. root: "<< (refTime/t) << endl;

        // In effect, this schedule turned a separable blur into the brute force
        // 2D blur
        // The compiler would also  merge the various divisions by 3
        // into a single division by 9, or better a multiplication by the reciprocal.
        // The reciprocal probably would stay in register, making everything blazingly fast.
        // Compilers can be pretty smart.
    }

    //////// SCHEDULE 3 : TILING and INTERLEAVING ////////
    {
        Halide::Func blur_x("blur_x"); //declare the horizontal blur function
        Halide::Func blur_y("blur_y"); //declare the vertical blur function
        blur_x(x,y) = cast<float>(input(x,y)+input(x+1,y)+input(x+2,y))/3.0f;
        blur_y(x,y) = (blur_x(x,y)+blur_x(x,y+1)+blur_x(x,y+2))/3.0f;

        // This is a good schedule (good locality, limited redundancy) that performs
        // computation in tiles and interleaves the two stages of the pipeline within a tile

        // First schedule the last (output) stage
        // In Halide, the schedule is always driven by the output
        // Earlier stages are scheduled with respect to later schedules
        // That is, we schedule a producer with respect to its consumer(s)

        blur_y.tile(x, y, xo, yo, xi, yi, 256, 32); //compute in tiles of 256x32
        // There is also a shorter version of the tile syntax that reuses the original
        // Halide::Vars x, y for the outer tile indices:
        // blur_y.tile(x, y, xi, yi, 256, 32)

        // We now specify when the earlier (producer) stage blur_x gets evaluated.
        // We decide to compute it at the tile granularity of blur_y and use the
        // compute_at method.
        // This means that blur_x will be evaluated in a loop nested inside the
        // "xo" outer tile loop of blur_y
        // note that we do not need to specify yo, xi, yi and they are directly
        // inherited from blur_y"s scheduling
        // More importantly, Halide performs automatic bound inference and enlarges
        // the tiles to make sure that all the inputs needed for a tile of blur_y
        // are available. In this case, it means making the blur_x tile one pixel
        // larger above and below to accomodate the 1x3 vertical stencil of blur_y
        // This is all done under the hood and the programmer doesn"t need to worry
        // about it
        blur_x.compute_at(blur_y, xo);

        // This schedule achieves better locality than root but with a lower redundancy
        // than inline. It still has some redundancy because of the enlargement at tile
        // boundaries

        cout << "\nSchedule 3: TILING\n";
        float t = profile(blur_y, width-2, height-2);
        cout << "  - speedup vs. root: "<< (refTime/t) << endl;
    }

    //////// SCHEDULE 4 : TILING, INTERLEAVING, and PARALLELISM ////////
    {
        Halide::Func blur_x("blur_x"); //declare the horizontal blur function
        Halide::Func blur_y("blur_y"); //declare the vertical blur function
        blur_x(x,y) = cast<float>(input(x,y)+input(x+1,y)+input(x+2,y))/3.0f;
        blur_y(x,y) = (blur_x(x,y)+blur_x(x,y+1)+blur_x(x,y+2))/3.0f;

        // This is a high-performance schedule that adds multicore and SIMD parallelism
        // to the tiled and interleaved schedule above.

        // First schedule the last (output) stage
        // We specify computation in tiles of 256x32
        blur_y.tile(x, y, xo, yo, xi, yi, 256, 32);
        // We then parallelize the for loop corresponding to the yo tile index
        // Halide will generate multithreaded code and runtime able to take advantage
        // of multicore processors.

        blur_y.parallel(yo);
        // We then specify that we want to use the SIMD vector units
        // (Single Instruction Multiple Data) and compute 8 pixels at once
        // Only try to vectorize the innermost loops.
        // There is no guarantee that the compiler will successfully achieve vectorization
        // For example, if you specify a width larger than what your processor can achieve,
        // it won"t work
        blur_y.vectorize(xi, 8);

        // the above three scheduling instructions can be piped into a more compact version:
        // blur_y.tile(x, y, xo, yo, xi, yi, 256, 32).parallel(yo).vectorize(xi, 8)
        // or with nicer formatting:
        // blur_y.tile(x, y, xo, yo, xi, yi, 256, 32)
        //       .parallel(yo)
        //       .vectorize(xi, 8)

        // We now specify when the earlier (producer) stage blur_x gets evaluated.
        // We decide to compute it at the tile granularity of blur_y and use the
        // compute_at method.
        // This means that blur_x will be evaluated in a loop nested inside the
        // "xo" outer tile loop of blur_y
        // since xo is nested inside blur_y"s yo and since yo is evaluated in parallel,
        // then blur_x will also be evaluated in parallel
        // Again, we don"t need to worry about bound expansion
        blur_x.compute_at(blur_y, xo);

        // We then specify that blur_x too should be vectorized
        // Unlike the parallelism that we inherited from blur_y"s yo loop,
        // vectorization needs to be specified again because its loop nest is lower than
        // the "compute_at" loop xo, whereas yo was above xo.
        // Note that blur_x gets vectorized at x whereas blur_y"s vectorize was called with xi
        // This is because blur_x does not have a notion of x_i. Its tiling piggybacked on that
        // of blur_y, and as far as blur_x is concerned, it just gets called for a tile and has
        // a single granularity of x and y for this tile (although of course vectorize then adds
        // a second.
        blur_x.vectorize(x, 8);


        // This schedule achieves the same excellent locality  and low redundancy
        // as the above tiling and fusion. In addition, it leverages high parallelism.

        cout << "\nSchedule 4: TILE & PARALLEL\n";
        float t = profile(blur_y, width-2, height-2);
        cout << "  - speedup vs. root: "<< (refTime/t) << endl;

        // the equivalent C++ code would be similar as above but with a parallel y loop
        // and a modified inner xi loop. In effect, vectorization adds an extra level of nesting
        // in strides of 8 but unrolls the innermost level into single vector instructions
    }

    cout << "\nSuccess!" << endl;
    return EXIT_SUCCESS;
}

////////////////////////// EXERCISES //////////////////////////////////////

// Report the runtime and throughout for the above 4 schedules in README.

// Write the equivalent C++ code for the following Halide schedules:
// for blur_x and blur_y defined in the same way
// You can assume that the image is an integer multiple of tile sizes when
// convenient.

// schedule 5:
// blur_y.compute_root()
// blur_x.compute_at(blur_y, x)

// schedule 6:
// blur_y.tile(x, y, xo, yo, xi, yi, 2, 2)
// blur_x.compute_at(blur_y, yo)

// schedule 7
// blur_y.split(x, xo, xi, 2)
// blur_x.compute_at(blur_y, y)
