#include "matrix.hpp"
#include "homography.hpp"
#include <iostream>
#include <cassert>

using namespace std;


void testStitchScience(){
    Image monu1 = Image("Input/science-1.png");
    Image monu2 = Image("Input/science-2.png");

    CorrespondencePair corresp[4] = {
    CorrespondencePair( 75,166,1, 261,157,1),
    CorrespondencePair( 106,308,1, 305,296,1),
    CorrespondencePair( 36,304,1, 232,297,1),
    CorrespondencePair( 75,417,1, 282,409,1)
    };
    
    Image monu = stitch(monu1, monu2, corresp);
    monu.write("./Output/science_stitch.png");
}

void testStitchConvention(){
    Image monu1 = Image("Input/convention-1.png");
    Image monu2 = Image("Input/convention-2.png");

    CorrespondencePair corresp[4] = {
    CorrespondencePair( 204,294,1, 67,306,1),
    CorrespondencePair( 306,294,1, 165,296,1),
    CorrespondencePair( 255,165,1, 111,175,1),
    CorrespondencePair( 165,202,1, 18,210,1)
    };
    
    Image monu = stitch(monu1, monu2, corresp);
    monu.write("./Output/convention_stitch.png");
}


void testStitchBoston1(){
    Image monu1 = Image("Input/boston1-1.png");
    Image monu2 = Image("Input/boston1-2.png");

    CorrespondencePair corresp[4] = {
    CorrespondencePair( 253,92,1, 98,95,1),
    CorrespondencePair( 216,90,1, 61,86,1),
    CorrespondencePair( 276,221,1, 121,224,1),
    CorrespondencePair( 193,120,1, 35,114,1)
    };
    
    Image monu = stitch(monu1, monu2, corresp);
    monu.write("./Output/boston1_stitch.png");
}
