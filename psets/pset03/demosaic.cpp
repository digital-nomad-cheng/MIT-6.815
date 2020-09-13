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
	// Image green(raw.width(), raw.height(), 1);
	// if (offset == 0) {
	// 	for (int h = 1; h < raw.height() - 1; h++) {
	// 		for (int w = 1; w < raw.width() - 1 ; w++) {
	// 			if (h % 2 == 0 && w % 2 == 1) 
	// 				green(w, h) = (raw(w, h-1) + raw(w, h+1) + raw(w-1, h) + raw(w+1, h)) / 4.0;
	// 			else if (h % 2 == 0 && w % 2 == 0) 
	// 				green(w, h) = raw(w, h);
	// 			else if (h % 2 == 1 && w % 2 == 1) 
	// 				green(w, h) = raw(w, h);
	// 			else if (h % 2 == 1 && w % 2 == 0) 
	// 				green(w, h) = (raw(w, h-1) + raw(w, h+1) + raw(w-1, h) + raw(w+1, h)) / 4.0;
	// 		}
	// 	}
	// } else { // offset == 1
	// 	for (int h = 1; h < raw.height() - 1; h++) {
	// 		for (int w = 1; w < raw.width() - 1; w++) {
	// 			if (h % 2 == 0 && w % 2 == 0) 
	// 				green(w, h) = (raw(w, h-1) + raw(w, h+1) + raw(w-1, h) + raw(w+1, h)) / 4.0;
	// 			else if (h % 2 == 0 && w % 2 == 1) 
	// 				green(w, h) = raw(w, h);
	// 			else if (h % 2 == 1 && w % 2 == 0) 
	// 				green(w, h) = raw(w, h);
	// 			else if (h % 2 == 1 && w % 2 == 1) 
	// 				green(w, h) = (raw(w, h-1) + raw(w, h+1) + raw(w-1, h) + raw(w+1, h)) / 4.0;
	// 		}
	// 	}
	// }
	

	// return green;	

	// std::cout << "raw.width() " << raw.width() << " raw.height() " << raw.height() << " raw.channels() " << raw.channels() << std::endl;
 //    Image output(raw.width(), raw.height(), 1);

 //    bool condition;
 //    if (offset % 2 == 0){
 //        condition = true;
 //    }
 //    else {
 //        condition = false;
 //    }

 //    for (int a = 1; a < raw.width() - 1; a++){
 //        for (int b = 1; b < raw.height() - 1; b++){
 //            if ((a % 2 == b % 2) == condition){
 //                output(a,b) = raw(a,b);
 //            }
 //            else{
 //                output(a,b) = (raw(a-1,b) + raw(a+1,b) + raw(a,b+1) + raw(a,b-1))/4.0; 
 //            }
 //        }
 //    }
 //    return output

// 	assert(offset == 1 || offset == 0);
// 	assert(raw.channels() >= 1);

// 	// Init output image
// 	int width, height, channels;
// 	width = raw.width();
// 	height = raw.height();
// 	Image output(width, height, 1);

// 	// If no offset, then 
// 	// G N G N G N G
// 	// N G N G N G N
// 	// G N G N G N G
// 	// Even rows have even greens
// 	// Odd rows have odd greens
// 	// With offset, even rows have odd greens, odd rows have even

// 	// Init loop variables
// 	int i, j;

// 	// Loop over output, filling in or interpolating green
// 	for (i = 0; i < width; i++) {
// 		for (j = 0; j < height; j++) {
// 			if (i > 1 && i < width - 1 && j > 1 && j < height -1 ) {
// 				if (i % 2 == 0) {
// 					output(i,j,0) = ((j + offset) % 2 == 0) ? raw(i,j,0) : interpGreen(raw,i,j);
// 				} else {
// 					output(i,j,0) = ((j + offset) % 2 != 0) ? raw(i,j,0) : interpGreen(raw,i,j);
// 				}
// 			} else {
// 				output(i,j,0) = raw(i,j,0);
// 			}
// 		}
// 	}
//     return output;
 	Image newImage(raw.width(), raw.height());
    for (int y = 0; y < (raw.height()); y++) {
      for (int x = 0; x < (raw.width()); x++) {
        if (x == 0 || y==0 || x == (raw.width() - 1) || y == (raw.height() - 1)) {
          newImage(x,y) = raw(x,y);
        } else {
          if ((x + y%2) % 2 == offset) {
            newImage(x,y) = raw(x,y);
          } else {
            float sum = raw(x-1,y) + raw(x+1,y) + raw(x,y-1) + raw(x,y+1);
            newImage(x,y) = sum / 4.0;
          }
        }
      }
    }
    return newImage;
}

