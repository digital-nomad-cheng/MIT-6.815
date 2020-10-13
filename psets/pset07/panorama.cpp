#include "panorama.hpp"

Image computeTensor(const Image &im, float sigmaG, float factorSigma)
{
	std::vector<Image> images = lumiChromi(im);
	Image lumi = images[0];
	Image blured = gaussianBlur_separable(im, sigmaG);

	Image gradient_x = gradientX(blured);
	Image gradient_y = gradientY(blured);

	Image output(im.width(), im.height(), 3);

	for (int h = 0; h < im.height(); h++) {
		for (int w = 0; w < im.width(); w++) {
			output(w, h, 0) =  gradient_x(w, h)*gradient_x(w, h);
			output(w, h, 1) =  gradient_x(w, h)*gradient_y(w, h);
			output(w, h, 2) =  gradient_y(w, h)*gradient_y(w, h);
		}
	}
	
	return gaussianBlur_separable(output, sigmaG*factorSigma);
}



/***********************************************************************
 * Point and Feature Definitions *
 **********************************************************************/
Point::Point(int xp, int yp) : x(xp), y(yp) {}
Point::Point() : x(0.0f), y(0.0f) {}
void Point::print() { printf("(%d, %d)\n", x, y); }
Vec3f Point::toHomogenousCoords() {
    Vec3f b;
    b(0) = x;
    b(1) = y;
    b(2) = 1;
    return b;
}

// Feature Constructors
Feature::Feature(Point ptp, const Image &descp) 
    : pt(ptp), dsc(descp)
{
    pt = ptp;
    dsc = descp;
}

// getter functions
Point Feature::point() { return pt;}
Image Feature::desc() { return dsc;}

// printer
void Feature::print() {
    printf("Feature:");
    point().print();
    for (int j = 0; j < dsc.height(); j++) {
        for (int i = 0; i < dsc.width(); i++) {
            printf("%+07.2f ", dsc(i, j));
        }
        printf("\n");
    }
}

// FeatureCorrespondence Constructors
FeatureCorrespondence::FeatureCorrespondence(const Feature &f1p, const Feature &f2p) 
    : f1(f1p), f2(f2p)
{
}


vector<Feature> FeatureCorrespondence::features() {
    vector<Feature> feats;
    feats.push_back(f1);
    feats.push_back(f2);
    return feats;
}


Feature FeatureCorrespondence::feature(int i) {
    if (i == 0)
        return f1;
    else
        return f2;
}


// printer
void FeatureCorrespondence::print() {
    printf("FeatureCorrespondence:");
    f1.print();
    f2.print();
}


CorrespondencePair FeatureCorrespondence::toCorrespondencePair() {

    return CorrespondencePair(
        (float) f1.point().x,
        (float) f1.point().y,
        1,
        (float) f2.point().x,
        (float) f2.point().y,
        1
    );
}

