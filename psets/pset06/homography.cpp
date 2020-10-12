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

// bbox related functions
BoundingBox computeTransformedBBox(int imwidth, int imheight, Matrix H)
{
	Matrix src0(3, 1);
	Matrix dst0(3, 1);
	Matrix src1(3, 1);
	Matrix dst1(3, 1);
	Matrix src2(3, 1);
	Matrix dst2(3, 1);
	Matrix src3(3, 1);
	Matrix dst3(3, 1);


	int x1 = INT_MAX;
	int x2 = INT_MIN;
	int y1 = INT_MAX;
	int y2 = INT_MIN;

	src0 << 0, 0, 1;
	dst0 = H * src0;

	src1 << 0, imheight, 1;
	dst1 = H * src1;

	src2 << imwidth, 0, 1;
	dst2 = H * src2;

	src3 << imwidth, imheight, 1;
	dst3 = H * src3;


	dst0(0, 0) /= dst0(2, 0);
	dst0(1, 0) /= dst0(2, 0);

	dst1(0, 0) /= dst1(2, 0);
	dst1(1, 0) /= dst1(2, 0);

	dst2(0, 0) /= dst2(2, 0);
	dst2(1, 0) /= dst2(2, 0);

	dst3(0, 0) /= dst3(2, 0);
	dst3(1, 0) /= dst3(2, 0);


	if (x1 > dst0(0, 0)) x1 = dst0(0, 0);
	if (x1 > dst1(0, 0)) x1 = dst1(0, 0);
	if (x1 > dst2(0, 0)) x1 = dst2(0, 0);
	if (x1 > dst3(0, 0)) x1 = dst3(0, 0);


	if (y1 > dst0(1, 0)) y1 = dst0(1, 0);
	if (y1 > dst1(1, 0)) y1 = dst1(1, 0);
	if (y1 > dst2(1, 0)) y1 = dst2(1, 0);
	if (y1 > dst3(1, 0)) y1 = dst3(1, 0);


	if (x2 < dst0(0, 0)) x2 = dst0(0, 0);
	if (x2 < dst1(0, 0)) x2 = dst1(0, 0);
	if (x2 < dst2(0, 0)) x2 = dst2(0, 0);
	if (x2 < dst3(0, 0)) x2 = dst3(0, 0);


	if (y2 < dst0(1, 0)) y2 = dst0(1, 0);
	if (y2 < dst1(1, 0)) y2 = dst1(1, 0);
	if (y2 < dst2(1, 0)) y2 = dst2(1, 0);
	if (y2 < dst3(1, 0)) y2 = dst3(1, 0);

	return BoundingBox(x1, x2, y1, y2);
}

// debug-useful
Image drawBoundingBox(const Image &im, BoundingBox bbox) {
    // // --------- HANDOUT  PS06 ------------------------------
    //  ________________________________________
    // / Draw me a bounding box!                \
    // |                                        |
    // | "I jumped to my                        |
    // | feet, completely thunderstruck. I      |
    // | blinked my eyes hard. I looked         |
    // | carefully all around me. And I saw a   |
    // | most extraordinary small person, who   |
    // | stood there examining me with great    |
    // | seriousness."                          |
    // \              Antoine de Saint-Exupery  /
    //  ----------------------------------------
    //         \   ^__^
    //          \  (oo)\_______
    //             (__)\       )\/\
    //                 ||----w |
    //                 ||     ||

    Image output = im;
    std::cout << "x1 is: " << bbox.x1 << " x2: " << bbox.x2 << " y1: " << bbox.y1 << " y2: " << bbox.y2 << endl;
    for (int i = bbox.x1; i < bbox.x2; i++){
        output(i,bbox.y1,0) = 0;
        output(i,bbox.y1,1) = 1;
        output(i,bbox.y1,2) = 0;  

        output(i,bbox.y2,0) = 0;
        output(i,bbox.y2,1) = 1;
        output(i,bbox.y2,2) = 0;  
    }


    for (int j = bbox.y1; j < bbox.y2; j++){
        output(bbox.x1,j,0) = 0;
        output(bbox.x1,j,1) = 1;
        output(bbox.x1,j,2) = 0;  

        output(bbox.x2,j,0) = 0;
        output(bbox.x2,j,1) = 1;
        output(bbox.x2,j,2) = 0;   
    }

    return output;
}
