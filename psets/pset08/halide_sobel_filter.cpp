#include <Halide.h>

using Halide::Image;
#include "image_io.h"

using namespace Halide;
using namespace std;

int start,diff,msec;
#define timing(func,name) start = clock(); func; diff = clock() - start; msec = diff * 1000 / CLOCKS_PER_SEC; cout << name << " Timing: " << msec << " ms" << endl;

Func convolution(Func f, Func hx, Expr kernel_width,Expr kernel_height);
void sobel();

int main(int argc, char **argv) {
	timing(sobel(),"sobel");
    return 0;
}

void sobel(){
	
    Image<uint8_t> input = load<uint8_t>("images/rgb.png");
    
	Var x("x"),y("y"),c("c");
	
	int W = input.width();
	int H = input.height();
	Func in("in");
	//This performs the sobel operator on only the red layer
 	in(x,y,c) = input(clamp(x,0,W-1),clamp(y,0,H-1),0);

// Calclate Gy
	Func vx1("vx1");
	vx1(x,y) = 0;
	vx1(0,0) = 1; vx1(1,0) = 2; vx1(2,0) = 1;
	
	Func vx2;
	vx2(x,y) = 0;
	vx2(0,0) = 1;
	vx2(0,2) = -1;
	
//	Func Gy1("Gy1");

	Func Gy("Gy");
	Gy = convolution(convolution(in,vx1,3,1),vx2,1,3);

// Performs the convolution by breaking kernel into two seperable matrices

//Calculate Gx
	Func hx1;
	hx1(x,y) = 0;
	hx1(0,0) = 1; hx1(1,0) = 0; hx1(2,0) = -1;

	Func hx2;
	hx2(x,y) = 0;
	hx2(0,0) = 1;
	hx2(1,0) = 2;
	hx2(2,0) = 1;
	
	Func Gx("Gx");
	Gx = convolution(convolution(in,hx1,3,1),hx2,1,3);
	
	Func mag("mag");
	Expr m = Halide::sqrt(Gx(x,y,c) * Gx(x,y,c) + Gy(x,y,c)*Gy(x,y,c));
	mag(x,y,c) = cast<uint8_t>(Halide:: min(m ,255.0f));

	//f(x,y,c) = cast<uint8_t>(Halide:: min(sqrt(vs*vs + hs*hs), 255.0f));
	Image<uint8_t>  ret = mag.realize(W,H,input.channels());
	
//	save(ret, "sobelFilter.png");
}

Func convolution(Func f, Func hx, Expr kernel_width,Expr kernel_height){
	Var x("x2"),y("y2"),c("c2");
	Func convolved("convolved");
	RDom k (0,kernel_width,0,kernel_height);
	convolved(x,y,c) += ( hx(k.x,k.y)*f(x+k.x-(kernel_width/2),y+k.y-(kernel_height/2),c));
	return convolved;
}