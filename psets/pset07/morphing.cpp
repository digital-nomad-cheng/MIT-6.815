#include "morphing.hpp"

// basic vector operations
Vec2f add(const Vec2f & a, const Vec2f & b)
{
	// a+b
	return Vec2f(a.x+b.x, a.y+b.y);
} 

Vec2f subtract(const Vec2f & a, const Vec2f & b)
{
	return vec2f(a.x-b.x, a.y-b.y);
}

Vec2f scalarMult(const Vec2f & a, float f)
{
	return (a.x*f, a.y*f);
}


float dot(const Vec2f & a, const Vec2f & b)
{
	return a.x*b.x + a.y*b.y;
}

// Length of a vector
float length(const Vec2f & a); // ||a||