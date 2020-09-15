#include "demosaic.hpp"

float interpGreen(const Image &raw, int x, int y) {
	assert(x > 0 && x < raw.width() - 1);
	assert(y > 0 && y < raw.height() - 1);

	float interpVal = raw(x + 1, y) +
					  raw(x - 1, y) +
					  raw(x, y + 1) +
					  raw(x, y - 1);
	interpVal /= 4;
	return interpVal;
}

Image basicGreen(const Image &raw, int offset)
{
	// If no offset, then 
	// G N G N G N G
	// N G N G N G N
	// G N G N G N G
	// Even rows have even greens
	// Odd rows have odd greens
	// With offset, even rows have odd greens, odd rows have even


	Image output(raw.width(), raw.height(), 1);

    bool condition;
    if (offset % 2 == 0){
        condition = true;
    }
    else {
        condition = false;
    }

    for (int a = 1; a < raw.width() - 1; a++){
        for (int b = 1; b < raw.height() - 1; b++){
            if ((a % 2 == b % 2) == condition){
                output(a,b) = raw(a,b);
            }
            else{
                output(a,b) = (raw(a-1,b) + raw(a+1,b) + raw(a,b+1) + raw(a,b-1))/4.0; 
            }
        }
    }
    return output;
}


Image basicRorB(const Image &raw, int offsetX, int offsetY)
{
    Image output(raw.width(), raw.height(), 1);
    for (int a = 1; a < raw.width() - 1; a++){
        for (int b = 1; b < raw.height() - 1; b++){
            if (a % 2 == offsetX && b % 2 == offsetY) {
                output(a,b) = raw(a,b);
            }
            else if (a % 2 == offsetX && b % 2 != offsetY){
                output(a,b) = (raw(a,b-1) + raw(a,b+1))/2.0;
            }
            else if (a % 2 != offsetX && b % 2 == offsetY){
                output(a,b) = (raw(a-1,b) + raw(a+1,b))/2.0;
            }
            else{
                output(a,b) = (raw(a-1,b-1) + raw(a-1,b+1) + raw(a+1,b-1) + raw(a+1,b+1))/4.0; 
            }
        }
    }
    return output;
}

Image basicDemosaic(const Image &raw, int offsetGreen, int offsetRedX, int offsetRedY, int offsetBlueX, int offsetBlueY)
{
	Image green = basicGreen(raw, offsetGreen);
	Image red = basicRorB(raw, offsetRedX, offsetRedY);
	Image blue = basicRorB(raw, offsetBlueX, offsetBlueY);
	Image output(raw.width(), raw.height(), raw.channels());


	for (int h = 0; h < raw.height(); h++) {
		for (int w = 0; w < raw.width(); w++) {
			output(w, h, 0) = red(w, h);
			output(w, h, 1) = green(w, h);
			output(w, h, 2) = blue(w, h);
		}
	}

	return output;
}

Image edgeBasedGreen(const Image &raw, int offset)
{
	Image output(raw.width(), raw.height(), 1);

    bool condition;
    if (offset % 2 == 0){
        condition = true;
    }
    else {
        condition = false;
    }

    for (int a = 1; a < raw.width() - 1; a++){
        for (int b = 1; b < raw.height() - 1; b++){
            if ((a % 2 == b % 2) == condition){
                output(a,b) = raw(a,b);
            }
            else if (abs(raw(a-1,b) - raw(a+1,b)) > abs(raw(a,b+1) - raw(a,b-1))) {
                output(a,b) = (raw(a, b+1) + raw(a, b-1))/2.0; 
            } else {
            	output(a,b) = (raw(a-1, b) + raw(a+1, b))/2.0; 
            }
        }
    }

    return output;
}

Image edgeBasedGreenDemosaic(const Image &raw, int offsetGreen, int offsetRedX, int offsetRedY, int offsetBlueX, int offsetBlueY)
{
	Image green = edgeBasedGreen(raw, offsetGreen);
	Image red = basicRorB(raw, offsetRedX, offsetRedY);
	Image blue = basicRorB(raw, offsetBlueX, offsetBlueY);
	Image output(raw.width(), raw.height(), raw.channels());


	for (int h = 0; h < raw.height(); h++) {
		for (int w = 0; w < raw.width(); w++) {
			output(w, h, 0) = red(w, h);
			output(w, h, 1) = green(w, h);
			output(w, h, 2) = blue(w, h);
		}
	}

	return output;

}

