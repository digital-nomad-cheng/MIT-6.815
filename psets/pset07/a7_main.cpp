#include "matrix.hpp"
#include "homography.hpp"
#include "panorama.hpp"
#include <iostream>
#include <cassert>
#include <ctime>


using namespace std;

void testComputeTensor() {
    // load images
    Image stata1("../Input/stata-1.png");
    Image stata2("../Input/stata-2.png");

    // compute tensors
    Image tensor1 = computeTensor(stata1);
    tensor1.write("../Output/stataTensor-1.png");
    float maxi = tensor1.max();
    if(maxi != 0) {
        tensor1 = tensor1 / maxi ;
        tensor1.write("../Output/stataTensor-1normed.png");
    }


    Image tensor2 = computeTensor(stata2);
    tensor2.write("../Output/stataTensor-2.png");
    maxi = tensor2.max();
    if(maxi != 0) {
        tensor2 = tensor2 / maxi ;
        tensor2.write("../Output/stataTensor-2normed.png");
    }
}


void testCornerResponse() {
    // load images
    Image stata1("../Input/stata-1.png");
    Image stata2("../Input/stata-2.png");

    // corner responses
    Image cr1 = cornerResponse(stata1);
    float maxi = cr1.max();
    if(maxi != 0) {
        (cr1/maxi).write("../Output/stata1-cornerResponse.png");
    }
    Image cr2 = cornerResponse(stata2);
    maxi = cr2.max();
    if(maxi != 0) {
        (cr2/maxi).write("../Output/stata2-cornerResponse.png");
    }
}


void testHarrisCorners() {
    // load images
    Image stata1("../Input/stata-1.png");
    Image stata2("../Input/stata-2.png");

    // get harris corners
    vector<Point> h1 = HarrisCorners(stata1);
    vector<Point> h2 = HarrisCorners(stata2);
    visualizeCorners(stata1, h1).write("../Output/stata1-corners.png");
    visualizeCorners(stata2, h2).write("../Output/stata2-corners.png");
}


// void testDescriptor() {
//     Image stata1("./Input/stata-1.png");
//     vector<Image> lc = lumiChromi(stata1);
//     cout << "HELLO " << endl;
//     Image desc = descriptor(lc[0], Point(10, 10));
//     cout << "HELLO 2" << endl;
//     BoundingBox bbox(10-4,10+4,10-4,10+4);
//     cout << "HELLO 3" << endl;
//     Image bounding_box_im = drawBoundingBox(lc[0], bbox);
//     cout << "HELLO 3.5" << endl;
//     bounding_box_im.write("./Output/stata1-descriptor-location.png");
//     cout << "HELLO 4" << endl;
//     desc.write("./Output/stata1-descriptor-test.png");
// }


// void testComputeFeatures() {
//     // load images
//     Image stata1("./Input/stata-1.png");
//     Image stata2("./Input/stata-2.png");

//     // get and output features
//     vector<Point> h1   = HarrisCorners(stata1);
//     vector<Feature> f1 = computeFeatures(stata1, h1);
//     visualizeFeatures(stata1, f1).write("./Output/stata1-features.png");

//     vector<Point> h2 = HarrisCorners(stata2);
//     vector<Feature> f2 = computeFeatures(stata2, h2);
//     visualizeFeatures(stata2, f2).write("./Output/stata2-features.png");
// }

// // Test your l2Features function !


// void testFindCorrespondences() {
//     // load images
//     Image stata1("./Input/stata-1.png");
//     Image stata2("./Input/stata-2.png");

//     // get and output features
//     vector<Point> h1   = HarrisCorners(stata1);
//     vector<Feature> f1 = computeFeatures(stata1, h1);
//     vector<Point> h2   = HarrisCorners(stata2);
//     vector<Feature> f2 = computeFeatures(stata2, h2);

//     // correspondances
//     vector<FeatureCorrespondence> corr = findCorrespondences(f1, f2);
//     visualizePairs(stata1, stata2, corr).write("./Output/stata-featcorr.png");
// }


// void testRANSAC() {
//     Image stata1("./Input/stata-1.png");
//     Image stata2("./Input/stata-2.png");

//     // get and output features
//     vector<Point> h1   = HarrisCorners(stata1);
//     vector<Feature> f1 = computeFeatures(stata1, h1);
//     vector<Point> h2   = HarrisCorners(stata2);
//     vector<Feature> f2 = computeFeatures(stata2, h2);

//     // correspondances
//     vector<FeatureCorrespondence> corr = findCorrespondences(f1, f2);

//     Matrix H = RANSAC(corr, 100);
//     vector<bool> ins = inliers(H, corr);
//     visualizePairsWithInliers(stata1, stata2, corr, ins).write("./Output/stata-RANSAC-featcorr-inliers.png");

//     vector<Image> output = visualizeReprojection(stata1, stata2, H, corr, ins);
//     output[0].write("./Output/stata-RANSAC-1-featreproject.png");
//     output[1].write("./Output/stata-RANSAC-2-featreproject.png");
// }


// void testVisualizeCorrespondenceInliers() {
//     Image stata1("./Input/stata-1.png");
//     Image stata2("./Input/stata-2.png");

//     // get and output features
//     vector<Point> h1   = HarrisCorners(stata1);
//     vector<Feature> f1 = computeFeatures(stata1, h1);
//     vector<Point> h2   = HarrisCorners(stata2);
//     vector<Feature> f2 = computeFeatures(stata2, h2);

//     // correspondances
//     vector<FeatureCorrespondence> corr = findCorrespondences(f1, f2);

//     Matrix H(3,3);
//     vector<FeatureCorrespondence> corrSample = sampleFeatureCorrespondences(corr);
//     vector<CorrespondencePair> listOfPairs = getListOfPairs(corr); // Call this line inside of your RANsac for loop

//     H = computeHomography(listOfPairs.data());

//     vector<bool> ins = inliers(H, corr);
//     visualizePairsWithInliers(stata1, stata2, corr, ins).write("./Output/stata-featcorr-inliers.png");

//     vector<Image> output = visualizeReprojection(stata1, stata2, H, corr, ins);
//     output[0].write("./Output/stata-1-featreproject.png");
//     output[1].write("./Output/stata-2-featreproject.png");
// }


// void testAutoStitch() {
//     Image stata1("./Input/stata-1.png");
//     Image stata2("./Input/stata-2.png");
//     autostitch(stata1, stata2).write("./Output/Stata-final-stitch.png");
// }


// void testAutoStitchBoston() {
//     Image stata1("./Input/boston-skyline-1.png");
//     Image stata2("./Input/boston-skyline-2.png");
//     autostitch(stata1, stata2).write("./Output/boston-skyline-final-stitch.png");
// }


// This is a way for you to test your functions. 
// We will only grade the contents of panorama.cpp
int main() {
    cout << "nothing done in a7_main.cpp, but read me! There's some useful info here." << endl;
    srand(0); // Fixed seed for deterministic results

    // testComputeTensor();
    // testCornerResponse();
    testHarrisCorners();
    // testDescriptor();
    // testComputeFeatures();
    // testFindCorrespondences();
    // testRANSAC();
    // testVisualizeCorrespondenceInliers();
    // testAutoStitch();
    // testAutoStitchBoston();
}
