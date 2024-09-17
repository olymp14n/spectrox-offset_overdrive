#include <windows.h>
#include "glx.h"
#include "../config/config.h"
#include "sound.h"
#include "types.h"
#include "render.h"
#include "broadcast.h"
#include "../client.h"

LRESULT CALLBACK fw_windowProc(HWND, UINT, WPARAM, LPARAM);
void fw_registerClass(HINSTANCE);
HWND fw_createWindow(HINSTANCE);
HGLRC fw_setupOpenGLContext(HWND);
void fw_setPixelFormat(HDC);
RECT fw_getDesktopRect();
RECT fw_getOutputRect();
void fw_shutdownOpenGL(HWND, HDC, HGLRC);
void fw_prepareRender(HDC);
void fw_render(HDC hDC);

RECT _fw_outputRect;
fw_timer_data _fw_time;

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow) {

    _fw_outputRect = fw_getOutputRect();
    fw_registerClass(hInstance);
    const HWND hWnd = fw_createWindow(hInstance);
    HGLRC hGLRC = fw_setupOpenGLContext(hWnd);
    const HDC hDC = GetDC(hWnd);

    if (FW_IS_MUSIC_ENABLED) {
        fw_sound_init();
    }

    fw_glx_init();
    fw_render_init(_fw_outputRect);

    client_init();

    if (FW_IS_FULLSCREEN_ENABLED) {
        ShowCursor(FALSE);
    }

    ShowWindow(hWnd, nCmdShow);
    fw_prepareRender(hDC);

    if (FW_IS_MUSIC_ENABLED) {
        fw_sound_play();
    }

    _fw_time.start = timeGetTime();
    _fw_time.previousFrame = _fw_time.start;

    MSG msg;
    while (!fw_broadcast_isExit()) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            if (msg.message == WM_QUIT) {
                break;
            }
        } else {
            fw_render(hDC);
        }
    }

    if (FW_IS_MUSIC_ENABLED) {
        fw_sound_shutdown();
    }

    fw_shutdownOpenGL(hWnd, hDC, hGLRC);
    DestroyWindow(hWnd);

    return 0; // msg.wParam;
}

LRESULT CALLBACK fw_windowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CLOSE:
    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    case WM_KEYDOWN:
        switch (wParam) {
        case VK_ESCAPE:
            PostQuitMessage(0);
            break;
        }
        break;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}

void fw_registerClass(HINSTANCE hInstance) {
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wcex.lpfnWndProc = fw_windowProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = FW_WINDOW_NAME;
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wcex)) {
        exit(1);
    }
}

static void fw_changeDisplaySettings(int width, int height) {
    DEVMODE dm = {0};
    dm.dmSize = sizeof(dm);
    dm.dmPelsWidth = width;
    dm.dmPelsHeight = height;
    dm.dmBitsPerPel = 32;
    dm.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

    if (ChangeDisplaySettings (&dm, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL) {
        exit(44001);
    }
}

HWND fw_createWindow(HINSTANCE hInstance) {

    // copy of the output rect that can be adjusted (if running in window mode) without modifying the original
    RECT actualOutputRect = _fw_outputRect;

    if (FW_IS_FULLSCREEN_ENABLED && FW_IS_FULLSCREEN_CHANGE_RESOLUTION) {
        fw_changeDisplaySettings(actualOutputRect.right, actualOutputRect.bottom);
    }

    if (!FW_IS_FULLSCREEN_ENABLED) {
        // adjust window position (https://stackoverflow.com/a/25926527/891846)
        AdjustWindowRect(&actualOutputRect, WS_OVERLAPPEDWINDOW, FALSE);
    }

    return CreateWindowEx(0,
                          FW_WINDOW_NAME,
                          FW_WINDOW_NAME,
                          FW_IS_FULLSCREEN_ENABLED ? WS_POPUP : WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT,
                          CW_USEDEFAULT,
                          actualOutputRect.right - actualOutputRect.left,
                          actualOutputRect.bottom - actualOutputRect.top,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);
}

HGLRC fw_setupOpenGLContext(HWND hWnd) {
    HDC hDC = GetDC(hWnd);
    fw_setPixelFormat(hDC);

    HGLRC hGLRC = wglCreateContext(hDC);
    wglMakeCurrent(hDC, hGLRC);
    return hGLRC;
}

void fw_setPixelFormat(HDC hDC) {
    PIXELFORMATDESCRIPTOR pfd;
    ZeroMemory(&pfd, sizeof(pfd));
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cAlphaBits = 8;
    pfd.cDepthBits = 16;
    pfd.cStencilBits = 8;
    pfd.iLayerType = PFD_MAIN_PLANE;

    int iFormat = ChoosePixelFormat(hDC, &pfd);
    SetPixelFormat(hDC, iFormat, &pfd);
}

RECT fw_getDesktopRect() {
    const HWND hDesktop = GetDesktopWindow();
    RECT desktopRect;
    GetWindowRect(hDesktop, &desktopRect);
    return desktopRect;
}

RECT fw_getOutputRect() {
    RECT outputRect = {0, 0, FW_WINDOW_WIDTH, FW_WINDOW_HEIGHT};

    // Windowsed.
    if (!FW_IS_FULLSCREEN_ENABLED) {
        return outputRect;
    }

    // Fullscreen with resolution change.
    if (FW_IS_FULLSCREEN_CHANGE_RESOLUTION) {
        outputRect = (RECT){0, 0, FW_FULLSCREEN_WIDTH, FW_FULLSCREEN_HEIGHT};
        return outputRect;
    }

    // Fullscreen with desktop resolution.
    outputRect = fw_getDesktopRect();
    if (FW_IS_FULLSCREEN_BORDERLESS_ENABLED) {
        // https://stackoverflow.com/q/23145217/891846
        outputRect.right = outputRect.right + 1;
    }

    return outputRect;
}

void fw_shutdownOpenGL (HWND hWnd, HDC hDC, HGLRC hGLRC) {
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hGLRC);
    ReleaseDC(hWnd, hDC);
}

/*
 * Omit initial flicker when starting in fullscreen mode by clearing and swapping the buffer a few times.
 */
void fw_prepareRender(HDC hDC) {
    glClearColor(0,0,0,0);
    for (int i=0; i<2; i++) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        SwapBuffers(hDC);
        Sleep(100);
    }

    if (FW_STARTUP_DELAY_MILLIS > 0) {
        Sleep(FW_STARTUP_DELAY_MILLIS);
    }
}

void fw_render(HDC hDC) {
    _fw_time.current = timeGetTime();
    _fw_time.delta = 0.001f * (float)(_fw_time.current - _fw_time.previousFrame);
    _fw_time.previousFrame = _fw_time.current;
    _fw_time.elapsed = 0.001f * (float)(_fw_time.current - _fw_time.start);

    client_execute(&_fw_time);

    SwapBuffers(hDC);
}
