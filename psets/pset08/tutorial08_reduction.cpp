// Halide tutorial lesson 8.

// This lesson illustrates reductions.

// We will compute the average RGB value of an image
// that is, we will return a 1D array with only 3 values

// One important thing to remember from this lesson is that there is never
// any explicit for loop in Halide, whether is't the loop over output pixels
// or the reduction loop over the input pixels for the reduction. It's all implicit

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
    // As usual, let's load an input
    Halide::Buffer<uint8_t> input = load_image("../Input/ante1-1.png");

    // Next we declaure the Vars and Func
    Var x("x"), y("y"), c("c");

    //// SUM ONLY
    {
        Func mySum("mySum");

        // The central tool to express a reduction is a reduction domain, called RDom
        // it corresponds to the bounds of the reduction loop you would write in an
        // imperative language. Here we want to iterate over a 2D domain corresponding
        // to the whole image.
        // Note however that we have decided to compute a different sum for each channel,
        // So we will not reduce over channels.
        RDom r(0, input.width(), 0, input.height());
        // careful, RDom are defined as base, extent, not min, max.
        // This means that RDom(a, b) goes from a to a+b (obviously not an issue here)

        // Given a reduction domain, we define the Expr that we will sum over, in this
        // case the pixel values. By construction, the first and second dimension of a
        // reduction domain are called x and y. In this case they happen to correspond
        // to the image x and y coordinates but they don't have to.
        // Note that x & y are the reduction variables but c is a normal Var.
        // this is because our sum is over x,y but not over c. There will be a different
        // sum for each channel
        Expr val = input(r.x, r.y, c);

        // A reduction Func first needs to be initialized. Here, our sum gets initialized to 0
        // Note that the function domain here is only the channel.
        mySum(c) = 0.0f;

        // Finally, we define what the reduction should do for each reduction value.
        // In this case, we eant to add each reduction value to the output
        // This is called the update function, and it's going to be called for each
        // location in the RDom.
        // You never write an explicit loop over the RDom, Halide does it for you.
        mySum(c) +=val;

        // We now call realize() to compile and execute.
        Halide::Buffer<float> output = mySum.realize(input.channels());

        cout << "Computed sum: ";
        for (int i=0; i<output.width(); i++) {
            cout << output(i) << " ";
        }
        cout << endl;
    }

    //// AVERAGE
    {
        Func mySum    ("mySum");
        Func myAverage("myAverage");

        // The central tool to express a reduction is a reduction domain, called RDom
        // it corresponds to the bounds of the reduction loop you would write in an
        // imperative language. Here we want to iterate over a 2D domain corresponding
        // to the whole image. Note however that we will note reduce over channels.
        RDom r(0, input.width(), 0, input.height(), "r");

        // Given a reduction domain, we define the Expr that we will sum over, in this
        // case the pixel values. By construction, the first and second dimension of a
        // reduction domain are called x and y. In this case they happen to correspond
        // to the image x and y coordinates but they don't have to.
        // Note that x & y are the reduction variables but c is a normal Var.
        // this is because our sum is over x,y but not over c. There will be a different
        // sum for each channel.
        Expr val= input(r.x, r.y, c);

        // A reduction Func first needs to be initialized. Here, our sum gets initialized at 0
        // Note that the function domain here is only the channel.
        mySum(c)=0.0f;

        // Finally, we define what the reduction should do for each reduction value.
        // In this case, we eant to add each reduction value

        // update function is going to be called....
        // can be any Expr  as a function of mySum(c) and the RDom
        mySum(c) += val;
        // as usual, the short version is
        // mySum(c) += input(r.x, r.y, c)

        // Finally, we define our final Func as the sum divided by the image number of pixels.
        myAverage(c)=mySum(c)/(input.width()*input.height());

        // As usual, all we have done so far is create a Halide internal representation.
        // We now call realize() to compile and execute.
        Halide::Buffer<float> output = myAverage.realize(input.channels());

        cout << "Computed average ";
        for (int i=0; i<output.width(); i++) {
            cout << output(i) << " ";
        }
        cout << endl;
    }

    // let's extend the example above with an extra reduction stage that computes the
    // average across the three channels
    {
        Func mySum("mySum");
        Func myAverage("myAverage");
        RDom r(0, input.width(), 0, input.height(), "r");

        // compute the sum and average as above
        mySum(c)  = 0.0f;
        mySum(c) += input(r.x, r.y, c);
        myAverage(c) =mySum(c)/(input.width()*input.height());

        // compute the average across channels by defining a RDom that
        // loops over channels
        Func avAcrossChannel("avAcrossChannel");
        RDom k(0, input.channels(), "rChannel");
        avAcrossChannel()  = 0.0f;
        avAcrossChannel() += myAverage(k.x) / input.channels();

        // size of output is 1 number
        Halide::Buffer<float> output = avAcrossChannel.realize();

        cout << "Computed average across channels " << output(0) << endl;
    }

    return EXIT_SUCCESS;
}
