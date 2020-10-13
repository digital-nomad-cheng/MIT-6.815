#ifndef __IMAGE_EXCEPTION_HPP
#define __IMAGE_EXCEPTION_HPP

#include <stdexcept>
class DivideByZeroException : public std::runtime_error 
{
public:
	DivideByZeroException() : std::runtime_error("Divisor is zero!") {}
};

class MismatchedDimensionsException : public std::runtime_error {
    public:
        MismatchedDimensionsException() :
            std::runtime_error("Image dimensions are not the same.") {}
};

class NegativeDimensionException : public std::runtime_error {
    public:
        NegativeDimensionException() :
            std::runtime_error("Image dimensions must be nonnegative.") {}
};

class ChannelException : public std::runtime_error {
    public:
        ChannelException() :
            std::runtime_error("Number of channels must be 1, 3 or 4 when writing to image.") {}
};

class OutOfBoundsException : public std::runtime_error {
    public:
        OutOfBoundsException() :
            std::runtime_error("Index is out of the image bounds.") {}
};

class InvalidArgument : public std::runtime_error {
    public:
        InvalidArgument() : 
            std::runtime_error("Argument is not valid.") {}
};

class FileNotFoundException : public std::runtime_error {
    public:
        FileNotFoundException() : 
            std::runtime_error("Empty input or file does not exist.") {}
};

#endif