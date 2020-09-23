#include "homography.hpp"

Vec2f computeTransformCoordinates(const Matrix &H, int w, int h) 
{
	Vec3f target = Vec3f(w, h, 1);
	Vec3f source = H * target;
	return Vec2f (source[0] / source[2], source[1] / source[2]);
}

void applyHomography(const Image &source, const Matrix &H, Image &out, bool bilinear)
{
	Matrix inverse = H.inverse();
	for (int c = 0; c < out.channels(); c++) {
		for (int h = 0; h < out.height(); h++) {
			for (int w = 0; w < out.width(); w++) {
				Vec2f coord = computeTransformCoordinates(inverse, w, h);
				if (coord[0] >= 0 && coord[0] < source.width() && coord[1] >= 0 && coord[1] < source.height()) {
					out(w, h, c) = source(coord[0], coord[1], c);
				}
			}
		}
	}
}

// Estimate homography
Matrix computeHomography(const CorrespondencePair pairs[4])
{
	Matrix H(3, 3);
	H(2, 2) = 1.0f;


	// float a, b, c, d, e, f, g, h;
	Matrix x(8, 1);
	// x << a << b << c << d << e << f << g << h;

	Matrix A(8, 8);
	Matrix B(8, 1);
	B << pairs[0].point2[0], pairs[0].point2[1],
		 pairs[1].point2[0], pairs[1].point2[1],
		 pairs[2].point2[0], pairs[2].point2[1],
		 pairs[3].point2[0], pairs[3].point2[1];

	A << pairs[0].point1[0], pairs[0].point1[1], pairs[0].point1[2], 0, 0, 0, -pairs[0].point1[0]*pairs[0].point2[0], -pairs[0].point1[1]*pairs[0].point2[0], 
		 0, 0, 0, pairs[0].point1[0], pairs[0].point1[1], pairs[0].point1[2], -pairs[0].point1[0]*pairs[0].point2[1], -pairs[0].point1[1]*pairs[0].point2[1], 
		 pairs[1].point1[0], pairs[1].point1[1], pairs[1].point1[2], 0, 0, 0, -pairs[1].point1[0]*pairs[1].point2[0], -pairs[1].point1[1]*pairs[1].point2[0], 
		 0, 0, 0, pairs[1].point1[0], pairs[1].point1[1], pairs[1].point1[2], -pairs[1].point1[0]*pairs[1].point2[1], -pairs[1].point1[1]*pairs[1].point2[1], 
		 pairs[2].point1[0], pairs[2].point1[1], pairs[2].point1[2], 0, 0, 0, -pairs[2].point1[0]*pairs[2].point2[0], -pairs[2].point1[1]*pairs[2].point2[0], 
		 0, 0, 0, pairs[2].point1[0], pairs[2].point1[1], pairs[2].point1[2], -pairs[2].point1[0]*pairs[2].point2[1], -pairs[2].point1[1]*pairs[2].point2[1],
		 pairs[3].point1[0], pairs[3].point1[1], pairs[3].point1[2], 0, 0, 0, -pairs[3].point1[0]*pairs[3].point2[0], -pairs[3].point1[1]*pairs[3].point2[0], 
		 0, 0, 0, pairs[3].point1[0], pairs[3].point1[1], pairs[3].point1[2], -pairs[3].point1[0]*pairs[3].point2[1], -pairs[3].point1[1]*pairs[3].point2[1];


    // A * x = B;
	x = A.inverse() * B;
	
	H(0, 0) = x(0, 0);
	H(0, 1) = x(1, 0);
	H(0, 2) = x(2, 0);
	H(1, 0) = x(3, 0);
	H(1, 1) = x(4, 0);
	H(1, 2) = x(5, 0);
	H(2, 0) = x(6, 0);
	H(2, 1) = x(7, 0);
	
	return H;
}