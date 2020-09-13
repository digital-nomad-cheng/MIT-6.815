#include "basic_image_manipulation.hpp"
#include "filtering.hpp"
#include "align.hpp"

using namespace std;

// This is a way for you to test your functions. 
// We will only grade the contents of a0.cpp and Image.cpp
int main() {
	// Denoise
    vector<Image> seq;
    int n_images = 4;
    for (int i = 1; i <= n_images; ++i) {
        ostringstream fname;
        // fname << "./Input/aligned-ISO400/1D2N-iso400-under-";
        fname << "../Input/aligned-ISO3200/1D2N-iso3200-";
        fname << i;
        fname << ".png";
        seq.push_back(Image(fname.str()));
    }
    
    Image output1 = denoiseSeq(seq);
    output1.write("../Output/denoised.png");


    Image SNRIm = logSNR(seq,float(1/30.0));
    SNRIm.write("../Output/snr_map.png");

}
