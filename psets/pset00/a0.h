/* -----------------------------------------------------------------
 * File:    a0.h
 * Created: 2015-08-29
 * -----------------------------------------------------------------
 * 
 * Assignment 00
 * 
 * ---------------------------------------------------------------*/


#ifndef A0_H_IEUMPAZD
#define A0_H_IEUMPAZD


#include <iostream>
#include <vector>

#include "image.hpp"
#include "message_assert.hpp"

using namespace std;

// Return the sum of two numbers
float getSum(float a, float b);

// Print the sum of two numbers
void helloworld(float a, float b);

// Create an image and return it
Image readAnImage(const std::string &filename);

#endif /* end of include guard: A0_H_IEUMPAZD */

