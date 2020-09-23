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