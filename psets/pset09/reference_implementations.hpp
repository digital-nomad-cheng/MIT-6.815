#include "a9.hpp"
#include <cmath>
using namespace std;

Image<float> maximum_filter(const Image<float> &input, int maxiDiam) {
    int w = input.width();
    int h = input.height();

    Image<float> output(input.width(), input.height());

    for (int y = 0; y < h; ++y) 
    for (int x = 0; x < w; ++x) 
    {
        float maxi = input(x,y);
        for(int fy = 0; fy < maxiDiam; ++fy) 
        for(int fx = 0; fx < maxiDiam; ++fx) 
        {
            // Repeat-edge boundary condition
            int xn = x+fx-maxiDiam/2;
            int yn = y+fy-maxiDiam/2;
            xn = min(max(xn,0),w-1);
            yn = min(max(yn,0),h-1);
            maxi = max(maxi, input(xn,yn));
        }
        output(x,y) = maxi;
    }

    return output;
}

Image<float> normalize_image(const Image<float> &input) {
    int w = input.width();
    int h = input.height();
    int c = input.channels();

    Image<float> output(w, h, c);
    // compute min/max
    float mini = input(0,0,0);
    float maxi = input(0,0,0);
    for (int z = 0; z < c; ++z) 
    for (int y = 0; y < h; ++y) 
    for (int x = 0; x < w; ++x) 
    {
        mini = fmin(mini, input(x,y,z));
        maxi = fmax(maxi, input(x,y,z));
    }

    float normalizer = 1.0/(maxi-mini + 1e-10);

    for (int z = 0; z < c; ++z) 
    for (int y = 0; y < h; ++y) 
    for (int x = 0; x < w; ++x) 
    {
        output(x,y,z) = input(x,y,z)- mini;
        output(x,y,z) *= normalizer;
    }

    return output;
}

// Filter input with the x- sobel filter
// -1 0 1
// -2 0 2
// -1 0 1
Image<float> sobelFilter_x(const Image<float> &input) {
    int w = input.width();
    int h = input.height();
    int c = input.channels();
    Image<float> output(w, h, c);


    for (int y = 0; y < h; ++y) 
    for (int x = 0; x < w; ++x) 
    {
        int xmin = max(0,x-1);
        int xmax = min(w-1,x+1);
        int ymin = max(0,y-1);
        int ymax = min(h-1,y+1);
        for (int z = 0; z < c; ++z) 
        {
            output(x,y,z) = 
               -1 * input(xmin, ymin, z) +
                0 * input(x   , ymin, z) +
                1 * input(xmax, ymin, z) +

               -2 * input(xmin, y   , z) +
                0 * input(x   , y   , z) +
                2 * input(xmax, y   , z) +

               -1 * input(xmin, ymax, z) +
                0 * input(x   , ymax, z) +
                1 * input(xmax, ymax, z);
        }
    }
    return output;
}

// Filter input with the y- sobel filter
// -1 -2 -1
//  0  0  0
//  1  2  1
Image<float> sobelFilter_y(const Image<float> &input) {
    int w = input.width();
    int h = input.height();
    int c = input.channels();
    Image<float> output(w, h, c);

    for (int y = 0; y < h; ++y) 
    for (int x = 0; x < w; ++x) 
    {
        int xmin = max(0,x-1);
        int xmax = min(w-1,x+1);
        int ymin = max(0,y-1);
        int ymax = min(h-1,y+1);
        for (int z = 0; z < c; ++z) 
        {
            output(x,y,z) = 
               -1 * input(xmin, ymin, z) +
               -2 * input(x   , ymin, z) +
               -1 * input(xmax, ymin, z) +

                0 * input(xmin, y   , z) +
                0 * input(x   , y   , z) +
                0 * input(xmax, y   , z) +

                1 * input(xmin, ymax, z) +
                2 * input(x   , ymax, z) +
                1 * input(xmax, ymax, z);
        }
    }
    return output;
}

Image<float> gaussian_blur(const Image<float> & input, float sigma, float truncate) {
    int w = input.width();
    int h = input.height();
    int c = input.channels();

    int offset = int(ceil(sigma*truncate));
    int fsize = 2*offset+1;

    // Compute filter values
    Image<float> gaussian_x(fsize,1);
    float tot = 0.0f;
    for (int x = 0; x < fsize; ++x) {
        gaussian_x(x,0) = exp(-(x-offset)*(x-offset) / (2.0f*sigma*sigma));
        tot += gaussian_x(x);
    }
    for (int x = 0; x < fsize; ++x) {
        gaussian_x(x,0) /= tot;
    }

    // Apply filter on x
    Image<float> blurx(w,h, c);
    for (int y = 0; y < h; ++y) 
    for (int x = 0; x < w; ++x) 
    {
        for(int fx = 0; fx < fsize; ++fx) {
            for (int z = 0; z < c; ++z) 
            {

                // Repeat-edge boundary condition
                int xn = x+fx-offset;
                xn = min(max(xn,0), w-1);

                blurx(x,y,z) += gaussian_x(fx)*input(xn,y,z);
            }
        }
    }

    // Apply filter on y
    Image<float> blury(w,h, c);
    for (int y = 0; y < h; ++y) 
    for (int x = 0; x < w; ++x) 
    {
        for(int fy = 0; fy < fsize; ++fy) {
            for (int z = 0; z < c; ++z) 
            {
                // Repeat-edge boundary condition
                int yn = y+fy-offset;
                yn = min(max(yn,0), h-1);
                blury(x,y,z) += gaussian_x(fy)*blurx(x, yn,z);
            }
        }
    }

    return blury;
}


// Compute luminance with weight [0.3, 0.6, 0.1]
Image<float> computeLuminance(const Image<float> &input) {
    Image<float> luminance(input.width(), input.height());
    for (int y = 0; y < input.height(); ++y) 
    for (int x = 0; x < input.width(); ++x) 
    {
        luminance(x,y) = 
            0.3 * input(x,y,0) +
            0.6 * input(x,y,1) +
            0.1 * input(x,y,2);
    }

    return luminance;
}

Image<float> HarrisCornersRef(const Image<float> &input, 
    float sigmaG,
    float factorSigma,
    float truncate,
    float k,
    float thresh,
    int maxiDiam ) 
{

    int w = input.width();
    int h = input.height();
    Image<float> output(w, h);

    // luminance
    Image<float> luminance = computeLuminance(input);
    convert_and_save_image(luminance, "../Output/ref_luminance.png");

    // blur
    Image<float> blurred_luminance = gaussian_blur(luminance, sigmaG, truncate);
    convert_and_save_image(blurred_luminance, "../Output/ref_blur_luminance.png");

    // Compute gradients dx and dy
    Image<float> gradientX = sobelFilter_x(blurred_luminance);
    Image<float> gradientY = sobelFilter_y(blurred_luminance);

    // Debug viz
    Image<float> normX = normalize_image(gradientX);
    Image<float> normY = normalize_image(gradientY);
    convert_and_save_image(normX, "../Output/ref_gradientX.png");
    convert_and_save_image(normY, "../Output/ref_gradientY.png");

    // Assemble tensor
    // M = | dx*dx dx*dy |
    //     | dx*dy dy*dy |
    Image<float> tensor(w, h, 3);
    for (int y = 0; y < h; ++y) 
    for (int x = 0; x < w; ++x) 
    {
        tensor(x,y,0) = gradientX(x,y)*gradientX(x,y);
        tensor(x,y,1) = gradientX(x,y)*gradientY(x,y);
        tensor(x,y,2) = gradientY(x,y)*gradientY(x,y);
    }

    Image<float> normalized_tensor = normalize_image(tensor);
    convert_and_save_image(normalized_tensor, "../Output/ref_tensor.png");

    // Blur tensor
    Image<float> blurred_tensor = gaussian_blur(tensor, sigmaG*factorSigma, truncate);
    Image<float> norm_blurred_tensor = normalize_image(blurred_tensor);
    convert_and_save_image(norm_blurred_tensor, "../Output/ref_blurred_tensor.png");
    
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

    // Debug viz
    Image<float> norm_resp = normalize_image(response);
    convert_and_save_image(norm_resp, "../Output/ref_response.png");


    // Maximum filter
    Image<float> maxifilt = maximum_filter(response, maxiDiam);
    Image<float> norm_maxi = normalize_image(maxifilt);
    convert_and_save_image(norm_maxi, "../Output/maximum_filter.png");

    // Non-max. suppression
    for (int y = 0; y < h; ++y) 
    for (int x = 0; x < w; ++x) 
    {
        // Get the local max with response above threshold
        if(response(x,y) == maxifilt(x,y) && response(x,y) > thresh) {
            output(x,y) = 1.0f;
        } // 0.0f otherwise
    }

    return output;
}