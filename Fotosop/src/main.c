

#include <windows.h>
#include <stdio.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stblib/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stblib/stb_image_write.h"
#define CL_TARGET_OPENCL_VERSION 300
#include <CL/cl.h>
#include <commdlg.h>

#define also 
#define and &&
#define but &&
#define is ==
#define or ||
#define isnt !=


unsigned char *img_data;
unsigned char *latest_img_data;
int image_width, image_height, image_channels; 
int preview_width ;
uint64_t image_size;
char filename[100];


/**
 *    OpenCL GPU Kernel source code
 */

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
    if (Hmod2 >= 6.0F) Hmod2 -= 6;                     \
    if (Hmod2 >= 4.0F) Hmod2 -= 4;                     \
    if (Hmod2 >= 2.0F) Hmod2 -= 2;                     \
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


int gray ;
float contrast ;
float saturation ;
cl_int err;
cl_platform_id platform;
cl_device_id device;
cl_context context;
cl_command_queue queue;
cl_mem d_image;
cl_program program;
cl_kernel colonel;

void InitializeOpenCL();
void CleanOpenCL();
void CHECK_ERROR(cl_int err);

uint8_t is_suffix(char* s, char* suf)
{
    size_t slen = strlen(s), suflen = strlen(suf);
    if (slen < suflen) return 0;
    for (int i = 0; i < suflen ; i++)
        if (suf[suflen - i - 1] != s[slen - i - 1]) return 0;
    
    return 1;
}

void InitializeOpenCL()
{
    printf("Initializing OpenCL...\n");
    
    clGetPlatformIDs(1, &platform, NULL);
    if (err) 
    {
        MessageBox(NULL, "No OpenCL platform found. Please install an OpenCL runtime for your graphics drivers.", "Error", MB_OK | MB_ICONERROR);
        exit(0);
    }

    clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
    if (err) 
    {
        MessageBox(NULL, "No OpenCL GPU device found. Sorry :(", "Error", MB_OK | MB_ICONERROR);
        exit(0);
    }
    context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
    CHECK_ERROR(err);
    queue = clCreateCommandQueue(context, device, 0, &err);
    CHECK_ERROR(err);
    d_image = clCreateBuffer(context, CL_MEM_READ_WRITE, image_size, NULL, &err);
    CHECK_ERROR(err);
    program = clCreateProgramWithSource(context, 1, &kernel_source, NULL, &err);
    CHECK_ERROR(err);
    clBuildProgram(program, 1, &device, NULL, NULL, NULL);
    CHECK_ERROR(err);
    colonel = clCreateKernel(program, "filter", &err);
    CHECK_ERROR(err);
    printf("Done!\n");
}

void CleanOpenCL()
{
    printf("Cleaning OpenCL...\n");
    clReleaseMemObject(d_image);
    clReleaseKernel(colonel);
    clReleaseProgram(program);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);
    printf("Done!\n");

}

void CHECK_ERROR(cl_int err) 
{
    if (err isnt CL_SUCCESS) 
    { 
        char err_msg[50] = "OpenCL Error. Code: ";
        char err_str[20];
        itoa(err, err_str, 10);
        strcat(err_msg, err_str);

        MessageBox(NULL, err_msg, "Error", MB_OK | MB_ICONERROR);

        CleanOpenCL();
        if (img_data) free(img_data);
        if (latest_img_data) free(latest_img_data);
        exit(EXIT_FAILURE)  ; 
    }
}


/**
 * Sequentially copies the latest image data to pixels
 * i.e. updates pixels to match latest_img.
 * 
 * This process isn't parallelised because:
 *      a.) It's fast enough without parallel processing
 *      b.) When I tried to do that, My laptop black screened for 5 seconds, 
 *          which restarted the display
 *      c.) I ain't no Windows/OpenCL dev, so hell nah I ain't debuggin that shi
 *      d.) Nevermind, I solved it. If I haven't changed this it's because of point a.)
 *          (and also the processing is still done on GPU, this is just memory manipulation)
 *          
 * @param pixels: the pointer to DI Bitmap bits.
 */
void CpyImgToBMAP(BYTE* bmpixels)
{
    int y, x;
    for (y=0;y<image_height;++y)
    {
        for (x=0;x<preview_width;++x)
        {
            int targetidx = (y * preview_width + x) * 3;
            int originalidx = (y * image_width + x) * image_channels;

            if (x >= image_width)
            {
                bmpixels[targetidx + 0]=255;
                bmpixels[targetidx + 1]=255;
                bmpixels[targetidx + 2]=255;
            }
            else
            {
                bmpixels[targetidx + 2] = latest_img_data[originalidx + 0];
                bmpixels[targetidx + 1] = latest_img_data[originalidx + 1];
                bmpixels[targetidx + 0] = latest_img_data[originalidx + 2];
            }
        }
    }
}

/**
 * Re-copies img_data to latest_img_data, then on that data,
 * performs parallel execution of:
 *      1. Process each pixel based on gray, contrast, saturation params.
 *      2. Modifies each pixel data of latest_img_data in place.
 * 
 * @param pixels: the pointer to DI Bitmap bits.
 */
void FOTOSOP(BYTE* pixels)
{
    memcpy(latest_img_data, img_data, image_size);

    clSetKernelArg(colonel, 0, sizeof(cl_mem), &d_image);
    clSetKernelArg(colonel, 1, sizeof(int), &image_width);
    clSetKernelArg(colonel, 2, sizeof(int), &image_height);
    clSetKernelArg(colonel, 3, sizeof(int), &image_channels);
    clSetKernelArg(colonel, 4, sizeof(int), &gray);
    clSetKernelArg(colonel, 5, sizeof(float), &contrast);
    clSetKernelArg(colonel, 6, sizeof(float), &saturation);

    // Transfer data to GPU
    clEnqueueWriteBuffer(queue, d_image, CL_TRUE, 0, image_size, latest_img_data, 0, NULL, NULL);

    // Execute kernel
    size_t global_work_size[2] = { (size_t)image_width, (size_t)image_height };
    clEnqueueNDRangeKernel(queue, colonel, 2, NULL, global_work_size, NULL, 0, NULL, NULL);
    clFinish(queue);
    CHECK_ERROR(err);

    // Transfer data back to CPU
    clEnqueueReadBuffer(queue, d_image, CL_TRUE, 0, image_size, latest_img_data, 0, NULL, NULL);
    CHECK_ERROR(err);
}


void LoadImageFromFile() 
{
    printf("Loading image...\n");

    img_data = stbi_load(filename, &image_width, &image_height, &image_channels, 0);
    if (!img_data) 
    {
        MessageBox(NULL, "Uhh, I think there is something wrong with the image?", "Error", MB_OK | MB_ICONERROR);
        exit(0);
    }
    image_size = (uint64_t)image_height * (uint64_t)image_width * (uint64_t)image_channels;
    latest_img_data = (uint8_t*) malloc (image_size * sizeof(uint8_t)) ;
    memcpy(latest_img_data, img_data, image_size);

    printf("Done!\n");
}

#define PENCET_SAVE 101

LRESULT CALLBACK PreviewWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

    static HBITMAP hBitmap = NULL;
    static BYTE *pixels = NULL;
    static int window_width, window_height;
    
    switch (msg) {

        case WM_CREATE: 
        {
            window_height = image_height ;
            window_width = image_width ;
            
            // Initialize bitmap used for display
            HDC hdc = GetDC(hwnd);
            HDC hdcMem = CreateCompatibleDC(hdc);
            BITMAPINFO bmi = {0};
            bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
            bmi.bmiHeader.biWidth = preview_width;
            bmi.bmiHeader.biHeight = -image_height;
            bmi.bmiHeader.biPlanes = 1;
            bmi.bmiHeader.biBitCount = 24;
            bmi.bmiHeader.biCompression = BI_RGB;
            hBitmap = CreateDIBSection(hdcMem, &bmi, DIB_RGB_COLORS, (void **)&pixels, NULL, 0);
            
            if (!hBitmap) 
            {
                MessageBox(hwnd, "Failed to create DIB section", "Error", MB_OK | MB_ICONERROR);
                free(pixels);
                ReleaseDC(hwnd, hdc);
                DeleteDC(hdcMem);
                return -1;
            }

            ReleaseDC(hwnd, hdc);
            DeleteDC(hdcMem);
        }
        break;

        case WM_SIZE :
            window_width = LOWORD(lParam);
            window_height = HIWORD(lParam);
            InvalidateRect(hwnd, NULL, TRUE);
            break;

        case WM_DESTROY:
            DeleteObject(hBitmap);
            PostQuitMessage(0);
            break;

        case WM_PAINT: {

            // paint
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            CpyImgToBMAP(pixels);
            HDC hdcMem = CreateCompatibleDC(hdc);
            HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdcMem, hBitmap);
            BITMAP bitmap;
            GetObject(hBitmap, sizeof(bitmap), &bitmap); 
            StretchBlt(hdc, 0, 0, window_width, window_height, hdcMem, 
            0, 0, bitmap.bmWidth, bitmap.bmHeight, SRCCOPY);

            // clean
            SelectObject(hdcMem, hOldBitmap);
            DeleteDC(hdcMem);
            EndPaint(hwnd, &ps);
        }
        break;

        case WM_SETTEXT: {
            
            LPCSTR text = (LPCSTR)lParam;
            // puts(text);

            // gets and parses the params from the textboxes
            char params[3][100];
            int i, paramIdx = 0, j = 0;
            for (i = 0; text[i]; i++)
            {
                if (text[i] == 10)
                    params[paramIdx][j] = '\0', paramIdx++, j = 0;
                else
                    params[paramIdx][j++] = text[i];
            }
            gray = atoi(params[0]);
            if (!params[1][0]) contrast = 1.0F;
            else contrast = (float)atoi(params[1]) / 100;
            if (!params[2][0]) saturation = 1.0F; 
            else saturation = (float)atoi(params[2]) / 100;

            // if gray isn't 0 but also gray isn't 1, set gray to 0
            if (gray isnt 0 but also gray isnt 1) gray = 0;
            
            if (contrast < 0.0F) contrast = 0.0F;
            if (contrast > 2.0F) contrast = 2.0F;
            
            if (saturation < 0.0F) saturation = 0.0F;
            if (saturation > 2.0F) saturation = 2.0F;

            // FOTOSOP a.k.a. recalculate latest_img_data, then redraw bitmap
            FOTOSOP(pixels);
            InvalidateRect(hwnd, NULL, TRUE);
        }
        break;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

void CreatePreviewWindow(HINSTANCE hInstance) 
{
    WNDCLASS wc = {0};

    wc.lpfnWndProc = PreviewWndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "PreviewWindowClass";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    RegisterClass(&wc);

    int padding = 4 - (image_width % 4);
    preview_width = padding + image_width;

    HWND hwndPreview = CreateWindow(
        wc.lpszClassName, "Image Preview",
        WS_SIZEBOX | WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT,
        image_width * 6/5, image_height* 6/5 , NULL, NULL, hInstance, NULL
    ); // here for some reason, setting the window to *exactly* the image size still causes tearing, which is why I gave it a 6/5 multiplier
    
    ShowWindow(hwndPreview, SW_SHOW);
    UpdateWindow(hwndPreview);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {


    static HWND hButton;

    switch (msg) {
        case WM_CREATE: {

            int center = 200;

            int titleHeight = 75;
            int titleWidth = 270;
            int titleX = center - (titleWidth/2);
            int titleY = 30;

            HFONT calibri20 = CreateFont(20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET,
                             OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                             DEFAULT_PITCH | FF_SWISS, "Calibri");
            HFONT calibri68 = CreateFont(68, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET,
                             OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                             DEFAULT_PITCH | FF_SWISS, "Calibri");

            HWND hTitle = CreateWindow("STATIC", "EDIT HERE", SS_CENTER | WS_CHILD | WS_VISIBLE,
                        titleX, titleY, titleWidth, titleHeight, hwnd, NULL,
                        (HINSTANCE) GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
            SendMessage(hTitle, WM_SETFONT, (WPARAM) calibri68, 1);


            int offset = 60;
            int width = 80;            // Width of each text box
            int height = 20;          // Height of each text box
            int x = center-(width/2) + offset;  // Center horizontally
            int y = 125;  // Starting y position
            int promptWidth = 135;
            int promptX = center-(promptWidth/2) - offset;

            // First text box
            HWND graytext = CreateWindow("STATIC", "Grayscale (0/1):", WS_CHILD | WS_VISIBLE | SS_RIGHT ,
                        promptX, y, promptWidth, height, hwnd, NULL,
                     (HINSTANCE) GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
            SendMessage(graytext, WM_SETFONT, (WPARAM) calibri20, 1);
            HWND grsc_inp = CreateWindow("EDIT", "", WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT,
                                x, y, width, height, hwnd, (HMENU) 1,
                                (HINSTANCE) GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
            SetWindowText(grsc_inp, "0");

            // Second text box
            y += 30;  // Adjust y position for the second text box
            HWND contrastext = CreateWindow("STATIC", "Contrast (0-200%):", WS_CHILD | WS_VISIBLE | SS_RIGHT,
                        promptX, y, promptWidth, height, hwnd, NULL,
                     (HINSTANCE) GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
            SendMessage(contrastext, WM_SETFONT, (WPARAM) calibri20, 1);
            HWND cntr_inp = CreateWindow("EDIT", "", WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT,
                        x, y, width, height, hwnd, (HMENU) 2,
                        (HINSTANCE) GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
            SetWindowText(cntr_inp, "100");
            
            // Third text box
            y += 30;  // Adjust y position for the third text box
            HWND satext = CreateWindow("STATIC", "Saturation (0-200%):",  WS_CHILD | WS_VISIBLE | SS_RIGHT,
                    promptX, y, promptWidth, height, hwnd, NULL,
                     (HINSTANCE) GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
            SendMessage(satext, WM_SETFONT, (WPARAM) calibri20, 1);
            HWND satr_inp = CreateWindow("EDIT", "", WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT,
                        x, y, width, height, hwnd, (HMENU) 3,
                        (HINSTANCE) GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
            SetWindowText(satr_inp, "100");

            int button_height = 30;
            int button_width = 100;
            y += 50;

            hButton = CreateWindowEx(
                0,
                "BUTTON",
                "Save image",
                WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                center - button_width/2,     // x position
                y,                          // y position
                button_width,
                button_height,
                hwnd,
                (HMENU) PENCET_SAVE,
                (HINSTANCE) GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
                NULL
            );
            
        } break;

        case WM_CTLCOLORSTATIC: /* To remove ugly gray box in prompt text */
        {
            HDC hdcStatic = (HDC)wParam;
            SetBkMode(hdcStatic, TRANSPARENT);
            return (INT_PTR)GetStockObject(NULL_BRUSH);
        }

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        case WM_COMMAND: {
            switch (HIWORD(wParam)) {
                case BN_CLICKED : {
                    
                    if (LOWORD(wParam) == PENCET_SAVE)
                    {

                        /* Save file routine */
                        OPENFILENAME ofn;
                        char szFile[260];
                        ZeroMemory(&ofn, sizeof(ofn));
                        ofn.lStructSize = sizeof(ofn);
                        ofn.hwndOwner = hwnd;
                        ofn.lpstrFile = szFile;
                        ofn.lpstrFile[0] = '\0';
                        ofn.nMaxFile = sizeof(szFile);
                        ofn.lpstrFilter = "All JPEG,PNG,BMP Files\0*.JPEG;*.JPG;*.PNG;*.BMP\0JPEG Files\0*.JPEG;*.JPG\0PNG Files\0*.PNG\0BMP Files\0*.bmp\0";
                        ofn.nFilterIndex = 1;
                        ofn.lpstrFileTitle = NULL;
                        ofn.nMaxFileTitle = 0;
                        ofn.lpstrInitialDir = NULL;
                        ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
                        uint8_t SAVE_FILE_SUCCESS = 0;


                        if (GetSaveFileName(&ofn) == TRUE) 
                        {
                            puts(szFile);
                            if (is_suffix(szFile, ".bmp")) 
                                stbi_write_bmp(   
                                    szFile, 
                                    image_width, 
                                    image_height, 
                                    image_channels, 
                                    latest_img_data
                                ), SAVE_FILE_SUCCESS = 1;
                            
                            if (is_suffix(szFile, ".jpg") || is_suffix(szFile, ".jpeg")) 
                                stbi_write_jpg(   
                                    szFile, 
                                    image_width, 
                                    image_height, 
                                    image_channels, 
                                    latest_img_data, 
                                    image_width * image_channels
                                ), SAVE_FILE_SUCCESS = 1;

                            if (is_suffix(szFile, ".png"))
                                stbi_write_png(
                                    szFile, 
                                    image_width, 
                                    image_height, 
                                    image_channels, 
                                    latest_img_data, 
                                    image_width * image_channels
                                ), SAVE_FILE_SUCCESS = 1;
                            
                            if (SAVE_FILE_SUCCESS)
                            {
                                MessageBox(hwnd, "File saved successfully.", "OK !", MB_OK);
                                PostQuitMessage(0);
                            }
                            else
                            {
                                char file_err_msg[300] =
                                "You should write a file name with a valid extension.\n"\
                                "The accepted file extensions are .bmp, .jpeg or .jpg, and .png.\n\n"\
                                "(Your input was: ";
                                
                                strcat(file_err_msg, szFile);                                
                                strcat(file_err_msg, ")");

                                MessageBox(hwnd, file_err_msg, "NG !", MB_OK);
                            }
                        }
                    }

                    else if (LOWORD(wParam) == IDCANCEL) /* esc*/
                    {
                        PostQuitMessage(0);
                    }

                    else if (LOWORD(wParam) == IDOK and GetFocus() == hButton) 
                    {
                        SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(PENCET_SAVE, BN_CLICKED), (LPARAM)hButton);
                    }

                }
                break;



                case EN_CHANGE: {

                    HWND hwndPreview = FindWindow("PreviewWindowClass", NULL);

                    HWND hEdit1 = GetDlgItem(hwnd, 1);
                    HWND hEdit2 = GetDlgItem(hwnd, 2);
                    HWND hEdit3 = GetDlgItem(hwnd, 3);

                    char buffer1[256], buffer2[256], buffer3[256];

                    GetWindowText(hEdit1, buffer1, sizeof(buffer1));
                    GetWindowText(hEdit2, buffer2, sizeof(buffer2));
                    GetWindowText(hEdit3, buffer3, sizeof(buffer3));

                    char combinedText[768];
                    snprintf(combinedText, sizeof(combinedText), "%s\n%s\n%s",
                            buffer1, buffer2, buffer3);

                    // Send text to big window
                    SendMessage(hwndPreview, WM_SETTEXT, 0, (LPARAM) combinedText);

                    UpdateWindow(hwndPreview);
                }
                break;
            }
        } break;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);

    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASS wc = {0};

    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = "EditPanelClass";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    if (!RegisterClass(&wc)) return -1;

    HWND hwndEdit = CreateWindow(
        wc.lpszClassName, "Edit Panel",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT,
        400, 320, NULL, NULL, hInstance, NULL
    );


    MessageBox(NULL, "Select an image.", "Welcome!", MB_OK);

    /* Open file routine */
    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = filename;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(filename);
    ofn.lpstrFilter = "All JPEG,PNG,BMP Files\0*.JPEG;*.JPG;*.PNG;*.BMP\0JPEG Files\0*.JPEG;*.JPG\0PNG Files\0*.PNG\0BMP Files\0*.bmp\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_READONLY ;

    if (!GetSaveFileName(&ofn)) exit(0);
    LoadImageFromFile();
    InitializeOpenCL();


    CreatePreviewWindow(hInstance);
    UpdateWindow(hwndEdit);

    HWND hwndPreview = FindWindow("PreviewWindowClass", NULL);
    SetForegroundWindow(hwndPreview);

    ShowWindow(hwndEdit, nCmdShow);
    SetForegroundWindow(hwndEdit);
    SetWindowPos(hwndEdit, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);


    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) 
    {
        if (!IsDialogMessage(hwndEdit, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    // nyapu dan ngepel
    CleanOpenCL();
    stbi_image_free(img_data);
    stbi_image_free(latest_img_data);
    return (int) msg.wParam;
}
