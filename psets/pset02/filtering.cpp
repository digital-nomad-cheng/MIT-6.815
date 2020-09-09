#include "filtering.hpp"

// ---------------------------- Filter ---------------------------- //

// Filter Constructor
Filter::Filter(const vector<float> &fData, int fWidth, int fHeight){
	if (fData.size() != fWidth*fHeight) {
		throw InvalidArgument();
	}
	kernel.resize(fWidth*fHeight);
	for (int i = 0; i < fData.size(); i++) {
		kernel[i] = fData[i];
	}
	this->width = fWidth;
	this->height = fHeight;
}

Filter::Filter(int fWidth, int fHeight)
{
	kernel.resize(fWidth*fHeight);
	for (int i = 0; i < kernel.size(); i++) {
		kernel[i] = 0.0;
	}
	this->width = fWidth;
	this->height = fHeight;
}


Filter::~Filter()
{

}

// function to convolve your filter with an image
Image Filter::convolve(const Image &im, bool clamp)
{
	Image output(im.width(), im.height(), im.channels());
	int xx = this->width/2;
	int yy = this->height/2;
	for (int h = 0; h < im.height(); h++) {
		for (int w = 0; w < im.width(); w++) {
			for (int y = -yy; y <= yy; y++) {
				for (int x = -xx; x <= xx; x++) {
					for (int c = 0; c < im.channels(); c++) {
						output(w, h, c) += this->kernel[this->width*(y+yy) + x+xx] * im.smartAccessor(w+x, h+y, c, clamp);
					}
				}
			}
		}
	}
	return output;
}
    
// Accessors of the filter values
const float & Filter::operator()(int x, int y) const
{
	return this->kernel[this->width*y + x];
}

float & Filter::operator()(int x, int y) {
	return this->kernel[this->width*y + x];
}



// --------------------------- Filter -------------------------- //

Image boxBlur(const Image &im, int k, bool clamp)
{
	Image output = Image(im.width(), im.height(), im.channels());
	int r = k / 2;
	float weight = 1.0 / float(k * k);
	for (int h = 0; h < im.height(); h++) {
		for (int w = 0; w < im.width(); w++) {
			for (int y = -r; y <=r; y++) {
				for (int x = -r; x <= r; x++) {
					output(w, h, 0) += im.smartAccessor(w+x, h+y, 0, clamp) * weight;
					output(w, h, 1) += im.smartAccessor(w+x, h+y, 1, clamp) * weight;
					output(w, h, 2) += im.smartAccessor(w+x, h+y, 2, clamp) * weight;
				}
			}
		}
	} 
	return output;
}

Image boxBlur_filterClass(const Image &im, int k, bool clamp)
{
	std::vector<float> kernel;
	for (int i = 0; i < k*k; i++) {
		kernel.push_back(1.0/(float(k*k)));
	}
	Filter box_filter = Filter(kernel, k, k);
	return box_filter.convolve(im, clamp);
}

// Gradient Filter
Image gradientMagnitude(const Image &im, bool clamp)
{
	// Image gray = color2gray(im);
	std::vector<float> h_sobel = {-1, 0, 1, -2, 0, 2, -1, 0, 1};
	std::vector<float> v_sobel = {-1.0, -2.0, -1.0, 0.0, 0.0, 0.0, 1.0, 2.0, 1.0};
	Filter v_filter = Filter(v_sobel, 3, 3);
	Filter h_filter = Filter(h_sobel, 3, 3);
	
	Image v_image = v_filter.convolve(im, clamp);
	Image h_image = h_filter.convolve(im, clamp);
	

	Image output(im.width(), im.height(), im.channels());

	for (int h = 0; h < im.height(); h++) {
		for (int w = 0; w < im.width(); w++) {
			for (int c = 0; c < im.channels(); c++) {
				output(w, h, c) = sqrt(h_image(w, h, c)*h_image(w, h, c) + v_image(w, h, c)*v_image(w, h, c));
			}
			
		}
	}
	return output;
}

vector<float> gauss1DFilterValues(float sigma, float truncate)
{
	const float pi = 3.1415926;
	int r = ceil(sigma * truncate);
	std::vector<float> result;

	float weight = 1 / sqrt(2*pi*sigma*sigma);
	result.resize(2*r+1);
	for (int i = -r; i <=r; i++) {
		result[i+r] = weight * exp(-(r*r) / (2*sigma*sigma));
	}
	int total = 0;
	for (int i = 0; i < result.size(); i++) {
		total += result[i];
	}
	for (int i = 0; i < result.size(); i++) {
		result[i] /= total;
	}
	
	return result;
}

// vector<float> gauss2DFilterValues(float sigma, float truncate);

Image gaussianBlur_horizontal(const Image &im, float sigma, float truncate, bool clamp)
{
	std::vector<float> kernel = gauss1DFilterValues(sigma, truncate);
	Filter filter(kernel, 2*truncate*sigma+1, 1);
	return filter.convolve(im);
}

// Image gaussianBlur_separable(const Image &im, float sigma, float truncate=3.0, bool clamp=true);
// Image gaussianBlur_2D(const Image &im, float sigma, float truncate=3.0, bool clamp=true);

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