// Halide tutorial lesson 7.

// This lesson demonstrates use select statement to implement if-then-else statements

#include <iostream>
#include <cmath>
#include <Halide.h>
#include <halide_image_io.h>

using std::cout;
using std::endl;

using namespace Halide;
using namespace Halide::Tools;


int main(int argc, char** argv)
{
    // This program defines a single-stage imaging pipeline that
    // brightens an image.

    // First we'll load the input image we wish to brighten.
    // We'll use imageIO to get a numpy array from a PNG image
    Halide::Buffer<uint8_t> input = load_image("../Input/ante1-1.png");

    // Next we declare our Func object that represents our one pipeline
    // stage.
    Func sel;

    // Our Func will have three arguments, representing the position
    // in the image and the color channel. Halide treats color
    // channels as an extra dimension of the image, just like in numpy
    // let's declare the corresponding Vars before we can use them
    Var x, y, c;

    // Finally define the function.
    sel(x, y, c) = cast<uint8_t>(select(input(x,y,c)<127, 0, 255));

    // Remember. All we've done so far is build a representation of a
    // Halide program in memory. We haven't actually processed any
    // pixels yet. We haven't even compiled that Halide program yet.

    // So now we'll realize the Func. The size of the output image
    // should match the size of the input image. If we just wanted to
    // brighten a portion of the input image we could request a
    // smaller size. If we request a larger size Halide will throw an
    // error at runtime telling us we're trying to read out of bounds
    // on the input image.
    Halide::Buffer<uint8_t> output = sel.realize(input.width(), input.height(), input.channels());

    save_image(output, "Output/tut7_select.png");

    cout << "Success!\n" << endl;
    return EXIT_SUCCESS;
}
