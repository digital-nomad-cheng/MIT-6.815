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
	// --------- HANDOUT  PS04 ------------------------------
    // Merge images to make a single hdr image
    // For each image in the sequence, compute the weight map (special cases
    // for the first and last images).
    // Compute the exposure factor for each consecutive pair of image.
    // Write the valid pixel to your hdr output, taking care of rescaling them
    // properly using the factor.

    vector<Image> weightSeq;
    for (int i = 0; i < imSeq.size(); i++){
        float mini = epsilonMini;
        if (i == imSeq.size() - 1){
            mini = FLT_MIN;
        }
        float maxi = epsilonMaxi;
        if (i == 0){
            maxi = FLT_MAX;
        }
        weightSeq.push_back(computeWeight(imSeq[i], mini, maxi));
    }

    vector<float> weight_factors_individual;
    vector<float> weight_factors_cumulative;
    float cumulative_factor = 1.0;
    weight_factors_cumulative.push_back(cumulative_factor);
    for (int a = 0; a < weightSeq.size() - 1; a++){    
        float factor = computeFactor(imSeq[a], weightSeq[a], imSeq[a+1], weightSeq[a+1]);
        weight_factors_individual.push_back(factor);
        
        cumulative_factor *= factor;
        weight_factors_cumulative.push_back(cumulative_factor);
    }
    
    Image darkest = imSeq[0];
    Image output(imSeq[0].width(), imSeq[0].height(), imSeq[0].channels());
    for (int a = 0; a < darkest.width(); a++){
        for (int b = 0; b < darkest.height(); b++){
            for (int c = 0; c < darkest.channels(); c++){
                float pixel_value_sum = 0;
                float valid_counter = 0;
                for (int i = 0; i < imSeq.size(); i++){
                    float pixel_weight = weightSeq[i](a,b,c);
                    float pixel_value = imSeq[i](a,b,c);
                    if (pixel_weight > 0){
                        pixel_value_sum += pixel_value/weight_factors_cumulative[i];
                        valid_counter++;
                    }
                }
                if (valid_counter == 0){
                    output(a,b,c) = darkest(a,b,c);
                }
                else {
                    output(a,b,c) = pixel_value_sum/valid_counter;
                }
            }
        }
    }

    return output;
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