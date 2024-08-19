#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CL_TARGET_OPENCL_VERSION 300
#include <CL/cl.h>
void CHECK_ERROR(cl_int err) 
{
    if (err != CL_SUCCESS) 
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

const char *kernel_source =

    "__kernel void grayscale(                           \
        __global unsigned char *img,                    \
        const int width,                                \
        const int height,                               \
        const int channels                              \
    )                                                   \
    {                                                   \
        int x = get_global_id(0);                       \
        int y = get_global_id(1);                       \
        int idx = (y * width + x) * channels;           \
        unsigned char R = (img[idx]);                   \
        unsigned char G = (img[idx+1]);                 \
        unsigned char B = (img[idx+2]);                 \
        unsigned char res=(unsigned char)((R+G+B)/3);   \
                                                        \
        if (x < width && y < height)                    \
            img[idx]=img[idx+1]=img[idx+2]=res;         \
    }                                                   \
                                                        \
    __kernel void contrast(                             \
        __global unsigned char *img,                    \
        const int width,                                \
        const int height,                               \
        const int channels                              \
    )                                                   \
    {                                                   \
        int x = get_global_id(0);                       \
        int y = get_global_id(1);                       \
        int idx = (y * width + x) * channels;           \
        unsigned char R = (img[idx]);                   \
        unsigned char G = (img[idx+1]);                 \
        unsigned char B = (img[idx+2]);                 \
        unsigned char res=(unsigned char)((R+G+B)/3);   \
                                                        \
        if (x < width && y < height)                    \
            img[idx]=img[idx+1]=img[idx+2]=res;         \
    }                                                   \
                                                        \
    __kernel void saturation(                           \
        __global unsigned char *img,                    \
        const int width,                                \
        const int height,                               \
        const int channels                              \
    )                                                   \
    {                                                   \
        int x = get_global_id(0);                       \
        int y = get_global_id(1);                       \
        int idx = (y * width + x) * channels;           \
        unsigned char R = (img[idx]);                   \
        unsigned char G = (img[idx+1]);                 \
        unsigned char B = (img[idx+2]);                 \
        unsigned char res=(unsigned char)((R+G+B)/3);   \
                                                        \
        if (x < width && y < height)                    \
            img[idx]=img[idx+1]=img[idx+2]=res;         \
    }" ;


int main()
{
    // test open cl interface
    uint8_t c = opencl_test() ;
    if (c) return 1;

    // Load image
    char* in = (char*) malloc (50) ;
    printf("Load image: ") ;
    scanf("%s", in) ;
    printf("Loading image... \n") ;
    int width, height, channels;
    uint8_t *image = stbi_load(in, &width, &height, &channels, 0);
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

    #define GRAYSCALE_KERNEL    0
    #define CONTRAST_KERNEL     1
    #define SATURATION_KERNEL   2

    cl_kernel kernels[3] ;
    kernels[0]=clCreateKernel(program, "grayscale", &err),
    CHECK_ERROR(err);
    kernels[1]=clCreateKernel(program, "contrast", &err);
    CHECK_ERROR(err);
    kernels[2]=clCreateKernel(program, "saturation", &err);
    CHECK_ERROR(err);

    cl_kernel current_colonel ;
    CHECK_ERROR(err);
    printf("Done!\n\n") ;

    
        
        char* in_token = (char*) malloc(100) ;
        main_validateinput:
        printf(">>> ") ;
        scanf("%s", in_token) ;
        if (strcmp(in_token, "g") == 0 || strcmp(in_token, "grayscale") == 0 )
        {
            current_colonel = kernels[GRAYSCALE_KERNEL] ;
        }
        else if (strcmp(in_token, "c") == 0 || strcmp(in_token, "contrast") == 0)
        {
            current_colonel = kernels[CONTRAST_KERNEL];
        }
        else if (strcmp(in_token, "s") == 0 || strcmp(in_token, "saturation") == 0)
        {
            current_colonel = kernels[SATURATION_KERNEL];
        }
        else if (strcmp(in_token, "done") == 0 || strcmp(in_token, "stop") == 0)
        {
            goto main_save;
        }
        else
        {
            printf("Faulty input!\n");
            goto main_validateinput;
        }
        CHECK_ERROR(err);


        // Set kernel arguments
        clSetKernelArg(current_colonel, 0, sizeof(cl_mem), &d_image);
        CHECK_ERROR(err);
        clSetKernelArg(current_colonel, 1, sizeof(int), &width);
        CHECK_ERROR(err);
        clSetKernelArg(current_colonel, 2, sizeof(int), &height);
        CHECK_ERROR(err);
        clSetKernelArg(current_colonel, 3, sizeof(int), &channels);
        CHECK_ERROR(err);

        // Transfer data to GPU
        clEnqueueWriteBuffer(queue, d_image, CL_TRUE, 0, image_size, image, 0, NULL, NULL);
        CHECK_ERROR(err);

        // Execute kernel
        size_t global_work_size[2] = { (size_t)width, (size_t)height };
        CHECK_ERROR(err);
        clEnqueueNDRangeKernel(queue, current_colonel, 2, NULL, global_work_size, NULL, 0, NULL, NULL);
        CHECK_ERROR(err);
        clFinish(queue);
        CHECK_ERROR(err);

        // Transfer data back to CPU
        clEnqueueReadBuffer(queue, d_image, CL_TRUE, 0, image_size, image, 0, NULL, NULL);
        CHECK_ERROR(err);

        goto main_validateinput ;

    // Save image using stb_image_write
    main_save:

    #define WHITE printf("\e[1;37m")
    #define RESET printf("\033[0m")
    char out[100];
    WHITE; printf("Name your file (\"out.png\" if blank): ") ; RESET ;
    scanf("%s", out);


    // save and clean up
    printf("Saving and cleaning up...\n") ;
    stbi_write_png(out, width, height, channels, image, width * channels);
    stbi_image_free(image);

    free(in) ;
    
    clReleaseMemObject(d_image);
    clReleaseKernel(kernels[GRAYSCALE_KERNEL]);
    clReleaseKernel(kernels[CONTRAST_KERNEL]);
    clReleaseKernel(kernels[SATURATION_KERNEL]);
    clReleaseProgram(program);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);

    printf("OK !\n") ;

    return 0;

    
}


uint8_t opencl_test()
{
    cl_int err;
    cl_uint platformCount;
    // system("rm gray.png");

    // Get the number of platforms
    err = clGetPlatformIDs(0, NULL, &platformCount);
    if (err != CL_SUCCESS || platformCount == 0) {
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
    if (err != CL_SUCCESS) {
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
        if (err != CL_SUCCESS) 
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
        if (err != CL_SUCCESS) 
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