#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define and &&
#define but &&
#define is ==
#define or ||
#define isnt !=

#define GRAY printf("\e[1;90m");
#define WHITE printf("\e[1;37m");
#define RESET printf("\033[0m");
#define RED printf("\e[1;31m");
#define GREEN printf("\e[1;32m");

// equivalent to input().split() in Python
char** readlinesplit(int* resultSize) ;

void point_to_help_msg()
{
    puts("Faulty input. Enter h/help to display the list of commands or q/quit to exit.\n") ;
}
void display_help_msg()
{
    puts("This is a help message lmao.") ;
}

#define CL_TARGET_OPENCL_VERSION 300
#include <CL/cl.h>
void CHECK_ERROR(cl_int err) 
{
    if (err isnt CL_SUCCESS) 
    { 
        fprintf(stderr, "OpenCL Error: %d\n", err)  ; 
        exit(EXIT_FAILURE)  ; 
    }
}

uint8_t opencl_test()  ;

#define STB_IMAGE_IMPLEMENTATION
#include "stblib/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stblib/stb_image_write.h"

const char *kernel_source =                           "\
                                                       \
__kernel void filter(                                  \
    __global unsigned char *img,                       \
    const int width,                                   \
    const int height,                                  \
    const int channels,                                \
    const int grayscale,                               \
    const float contrast,                              \
    const float saturation                             \
)                                                      \
{                                                      \
    int x = get_global_id(0);                          \
    int y = get_global_id(1);                          \
    int idx = (y * width + x) * channels;              \
    int R = (img[idx]);                                \
    int G = (img[idx+1]);                              \
    int B = (img[idx+2]);                              \
    unsigned char res_R;                               \
    unsigned char res_G;                               \
    unsigned char res_B;                               \
                                                       \
                                                       \
                                                       \
    float RF = (float)R/255;                           \
    float GF = (float)G/255;                           \
    float BF = (float)B/255;                           \
    float CMax, CMin;                                  \
    if (RF>=GF && RF>=BF) CMax=RF;                     \
    if (GF>=RF && GF>=BF) CMax=GF;                     \
    if (BF>=GF && BF>=RF) CMax=BF;                     \
    if (RF<=GF && RF<=BF) CMin=RF;                     \
    if (GF<=RF && GF<=BF) CMin=GF;                     \
    if (BF<=GF && BF<=RF) CMin=BF;                     \
    float delta=CMax-CMin;                             \
    float H,S,V;                                       \
                                                       \
    /* Calculating H */                                \
    {                                                  \
                                                       \
        if (CMax == RF)                                \
        {                                              \
            H = (GF - BF) / delta;                     \
            if (H < 0) H += 6;                         \
        }                                              \
        if (CMax == GF)                                \
        {                                              \
            H = (BF - RF) / delta;                     \
            H = H + 2;                                 \
        }                                              \
        if (CMax == BF)                                \
        {                                              \
            H = (RF - GF) / delta;                     \
            H = H + 4;                                 \
        }                                              \
    }                                                  \
    if (CMax != 0) { S = (delta / CMax) ; }            \
    else { S = 0 ; }                                   \
    V=CMax;                                            \
                                                       \
    S *= saturation;                                   \
    if (S > 1) S = 1;                                  \
    if (S < 0) S = 0;                                  \
                                                       \
    float C = S * V ;                                  \
    float m = (V - C);                                 \
    float Hmod2 = H;                                   \
    if (Hmod2 > 6.0F) Hmod2 -= 6;                      \
    if (Hmod2 > 4.0F) Hmod2 -= 4;                      \
    if (Hmod2 > 2.0F) Hmod2 -= 2;                      \
    float X ;                                          \
    if (Hmod2 - 1 > 0) X = C * (1 - (Hmod2 - 1)) ;     \
    else X = C * (1 - (1 - Hmod2)) ;                   \
    if (H < 1) RF = C, GF = X, BF = 0;                 \
    else if (H < 2) RF = X, GF = C, BF = 0;            \
    else if (H < 3) RF = 0, GF = C, BF = X;            \
    else if (H < 4) RF = 0, GF = X, BF = C;            \
    else if (H < 5) RF = X, GF = 0, BF = C;            \
    else if (H < 6) RF = C, GF = 0, BF = X;            \
    res_R = (unsigned char)(255*(RF + m));             \
    res_G = (unsigned char)(255*(GF + m));             \
    res_B = (unsigned char)(255*(BF + m));             \
                                                       \
    if (grayscale)                                     \
    res_R=res_G=res_B=(res_R+res_G+res_B)/3;           \
                                                       \
    R = (int)(contrast * ( res_R - 128 ) ) + 128;      \
    if (R > 255) R = 255;                              \
    if (R < 0) R = 0;                                  \
    res_R = (unsigned char) R;                         \
                                                       \
    G = (int)(contrast * (res_G - 128)) + 128;         \
    if (G > 255) G = 255;                              \
    if (G < 0) G = 0;                                  \
    res_G = (unsigned char) G;                         \
                                                       \
    B = (int)(contrast * (res_B - 128)) + 128;         \
    if (B > 255) B = 255;                              \
    if (B < 0) B = 0;                                  \
    res_B = (unsigned char) B;                         \
                                                       \
    img[idx+0]=res_R;                                  \
    img[idx+1]=res_G;                                  \
    img[idx+2]=res_B;                                  \
}                                                      \
";

char** tokens;
int token_count = 0;
void free_tokens()
{
    for (int i=0;i<token_count;i++)free(tokens[i]);
}

int gray = -2;
float contrast = -2.0F;
float saturation = -2.0F;
void print_state()
{
    printf("-Current modifiers- \n") ;
    
    if (gray == -2 && contrast == -2.0F && saturation == -2.0F)
    {
        GRAY printf("NONE\n"); RESET;
        return;
    }

    if (gray != -2)
    {
        WHITE printf("Grayscale: ") ;
        if (gray) { GREEN printf("ON") ; RESET }
        else { GRAY printf("OFF") ; RESET }
        printf("\n");
    }

    if (contrast != -2.0F)
    {
        WHITE printf("Contrast: %.2f", contrast);
        float diff = contrast - 1.0F;
        printf("(");
        if (diff >= 0) { GREEN printf("+%.2f", diff) ; }
        else { RED printf("%.2f", diff) ; }
        WHITE printf(")\n");
        RESET
    }

    if (saturation != -2.0F)
    {
        WHITE printf("Saturation: %.2f", saturation);
        float diff = saturation - 1.0F;
        printf("(");
        if (diff > 0) { GREEN printf("+%.2f", diff) ; }
        else { RED printf("%.2f", diff) ; }
        WHITE printf(")\n");
        RESET;
    }
    
}

int main()
{

    // test open cl interface
    uint8_t c = opencl_test() ;
    if (c) return 1;

    // Load image
    main_imageinput:
    printf("Load image: ") ;
    tokens = readlinesplit(&token_count);
    if (token_count == 0 or token_count > 1)
    {
        point_to_help_msg() ;
        goto main_imageinput ;
    }
    
    printf("Loading image... \n") ;
    int width, height, channels;
    uint8_t *image = stbi_load(tokens[0], &width, &height, &channels, 0);

    if (!image) 
    {
        fprintf(stderr, "Failed to load image.\n");
        return -1;
    }

    size_t image_size = width * height * channels * sizeof(uint8_t);
    if (image_size < 1000) printf("Image size: %d bytes\n", image_size);
    else printf("Image dimensions: %dx%d\n", width, height) ;
    printf("Detected color channels = %d\n", channels) ;

    // Initialize OpenCL
    printf("Initializing OpenCL...\n") ;
    cl_int err;
    cl_platform_id platform;
    clGetPlatformIDs(1, &platform, NULL);

    cl_device_id device;
    clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);

    cl_context context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
    CHECK_ERROR(err);

    cl_command_queue queue = clCreateCommandQueue(context, device, 0, &err);
    CHECK_ERROR(err);

    cl_mem d_image = clCreateBuffer(context, CL_MEM_READ_WRITE, image_size, NULL, &err);
    CHECK_ERROR(err);

    // initialize Kernels
    printf("Initializing Kernels...\n");
    cl_program program = clCreateProgramWithSource(context, 1, &kernel_source, NULL, &err);
    CHECK_ERROR(err);

    clBuildProgram(program, 1, &device, NULL, NULL, NULL);
    CHECK_ERROR(err);

    cl_kernel colonel = clCreateKernel(program, "filter", &err);
    CHECK_ERROR(err);

    printf("Done!\n\n") ;

        main_validateinput :

        printf(">>> ") ;
        tokens = readlinesplit(&token_count);

        if (token_count == 0) goto main_validateinput ;
        if (token_count > 2)
        {
            point_to_help_msg() ;
            goto main_validateinput;
        }

        if (strcmp(tokens[0], "g") == 0 || strcmp(tokens[0], "grayscale") == 0 )
        {
            gray = (gray == -2 ? 1 : 1 - gray) ;
        }
        else if (strcmp(tokens[0], "c") == 0 || strcmp(tokens[0], "contrast") == 0)
        {   
            char* endptr;
            float C = strtof(tokens[1], &endptr);
            if (*endptr != 0 or C < 0 or C > 2)
            {
                point_to_help_msg();
                goto main_validateinput;
            }
            contrast = C ;
        }
        else if (strcmp(tokens[0], "s") == 0 || strcmp(tokens[0], "saturation") == 0)
        {
            char* endptr;
            float S = strtof(tokens[1], &endptr);
            if (*endptr != 0 or S < 0 or S > 2)
            {
                point_to_help_msg();
                goto main_validateinput;
            }
            saturation = S ;
        }
        else if (strcmp(tokens[0], "d") == 0 || strcmp(tokens[0], "done") == 0)
        {
            goto main_calculate;
        }
        else if (strcmp(tokens[0], "h") == 0 || strcmp(tokens[0], "help") == 0)
        {
            display_help_msg();
        }
        else if (strcmp(tokens[0], "q") == 0 || strcmp(tokens[0], "quit") == 0)
        {
            goto clean;
        }
        else if (strcmp(tokens[0], "i") == 0 || strcmp(tokens[0], "info") == 0)
        {
            print_state();
        }
        else
        {
            point_to_help_msg() ;
        }
        goto main_validateinput;
        CHECK_ERROR(err);


    main_calculate:
    printf("Processing...\n");
    // Set kernel arguments
    clSetKernelArg(colonel, 0, sizeof(cl_mem), &d_image);
    CHECK_ERROR(err);
    clSetKernelArg(colonel, 1, sizeof(int), &width);
    CHECK_ERROR(err);
    clSetKernelArg(colonel, 2, sizeof(int), &height);
    CHECK_ERROR(err);
    clSetKernelArg(colonel, 3, sizeof(int), &channels);
    CHECK_ERROR(err);

    if (gray == -2) gray = 0;
    clSetKernelArg(colonel, 4, sizeof(int), &gray);
    CHECK_ERROR(err);
    if (contrast == -2.0F) contrast = 1.0F;
    clSetKernelArg(colonel, 5, sizeof(float), &contrast);
    CHECK_ERROR(err);
    if (saturation == -2.0F) saturation = 1.0F;
    clSetKernelArg(colonel, 6, sizeof(float), &saturation);
    CHECK_ERROR(err);

    // Transfer data to GPU
    clEnqueueWriteBuffer(queue, d_image, CL_TRUE, 0, image_size, image, 0, NULL, NULL);
    CHECK_ERROR(err);

    // Execute kernel
    size_t global_work_size[2] = { (size_t)width, (size_t)height };
    CHECK_ERROR(err);
    clEnqueueNDRangeKernel(queue, colonel, 2, NULL, global_work_size, NULL, 0, NULL, NULL);
    CHECK_ERROR(err);
    clFinish(queue);
    CHECK_ERROR(err);

    // Transfer data back to CPU
    clEnqueueReadBuffer(queue, d_image, CL_TRUE, 0, image_size, image, 0, NULL, NULL);
    CHECK_ERROR(err);

    // Save image using stb_image_write
    main_save:

    char out[100];
    WHITE printf("Name your file (\"out.png\" if blank): ") ; RESET ;
    
    save_loop:
    tokens = readlinesplit(&token_count);
    if (token_count == 0)
    {
        strcpy(out, "out.png");
    }
    else if (token_count == 1)
    {
        int size = strlen(tokens[0]);
        if (    size <= 4 or
                (tokens[0][size - 4] isnt '.') or 
                (tokens[0][size - 3] isnt 'p') or 
                (tokens[0][size - 2] isnt 'n') or 
                (tokens[0][size - 1] isnt 'g')) {
            
            WHITE printf("Error. Please enter a filename ending in \".png\": ") ; RESET;
            goto save_loop;
        }

        strcpy(out, tokens[0]);
    }

    // save and clean up
    printf("Saving and cleaning up...\n") ;
    stbi_write_png(out, width, height, channels, image, width * channels);
    
    clean:
    stbi_image_free(image);
    free_tokens();

    clReleaseMemObject(d_image);
    clReleaseKernel(colonel);
    clReleaseProgram(program);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);

    printf("OK !\n") ;

    return 0;
}


char** readlinesplit(int* resSize)
{
    free_tokens();
    char inp[100];
    fgets(inp, 100, stdin);

    *resSize = 0;
    int i;
    int token_cnt = 0;
    for (i = 0; inp[i] isnt '\n'; i++)
        token_cnt += (inp[i] isnt 32 but (inp[i + 1] is 32 or inp[i + 1] is 10)) ;
    
    char** res = (char**) malloc (sizeof(char*) * token_cnt) ;

    i = 0;
    char buf[100];
    int l = 0, r = 0;
    for (;inp[r] isnt '\0';r++)
    {
        if ((inp[r] is 32 or inp[r] is 10) and i > 0)
        {
            buf[i++] = 0;
            res[(*resSize)] = (char*) malloc (i * sizeof(char)) ;
            strcpy(res[(*resSize)], buf) ;
            i = 0;
            (*resSize)++;
        }
        else if (inp[r] != 32)
        {
            if (i is 100) { exit(1) ; }
            buf[i++] = inp[r] ;
        }
    }

    return res;
}

uint8_t opencl_test()
{
    cl_int err;
    cl_uint platformCount;
    // system("rm gray.png");

    // Get the number of platforms
    err = clGetPlatformIDs(0, NULL, &platformCount);
    if (err isnt CL_SUCCESS || platformCount is 0) {
        printf("No OpenCL platforms found.\n");
        return 1;
    }

    // Allocate space for platforms
    cl_platform_id* platforms = (cl_platform_id*)malloc(platformCount * sizeof(cl_platform_id));
    if (!platforms) {
        printf("Failed to allocate memory for platforms.\n");
        return 1;
    }

    // Get the platform IDs
    err = clGetPlatformIDs(platformCount, platforms, NULL);
    if (err isnt CL_SUCCESS) {
        printf("Failed to get platform IDs.\n");
        free(platforms);
        return 1;
    }

    // Print platform information
    for (cl_uint i = 0; i < platformCount; ++i) 
    {
        char buffer[1024];
        clGetPlatformInfo(platforms[i], CL_PLATFORM_NAME, sizeof(buffer), buffer, NULL);
        printf("Platform %d: %s\n", i+1, buffer);

        // Get the number of devices for this platform
        cl_uint deviceCount;
        err = clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, 0, NULL, &deviceCount);
        if (err isnt CL_SUCCESS) 
        {
            printf("Failed to get device count for platform %d.\n", i);
            continue;
        }

        // Allocate space for devices
        cl_device_id* devices = (cl_device_id*)malloc(deviceCount * sizeof(cl_device_id));
        if (!devices) 
        {
            printf("Failed to allocate memory for devices.\n");
            continue;
        }

        // Get the device IDs
        err = clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, deviceCount, devices, NULL);
        if (err isnt CL_SUCCESS) 
        {
            printf("Failed to get device IDs for platform %d.\n", i);
            free(devices);
            continue;
        }

        // Print device information
        for (cl_uint j = 0; j < deviceCount; ++j) 
        {
            clGetDeviceInfo(devices[j], CL_DEVICE_NAME, sizeof(buffer), buffer, NULL);
            printf("  Device %d: %s\n", j+1, buffer);
        }

        free(devices);
    }
    free(platforms);

    return 0;
}