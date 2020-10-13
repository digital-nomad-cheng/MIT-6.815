/* --------------------------------------------------------------------------
 * File:    panorama.hpp
 * Created: 2020-10-29
 * --------------------------------------------------------------------------
 * 
 * 
 * 
 * ------------------------------------------------------------------------*/


#ifndef __panorama__hpp
#define __panorama__hpp

#include <iostream>
#include <algorithm>    // std::max, random_shuffle
#include <cmath>

#include "image.hpp"
#include "matrix.hpp"
#include "filtering.hpp"
#include "homography.hpp"
#include "basic_image_manipulation.hpp"

using namespace std;


// panorama.hpp
// Assignment 7

// ---------------------------------------------------------
// Point and Feature classes, already implemented for Pset07
class Point {
public:
    int x, y; // point info
    Point(int xp, int yp); // Constructors
    Point();              // Constructors
    void print(); // helpful printing
    Vec3f toHomogenousCoords();
};


class Feature {
public:
    Feature(Point ptp, const Image &descp); // Constructor
    Point point(); // get point (this calls point cc)
    Image desc(); // get Image (this maybe calls image cc)
    void print(); // pretty printing
private:
    Point pt;
    Image dsc; // 9x9 descriptor
};


class FeatureCorrespondence {
public:
    FeatureCorrespondence(const Feature &f1p, const Feature &f2p); // Constructors
    vector<Feature> features();
    Feature feature(int i);
    void print(); // helpful printing
    CorrespondencePair toCorrespondencePair();
private:
    Feature f1, f2; // corresp info
};
// ---------------------------------------------------------



// Pset07: Harris Corners
Image computeTensor(const Image &im, float sigmaG=1, float factorSigma=4);
Image cornerResponse(const Image &im, float k=0.15, float sigmaG=1, float factorSigma=4);
vector <Point> HarrisCorners(const Image &im, float k=0.15, float sigmaG=1, float factorSigma=4, float maxiDiam=7, float boundarySize=5);

// Pset07: FeatureCorrespondences
Image descriptor(const Image &blurredIm, Point p, float radiusDescriptor=4);
vector <Feature> computeFeatures(const Image &im, vector<Point> cornersL, float sigmaBlurDescriptor=0.5, float radiusDescriptor=4);
float l2Features(Feature &f1, Feature &f2);
vector <FeatureCorrespondence> findCorrespondences(vector<Feature> listFeatures1, vector<Feature> listFeatures2, float threshold=1.7);

// Pset07: RANSAC
vector<CorrespondencePair> getListOfPairs(vector <FeatureCorrespondence> listOfCorrespondences);
vector<bool> inliers(Matrix H, vector <FeatureCorrespondence> listOfCorrespondences, float epsilon=4);
Matrix RANSAC(vector <FeatureCorrespondence> listOfCorrespondences, int Niter=200, float epsilon=4);
vector<FeatureCorrespondence> sampleFeatureCorrespondences(vector <FeatureCorrespondence> listOfCorrespondences);

// PSet07: Final stitching
Image autostitch(Image &im1, Image &im2, float blurDescriptor=0.5, float radiusDescriptor=4);



// ---------------------------------------------------------

// potentially useful function, optional to implement
Image getBlurredLumi(const Image &im, float sigmaG);
int countBoolVec(vector<bool> ins);

// ---------------------------------------------------------

/***********************************************************************
 * Helper Functions, already implemented in Pset07 *
 ***********************************************************************/

static const float color_init[3] = {1.0, 1.0, 1.0};
static const vector<float> color_vinit(color_init, color_init + 3);

void drawLine(Point p1, Point p2, Image &im, const vector<float> & color = color_vinit);

Image visualizeCorners(const Image &im, vector<Point> pts, int rad=2, const vector<float> & color = color_vinit);
Image visualizeFeatures(const Image &im, vector <Feature> LF, float radiusDescriptor=4);

Image visualizePairs(const Image &im1, const Image &im2, vector<FeatureCorrespondence> corr);
Image visualizePairsWithInliers(const Image &im1, const Image &im2, vector<FeatureCorrespondence> corr, const vector<bool> & ins);

vector<Image> visualizeReprojection(const Image &im1, const Image &im2, Matrix H, vector<FeatureCorrespondence> & corr, const vector<bool> & ins);
#endif
