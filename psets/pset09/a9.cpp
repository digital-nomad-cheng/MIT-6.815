/**
 * Instructions.
 *
 * This file contains a number of empty function stubs with the problem
 * statements. You need to fill in all details. These problems
 * are designed as extensions to the tutorials in this Pset. Please run
 * each tutorial and go through the code and comments. You are welcome
 * to discuss details on Piazza if you do not understand the tutorials.
 *
 * The main function is in a9_main.cpp. You can modify the contents for
 * your debugging purposes but restore the original contents.
 *
 * Live long and propser.
 */

#include <a9.hpp>

using namespace Halide;


Func HarrisCorners(const Image<float> &input,
    int schedule_index,
    vector<int> schedule_parameters,
    float sigmaG,
    float factorSigma,
    float truncate,
    float k,
    float thresh,
    int maxiDiam
    ) 
{

    // --------- HANDOUT  PS09 ------------------------------
    // Re-implement the Harris Corner detector using Halide. By fusing
    // different stages of the processing pipeline and properly scheduling your
    // opertations, you should be able to get a drastic speed-up over the
    // reference implementation given in 'reference_implementation.cpp'.
    //
    // Make sure you debug your intermediate Func (by
    // realizing them over some domain for example). You can use the
    // normalize_image helper function to remap the pixel value to the [0,1]
    // range, valid for display.
    //
    // Start by building a correct implementation of the algorithm,
    // Then walk your way up to a fast implementation by trying
    // various schedules.
    //
    // Return the output Func, properly scheduled. Chek the reference
    // implementation to see how the input parameters are used.
    //
    // You can use scheule_index to switch between different schedules and
    // compare them, this will come in handy when you implement an autotuner
    // (6.865). You can also pass schedule parameters as a vector of ints.
    

    // --------- Algorithm ----------------------------------
    // 1. calculate lumi
    Func clamped("clamped");
    Func lumi("lumi");
    Var x("x");
    Var y("y");
    Var c("c");
    std::cout << input.channels() << std::endl;
    clamped = Halide::BoundaryConditions::repeat_edge(input, 0, input.width(), 0, input.height());
    lumi(x, y) = 0.3f * clamped(x,y,0) + 0.6f * clamped(x,y,1) + 0.1f * clamped(x,y,2);

    // Image<float> lumi_image = lumi.realize(input.width(), input.height());
    // convert_and_save_image(lumi_image, "../Output/halide_lumi.png");

    lumi.compute_root();

    // 2. gaussian blur
    int offset = int(ceil(sigmaG*truncate));
    Func blur_x("gaussian_x");
    Func blur_y("gaussian_y");
    Func kernel("kernel");
    Func kernel_sum("kernel_sum");
    Func kernel_normed("kernel_normed");
    RDom r(-offset, 2*offset+1);

    kernel(x) = exp(-(x - offset)*(x - offset) / (2.0f*sigmaG*sigmaG));
    kernel_sum(x) = sum(kernel(r.x));
    kernel_normed(x) = kernel(x) / kernel_sum(0);

    // blur_x
    blur_x(x, y) = sum(kernel_normed(r.x) * lumi(x + r.x, y));

    // blur_y
    blur_y(x, y) = sum(kernel_normed(r.x) * blur_x(x, y + r.x));
    
    
    // Image<float> gaussian_image = blur_y.realize(input.width(), input.height());
    // convert_and_save_image(gaussian_image, "../Output/gaussian_lumi.png");

    blur_x.compute_root();
    blur_y.compute_root();



    // 3. gradient dx and dy
    Func sobel_x("sobel_x");
    Func sobel_y("sobel_y");
    

    sobel_x(x, y) = -1.0f * blur_y(x-1, y-1) + 0.0f * blur_y(x, y-1) + 1.0f * blur_y(x+1, y-1) -
                     2.0f * blur_y(x-1, y) + 0.0f * blur_y(x, y) + 2.0f * blur_y(x+1, y) -
                     1.0f * blur_y(x-1, y+1) + 0.0f * blur_y(x, y+1) + 1.0f * blur_y(x+1, y+1);

    sobel_y(x, y) = -1.0f * blur_y(x-1, y-1) - 2.0f * blur_y(x, y-1) - 1.0f * blur_y(x+1, y-1) + 
                    1.0f * blur_y(x-1, y+1) + 2.0f * blur_y(x, y+1) + 1.0f * blur_y(x+1, y+1);
    

    // Image<float> sobel_x_image = sobel_x.realize(input.width(), input.height());
    // Image<float> norm_sobel_x_image = normalize_image(sobel_x_image);
    // convert_and_save_image(norm_sobel_x_image, "../Output/halide_sobel_x.png");
    
    // Image<float> sobel_y_image = sobel_y.realize(input.width(), input.height());
    // Image<float> norm_sobel_y_image = normalize_image(sobel_y_image);
    // convert_and_save_image(norm_sobel_y_image, "../Output/halide_sobel_y.png");


    sobel_x.compute_root();
    sobel_y.compute_root();


    // 4. structure tensor
    Func tensor("tensor");
    tensor(x,y,c) = 0.0f;

    tensor(x, y, 0) = sobel_x(x, y) * sobel_x(x, y);
    tensor(x, y, 1) = sobel_x(x, y) * sobel_y(x, y);
    tensor(x, y, 2) = sobel_y(x, y) * sobel_y(x, y);

    // Image<float> tensor_image = tensor.realize(input.width(), input.height());
    // Image<float> norm_tensor_image = normalize_image(tensor_image);
    // convert_and_save_image(norm_tensor_image, "../Output/halide_tensor.png");

    tensor.compute_root();

    // 5. blur tensor
    float tensor_sigma = sigmaG*factorSigma;
    int tensor_offset = tensor_sigma*truncate;

    Func tensor_blur_x("tensor_gaussian_x");
    Func tensor_blur_y("tensor_gaussian_y");
    Func tensor_kernel("tensor_kernel");
    Func tensor_kernel_sum("tensor_kernel_sum");
    Func tensor_kernel_normed("tensor_kernel_normed");
    RDom tensor_r(-tensor_offset, 2*tensor_offset+1);

    tensor_kernel(x) = exp(-(x - offset)*(x - offset) / (2.0f*tensor_sigma*tensor_sigma));
    tensor_kernel_sum(x) = sum(tensor_kernel(tensor_r.x));
    tensor_kernel_normed(x) = tensor_kernel(x) / tensor_kernel_sum(0);
    
    tensor_kernel.compute_root();
    tensor_kernel_sum.compute_root();
    tensor_kernel_normed.compute_root();
    tensor_kernel_normed.realize(2*offset+1);


    tensor_blur_x(x, y, c) = 0.0f;
    tensor_blur_y(x, y, c) = 0.0f;    

    // tensor blur_x
    tensor_blur_x(x, y, c) = sum(tensor_kernel_normed(tensor_r.x) * tensor(x + tensor_r.x, y, c));
    tensor_blur_x.realize(input.width(), input.height());

    // // tensor blur_y
    // // tensor_blur_y(x, y, c) = sum(tensor_kernel_normed(tensor_r.x) * tensor_blur_x(x, y + tensor_r.x, c));


    Image<float> blur_tensor_image = tensor_blur_x.realize(input.width(), input.height());
    // Image<float> norm_blur_tensor_image = normalize_image(blur_tensor_image);
    // convert_and_save_image(norm_blur_tensor_image, "../Output/halide_blur_tensor.png");

    return sobel_y;

}

