#include "image.hpp"
#include "basic_image_manipulation.hpp"
#include "hdr.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>


using namespace std;

void testComputeWeight() { 
    // load in image and invert gamma correction
    Image im1("../Input/ante1-1.png");
    im1 = changeGamma(im1, 1.0/2.2, 1.0f);

    // compute the weight image and save it
    Image weight = computeWeight(im1, 0.01, 0.99);
    weight.write("../Output/weight.png");
}

void testComputeFactor() {
    // load 2 images
    Image im1("../Input/ante2-1.png");
    Image im2("../Input/ante2-2.png");
    
    // invert gamma correction
    im1 = changeGamma(im1, 1.0/2.2, 1.0f);
    im2 = changeGamma(im2, 1.0/2.2, 1.0f);

    // compute weight images and save them
    Image w1 = computeWeight(im1);
    Image w2 = computeWeight(im2);
    w1.write("../Output/ante2-w1.png");
    w2.write("../Output/ante2-w2.png");

    // compute the factor
    float factor = computeFactor(im1, w1, im2, w2);
    cout << "factor: " << factor << endl;
}


void testMakeHDR() {
    // load an image sequence
    vector<Image> imSeq;

    imSeq.push_back(changeGamma(Image("../Input/design-1.png"), 1.0/2.2, 1.0f));
    imSeq.push_back(changeGamma(Image("../Input/design-2.png"), 1.0/2.2, 1.0f));
    imSeq.push_back(changeGamma(Image("../Input/design-3.png"), 1.0/2.2, 1.0f));
    imSeq.push_back(changeGamma(Image("../Input/design-4.png"), 1.0/2.2, 1.0f));
    imSeq.push_back(changeGamma(Image("../Input/design-5.png"), 1.0/2.2, 1.0f));
    imSeq.push_back(changeGamma(Image("../Input/design-6.png"), 1.0/2.2, 1.0f));
    imSeq.push_back(changeGamma(Image("../Input/design-7.png"), 1.0/2.2, 1.0f));

    // generate an hdr image
    Image hdr = makeHDR(imSeq);
    hdr.debug_write();

    // save out images clipped to different ranges.
    float maxVal = hdr.max();
    std::cout << maxVal << std::endl;
    Image hdrScale0 = hdr/maxVal;
    hdrScale0.write("../Output/scaledHDR_design_0.png");
    Image hdrScale2 = (2e2)*hdr/maxVal;
    hdrScale2.write("../Output/scaledHDR_design_2.png");
    Image hdrScale4 = (2e4)*hdr/maxVal;
    hdrScale4.write("../Output/scaledHDR_design_4.png");
    Image hdrScale6 = (2e6)*hdr/maxVal;
    hdrScale6.write("../Output/scaledHDR_design_6.png");
    Image hdrScale8 = (2e8)*hdr/maxVal;
    hdrScale8.write("../Output/scaledHDR_design_8.png");
    Image hdrScale10 = (2e10)*hdr/maxVal;
    hdrScale10.write("../Output/scaledHDR_design_10.png");

}

// HDR and Tone Mapping on Ante2 images
void testToneMapping_ante2() {
    
    // load images
    vector<Image> imSeq;
    imSeq.push_back(changeGamma(Image("../Input/ante2-1.png"), 1.0/2.2, 1.0f));
    imSeq.push_back(changeGamma(Image("../Input/ante2-2.png"), 1.0/2.2, 1.0f));
    
    // create hdr image
    std::cout << "In ante2 before makeHDR" << std::endl;

    Image hdr = makeHDR(imSeq);
    std::cout << "In ante2 after makeHDR" << std::endl;
    
    std::cout << "In tone map with gaussian blur " << std::endl;
    // tone map with gaussian blur
    Image tm = toneMap(hdr, 100, 1, false);
    std::cout << "After tone map with gaussian blur" << std::endl;
    tm = changeGamma(tm, 1.0, 1.0/2.2);
    tm.write("../Output/ante2-tonedHDRsimple-gauss.png");

    // tone map with bilateral blur
    tm = toneMap(hdr, 100, 3, true, 0.1);
    tm = changeGamma(tm, 1.0, 1.0/2.2);
    tm.write("../Output/ante2-tonedHDRsimple-bilateral.png");
    
}

// HDR and Tone Mapping on Ante3 images
void testToneMapping_ante3() {
    
    // load images
    vector<Image> imSeq;
    imSeq.push_back(changeGamma(Image("../Input/ante3-1.png"), 1.0/2.2, 1.0f));
    imSeq.push_back(changeGamma(Image("../Input/ante3-2.png"), 1.0/2.2, 1.0f));
    imSeq.push_back(changeGamma(Image("../Input/ante3-3.png"), 1.0/2.2, 1.0f));
    imSeq.push_back(changeGamma(Image("../Input/ante3-4.png"), 1.0/2.2, 1.0f));

    // create hdr image
    Image hdr = makeHDR(imSeq);

    // tone map with gaussian blur
    Image tm = changeGamma(toneMap(hdr, 100, 1),1.0f, 1.0f/2.2f);
    tm.write("../Output/ante3-tonedHDRsimple-gauss.png");
    
    // tone map with bilaterial
    tm = changeGamma(toneMap(hdr, 100, 3, true, 0.1),1.0f, 1.0f/2.2f);
    tm.write("../Output/ante3-tonedHDRsimple-bilateral.png");
    
}

// HDR and Tone Mapping on Boston Images
void testToneMapping_boston() {
    
    // load images
    vector<Image> imSeq;
    imSeq.push_back(changeGamma(Image("../Input/boston-1.png"), 1.0/2.2, 1.0f));
    imSeq.push_back(changeGamma(Image("../Input/boston-2.png"), 1.0/2.2, 1.0f));
    imSeq.push_back(changeGamma(Image("../Input/boston-3.png"), 1.0/2.2, 1.0f));

    // create hdr image
    Image hdr = makeHDR(imSeq);
    
    // tone map with gaussian blur
    Image tm = toneMap(hdr, 100, 1, false);
    tm = changeGamma(tm, 1.0, 1.0/2.2);
    tm.write("../Output/boston-tonedHDRsimple-gauss.png");

    // tone map with bilaterial
    tm = toneMap(hdr, 100, 3, true, 0.1);
    tm = changeGamma(tm, 1.0, 1.0/2.2);
    tm.write("../Output/boston-tonedHDRsimple-bilateral.png");
}

void testToneMapping_design() {
    
    // load images
    vector<Image> imSeq;
    imSeq.push_back(changeGamma(Image("../Input/design-1.png"), 1.0/2.2, 1.0f));
    imSeq.push_back(changeGamma(Image("../Input/design-2.png"), 1.0/2.2, 1.0f));
    imSeq.push_back(changeGamma(Image("../Input/design-3.png"), 1.0/2.2, 1.0f));
    imSeq.push_back(changeGamma(Image("../Input/design-4.png"), 1.0/2.2, 1.0f));
    imSeq.push_back(changeGamma(Image("../Input/design-5.png"), 1.0/2.2, 1.0f));
    imSeq.push_back(changeGamma(Image("../Input/design-6.png"), 1.0/2.2, 1.0f));
    imSeq.push_back(changeGamma(Image("../Input/design-7.png"), 1.0/2.2, 1.0f));
    
    // create hdr image
    Image hdr = makeHDR(imSeq);
    
    // tone map with gaussian blur
    Image tm = toneMap(hdr, 100, 1, false);
    tm = changeGamma(tm, 1.0, 1.0/2.2);
    tm.write("../Output/design-tonedHDRsimple-gauss.png");
    
    // Note: bilaterial filtering these images takes a very long time. It is not
    // necessary to attempt this for testing
}


// This is a way for you to test your functions. 
// We will only grade the contents of demosaic.cpp and align.cpp
int main() {
    
    /*
    std::cout << "Before compute weight" << std::endl;
    testComputeWeight();
    
    std::cout << "Before compute factor" << std::endl;
    testComputeFactor();

    std::cout << "Before makeHDR" << std::endl;
    testMakeHDR();
    */

    
    std::cout << "before tone ante2" << std::endl;
    testToneMapping_ante2();

    
    std::cout << "before tone ante3" << std::endl;
    testToneMapping_ante3();

    std::cout << "before tone boston" << std::endl;
    testToneMapping_boston();

    std::cout << "before tone desing" << std::endl;
    testToneMapping_design();
    
    return 0;
}