#include "hdr.hpp"

Image computeWeight(const Image &im, float epsilonMini, float epsilonMaxi)
{
	Image output(im.width(), im.height(), im.channels());
	for (int h = 0; h < im.height(); h++) {
		for (int w = 0; w < im.width(); w++) {
			for (int c = 0; c < im.channels(); c++) {
				if (epsilonMini <= im(w, h, c) && im(w, h, c) <= epsilonMaxi) {
					output(w, h, c) = 1.0;
				} else {
					output(w, h, c) = 0.0;
				}
			}
		}
	}
	return output;
}

float computeFactor(const Image &im1, const Image &w1, const Image &im2, const Image &w2)
{
	std::vector<float> usable_im1;
	std::vector<float> usable_im2;

	for (int h = 0; h < im1.height(); h++) {
		for (int w = 0; w < im1.width(); w++) {
			for (int c = 0; c < im1.channels(); c++) {
				if (w1(w, h, c) == w2(w, h, c) == 1.0) {
					usable_im1.push_back(im1(w, h, c));
					usable_im2.push_back(im2(w, h, c));
				}
			}
		}
	}

	std::sort(usable_im1.begin(), usable_im1.end());
	std::sort(usable_im2.begin(), usable_im2.end());
	// for (float val : usable_im1) {
	// 	std::cout << val << " ";
	// }
	// for (float val : usable_im2) {
	// 	std::cout << val << " ";
	// }
	// std::cout << usable_im1.size() << std::endl;
	// im1.debug_write();
	// std::cout << usable_im2[mid] << " " << usable_im1[mid] << std::endl;
	int mid = usable_im1.size() / 2;
	
	return usable_im2[mid] / (usable_im1[mid] + pow(10, -10));
}

Image makeHDR(vector<Image> &imSeq, float epsilonMini, float epsilonMaxi)
{

}

/*********************************************************************
 *                       Do not edit below                           *
 *********************************************************************/
Image changeGamma(const Image & im, float old_gamma, float new_gamma) {
    // Image output(im.width(), im.height(), im.channels());
    // Figure out what power to take the values of im, to get the values of output
    // return output;
    float exponent = new_gamma/old_gamma;
    Image output = Image(im.width(), im.height(), im.channels());
    for (int i = 0; i < im.number_of_elements(); i++) {
        output(i) = pow(im(i), exponent);
    }
    return output;
}