#include "a0.h"

using namespace std;

// This is a way for you to test your functions. 
// We will only grade the contents of a0.cpp and Image.cpp
int main() {
    // --------- HANDOUT  PS00 ------------------------------
    cout << "Congratulations, you have compiled the starter code!" << endl;  
    helloworld(5,10);
    // Example Test
    // Read an Image
    Image test_image = readAnImage("./Input/Boston_low_contrast.png");

    // // Write an Image
    // cout << "Image size: " << im.width() << ", " << im.height() << ", " << im.channels() << endl;
    test_image.write("./Output/SameAsInput.png");
}
