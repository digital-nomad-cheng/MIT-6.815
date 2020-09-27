#include "a9.hpp"
#include "timing.hpp"

#include "reference_implementations.hpp"


int main(int argc, char** argv)
{
    // -------------------------------------------------
    //          README                                  
    // -------------------------------------------------
    // If you want more accurate timings, change the macro constant
    // N_TIMES in timing.hpp to a value greater than 1 (e.g. 10).
    // This will average the performance over several runs of the profile.
    // -------------------------------------------------

    Image<float> input = load_image("../Input/hk.png");
    int w = input.width();
    int h = input.height();
    Image<float> output;

    /*
    // ------- C++ baseline -------------------------------------
    unsigned long s = millisecond_timer();
    for (int i=0; i<N_TIMES; i++) {
        output = HarrisCornersRef(input);
    }
    float total_time = float(millisecond_timer()-s);
    float mpixels = float(w*h)/1e6;
    cout<< endl
        << "Reference implementation" << endl
        << "------------------------" << endl
        << "  - runtime " << total_time/N_TIMES << " ms " << endl
        << "  - throughput " << (mpixels*N_TIMES)/(total_time/1000) << " megapixels/sec" << endl;
    save_image(output, "Output/harris_reference.png");
    // ----------------------------------------------------------
    */
    
    //------- Halide version -----------------------------------
    
    Func harris_halide = HarrisCorners(input, 0);
    
    // Profile runtime
    cout<< endl
        << "My Halide implementation" << endl
        << "-------------------------" << endl;
    profile(harris_halide, w, h);
    
    // Save output
    output = harris_halide.realize(w, h);
    Image<float> norm_out = normalize_image(output);
    save_image(norm_out, "../Output/harris_halide.png");
    
    // ----------------------------------------------------------
    /*
    Func harris_halide2 = HarrisCorners(input);
    
    // Profile runtime
    cout<< endl
        << "Default Halide implementation" << endl
        << "------------------------" << endl;
    profile(harris_halide2, w, h);
    // Save output
    output = harris_halide2.realize(w, h);
    norm_out = normalize_image(output);
    save_image(norm_out, "Output/harris_halide_2.png");
    */
    // ----------------------------------------------------------

    // ------- 6.865 autoscheduled version ----------------------
    
    //autoschedule_harris(input);
    
    // ----------------------------------------------------------
    
    return EXIT_SUCCESS;
}