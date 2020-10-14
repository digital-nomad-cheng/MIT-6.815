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
    

    // ---------------------- Algorithm ----------------------------------

    /////////////////////////////////////////////////////////////////////
    // 1. calculate lumi
    Func clamped("clamped");
    Func lumi("lumi");
    Var x("x");
    Var y("y");
    Var c("c");
    
    clamped = Halide::BoundaryConditions::repeat_edge(input, 0, input.width(), 0, input.height());
    lumi(x, y) = 0.3f * clamped(x,y,0) + 0.6f * clamped(x,y,1) + 0.1f * clamped(x,y,2);

    Image<float> lumi_image = lumi.realize(input.width(), input.height());
    Image<float> norm_lumi_image = normalize_image(lumi_image);
    convert_and_save_image(norm_lumi_image, "../Output/halide_lumi.png");

    lumi.compute_root();

    /////////////////////////////////////////////////////////////////////
    // 2. gaussian blur
    int offset = int(ceil(sigmaG*truncate));
    Func blur_x("gaussian_x");
    Func blur_y("gaussian_y");
    Func kernel("kernel");
    Func kernel_sum("kernel_sum");
    Func kernel_normed("kernel_normed");
    RDom r(-offset, 2*offset+1);

    kernel.compute_root();
    kernel_sum.compute_root();
    kernel_normed.compute_root();

    kernel(x) = exp(-(x - offset)*(x - offset) / (2.0f*sigmaG*sigmaG));
    kernel_sum(x) = sum(kernel(r));
    kernel_normed(x) = kernel(x) / kernel_sum(0);

    // blur_x
    blur_x(x, y) = sum(kernel_normed(r.x) * lumi(x + r.x, y));

    // blur_y
    blur_y(x, y) = sum(kernel_normed(r.x) * blur_x(x, y + r.x));
    
    
    Image<float> gaussian_image = blur_y.realize(input.width(), input.height());
    Image<float> norm_gaussian_image = normalize_image(gaussian_image);
    convert_and_save_image(norm_gaussian_image, "../Output/halide_gaussian_lumi.png");

    blur_x.compute_root();
    blur_y.compute_root();


    /////////////////////////////////////////////////////////////////////
    // 3. gradient dx and dy
    Func sobel_x("sobel_x");
    Func sobel_y("sobel_y");

    sobel_x(x, y) = -1.0f * blur_y(x-1, y-1) + 0.0f * blur_y(x, y-1) + 1.0f * blur_y(x+1, y-1) -
                     2.0f * blur_y(x-1, y) + 0.0f * blur_y(x, y) + 2.0f * blur_y(x+1, y) -
                     1.0f * blur_y(x-1, y+1) + 0.0f * blur_y(x, y+1) + 1.0f * blur_y(x+1, y+1);

    sobel_y(x, y) = -1.0f * blur_y(x-1, y-1) - 2.0f * blur_y(x, y-1) - 1.0f * blur_y(x+1, y-1) + 
                    1.0f * blur_y(x-1, y+1) + 2.0f * blur_y(x, y+1) + 1.0f * blur_y(x+1, y+1);
    
    Image<float> sobel_x_image = sobel_x.realize(input.width(), input.height());
    Image<float> norm_sobel_x_image = normalize_image(sobel_x_image);
    convert_and_save_image(norm_sobel_x_image, "../Output/halide_sobel_x.png");
    
    Image<float> sobel_y_image = sobel_y.realize(input.width(), input.height());
    Image<float> norm_sobel_y_image = normalize_image(sobel_y_image);
    convert_and_save_image(norm_sobel_y_image, "../Output/halide_sobel_y.png");

    sobel_x.compute_root();
    sobel_y.compute_root();


    /////////////////////////////////////////////////////////////////////
    // 4. structure tensor
    Func tensor("tensor");
    tensor(x,y,c) = 0.0f;

    tensor(x, y, 0) = sobel_x(x, y) * sobel_x(x, y);
    tensor(x, y, 1) = sobel_x(x, y) * sobel_y(x, y);
    tensor(x, y, 2) = sobel_y(x, y) * sobel_y(x, y);

    Image<float> tensor_image = tensor.realize(input.width(), input.height());
    Image<float> norm_tensor_image = normalize_image(tensor_image);
    convert_and_save_image(norm_tensor_image, "../Output/halide_tensor.png");

    tensor.compute_root();

    /////////////////////////////////////////////////////////////////////
    // 5. blur tensor
    float tensor_sigma = sigmaG*factorSigma;
    int tensor_offset = tensor_sigma*truncate;

    Func tensor_blur_x("tensor_gaussian_x");
    Func tensor_blur_y("tensor_gaussian_y");
    Func tensor_kernel("tensor_kernel");
    Func tensor_kernel_sum("tensor_kernel_sum");
    Func tensor_kernel_normed("tensor_kernel_normed");
    RDom tensor_r(-tensor_offset, 2*tensor_offset+1);

    tensor_kernel(x) = exp(-(x - tensor_offset)*(x - tensor_offset) / (2.0f*tensor_sigma*tensor_sigma));
    tensor_kernel_sum(x) = sum(tensor_kernel(tensor_r));
    tensor_kernel_normed(x) = tensor_kernel(x) / tensor_kernel_sum(0);
    
    tensor_kernel.compute_root();
    tensor_kernel_sum.compute_root();
    tensor_kernel_normed.compute_root();
    tensor_kernel_normed.realize(2*tensor_offset+1);


    tensor_blur_x(x, y, c) = 0.0f;
    tensor_blur_y(x, y, c) = 0.0f;    

    // tensor blur_x
    tensor_blur_x(x, y, c) = sum(tensor_kernel_normed(tensor_r.x) * tensor(x + tensor_r.x, y, c));
    
    // tensor blur_y
    tensor_blur_y(x, y, c) = sum(tensor_kernel_normed(tensor_r.x) * tensor_blur_x(x, y + tensor_r.x, c));

    Image<float> blur_tensor_image = tensor_blur_y.realize(input.width(), input.height(), 3);
    Image<float> norm_blur_tensor_image = normalize_image(blur_tensor_image);
    convert_and_save_image(norm_blur_tensor_image, "../Output/halide_blur_tensor.png");

    tensor_blur_x.compute_root();
    tensor_blur_y.compute_root();

    /////////////////////////////////////////////////////////////////////
    // 6. compute response
    /*
    // Determinant and trace
    // Corner response
    // response = det(M) - k*trace(M)^2
    Image<float> response(w, h);
    for (int y = 0; y < h; ++y) 
    for (int x = 0; x < w; ++x) 
    {
        float xx = blurred_tensor(x,y,0);
        float xy = blurred_tensor(x,y,1);
        float yy = blurred_tensor(x,y,2);
        response(x, y) = xx * yy - xy * xy - k * ((xx + yy) * (xx + yy));
    }
    */
    Func response("response");
    response(x, y) = tensor_blur_y(x,y,0)*tensor_blur_y(x,y,2) - tensor_blur_y(x,y,1)*tensor_blur_y(x,y,1) - 
                     k * (tensor_blur_y(x,y,0) + tensor_blur_y(x,y,2)) * (tensor_blur_y(x,y,0) + tensor_blur_y(x,y,2));

    Image<float> response_image = response.realize(input.width(), input.height());
    Image<float> normalize_response_image = normalize_image(response_image);
    convert_and_save_image(normalize_response_image, "../Output/halide_response.png");

    response.compute_root();

    /////////////////////////////////////////////////////////////////////
    // 7. compute maximum response image
    Func maximum_response("maximum_response");
    RDom maximum_r(-maxiDiam/2, maxiDiam, -maxiDiam/2, maxiDiam);
    maximum_response(x, y) = maximum(response(x + maximum_r.x, y + maximum_r.y));
    
    Image<float> max_response_image = maximum_response.realize(input.width(), input.height());
    Image<float> norm_max_response_image = normalize_image(max_response_image);
    convert_and_save_image(norm_max_response_image, "../Output/halide_max_response.png");

    maximum_response.compute_root();

    /////////////////////////////////////////////////////////////////////
    // 8. non-maximum suppression
    Func output("output");
    output(x, y) = select(response(x, y) == maximum_response(x, y) && response(x, y) > thresh, 1.0f, 0.0f);

    output.realize(input.width(), input.height());
    return output;

}

