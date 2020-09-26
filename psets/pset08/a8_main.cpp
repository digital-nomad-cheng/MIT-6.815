#include "a8.hpp"
#include <timing.h>

#include "reference_implementations.h"


int main(int argc, char** argv)
{
    Image<uint8_t> input = load_image("../Input/rgb.png");
    Image<uint8_t> lumi  = load_image("../Input/grey.png");
    Image<uint8_t> out0  = Image<uint8_t>(lumi.width(), lumi.height());

    /*
    // Part 1
    {
        Image<uint8_t> out1 = SmoothGradNormalized();
        Image<uint8_t> out2 = WavyRGB();
        Image<uint8_t> out3 = Luminance(input);
        Image<uint8_t> out4 = Sobel(lumi);

        if (out1.defined()) { save_image(out1, "../Output/Smooth.png");    } else { save_image(out0, "../Output/Smooth.png");    }
        if (out2.defined()) { save_image(out2, "../Output/WavyRGB.png");   } else { save_image(out0, "../Output/WavyRGB.png");   }
        if (out3.defined()) { save_image(out3, "../Output/Luminance.png"); } else { save_image(out0, "../Output/Luminance.png"); }
        if (out4.defined()) { save_image(out4, "../Output/Sobel.png");     } else { save_image(out0, "../Output/Sobel.png");     }

        // Test image
        Image<uint8_t> tiny(8,8);
        tiny(3,3) = 255;
        tiny(4,3) = 255;
        tiny(4,4) = 255;

        // Example cpp-equivalent schedule
        Image<uint8_t> tiny_schedule_1 = boxSchedule1(tiny);
        save_image(tiny_schedule_1, "../Output/boxblur_schedule1.png");

        // // Your turn !
        Image<uint8_t> tiny_schedule_5 = boxSchedule5(tiny);
        save_image(tiny_schedule_5, "../Output/boxblur_schedule5.png");
        Image<uint8_t> tiny_schedule_6 = boxSchedule6(tiny);
        save_image(tiny_schedule_6, "../Output/boxblur_schedule6.png");
        Image<uint8_t> tiny_schedule_7 = boxSchedule7(tiny);
        save_image(tiny_schedule_7, "../Output/boxblur_schedule7.png");
    }
    */

    // Part 2
    {
        Image<uint8_t> out1, out2, out3;

        int w = lumi.width();
        int h = lumi.height();

        Image<uint8_t> out1_ref = LocalMax_cpp(lumi);
        save_image( out1_ref, "../Output/LocalMax_cpp_reference.png");
        Func r1 = LocalMax   (lumi);
        if (r1.defined()) { out1 = r1.realize(w, h); cerr << "r1 "; profile(r1, w, h); }
        if (out1.defined()) { save_image(out1, "../Output/LocalMax.png");    } else { save_image(out0, "../Output/LocalMax.png");    }

        Image<uint8_t> out2_ref = Gaussian_cpp(lumi, 3.0f);
        save_image( out2_ref, "../Output/Gaussian_cpp_reference.png");
        Func r2 = Gaussian   (lumi, 3.0f);
        if (r2.defined()) { out2 = r2.realize(w, h); cerr << "r2 "; profile(r2, w, h); }
        if (out2.defined()) { save_image(out2, "../Output/Gaussian.png");    } else { save_image(out0, "../Output/Gaussian.png");    }

        Func r3 = UnsharpMask(lumi, 3.0f);
        if (r3.defined()) { out3 = r3.realize(w, h); cerr << "r3 "; profile(r3, w, h); }
        if (out3.defined()) { save_image(out3, "../Output/UnsharpMask.png"); } else { save_image(out0, "../Output/UnsharpMask.png"); }
        
    }



    return EXIT_SUCCESS;
}
