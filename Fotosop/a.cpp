
#include "CImg.h"
#include <iostream>



/**
 * Grayscale
 */
cimg_library::CImg<__uint8_t> grayscale(cimg_library::CImg<__uint8_t> img)
{
    int W = img.width(), H = img.height();
    cimg_library::CImg<__uint8_t> result(W, H, 1, 1, 0);

    for (int i = 0; i < H; i++)
    {
        for (int j = 0; j < W; j++) 
        {
            __uint8_t r = img(j, i, 0, 0);
            __uint8_t g = img(j, i, 0, 1); 
            __uint8_t b = img(j, i, 0, 2);
            __uint8_t gray = 0.3*r + 0.6*g + 0.1*b;
            result(j, i, 0, 0) = gray;
        }    
    }
    
    return result ;
}


int main(int argc, char** argv) {

    if (argc != 4) 
    {
        std::cerr << "Usage: " << argv[0] << " <input_image> <output_image> <option>" << std::endl;
        return 1;
    }

    try 
    {

        cimg_library::CImg<__uint8_t> img(argv[1])  ;
        cimg_library::CImg<__uint8_t> result        ;
        if (argv[3] == "g") result = grayscale(img) ;
        if (argv[3] == "a") result = grayscale(img) ;
        result.save(argv[2]) ;
    } 
    catch (cimg_library::CImgException& e) 
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
