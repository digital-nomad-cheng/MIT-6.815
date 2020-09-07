#include "a1.h"

using namespace std;

// This is a way for you to test your functions. 
// We will only grade the contents of a0.cpp and Image.cpp
int main() {
	Image input = Image('./Input/boston_low_contrast.png');
	Image output = brightness(input, 0.2);
	output.debug_write();
}
