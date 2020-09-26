/**
 * Instructions.
 *
 * This file contains a number of empty function stubs with the problem
 * statements. You need to fill in all details. These problems
 * are designed as extensions to the tutorials in this Pset. Please run
 * each tutorial and go through the code and comments. You are welcome
 * to discuss details on Piazza if you do not understand the tutorials.
 *
 * The main function is in a8_main.cpp. You can modify the contents for
 * your debugging purposes but restore the original contents.
 *
 * Live long and propser.
 */

#include <a8.hpp>

using namespace Halide;

// This applies a compute_root() schedule to all the Func's that are consumed
// by the calling Func. DO NOT EDIT IT.
void apply_auto_schedule(Func F) {
    map<string, Internal::Function> flist = Internal::find_transitive_calls(F.function());
    flist.insert(std::make_pair(F.name(), F.function()));
    map<string, Internal::Function>::iterator fit;
    for (fit=flist.begin(); fit!=flist.end(); fit++) {
        Func f(fit->second);
        f.compute_root();
        cout << "Warning: applying default schedule to " << f.name() << endl;
    }
    cout << endl;
}

// ---------------------------------- PART 1 -----------------------------------
// For all Halide Func's you write in Part 1, you do not need to worry about
// schedule. However, things will be faster if you just use:
// apply_auto_schedule(final_Result_Func);

Image<uint8_t> SmoothGradNormalized(void) {
    // // --------- HANDOUT  PS08 ------------------------------
    // Use Halide to compute a 512x512 smooth gradient where pixel (x,y) is
    // equal to x+y divided by 1024.
    //
    // HINT: tutorial 1 has a very similar function except that it does not
    // normalize by 1024
    //
    // OUPUT: a single channel output image of size 512x512 (uint8_t)
    //
    // SCHEDULE: use compute_root() on all the Func's you create (i.e.
    // use apply_auto_schedule(Func f).


    Halide::Func smooth_gradient("smooth_gradient");

    Halide::Var x("x");
    Halide::Var y("y");

    Halide::Expr e = x + y;

    e = Halide::cast<float>(e);

    e = e / 1024.0f * 255.f;

    e = Halide::cast<uint8_t>(e);
    
    smooth_gradient(x, y) = e;


    Image<uint8_t> smooth_output = smooth_gradient.realize(512, 512);


    // Everything worked! We defined a Func, then called 'realize' on
    // it to generate and run machine code that produced an Image.

    return smooth_output;
}

Image<uint8_t> WavyRGB(void) {
    // // --------- HANDOUT  PS08 ------------------------------
    // Use a Halide Func to compute a wavy RGB image obtained
    // by the formula: output(x,y,c) = (1-c)*cos(x)*cos(y)
    // Schedule: use compute_root() on all the Func's you create.
    //
    // HINT: you need one more dimension than before
    //
    // OUTPUT 512x512 RGB image (uint8_t)
    //
    // SCHEDULE: use compute_root() on all the Func's you create (i.e.
    // use apply_auto_schedule(Func f).
    
    Halide::Func wavy_rgb("wavy_rgb");

    Halide::Var w("w");
    Halide::Var h("h");
    Halide::Var c("c");

    float pi = 3.1415927;

    Halide::Expr e = (1-c)*cos(w * pi / 180.0f)*cos(h * pi / 180.0f);

    e = Halide::cast<uint8_t>((e + 1)*127.5f);

    wavy_rgb(w, h, c) = e;

    // wavy_rgb.compute_root();
    apply_auto_schedule(wavy_rgb);

    Halide::Buffer<uint8_t> output = wavy_rgb.realize(512, 512, 3);

    return output;
}

Image<uint8_t> Luminance(Image<uint8_t> input) {
    // // --------- HANDOUT  PS08 ------------------------------
    // Use Halide to compute a 1-channel luminance image. Use the formula
    // L = 0.3R+0.6G+0.1B
    //
    // INPUT: RGB image
    //
    // OUTPUT: single channel luminance image the same size as the input image
    //
    // SCHEDULE: use compute_root() on all the Func's you create (i.e.
    // use apply_auto_schedule(Func f).
    Halide::Func luminance("luminance");
    Halide::Var x("x");
    Halide::Var y("y");
    Halide::Expr e("e");

    e = input(x, y, 0)*0.3f + input(x, y, 1)*0.6f + input(x, y, 2)*0.1f;

    e = Halide::cast<uint8_t>(e);

    luminance(x, y) = e;

    apply_auto_schedule(luminance);

    Halide::Buffer<uint8_t> output = luminance.realize(input.width(), input.height(), input.channels());
   
    return output;
}

Image<uint8_t> Sobel(Image<uint8_t> input) {
    // // --------- HANDOUT  PS08 ------------------------------
    // Use Halide to apply a Sobel filter and return the gradient magnitude.
    // Use the following kernels to compute the horizontal and vertical gradients
    //
    //            | -1 0 1 |             | -1 -2 -1 |
    // x kernel = | -2 0 2 |, y kernel = |  0  0  0 |
    //            | -1 0 1 |             |  1  2  1 |
    //
    // INPUT: single channel luminance image
    //
    // OUTPUT: single channel luminance image the same size as the input image
    // OUTPUT: single channel image where each pixel is the magnitude of Sobel x and y filters
    //
    // SCHEDULE: use compute_root() on all the Func's you create (i.e.
    // use apply_auto_schedule(Func f).

    Halide::Func sobel("sobel");
    Halide::Var x("x");
    Halide::Var y("y");

    Halide::Func clamped("clamped");
    
    // clamped = BoundaryConditions::repeat_edge(input);
    clamped(x,y) =  cast<float>(input(clamp(x, 0, input.width()-1),clamp(y, 0, input.height()-1)));

    Halide::Func x_output("x_output");
    x_output(x, y) = (-clamped(x-1, y-1) + clamped(x+1, y-1) -
                             2.0f*clamped(x-1, y) + 2.0f*clamped(x+1, y) -
                             clamped(x-1, y+1) + clamped(x+1, y+1));

    Halide::Func y_output("y_output");
    y_output(x, y) = (-clamped(x-1, y-1) - 2.0f*clamped(x, y-1) - clamped(x+1, y-1) + 
                                clamped(x-1, y+1) + 2.0f*clamped(x, y+1) + clamped(x+1, y+1));

    sobel(x, y) = Halide::cast<uint8_t>(sqrt(pow(x_output(x, y), 2.0f) + pow(y_output(x, y), 2.0f)));



    apply_auto_schedule(sobel);

    Halide::Buffer<uint8_t> output = sobel.realize(input.width(), input.height());

    return output;
}

// Example code for the equivalent .cpp loop questions
Image<uint8_t> boxSchedule1(Image<uint8_t> input) {
    cout << "Started boxSchedule1" << endl; // DO NOT CHANGE

    // Ignore boundaries: use this width and height
    int w = input.width()-2;
    int h = input.height()-2;

    // Intermediary stages of the pipeline, same size as input
    Image<float> blur_x(input.width(), input.height());
    Image<uint8_t> blur_y(input.width(), input.height());


    // - Schedule to replicate -
    // blur_y.compute_root();
    // blur_x.compute_root();
    // -------------------------

    // Your equivalent loops should go there --------
    // Compute blur_x root
    for (int y=0; y<h; y++) {
        for (int x=0; x<w; x++) {
            cout << "blur_x" << " " << x << " " << y << " " << endl;
            blur_x(x,y) = (static_cast<float>(input(x,y)) 
                        + static_cast<float>(input(x+1,y))
                        + static_cast<float>(input(x+2,y)))/3.0f;
        }
    }
    // Compute blur_y root
    for (int y=0; y<h; y++) {
        for (int x=0; x<w; x++) {
            cout << "blur_y" << " " << x << " " << y << " " << endl;
            blur_y(x,y) = static_cast<uint8_t>((blur_x(x,y) + blur_x(x,y+1) + blur_x(x,y+2))/3.0f);
        }
    }

    // ----------- till here ------------------------

    cout << "Completed boxSchedule1" << endl; // DO NOT CHANGE
    return blur_y;
}

Image<uint8_t> boxSchedule5(Image<uint8_t> input) {
    // // --------- HANDOUT  PS08 ------------------------------
    // Write the cpp nested loops corresponding to the 3x3 box schedule 5 in tutorial 6
    // print the order of evaluation. 

    // schedule 5:
    // blur_y.compute_root()
    // blur_x.compute_at(blur_y, x)
    cout << "Started boxSchedule5" << endl;

    int w = input.width()-2;
    int h = input.height()-2;

    // Intermediary stages of the pipeline, same size as input
    Image<float> blur_x(1, 3);
    Image<uint8_t> blur_y(input.width(), input.height());

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            cout << "blur_y" << " " << x << " " << y << endl;
            for (int yy = 0; yy < 3; yy++) {
                for (int yx = 0; yx < 1; yx++) {
                    blur_x(yx, yy) = (static_cast<float>(input(x+yx,y+yy)) 
                        + static_cast<float>(input(x+1+yx,y+yy))
                        + static_cast<float>(input(x+2+yx,y+yy)))/3.0f;
                }
            }
            blur_y(x, y) = static_cast<uint8_t>((blur_x(0, 0) + blur_x(0, 1) + blur_x(0, 3))/3.0f);
        }
    }

    cout << "Completed boxSchedule5" << endl;
    
}

Image<uint8_t> boxSchedule6(Image<uint8_t> input) {
    // // --------- HANDOUT  PS08 ------------------------------
    // Write the cpp nested loops corresponding to the 3x3 box schedule 6 in tutorial 6
    // print the order of evaluation. 
    // Each time you perform a computation of blur_x or
    // blur_y, put a triplet with the name of the Func ("blur_x" or "blur_y") and
    // the output coordinates x and y. For example, when you compute (50,75) print:
    // blur_x 50 75 ( with exactly one space between each) in a new line.
    //
    // Do not print anything else using cout within the current function.
    
    // schedule 6:
    // blur_y.tile(x, y, xo, yo, xi, yi, 2, 2)
    // blur_x.compute_at(blur_y, yo)
    cout << "Started boxSchedule6" << endl;

    int w = input.width()-2;
    int h = input.height()-2;

    // Intermediary stages of the pipeline, same size as input
    Image<float> blur_x(1, 3);
    Image<uint8_t> blur_y(input.width(), input.height());

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            cout << "blur_y" << " " << x << " " << y << endl;
            for (int yy = 0; yy < 3; yy++) {
                for (int yx = 0; yx < 1; yx++) {
                    blur_x(yx, yy) = (static_cast<float>(input(x+yx,y+yy)) 
                        + static_cast<float>(input(x+1+yx,y+yy))
                        + static_cast<float>(input(x+2+yx,y+yy)))/3.0f;
                }
            }
            blur_y(x, y) = static_cast<uint8_t>((blur_x(0, 0) + blur_x(0, 1) + blur_x(0, 3))/3.0f);
        }
    }

    cout << "Completed boxSchedule6" << endl;


}



Image<uint8_t> boxSchedule7(Image<uint8_t> input) {
    // // --------- HANDOUT  PS08 ------------------------------
    // Write the cpp nested loops corresponding to the 3x3 box schedule 7 in tutorial 6
    // print the order of evaluation. 
    // Each time you perform a computation of blur_x or
    // blur_y, put a triplet with the name of the Func ("blur_x" or "blur_y") and
    // the output coordinates x and y. For example, when you compute (50,75) print:
    // blur_x 50 75 ( with exactly one space between each) in a new line.
    //
    // Do not print anything else using cout within the current function.
    
    // // --------- HANDOUT  PS08 ------------------------------
    // Write the cpp nested loops corresponding to the 3x3 box schedule 7 in tutorial 6
    // print the order of evaluation. 
    // Each time you perform a computation of blur_x or
    // blur_y, put a triplet with the name of the Func ("blur_x" or "blur_y") and
    // the output coordinates x and y. For example, when you compute (50,75) print:
    // blur_x 50 75 ( with exactly one space between each) in a new line.
    //
    // Do not print anything else using cout within the current function.
    
    // schedule 7
    // blur_y.split(x, xo, xi, 2)
    // blur_x.compute_at(blur_y, y)
    cout << "Started boxSchedule7" << endl;

    int w = input.width()-2;
    int h = input.height()-2;

    // Intermediary stages of the pipeline, same size as input
    Image<float> blur_x(input.width(), 3);
    Image<uint8_t> blur_y(input.width(), input.height());

    for (int y = 0; y < h; y++) {
        for (int x_out = 0; x_out < w / 2; x_out++) { 
            for (int x_in = 0; x_in < 2; x_in++) {
                int x = x_out*2 + x_in;
                cout << "blur_y" << " " << x << " " << y << endl;
                for (int yy = 0; yy < 3; yy++) {
                    for (int yx = 0; yx < w; yx++) {
                        blur_x(yx, yy) = (static_cast<float>(input(yx, y+yy)) 
                        + static_cast<float>(input(yx, y+yy))
                        + static_cast<float>(input(yx, y+yy)))/3.0f;
                    }
                }
                blur_y(x, y) = static_cast<uint8_t>((blur_x(yx, 0) + blur_x(yx, 1) + blur_x(yx, 3))/3.0f);

            }
        }
    }

    cout << "Completed boxSchedule7" << endl;

}


// ---------------------------------- PART 2 -----------------------------------
// For all the problems in Part 2, yo are expected to write schedules. You will
// score points for faster schedules. You should also report the machine used
// for testing in the form accompanying this Pset. Each problem is supposed to
// return a Halide Func that computes the result in uint8_t. All intermediate
// operations should be performed in float for Gaussian and unsharp mask.
//
// First get the algorithm right and make sure it is generating the
// correct output. Use apply_auto_schedule() during this phase. Once you
// are confident of your result, remove apply_auto_schedule() and write
// your own schedule. You can set the macro AUTO_SCHEDULE to 0 or 1 to easily
// alternate between the default and updated schedules.
//
// Hint: your schedule should beat apply_auto_schedule()


Func LocalMax(Image<uint8_t> input, int window) {
    // // --------- HANDOUT  PS08 ------------------------------
    // Compute the local maximum in a window of given size centered at the pixel.
    // Return white color if it's a local maximum and black otherwise.
    // Run your function with autoschedule as you write the algorithm, then
    // write your own (better) schedule.
    //
    // Handle boundary conditions by clamping at boundaries (see tutorials)
    //
    // INPUT: single channel luminance image
    // OUTPUT: Halide Func that computes the localMax

    // output Func

}


Func Gaussian(Image<uint8_t> input, float sigma, float truncate) {
    // // --------- HANDOUT  PS08 ------------------------------
    // Perform 2D gaussian blur. use what you learnt in previous Pset about
    // fast Gaussian blur algorithm. The Gaussians have variance sigma and the
    // kernels are truncated at trunc*sigma on both sides. We give you the
    // kernel.
    //
    // Use the sum() sugar (see tutorials)
    // First generate the kernel as a Halide Func
    // Handle boundary conditions
    //
    // INPUT: single channel luminance image
    //
    // OUTPUT: Func that computes Gaussian blur
    

}

Func UnsharpMask(Image<uint8_t> input, float sigma, float truncate, float strength) {
    // // --------- HANDOUT  PS08 ------------------------------
    // Implement Unsharp Mask. Use a Gaussian of standard deviation sigma to extract
    // a lowpassed version of the image. Subtract that lowpassed version from the
    // original image to produce a highpassed version of the image and then add the
    // highpassed version back to it strength times.
    //
    // Handle boundary conditions
    // Use the Gaussian above for blur
    // You may want to write a new Gaussian with a new schedule because it may be
    // possible to use a different schedule for Gaussian blur such that it works
    // better with subsequent stages
    //
    // INPUT: single channel luminance image
    // OUTPUt: Halide Func that computes the localMax
    //
    // return Func("f");


}
