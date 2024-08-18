#include <stdio.h>

#define STB_IMAGE_IMPLEMENTATION
#define STB_ONLY_JPEG
#include "stblib/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stblib/stb_image_write.h"

#ifndef uint8_t
#define uint8_t unsigned char
#endif

int main()
{
    char* in ; printf("img name: ") ;
    scanf("%s", in) ;
    const char* out = "gray.jpeg" ;

    int W, H, channels;
    unsigned char* img = stbi_load(in, &W, &H, &channels, 0);

    if (img == NULL) {
        printf("%s\n", stbi_failure_reason());
        return 1;
    }

    unsigned char* gray_img = (unsigned char*)malloc(W * H);
    if (gray_img == NULL) 
    {
        printf("Couldn't malloc\n");
        stbi_image_free(img);
        return 1;
    }

    for (int i = 0; i < W * H; ++i) 
    {
        uint8_t r = img[i * channels];
        uint8_t g = img[i * channels + 1];
        uint8_t b = img[i * channels + 2];
        gray_img[i] = (unsigned char)(0.299 * r + 0.587 * g + 0.114 * b);
    }

    stbi_write_png(out, W, H, 1, gray_img, W);
    
    stbi_image_free(img);
    free(gray_img);
    printf("Grayscaled image saved as %s\n", out);
    return 0;
}