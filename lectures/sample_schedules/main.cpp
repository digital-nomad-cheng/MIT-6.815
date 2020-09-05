#include <iostream>
#include <Halide.h>

using namespace Halide;
using namespace std;

#define N_TIMES 50

#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
unsigned long millisecond_timer(void) {
    static SYSTEMTIME t;
    GetSystemTime(&t);
    return (unsigned long)((unsigned long)t.wMilliseconds
            + 1000*((unsigned long)t.wSecond
            + 60*((unsigned long)t.wMinute
            + 60*((unsigned long)t.wHour
            + 24*(unsigned long)t.wDay))));

#elif defined(_APPLE_) || defined(__APPLE__) || \
    defined(APPLE)   || defined(_APPLE)    || defined(__APPLE) || \
defined(unix)    || defined(__unix__)  || defined(__unix)
#include <unistd.h>
#include <sys/time.h>
unsigned long millisecond_timer(void) {
    struct timeval t;
    gettimeofday(&t, NULL);
    return (unsigned long)(t.tv_usec/1000 + t.tv_sec*1000);
}
#else
unsigned long millisecond_timer(void) {
    std::cout << "Warning: no timer implementation available" << std::endl;
    return 0;
}
#endif

void profile(Func myFunc, int w, int h) {
    myFunc.compile_jit();

    unsigned long s = millisecond_timer();
    for (int i=0; i<N_TIMES; i++) {
        myFunc.realize(w,h);
    }
    float total_time = float(millisecond_timer()-s);

    float mpixels = float(w*h)/1e6;
    cout << "runtime " << total_time/N_TIMES << " ms "
        << " throughput " << (mpixels*N_TIMES)/(total_time/1000) << " megapixels/sec" << endl;
}


// -----------------------------------------------------------------------------

Func LocalMaxSimple(Image<uint8_t> input, int window) {
    Func LM("LocalMax");

    Var x("x"), y("y");
    Var xi("xi"), yi("yi");
    Var xo("xo"), yo("yo");

    RDom r(-window/2, window, -window/2, window);

    Func in;
    in(x,y) = input(clamp(x,0,input.width()-1), clamp(y,0,input.height()-1));

    LM(x,y) = cast<uint8_t>(0);
    LM(x,y) = max(LM(x,y), in(x+r.x, y+r.y));
    LM(x,y) = select(LM(x,y)==in(x,y), cast<uint8_t>(255), cast<uint8_t>(0));

    // this is what the auto schedule did
    in.compute_root();
    LM.compute_root();

   Buffer b(type_of<uint8_t>(),input.height(),input.width());
   LM.compile_to_simplified_lowered_stmt("LocaLMaxSimple.html", b, HTML);

    return LM;
}

Func LocalMaxBetter(Image<uint8_t> input, int window) {
    Func LM("LocalMax");

    Var x("x"), y("y");
    Var xi("xi"), yi("yi");
    Var xo("xo"), yo("yo");

    RDom r(-window/2, window, -window/2, window);

    Func in;
    in(x,y) = input(clamp(x,0,input.width()-1), clamp(y,0,input.height()-1));

    LM(x,y) = cast<uint8_t>(0);
    LM(x,y) = max(LM(x,y), in(x+r.x, y+r.y));
    LM(x,y) = select(LM(x,y)==in(x,y), cast<uint8_t>(255), cast<uint8_t>(0));

   // default                             : 196 ms
   // in.compute_inline                   : 246 ms
   // in.compute root                     : 196 ms (due to clamp instructions)
   // LM tiling                           : 196 ms
   // LM tiling,parallel                  : 42 ms  (5x on 8 core machine)
   // LM tiling,parallel,unroll           : 26 ms  (1.8x)
   // LM tiling,parallel,unroll,vectorize : 19 ms

   int tile_width  = 32;
   int vector_width= 8;

   in.compute_root();
   LM.compute_root();

   in          .split(y,yo,yi,tile_width).parallel(yo).vectorize(x,vector_width);
   LM          .split(y,yo,yi,tile_width).parallel(yo).vectorize(x,vector_width);
   LM.update(0).split(y,yo,yi,tile_width).parallel(yo).unroll(r.x).unroll(r.y);
   LM.update(1).split(y,yo,yi,tile_width).parallel(yo).vectorize(x,vector_width);

   // its is useful to compute in at root because each pixel of LM needs 25
   // pixels of input image. If in was inline, then it would force the
   // clamp operation 25 times of each pixel. If the per pixel dependency
   // was smaller, we would prefer to keep in as compute_inline()

   Buffer b(type_of<uint8_t>(),input.height(),input.width());
   LM.compile_to_simplified_lowered_stmt("LocaLMaxBetter.html", b, HTML);

   return LM;
}

Func LocalMaxBetter2(Image<uint8_t> input, int window) {
    Func LM("LocalMax");

    Var x("x"), y("y");
    Var xi("xi"), yi("yi");
    Var xo("xo"), yo("yo");

    RDom r(-window/2, window, -window/2, window);

    Func in;
    in(x,y) = input(clamp(x,0,input.width()-1), clamp(y,0,input.height()-1));

    LM(x,y) = cast<uint8_t>(0);
    LM(x,y) = max(LM(x,y), in(x+r.x, y+r.y));
    LM(x,y) = select(LM(x,y)==in(x,y), cast<uint8_t>(255), cast<uint8_t>(0));

    // create another Func that copies the result of LM
    // we will compute LM at LM_final so that the max computation
    // and update happens at pixel level; this was LM does not write
    // its result in each of its definitions
    Func LM_final("LM_final");
    LM_final(x,y) = LM(x,y);

    int tile_width  = 32;
    int vector_width= 8;

    in.compute_root();
    in.split(y,yo,yi,tile_width).parallel(yo).vectorize(x,vector_width);

    LM.compute_at(LM_final, x);
    LM_final.compute_root();

    LM_final.split(y, yo, yi, tile_width).parallel(yo).vectorize(x, vector_width);

    LM.update(0).unroll(r.x).unroll(r.y);

    Buffer b(type_of<uint8_t>(),input.height(),input.width());
    LM_final.compile_to_simplified_lowered_stmt("LocalMaxBetter2.html", b, HTML);

    return LM_final;
}


int main(int argc, char **argv) {
    Image<uint8_t> random_image(2560, 1600);
    for (int j=0; j<random_image.height(); j++) {
        for (int i=0; i<random_image.width(); i++) {
            random_image(i,j) = rand() % 255;
        }
    }

    Func l1 = LocalMaxSimple(random_image, 5);
    Func l2 = LocalMaxBetter(random_image, 5);
    Func l3 = LocalMaxBetter2(random_image, 5);

    cout << "Simple  "; profile(l1, random_image.width(), random_image.height());
    cout << "Better  "; profile(l2, random_image.width(), random_image.height());
    cout << "Better2 "; profile(l3, random_image.width(), random_image.height());

    return 0;
}
