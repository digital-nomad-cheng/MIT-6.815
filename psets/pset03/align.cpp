#include "align.hpp"

Image denoiseSeq(const vector<Image> &imgs)
{
	Image output(imgs[0].width(), imgs[0].height(), imgs[0].channels());
	for (const Image & img : imgs) {
		for (int h = 0; h < output.height(); h++) {
			for (int w = 0; w < output.width(); w++) {
				for (int c = 0; c < output.channels(); c++) {
					output(w, h, c) += img(w, h, c) / imgs.size();
				}
			}
		}
	}
	return output;
}

Image logSNR(const vector<Image> &imSeq, float scale)
{
	Image denoised = denoiseSeq(imSeq);
	
	Image variance(imSeq[0].width(), imSeq[0].height(), imSeq[0].channels());
	for (const Image & img : imSeq) {
		for (int h = 0; h < denoised.height(); h++) {
			for (int w = 0; w < denoised.width(); w++) {
				for (int c = 0; c < denoised.channels(); c++) {
					variance(w, h, c) += scale * pow(denoised(w, h, c) - img(w, h, c), 2.0) / (imSeq.size() - 1);
				}
			}
		}
	}

	Image temp(imSeq[0].width(), imSeq[0].height(), imSeq[0].channels());
	for (const Image & img : imSeq) {
		for (int h = 0; h < denoised.height(); h++) {
			for (int w = 0; w < denoised.width(); w++) {
				for (int c = 0; c < denoised.channels(); c++) {
					temp(w, h, c) += pow(img(w, h, c), 2.0) / imSeq.size();
				}
			}
		}
	}

	Image output(imSeq[0].width(), imSeq[0].height(), imSeq[0].channels());
	
	for (int h = 0; h < output.height(); h++) {
		for (int w = 0; w < output.width(); w++) {
			for (int c = 0; c < output.channels(); c++) {
				output(w, h, c)  = 10 * log10(temp(w, h, c) / variance(w, h, c)) * scale;
			}
		}
	}

	return output;	
}

vector<int> align(const Image &im1, const Image &im2, int maxOffset)
{
    // returns the (x,y) offset that best aligns im2 to match im1.
    /*
    Write a function vector<int> align(const Image &im1, const Image &im2, int maxOffset=20) 
    in align.cpp that returns the [x, y] offset that best aligns im2 to match im1. 
    Ignore the difference for all the pixels less than or equal to MaxOffset away from the edges.
    
    Use a brute force approach that tries every possible integer translation and evaluates the quality
    of a match using the squared error norm (the sum of the squared pixel differences).
    
    The Image roll(const Image &im, int xRoll, int yRoll) func- tion in align.cpp might come in handy. 
    It circularly shifts an image, causing borders to wrap around. However, since you will be ignoring boundary pixels,
    wrapping the pixel values should not be a problem. Make sure to test your procedure before moving on.
    */

    int x = 0;
    int y = 0;
    float best_squared_error_norm = FLT_MAX;
    for (int x_offset = -maxOffset; x_offset <= maxOffset; x_offset++){
        for (int y_offset = -maxOffset; y_offset <= maxOffset; y_offset++){
            Image rolled_im2 = roll(im2, x_offset, y_offset);
            float squared_error_norm = 0;
            for (int a = maxOffset; a < im1.width() - maxOffset; a++){
                for (int b = maxOffset; b < im1.height() - maxOffset; b++){
                    for (int c = 0; c < im1.channels(); c++){
                        squared_error_norm += pow(rolled_im2(a,b,c) - im1(a,b,c),2);
                    }
                }
            }
            if (squared_error_norm < best_squared_error_norm) {
                best_squared_error_norm = squared_error_norm;
                x = x_offset;
                y = y_offset; 
            }
        }


    }
    return vector<int> {x,y};
}


Image alignAndDenoise(const vector<Image> &imSeq, int maxOffset)
{
	std::vector<Image> aligned_imSeq;
	aligned_imSeq.push_back(imSeq[0]);
	for (int i = 1; i < imSeq.size(); i++) {
		std::vector<int> offsets = align(imSeq[0], imSeq[i]);
		aligned_imSeq.push_back(roll(imSeq[i], offsets[0], offsets[1]));
	}

	return denoiseSeq(aligned_imSeq);

}


/**************************************************************
 //               DON'T EDIT BELOW THIS LINE                //
 *************************************************************/

// This circularly shifts an image by xRoll in the x direction and
// yRoll in the y direction. xRoll and yRoll can be negative or postive
Image roll(const Image &im, int xRoll, int yRoll){
    
    int xNew, yNew;
    Image imRoll(im.width(), im.height(), im.channels());
    
    // for each pixel in the original image find where its corresponding
    // location is in the rolled image
    for (int x=0; x<im.width(); x++){
        for (int y=0; y<im.height(); y++){
            
            // use modulo to figure out where the new location is in the
            // rolled image. Then take care of when this returns a negative number
            xNew = (x + xRoll) % im.width();
            yNew = (y + yRoll) % im.height();
            xNew = (xNew<0)*(imRoll.width() + xNew) + (xNew>=0)*xNew;
            yNew = (yNew<0)*(imRoll.height() + yNew) + (yNew>=0)*yNew;
            
            // assign the rgb values for each pixel in the original image to
            // the location in the new image
            for (int z=0; z<im.channels(); z++){
                imRoll(xNew, yNew, z) = im(x,y,z);
            }
        }
    }
    
    // return the rolled image
    return imRoll;
}