#include "matrix.hpp"
#include "homography.hpp"
#include <iostream>
#include <cassert>
#include <ctime>

// for 6.865 only
#include "homography_extra_tests.hpp"

using namespace std;


// test apply homography function
void testApplyHomography() {
    Matrix H(3, 3);
    // This is one way to fill a Matrix, in row-major order:
    H <<
        1.23445,  -0.0258246,    96,
       0.210363,     1.00462,   171,
     0.00108756, -0.00010989,     1;

    // This is another:
    // H(0,0) = 0.964545f;
    // ...

    cout << "Given manual homography for bus example:" << endl;
    cout << H << endl;

    // Load inputs
    Image poster("Input/poster.png");
    Image green("Input/green.png");

    // apply homography 
    applyHomography(poster, H, green, true);
    green.write("Output/apply_homography.png");
}


// void testComputeHomography() {
//     Image poster("Input/poster.png");
//     Image green("Input/green.png");

//     // build 3D array (easier to initialize)
//     float w = (float) poster.width();
//     float h = (float) poster.height();
//     Vec3f p1;
//     Vec3f p2;

//     CorrespondencePair corresp[4] = {
//         CorrespondencePair( 0,0,1, 96,171,1), 
//         CorrespondencePair( w-1,0,1, 235,174,1), 
//         CorrespondencePair( w-1,h-1,1, 235,232,1), 
//         CorrespondencePair( 0,h-1,1, 95,238,1)
//     };

//     // Compute homography.
//     Matrix Hcomputed = computeHomography(corresp);
    
//     // This should get you the values from 
//     // testApplyHomography(); (or close to)
//     cout << "Computed homography for bus example from given point pairs" << endl;
//     cout << Hcomputed << endl;

//     // Transform image and write output
//     applyHomography(poster,  Hcomputed, green,  true);
//     green.write("Output/computeHomography.png");
// }

// void testComputeTransformedBBox() {
//     Matrix H(3, 3);
//     // This is one way to fill a Matrix:
//     H <<
//         0.964545,   0.0249724,  655.465,
//         0.234809,   0.317685,   418.738,
//         0.00068247, 1.02387e-05,    1;

//     Image poster("Input/sunset-1.png");
//     BoundingBox bbox = computeTransformedBBox(poster.width(), poster.height(), H);
//     cout << "Computed transformed BBox for Poster:" << endl;
//     cout << bbox.x1 << " " <<  bbox.x2 << " " << bbox.y1 << " " << bbox.y2 << " " << endl;


//     // Students TODO: drawBoundingBox would be useful to implement for debugging
//     Image green("Input/bus_small.png");
//     Image greenwithbox = drawBoundingBox(green, bbox);
//     greenwithbox.write("Output/draw_bbox.png");
// }

// // test union
// void testBBoxUnion() {
//     BoundingBox bbox1( -15, 25, 3, 109);
//     BoundingBox bbox2( -75, 19, 9, 20);
//     BoundingBox bbox  = bboxUnion(bbox1, bbox2);
//     assert(bbox.x1 == -75 && bbox.x2 == 25 && bbox.y1 == 3 && bbox.y2 ==  109);
//     cout << bbox.x1 << " " <<  bbox.x2 << " " << bbox.y1 << " " << bbox.y2 << " " << endl;
// }

// void testTranslate() {
//     BoundingBox bbox(
//         -15,
//         25,
//         3,
//         109
//     );

//     // look at translation matrix
//     cout << "Translation Matrix:" << endl;
//     Matrix m = makeTranslation(bbox);
//     cout << m << endl;

//     // transform top left of bounding box, see if it's [0,0].
//     Matrix b(3, 1);
//     b << bbox.x1, bbox.y1, 1;
//     cout << "Corner of translated Bounding Box:" << endl;
//     cout << m*b << endl;
// }

// void testStitch() {
//     // test boston
//     Image monu1 = Image("Input/monu-1.png");
//     Image monu2 = Image("Input/monu-2.png");
//     CorrespondencePair corresp[4] = {
//         CorrespondencePair(84  , 108 , 1 , 1  , 96  , 1),
//         CorrespondencePair(173 , 131 , 1 , 96 , 131 , 1),
//         CorrespondencePair(94  , 218 , 1 , 14 , 224 , 1),
//         CorrespondencePair(135 , 202 , 1 , 64 , 202 , 1)
//     };
//     Image monu = stitch(monu1, monu2, corresp);
//     monu.write("./Output/stitch.png");
// }

// void testApplyHomographyFast() {

//     clock_t start;

//     Matrix H(3, 3);
//     // This is one way to fill a Matrix, in row-major order:
//     H <<
//         1.23445,  -0.0258246,    96,
//         0.210363,     1.00462,   171,
//         0.00108756, -0.00010989,     1;

//     cout << H << endl;

//     cout << "In apply homography fast! " << endl;

//     // apply homography and output image
//     Image poster("Input/poster.png");
//     Image green("Input/green.png");
//     start = std::clock();
//     cout << "Before applyHomography! " << endl;

//     applyHomography(poster, H, green, false);
//     cout << "After applyHomography! " << endl;

//     float timediff = (std::clock() - start) / (double)(CLOCKS_PER_SEC / 1000);
//     printf("Time taken with normal applyhomography: %fms\n", timediff);
//     green.write("Output/greenwithposter_slow.png");

//     // apply it fast
//     green = Image("Input/green.png");
//     start = std::clock();
//     applyHomographyFast(poster, H, green, false);
//     timediff = (std::clock() - start) / (double)(CLOCKS_PER_SEC / 1000);
//     printf("Time taken with fast time: %fms\n", timediff);
//     green.write("Output/greenwithposter_fast.png");
// }

// This is a way for you to test your functions. 
// We will only grade the contents of morphing.cpp and basicImageManipulation.cpp
int main() {
    cout << "nothing done in a6_main.cpp, but read me! There's some useful info here." << endl;

    // -------------------------------------------------------
    // In this pset we use Eigen as our linear algebra library
    // -------------------------------------------------------
    
    // This is how you allocate a matrix:
    // int n_rows = 6, n_cols = 10;
    // Matrix M(n_rows, n_cols);
    // At this point, M contains garbage data. 
    
    // You can inspect it like so:
    // cout << M << endl;
    
    // You can modify the individual entries like this:
    // A(0,4) = 0.4f;
    
    // Or you can initialize a matrix of all zeros like this
    // Matrix M2 = Matrix::Zero(n_rows, n_cols);
    
    // You can do matrix-matrix multiplications like that:
    // Matrix A(3,5);
    // Matrix B(5,7);
    // Matrix C = A*B;
    
    // We provide two shortcuts for 2D and 3D vectors:
    // Vec2f x; // equivalent to Matrix x(2,1);
    // Vec3f y; // equivalent to Matrix y(3,1);
    
    // So you can do matrix-vector product like so:
    // Matrix D(4,2);
    // Matrix E = D*x;
    
    // You get the inverse of a matrix as:
    // Matrix F = E.inverse();
    // For more information on the operations you can do with Eigen,
    // consult this page:
    // http://eigen.tuxfamily.org/index.php?title=Main_Page
    
    // To solve the linear system Ax = b, you can do x = A^{-1}*b
    // or use a solver routine such as:
    // Matrix x = A.fullPivLu().solve(b);

    // ---------------
    // | A few tests |
    // ---------------
    testApplyHomography();
    return 0;
    // testComputeHomography();
    // testComputeTransformedBBox();
    // testBBoxUnion();
    // testTranslate();
    // testStitch();


    // // ---------------
    // // for 6.865 only
    // testApplyHomographyFast();
    // testStitchScience();
    // testStitchConvention();
    // testStitchBoston1();
    // ---------------
}