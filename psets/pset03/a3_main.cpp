#include "basic_image_manipulation.hpp"
#include "filtering.hpp"

using namespace std;

// This is a way for you to test your functions. 
// We will only grade the contents of a0.cpp and Image.cpp
int main() {
	Image input = Image("../Input/lens.png");
	Image output1 = boxBlur(input, 3, true);
    output1.write("../Output/boston_box_blur.png");

    // a little dark here -> Fixed
    Image output2 = boxBlur_filterClass(input, 3, true);
    output2.write("../Output/boston_filter_class_box_blur.png");

    Image output3 = gradientMagnitude(input, true);
    output3.write("../Output/boston_magnitude.png");

    Image output4 = gaussianBlur_horizontal(input, 3.0);
    output4.write("../Output/boston_horizontal_gaussian_blur.png");

    Image output5 = gaussianBlur_2D(input, 3.0);
    output5.write("../Output/boston_2D_gaussian_blur.png");

    Image output6 = gaussianBlur_separable(input, 3.0);
    output6.write("../Output/boston_2D_separable_gaussian_blur.png");

    Image output7 = unsharpMask(input, 3.0, 3.0, 0.5);
    output7.write("../Output/boston_unsharp_mask.png");

    Image output8 = bilateral(input);
    output8.write("../Output/boston_bilateral.png");

    Image output9 = bilaYUV(input);
    output9.write("../Output/boston_bilaYUV.png");

}
