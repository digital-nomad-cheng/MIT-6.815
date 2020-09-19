// Halide tutorial lesson 5

// This lesson demonstrates how to manipulate the order in which you
// evaluate pixels in a Halide::Func, including vectorization,
// parallelization, unrolling, and tiling.

// A Halide algorithm only specifies the formula to compute  pixel values
// but it does not specify the order of execution and where/when things get stored
// This is the job of the Halide schedule.
// Once we have defined the algorithm of a Halide::Func (the formula as given by an Expr),
// we can specify its schedule by calling methods of our C++ class Halide::Func.
// For example, we specify that a Halide::Func f should gets computed in tiles by calling
// f.tile() with the proper arguments.
// Under the hood scheduling results in a series of nested loops that fully specify
// the execution order.
// By default, Halide::Funcs get scheduled in regular order:
//
// for y {
//     for x {
//         ...
//     }
// }

// Compile: g++ tutorial05*.cpp -g -I /Users/vincent/Libs/Halide/tools/ -L /usr/local/lib/ -lHalide `libpng-config --cflags --ldflags` -ljpeg -o tutorial05 -std=c++11
#include <iostream>
#include <cmath>
#include <Halide.h>
#include <halide_image_io.h>

using std::cout;
using std::endl;


int main(int argc, char** argv)
{
    // We're going to define and schedule our smooth gradient function in
    // several different ways, and see what order pixels are computed
    // in.

    // Each different schedule will produce its own psuedo code in a separate Halide::HTML
    // file. You can see these files in the Output folder to understand the loop
    // nests produced by the corresponding schedules

    Halide::Var x("x"), y("y");
    Halide::Var x_outer("x_outer"), x_inner("x_inner");
    Halide::Var y_outer("y_outer"), y_inner("y_inner");
    Halide::Var tile_index("tile_index");

    Halide::Buffer<int> out_buffer(Halide::type_of<int>(), 4, 4);

    // Let's try different schedules for the same Halide function.
    // Each schedule will create a Halide::HTML file that shows its generated pseudo-code
    // This should help you undersand the scheduling operations.

    std::string out_dir = "Output/";

    ////// First we observe the default ordering.
    {
        Halide::Func gradient("tut5_gradient");
        gradient(x, y) = x + y;
        // By default we walk along the rows and then down the columns.

        // Check the .html output to see the loop ordering in action
        cout << "Evaluating gradient row-major" << endl;
        gradient.compile_to_lowered_stmt(out_dir+gradient.name()+".html", {out_buffer}, Halide::HTML);
        gradient.realize(out_buffer);

        // The evaluation order is:
        //  1  2  3  4
        //  5  6  7  8
        //  9 10 11 12
        // 13 14 15 16
    }

    ////// Reorder variables.
    {
        // We need to declare and define a new version of the gradient.
        Halide::Func gradient("tut5_gradient_col_major");
        gradient(x, y) = x + y;

        // If we reorder x and y, we can first walk down the columns instead of
        // the rows. The reorder call takes the arguments of the func, and sets
        // a new nesting order for the for loops that are generated. The
        // arguments are specified from the innermost loop out, so the following
        // call puts y in the inner loop:

        gradient.reorder(y, x);

        cout << "Evaluating gradient column-major" << endl;
        gradient.compile_to_lowered_stmt(out_dir+gradient.name()+".html", {out_buffer}, Halide::HTML);
        gradient.realize(out_buffer);

        // Evaluation order:
        //  1  5  9 13
        //  2  6 10 14
        //  3  7 11 15
        //  4  8 12 16
    }

    ////// Split a variable into two.
    {
        Halide::Func gradient("tut5_gradient_split");
        gradient(x, y) = x + y;

        // The most powerful primitive scheduling operation you can do
        // to a var is to split it into inner and outer sub-variables:
        // First we need to declare the outer and inner Halide::Var
        // then do the split and provide the size of the inner sub-range
        gradient.split(x, x_outer, x_inner, 2);

        // This breaks the loop over x into two nested loops: an outer
        // one over x_outer, and an inner one over x_inner. The last
        // argument to split is the "split factor". The inner loop
        // runs from zero to the split factor. The outer loop runs
        // from zero to the extent required of x (4 in this case)
        // divided by the split factor, which gives us 2. Within the loops, the old
        // variable is defined to be outer * factor + inner. If the
        // old loop started at a value other than zero, then that is
        // also added within the loops.

        // Note that the order of evaluation of pixels didn't actually
        // change! Splitting by itself does nothing, but it does open
        // up all of the scheduling possibilities that we below.

        std::cout <<  "Evaluating gradient with x split into x_outer and x_inner" << endl;
        gradient.compile_to_lowered_stmt(out_dir+gradient.name()+".html", {out_buffer}, Halide::HTML);
        gradient.realize(out_buffer);

        // Evaluation order:
        //  1  2  3  4
        //  5  6  7  8
        //  9 10 11 12
        // 13 14 15 16

        // Values of xo    Values of xi
        //  0  0  1  1      0  1  0  1
        //  0  0  1  1      0  1  0  1
        //  0  0  1  1      0  1  0  1
        //  0  0  1  1      0  1  0  1


        // Splitting by factors that don't divide the extent.

        // Splitting guarantees that the inner loop runs from zero to
        // the split factor, which is important for the uses we saw
        // above. So what happens when the total extent we wish to
        // evaluate x over isn't a multiple of the split factor? For example if
        // we evaluate gradient over a 5x4 box instead of the 4x4 box we've been
        // using.
        // The general rule is: If we require x from x_min to x_min + x_extent, and
        // we split by a factor 'factor', then:
        //
        // x_outer runs from 0 to (x_extent + factor - 1)/factor
        // x_inner runs from 0 to factor
        // x = min(x_outer * factor, x_extent - factor) + x_inner + x_min
        //
        // In our example, x_min was 0, x_extent was 5, and factor was 2.
    }

    //// Fuse two variables into one.
    {
        Halide::Func gradient("tut5_gradient_fused");
        gradient(x, y) = x + y;

        // The opposite of splitting is 'fusing'. Fusing two variables
        // merges the two loops into a single for loop over the
        // product of the extents. Fusing is less important that
        // splitting, but it also sees use (as we'll see later in this
        // lesson). Like splitting, fusing by itself doesn't change
        // the order of evaluation.
        Halide::Var fused("fused");
        gradient.fuse(x, y, fused);

        cout << "Evaluating gradient with x and y fused" << endl;
        gradient.compile_to_lowered_stmt(out_dir+gradient.name()+".html", {out_buffer}, Halide::HTML);
        gradient.realize(out_buffer);

        // Evaluation order:
        //  1  2  3  4
        //  5  6  7  8
        //  9 10 11 12
        // 13 14 15 16

        // Values of fused
        //  0  1  2  3   
        //  4  5  6  7  
        //  8  9 10 11 
        // 12 13 14 15
    }

    //// Evaluating in tiles.
    {
        Halide::Func gradient("tut5_gradient_tiled");
        gradient(x, y) = x + y;

        // Now that we can both split and reorder, we can do tiled
        // evaluation. Let's split both x and y by a factor of two,
        // and then reorder the vars to express a tiled traversal.
        //
        // A tiled traversal splits the domain into small rectangular
        // tiles, and outermost iterates over the tiles, and within
        // that iterates over the points within each tile. It can be
        // good for performance if neighboring pixels use overlapping
        // input data, for example in a blur. We can express a tiled
        // traversal like so:

        gradient.split(x, x_outer, x_inner, 2)
            .split(y, y_outer, y_inner, 2)
            .reorder(x_inner, y_inner, x_outer, y_outer);

        // This pattern is common and important enough that there's a shorthand for it:
        // gradient.tile(x, y, x_outer, y_outer, x_inner, y_inner, 2, 2)

        // We will work more with tiles in the next tutorial.
        // They are critical for high-performance image processing

        cout << "Evaluating gradient in 2x2 tiles" << endl;
        gradient.compile_to_lowered_stmt(out_dir+gradient.name()+".html", {out_buffer}, Halide::HTML);
        gradient.realize(out_buffer);

        // Evaluation order:
        //  1  2 |  5  6
        //  3  4 |  7  8
        // ------+------
        //  9 10 | 13 14
        // 11 12 | 15 16

        // Values of xo    Values of xi
        //  0  0  1  1      0  1  0  1
        //  0  0  1  1      0  1  0  1
        //  0  0  1  1      0  1  0  1
        //  0  0  1  1      0  1  0  1

        // Values of yo    Values of yi
        //  0  0  0  0      0  0  0  0
        //  0  0  0  0      1  1  1  1
        //  1  1  1  1      0  0  0  0
        //  1  1  1  1      1  1  1  1
    }

    {
        // Evaluating in vectors.

        Halide::Func gradient("tut5_gradient_in_vectors");
        gradient(x, y) = x + y;

        // The nice thing about splitting is that it guarantees the
        // inner variable runs from zero to the split factor. Most of
        // the time the split-factor will be a compile-time constant,
        // so we can replace the loop over the inner variable with a
        // single vectorized computation. This time we'll split by a
        // factor of four, because on X86 we can use SSE to compute in
        // 4-wide vectors.
        gradient.split(x, x_outer, x_inner, 4)
            .vectorize(x_inner);

        // Splitting and then vectorizing the inner variable is common
        // enough that there's a short-hand for it. We could have also
        // said:
        //
        // gradient.vectorize(x, 4)
        //
        // which is equivalent to:
        //
        // gradient.split(x, x, x_inner, 4)
        // gradient.vectorize(x_inner)
        //
        // Note that in this case we reused the name 'x' as the new
        // outer variable. Later scheduling calls that refer to x
        // will refer to this new outer variable named x.

        cout << "Evaluating gradient with x_inner vectorized" << endl;
        gradient.compile_to_lowered_stmt(out_dir+gradient.name()+".html", {out_buffer}, Halide::HTML);
        gradient.realize(out_buffer);
    }

    {
        // Unrolling a loop.

        Halide::Func gradient("tut5_gradient_in_vectors");
        gradient(x, y) = x + y;

        // If multiple pixels share overlapping data, it can make
        // sense to unroll a computation so that shared values are
        // only computed or loaded once. We do this similarly to how
        // we expressed vectorizing. We split a dimension and then
        // fully unroll the loop of the inner variable. Unrolling
        // doesn't change the order in which things are evaluated.
        gradient.split(x, x_outer, x_inner, 2);
        gradient.unroll(x_inner);

        // The shorthand for this is:
        // gradient.unroll(x, 2)

        cout << "Evaluating gradient unrolled by a factor of two" << endl;
        gradient.compile_to_lowered_stmt(out_dir+gradient.name()+".html", {out_buffer}, Halide::HTML);
        gradient.realize(out_buffer);
    }

    // Fusing, tiling, and parallelizing.
    {
        // We saw in the previous lesson that we can vectorize
        // a variable. Here we will parallelize across a variable and 
        // combine it with fusing and tiling to express a useful pattern:
        // processing tiles in parallel.

        // This is where fusing shines. Fusing helps when you want to
        // parallelize across multiple dimensions without introducing
        // nested parallelism. Nested parallelism (parallel for loops
        // within parallel for loops) is supported by Halide, but
        // often gives poor performance compared to fusing the
        // parallel variables into a single parallel for loop.

        Halide::Func gradient("tut5_gradient_fused_tiles");
        gradient(x, y) = x + y;

        // First we'll tile, then we'll fuse the tile indices and
        // parallelize across the outer fused variable.
        gradient
            .tile(x, y, x_outer, y_outer, x_inner, y_inner, 2, 2)
            .fuse(x_outer, y_outer, tile_index)
            .parallel(tile_index);

        // The scheduling calls all return a reference to the Halide::Func, so
        // you can also chain them together into a single statement to
        // make things slightly clearer.

        cout << "Evaluating gradient tiles in parallel" << endl;
        gradient.compile_to_lowered_stmt(out_dir+gradient.name()+".html", {out_buffer}, Halide::HTML);
        gradient.realize(out_buffer);

        // The tiles can be evaluated in arbitrary order because of the
        // parallelization, but within each tile the pixels will be traversed in
        // row-major order.
    }

    // Putting it all together.
    // Are you ready? Now, we're going to use all of the features above to make
    // our filter fast:
    {
        Halide::Func gradient_fast("gradient_fast");
        gradient_fast(x, y) = Halide::cast<uint8_t>(255.0f*Halide::cast<float>(x + y)/(800+600));
        // (we normalize so that the output is viewable)

        // We'll process 256x256 tiles in parallel.
        gradient_fast.tile(x, y, x_outer, y_outer, x_inner, y_inner, 256, 256)
            .fuse(x_outer, y_outer, tile_index)
            .parallel(tile_index);

        // We'll compute two scanlines at once while we walk across
        // each tile. We'll also vectorize in x. The easiest way to
        // express this is to recursively tile again within each tile
        // into 4x2 subtiles, then vectorize the subtiles across x and
        // unroll them across y:
        Halide::Var x_inner_outer("x_inner_outer"), y_inner_outer("y_inner_outer");
        Halide::Var x_vectors("x_vectors"), y_pairs("y_pairs");
        gradient_fast.tile(x_inner, y_inner, x_inner_outer, y_inner_outer, x_vectors, y_pairs, 4, 2)
            .vectorize(x_vectors)
            .unroll(y_pairs);

        // Note that we didn't do any explicit splitting or
        // reordering. Those are the most important primitive
        // operations, but mostly they are buried underneath tiling,
        // vectorizing, or unrolling calls.

        // Now let's evaluate this over a range which is not a
        // multiple of the tile size.

        // if you had turned on HL_TRACE the following will produce a lot of output
        Halide::Buffer<int> out(Halide::type_of<int>(), 800, 600);
        gradient_fast.compile_to_lowered_stmt(out_dir+gradient_fast.name()+".html", {out}, Halide::HTML);
        Halide::Buffer<uint8_t> out_image = gradient_fast.realize(800, 600);
        Halide::Tools::save_image(out_image, "Output/fast_smooth_gradient.png");

        // Note that in the Halide , the algorithm is specified
        // once at the top, separately from the optimizations, and there
        // aren't that many lines of code total. Compare this to the C
        // version (or an approximation of it in the .html file). There's more
        // code (and it isn't even parallelized or vectorized properly). More
        // annoyingly, the statement of the algorithm (the result is x plus y)
        // is buried in multiple places within the mess. This C code is hard to
        // write, hard to read, hard to debug, and hard to optimize further.
        // This is why Halide exists!
        // (We'll measure performances in the next tutorial)
    }

    cout << "Success!\n" << endl;

    return EXIT_SUCCESS;
}
