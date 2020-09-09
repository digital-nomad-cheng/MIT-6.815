#include "filtering.hpp"

Image boxBlur(const Image &im, int k, bool clamp)
{
	Image output = Image(im.width(), im.height(), im.channels());
	int r = k / 2;
	int weight = k * k;

	for (int h = 0; h < im.height(); h++) {
		for (int w = 0; w < im.width(); w++) {
			for (int y = -r; y <=r; y++) {
				for (int x = -r; x <= r; x++) {
					output(w, h, 0) += im.smartAccessor(w+x, h+y, 0) / weight;
					output(w, h, 1) += im.smartAccessor(w+x, h+y, 1) / weight;
					output(w, h, 2) += im.smartAccessor(w+x, h+y, 2) / weight;
				}
			}
		}
	} 
	return output;
}



/**************************************************************
 //               DON'T EDIT BELOW THIS LINE                //
 *************************************************************/

// Create an image of 0's with a value of 1 in the middle. This function
// can be used to test that you have properly set the kernel values in your
// Filter object. Make sure to set k to be larger than the size of your kernel
Image impulseImg(int k){
    // initlize a kxkx1 image of all 0's
    Image impulse(k, k, 1);
    
    // set the center pixel to have intensity 1
    int center = floor(k/2);
    impulse(center, center, 0) = 1.0f;
    
    return impulse;
}