

#ifndef __morphing__hpp
#define __morphing__hpp


#include "image.hpp"
#include "basic_image_manipulation.hpp"
#include <iostream>
#include <cmath>


using namespace std;

/// Vec2f class
///
/// 2D vector represented by its (x,y) coordinates.
/// A point really is just a vector...
class Vec2f {
public:
    Vec2f(float x = 0.0f, float y = 0.0f) : x(x), y(y) {};
    float x;
    float y;
};


// basic vector operations
Vec2f add(const Vec2f & a, const Vec2f & b); // a+b
Vec2f subtract(const Vec2f & a, const Vec2f & b); // a-b
Vec2f scalarMult(const Vec2f & a, float f); // f*a

// Dot product between two vectors
float dot(const Vec2f & a, const Vec2f & b);

// Length of a vector
float length(const Vec2f & a); // ||a||

    
/// Segment class
///
/// We represent a directed line segment by its two extremities P,Q.
///
/// *--------->
/// P         Q
///
/// P,Q are 2D points represented as 2-vectors of their float
/// cartesian coordinates (x,y).
class Segment {
    
public:
    //Constructor
    Segment(Vec2f P_, Vec2f Q_);

    // Get coordinates of point X in the local frame defined by the Segment
    Vec2f XtoUV(Vec2f X) const;
    // Get global coordinates of point X form its coordinates u,v in local
    // frame defined by the Segment
    Vec2f UVtoX(Vec2f) const;

    // Get distance of point X from the segment
    float distance(Vec2f X) const;

    // Influence weight of this segment for spatial location X
    float weight(Vec2f X, float a, float b, float p) const;

    // Readonly accessors
    const Vec2f & getP() const { return P; };
    const Vec2f & getQ() const { return Q; };
    const Vec2f & getE1() const { return e1; };
    const Vec2f & getE2() const { return e2; };
    float getLength() const { return lPQ; };

private:
    Vec2f P;
    Vec2f Q;
    float lPQ; // length of PQ

    // We define the local frame of the segment as follows:
    //  ^
    //  |
    //  
    //  e2
    //   
    //  |
    //  *--- e1 --->
    //  P          Q
    //
    //  e1 = (Q-P)/(||Q-P||) in the paper
    //  e2 = perpendicalur(Q-P)/(||Q-P||) in the paper 

    // Local frame
    Vec2f e1;
    Vec2f e2;
};

Image warpBy1(const Image &im, const Segment &segBefore, const Segment &segAfter); 
Image warp(const Image &im, const vector<Segment> &segsBefore, const vector<Segment> &segsAfter, float a=10.0, float b=1.0, float p=1.0);
vector<Image> morph(const Image &im1, const Image &im2, const  vector<Segment> &segsBefore, const vector<Segment> &segsAfter, int N=1, float a=10.0, float b=1.0, float p=1.0);

 
#endif
