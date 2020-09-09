#include "basic_image_manipulation.hpp"
#include "filtering.hpp"

using namespace std;

// This is a way for you to test your functions. 
// We will only grade the contents of a0.cpp and Image.cpp
int main() {
	Image input = Image("../Input/boston_underexposed.png");
	Image output1 = boxBlur(input, 3, true);
    output1.write("../Output/boston_box_blur.png");

    Image output2 = boxBlur_filterClass(input, 3, true);
    output2.write("../Output/boston_filter_class_box_blur.png");



}
