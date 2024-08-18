#include <stdio.h>

#define CL_TARGET_OPENCL_VERSION 300
#include <CL/cl.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stblib/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stblib/stb_image_write.h"

int main()
{
    cl_int err;
    cl_uint platformCount;

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
        printf("Platform %d: %s\n", i, buffer);

        // Get the number of devices for this platform
        cl_uint deviceCount;
        err = clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, 0, NULL, &deviceCount);
        if (err != CL_SUCCESS) {
            printf("Failed to get device count for platform %d.\n", i);
            continue;
        }

        // Allocate space for devices
        cl_device_id* devices = (cl_device_id*)malloc(deviceCount * sizeof(cl_device_id));
        if (!devices) {
            printf("Failed to allocate memory for devices.\n");
            continue;
        }

        // Get the device IDs
        err = clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, deviceCount, devices, NULL);
        if (err != CL_SUCCESS) {
            printf("Failed to get device IDs for platform %d.\n", i);
            free(devices);
            continue;
        }

        // Print device information
        for (cl_uint j = 0; j < deviceCount; ++j) {
            clGetDeviceInfo(devices[j], CL_DEVICE_NAME, sizeof(buffer), buffer, NULL);
            printf("  Device %d: %s\n", j, buffer);
        }

        free(devices);
    }

    free(platforms);
    return 0;
}