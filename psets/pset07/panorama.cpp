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


Image cornerResponse(const Image &im, float k, float sigmaG, float factorSigma)
{
	Image struct_tensor = computeTensor(im, sigmaG, factorSigma);
	Image output(im.width(), im.height(), 1);

	for (int h = 0; h < output.height(); h++) {
		for (int w = 0; w < output.width(); w++) {
			Matrix M(2, 2);
			M << struct_tensor(w, h, 0), struct_tensor(w, h, 1), struct_tensor(w, h, 1), struct_tensor(w, h, 2);
			output(w, h) = M.determinant() - k*M.trace()*M.trace();
		}
	}
	return output;
}

vector <Point> HarrisCorners(const Image &im, float k, float sigmaG, float factorSigma, float maxiDiam, float boundarySize)
{
	std::vector<Point> results;
	Image response = cornerResponse(im);
	Image max_response = maximum_filter(response, maxiDiam);
	for (int h = boundarySize; h < im.height() - boundarySize; h++) {
		for (int w = boundarySize; w < im.width() - boundarySize; w++) {
			if (response(w, h) > 0.0f && max_response(w, h) == response(w, h)) {
				results.push_back(Point(w, h));
			}
		}
	}

	return results;
}



// Corner visualization.
Image visualizeCorners(const Image &im, vector<Point> pts, 
        int rad, const vector <float> & color) 
{
    Image vim = im;
    for (int i = 0; i < (int) pts.size(); i++) {
        int px = pts[i].x;
        int py = pts[i].y;

        int minx = max(px - rad, 0);

        for (int delx = minx; delx < min(im.width(), px + rad + 1); delx++) 
        for (int dely = max(py - rad, 0); dely < min(im.height(), py + rad + 1); dely++) 
        {
            if ( sqrt(pow(delx-px, 2) + pow(dely - py, 2)) <= rad) {
                for (int c = 0; c < im.channels(); c++) {
                    vim(delx, dely, c) = color[c];
                }
            }
        }
    }
    return vim;
}

Image visualizeFeatures(const Image &im, vector <Feature> LF, float radiusDescriptor) {
    // assumes desc are within image range
    Image vim = im;
    int rad = radiusDescriptor;

    for (int i = 0; i < (int) LF.size(); i++) {
        int px = LF[i].point().x;
        int py = LF[i].point().y;
        Image desc = LF[i].desc();

        for (int delx = px - rad; delx < px + rad + 1; delx++) {
            for (int dely = py - rad; dely < py + rad + 1; dely++) {
                vim(delx, dely, 0) = 0;
                vim(delx, dely, 1) = 0;
                vim(delx, dely, 2) = 0;

                if (desc(delx - (px-rad), dely - (py - rad)) > 0) {
                    vim(delx, dely, 1) = 1;
                } else if (desc(delx - (px-rad), dely - (py - rad)) < 0) {
                    vim(delx, dely, 0) = 1;
                }
            }
        }
    }
    return vim;
}

void drawLine(Point p1, Point p2, Image &im,  const vector<float> & color) {
    float minx = min(p1.x, p2.x);
    float miny = min(p1.y, p2.y);
    float maxx = max(p1.x, p2.x);
    float maxy = max(p1.y, p2.y);

    int spaces = 1000;
    for (int i = 0; i < spaces; i++) {
        float x = minx + (maxx - minx) / spaces * (i+1);
        float y = miny + (maxy - miny) / spaces * (i+1);
        for (int c = 0; c < im.channels(); c++) {
            im(x, y, c) = color[c];
        }
    }
}

Image visualizePairs(const Image &im1, const Image &im2, vector<FeatureCorrespondence> corr) {
    Image vim(im1.width() + im2.width(), im1.height(), im1.channels());

    // stack the images
    for (int j = 0; j < im1.height(); j++) {
        for (int c = 0; c < im1.channels(); c++) {
            for (int i = 0; i < im1.width(); i++) {
                vim(i,j,c) = im1(i,j,c);
            }
            for (int i = 0; i < im2.width(); i++) {
                vim(i+im1.width(),j,c) = im2(i,j,c);
            }
        }
    }

    // draw lines
    for (int i = 0; i < (int) corr.size(); i++) {
        Point p1 = corr[i].feature(0).point();
        Point p2 = corr[i].feature(1).point();
        p2.x = p2.x + im1.width();
        drawLine(p1, p2, vim);
    }
    return vim;
}

Image visualizePairsWithInliers(const Image &im1, const Image &im2, vector<FeatureCorrespondence> corr, const vector<bool> & ins) {
    Image vim(im1.width() + im2.width(), im1.height(), im1.channels());

    // stack the images
    for (int j = 0; j < im1.height(); j++) {
        for (int c = 0; c < im1.channels(); c++) {
            for (int i = 0; i < im1.width(); i++) {
                vim(i,j,c) = im1(i,j,c);
            }
            for (int i = 0; i < im2.width(); i++) {
                vim(i+im1.width(),j,c) = im2(i,j,c);
            }
        }
    }

    // draw lines
    vector<float> red(3,0);
    vector<float> green(3,0);
    red[0] = 1.0f;
    green[1]= 1.0f;

    for (int i = 0; i < (int) corr.size(); i++) {
        Point p1 = corr[i].feature(0).point();
        Point p2 = corr[i].feature(1).point();
        p2.x = p2.x + im1.width();
        if (ins[i]) {
            drawLine(p1, p2, vim, green);
        } else {
            drawLine(p1, p2, vim, red);
        }
    }
    return vim;

}

// Inliers:  Detected corners are in green, reprojected ones are in red
// Outliers: Detected corners are in yellow, reprojected ones are in blue
vector<Image> visualizeReprojection(const Image &im1, const Image &im2, Matrix  H, vector<FeatureCorrespondence> & corr, const vector<bool> & ins) {
    // Initialize colors
    vector<float> red(3,0);
    vector<float> green(3,0);
    vector<float> blue(3,0);
    vector<float> yellow(3,0);
    red[0] = 1.0f;
    green[1]= 1.0f;
    blue[2] = 1.0f;
    yellow[0] = 1.0f;
    yellow[1] = 1.0f;

    vector<Point> detectedPts1In;
    vector<Point> projectedPts1In;
    vector<Point> detectedPts1Out;
    vector<Point> projectedPts1Out;

    vector<Point> detectedPts2In;
    vector<Point> projectedPts2In;
    vector<Point> detectedPts2Out;
    vector<Point> projectedPts2Out;

    for (int i = 0 ; i < (int) corr.size(); i++) {
        Point pt1 = corr[i].feature(0).point();
        Point pt2 = corr[i].feature(1).point();
        Matrix P1 = pt1.toHomogenousCoords();
        Matrix P2 = pt2.toHomogenousCoords();
        Matrix P2_proj = H*P1;
        Matrix P1_proj = H.inverse()*P2;
        Point reproj1 = Point(P1_proj(0)/P1_proj(2), P1_proj(1)/P1_proj(2));
        Point reproj2 = Point(P2_proj(0)/P2_proj(2), P2_proj(1)/P2_proj(2));
        if (ins[i]) { // Inlier
            detectedPts1In.push_back(pt1);
            projectedPts1In.push_back(reproj1);
            detectedPts2In.push_back(pt2);
            projectedPts2In.push_back(reproj2);
        } else { // Outlier
            detectedPts1Out.push_back(pt1);
            projectedPts1Out.push_back(reproj1);
            detectedPts2Out.push_back(pt2);
            projectedPts2Out.push_back(reproj2);
        }
    }

    vector<Image> output;
    Image vim1(im1);
    Image vim2(im2);
    vim1 = visualizeCorners(im1, detectedPts1In,2, green);
    vim1 = visualizeCorners(vim1, projectedPts1In,1, red);
    vim1 = visualizeCorners(vim1, detectedPts1Out,2, yellow);
    vim1 = visualizeCorners(vim1, projectedPts1Out,1, blue);

    vim2 = visualizeCorners(im2, detectedPts2In,2, green);
    vim2 = visualizeCorners(vim2, projectedPts2In,1, red);
    vim2 = visualizeCorners(vim2, detectedPts2Out,2, yellow);
    vim2 = visualizeCorners(vim2, projectedPts2Out,1, blue);


    output.push_back(vim1);
    output.push_back(vim2);
    return output;
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

