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
	int r = ceil(sigma * truncate);
	std::vector<float> result;

	float total_sum = 0;
	for (int i = -r; i <=r; i++) {
		result.push_back(exp(-(i*i) / (2*sigma*sigma)));
		total_sum += exp(-(i*i) / (2*sigma*sigma));
	}

	for (auto & val : result) {
		val /= total_sum;
	}
	
	return result;
}

vector<float> gauss2DFilterValues(float sigma, float truncate)
{
	std::vector<float> result;
	int r = ceil(sigma * truncate);
	float total_sum = 0;
	for (int y = -r; y <= r; y++) {
		for (int x = -r; x <= r; x++) {
			result.push_back(exp(-(x*x+y*y) / (2*sigma*sigma)));
			total_sum += exp(-(x*x+y*y) / (2*sigma*sigma));
		}
	}
	
	for (auto & val : result) {
		val /= total_sum;
	}
	
	return result;
}

Image gaussianBlur_horizontal(const Image &im, float sigma, float truncate, bool clamp)
{
	std::vector<float> kernel = gauss1DFilterValues(sigma, truncate);
	Filter filter(kernel, 2*ceil(truncate*sigma)+1, 1);
	return filter.convolve(im, clamp);
}


Image gaussianBlur_2D(const Image &im, float sigma, float truncate, bool clamp)
{
	std::vector<float> kernel = gauss2DFilterValues(sigma, truncate);
	Filter filter(kernel, 2*ceil(truncate*sigma)+1, 2*ceil(truncate*sigma)+1);
	return filter.convolve(im, clamp);
}

Image gaussianBlur_separable(const Image &im, float sigma, float truncate, bool clamp)
{
	std::vector<float> kernel = gauss1DFilterValues(sigma, truncate);
	Filter h_filter(kernel, 2*ceil(truncate*sigma)+1, 1);
	Filter v_filter(kernel, 1, 2*ceil(truncate*sigma)+1);
	return v_filter.convolve(h_filter.convolve(im, clamp));
}

// Sharpen an Image
Image unsharpMask(const Image &im, float sigma, float truncate, float strength, bool clamp)
{
	Image low_pass = gaussianBlur_separable(im, sigma, truncate, clamp);
	Image output(im.width(), im.height(), im.channels());

	for (int h = 0; h < im.height(); h++) {
		for (int w = 0; w < im.width(); w++) {
			output(w, h, 0) = -strength * low_pass(w, h, 0) + im(w, h, 0);
			output(w, h, 1) = -strength * low_pass(w, h, 1) + im(w, h, 1);
			output(w, h, 2) = -strength * low_pass(w, h, 2) + im(w, h, 2);
		}
	}

	return output;
}


// Bilaterial Filtering
Image bilateral(const Image &im, float sigmaRange, float sigmaDomain, float truncateDomain, bool clamp)
{

	
	int r = ceil(sigmaDomain * truncateDomain);
	float kernel_dimenions = 2*ceil(sigmaDomain * truncateDomain) + 1;   
	std::vector<float> domain_kernel = gauss2DFilterValues(sigmaDomain, truncateDomain);
	Filter domain_filter(domain_kernel, kernel_dimenions, kernel_dimenions);


	float gaussian_range_constant = 1.0/sqrt(2*M_PI*pow(sigmaRange, 2));
	// Read http://people.csail.mit.edu/sparis/publi/2009/fntcgv/Paris_09_Bilateral_filtering.pdf for clarification	
	
	Image output(im.width(), im.height(), im.channels());
	Image bilaterl_image(im.width(), im.height(), im.channels());

	for (int h = 0; h < im.height(); h++) {
		for (int w = 0; w < im.width(); w++) {
			for (int c = 0; c < im.channels(); c++) {
				float filter_value = 0; 
                float k_for_i_j = 0;
                float pixel_val = im.smartAccessor(w, h, c);

				for (int y = -r; y <= r; y++) {
					for (int x = -r; x <= r; x++) {
						float domain_weight = domain_filter(x+r, y+r);

						float pixel_prime = im.smartAccessor(w + x, h + y, c, clamp);

						float range_weight = gaussian_range_constant*exp(-1*pow((pixel_val - pixel_prime),2)/(2*pow(sigmaRange,2)));

						float intensity_at_examined_point = pixel_prime;

						float final_value = domain_weight*range_weight*intensity_at_examined_point;
                        filter_value += final_value;

                        k_for_i_j += domain_weight*range_weight;

					}
				}
				output(w,h,c) = filter_value/k_for_i_j;

			}
		}
	}

	return output;
}

Image bilaYUV(const Image &im, float sigmaRange, float sigmaY, float sigmaUV, float truncateDomain, bool clamp)
{
	Image yuv = rgb2yuv(im);
	Image Y(im.width(), im.height(), 1);
	Image UV(im.width(), im.height(), 2);
	for (int h = 0; h < im.height(); h++) {
		for (int w = 0; w < im.width(); w++) {
			Y(w, h, 0) = yuv(w, h, 0);
			UV(w, h, 0) = yuv(w, h, 1);
			UV(w, h, 1) = yuv(w, h, 2);
		}
	}

	Image Y_bilateral = bilateral(Y, sigmaRange, sigmaY, truncateDomain, clamp);
	Image UV_bilateral = bilateral(UV, sigmaRange, sigmaUV, truncateDomain, clamp);

	Image output(im.width(), im.height(), im.channels());

	for (int h = 0; h < im.height(); h++) {
		for (int w = 0; w < im.width(); w++) {
			output(w, h, 0) = Y_bilateral(w, h, 0);
			output(w, h, 1) = UV_bilateral(w, h, 0);
			output(w, h, 2) = UV_bilateral(w, h, 1);
		}
	}

	output = yuv2rgb(output);
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



// --------- HANDOUT  PS07 ------------------------------
Image gradientX(const Image &im, bool clamp){
    Filter sobelX(3, 3);
    sobelX(0,0) = -1.0; sobelX(1,0) = 0.0; sobelX(2,0) = 1.0;
    sobelX(0,1) = -2.0; sobelX(1,1) = 0.0; sobelX(2,1) = 2.0;
    sobelX(0,2) = -1.0; sobelX(1,2) = 0.0; sobelX(2,2) = 1.0;

    Image imSobelX = sobelX.convolve(im, clamp);
    return imSobelX;
}


Image gradientY(const Image &im, bool clamp) {

    // sobel filtering in y direction
    Filter sobelY(3, 3);
    sobelY(0,0) = -1.0; sobelY(1,0) = -2.0; sobelY(2,0) = -1.0;
    sobelY(0,1) = 0.0; sobelY(1,1) = 0.0; sobelY(2,1) = 0.0;
    sobelY(0,2) = 1.0; sobelY(1,2) = 2.0; sobelY(2,2) = 1.0;


    Image imSobelY = sobelY.convolve(im, clamp);
    return imSobelY;
}

Image maximum_filter(const Image &im, float maxiDiam) {
    float mi = floor((maxiDiam) / 2);
    float ma = maxiDiam - mi - 1;

    Image mf(im.width(), im.height(), im.channels());
    for (int c = 0; c < im.channels(); c++) 
    for (int i = mi; i < im.width() - ma; i++) 
    for (int j = mi; j < im.height() - ma; j++) 
    {

        for (int deli = -mi; deli <= ma; deli++) 
        for (int delj = -mi; delj <= ma; delj++) 
        {
            mf(i, j, c) = max(mf(i, j, c), im(i+deli, j+delj, c));
        }

    }
    return mf;
}