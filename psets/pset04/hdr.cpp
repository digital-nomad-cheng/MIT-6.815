#include "hdr.hpp"

Image computeWeight(const Image &im, float epsilonMini, float epsilonMaxi=)
{
	Image output(im.width(), im.height(), im.channels());
	for (int h = 0; h < im.height(); h++) {
		for (int w = 0; w < im.width(); w++) {
			for (int c = 0; c < im.channels(); c++) {
				output(w, h, c) = (epsilonMini <= im(w, h, c) <= epsilonMaxi ? 1.0 : 0.0);
			}
		}
	}
	return output;
}

float computeFactor(const Image &im1, const Image &w1, const Image &im2, const Image &w2)
{
	std::vector<float> usable_im1;
	std::vector<float> usable_im2;

	for (int h = 0; h < im.height(); h++) {
		for (int w = 0; w < im.width(); w++) {
			for (int c = 0; c < im.channels(); c++) {
				if (w1(w, h, c) == w2(w, h, c) == 1) {
					usable_im1.push_back(im1(w, h, c));
					usable_im2.push_back(im2(w, h, c));
				}
			}
		}
	}

	std::sort(usable_im1);
	std::sort(usable_im2);

	int mid = usable_im1.size() / 2;

	return usable_im2[mid] / (usable_im1[mid] + pow(10, -10))
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
    Image output = im;
    for (int i = 0 ; i < im.number_of_elements();i++) {
        output(i) = pow(im(i), exponent);
    }
    return output;
}