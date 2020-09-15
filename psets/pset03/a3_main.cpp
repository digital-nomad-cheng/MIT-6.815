#include "basic_image_manipulation.hpp"
#include "filtering.hpp"
#include "align.hpp"
#include "demosaic.hpp"


using namespace std;

// This is a way for you to test your functions. 
// We will only grade the contents of a0.cpp and Image.cpp
int main() {
    /*
	// Denoise
    vector<Image> seq;
    int n_images = 44;
    for (int i = 1; i <= n_images; ++i) {
        ostringstream fname;
        fname << "../Input/aligned-ISO400/1D2N-iso400-under-";
        // fname << "../Input/aligned-ISO3200/1D2N-iso3200-";
        fname << i;
        fname << ".png";
        seq.push_back(Image(fname.str()));
    }
    
    Image output1 = denoiseSeq(seq);
    output1.write("../Output/denoised.png");


    Image SNRIm = logSNR(seq,float(1/30.0));
    SNRIm.write("../Output/snr_map.png");

    Image raw = Image("../Input/raw/signs-small.png");
    Image green = basicGreen(raw, 0);
    green.write("../Output/demosaic_green.png");

    */

    Image raw = Image("../Input/raw/signs-small.png");
    raw.debug_write();
    Image green = basicGreen(raw, 1);
    green.write("../Output/demosaic_green.png");
    Image red = basicRorB(raw, 1, 1);
    red.write("../Output/demosaic_red.png");
    Image blue = basicRorB(raw, 0, 0);
    blue.write("../Output/demosaic_blue.png");
    
    Image rgb = basicDemosaic(raw, 1, 1, 1, 0, 0);
    rgb.write("../Output/demosaiced.png");

    Image edge_green = edgeBasedGreen(raw, 1);
    edge_green.write("../Output/demosaic_edge_green.png");

    Image rgb_edge = edgeBasedGreenDemosaic(raw, 1, 1,1,0,0);
    rgb_edge.write("../Output/demosaiced_edge.png");
}
