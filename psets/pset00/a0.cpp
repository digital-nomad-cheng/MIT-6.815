/* -----------------------------------------------------------------
 * File:    a0.cpp
 * Created: 2015-09-09
 * -----------------------------------------------------------------
 * 
 * Implementation file for PS00
 *
 * We define and implement the functions we declared in a0.h
 * 
 * ---------------------------------------------------------------*/

// This command imports all the content of a0.h, thus declaring our functions.
#include "a0.h"

// We use the namespace "std" so that we can write the command "cout" instead
// of the verbose version "std::cout". 
using namespace std;

// Return the sum of two numbers
float getSum(float a, float b) {
    float c = a + b;
    return c;
}

// Print the sum of two numbers to the screen
void helloworld(float a, float b) {
    // --------- HANDOUT  PS00 ------------------------------
    // Create a variable c that is the sum of a and b
    // Use cout to display the values
    float c = getSum(a,b);
    std::cout << "Hello World!" << std::endl;
    std::cout << "The value of a is " << a << "." << std::endl;
    std::cout << "The value of b is " << b << "." << std::endl;
    std::cout << "The value of a and b is " << c << "." << std::endl;
}

// Create an image and return it
// You can test your function by calling my_image.write(filename) to write the
// output somewhere
Image readAnImage(const std::string &filename) {
    // --------- HANDOUT  PS00 ------------------------------
    // Use the constructor in Image.h
    return Image(filename);
}
