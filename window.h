//personal fork of fenster.h (https://github.com/zserge/fenster)

/*
This is very much a work in progress and might be buggy.
It does not create any kind of graphics context, also it has no buffer for rendering stuff.
It really just opens a window, the rest is up to you.
It should work on Windows and Linux (link against X11).
*/

#ifndef WINDOW_HEADER
#define WINDOW_HEADER

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#pragma comment(lib, "user32.lib")
#elif defined __linux__
#include <X11/XKBlib.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <time.h>
#else
#pragma error "[window.h] Unsupported platform!"
#endif

#ifndef WINDOW_IMPL
#define WINDOW_FN extern
#else
#define WINDOW_FN
#endif

#include <stdint.h>
#include <stdbool.h>

typedef struct window_desc_t {
    const char *title;
    int width;
    int height;
} window_desc_t;

typedef struct window_t {
    const char *title;
    int width;
    int height;
    uint8_t keys[256];
    uint8_t prev_keys[256];
    int mod;
    int x;
    int y;
    int mouse;
    bool fullscreen;
#if defined(_WIN32)
    HWND win;
#elif defined(__linux__)
    Display *dpy;
    Window win;
#endif
} window_t;

WINDOW_FN bool window_open(window_t *w, const window_desc_t *desc);
WINDOW_FN bool window_loop(window_t *w);
WINDOW_FN void window_close(window_t *w);

WINDOW_FN void window_toggle_fullscreen(window_t *w);

WINDOW_FN bool window_key_pressed(window_t *w, int key);
WINDOW_FN bool window_key_down(window_t *w, int key);
WINDOW_FN bool window_key_released(window_t *w, int key);

WINDOW_FN void window_sleep(int64_t ms);
WINDOW_FN int64_t window_time(void);

#endif //WINDOW_HEADER

#ifdef WINDOW_IMPL

#if defined(_WIN32)

static const uint8_t FENSTER_KEYCODES[] = {0,27,49,50,51,52,53,54,55,56,57,48,45,61,8,9,81,87,69,82,84,89,85,73,79,80,91,93,10,0,65,83,68,70,71,72,74,75,76,59,39,96,0,92,90,88,67,86,66,78,77,44,46,47,0,0,0,32,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,17,3,0,20,0,19,0,5,18,4,26,127};

static LRESULT CALLBACK window_wndproc(HWND win, UINT msg, WPARAM wParam, LPARAM lParam) {
    window_t* w = (window_t *)GetWindowLongPtr(win, GWLP_USERDATA);
    switch (msg) {
    case WM_CLOSE:
        DestroyWindow(win);
        break;
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
        w->mouse = (msg == WM_LBUTTONDOWN);
        break;
    case WM_MOUSEMOVE:
        w->y = HIWORD(lParam), w->x = LOWORD(lParam);
        break;
    case WM_KEYDOWN:
    case WM_KEYUP: {
        w->mod = ((GetKeyState(VK_CONTROL) & 0x8000) >> 15) |
                ((GetKeyState(VK_SHIFT) & 0x8000) >> 14) |
                ((GetKeyState(VK_MENU) & 0x8000) >> 13) |
                (((GetKeyState(VK_LWIN) | GetKeyState(VK_RWIN)) & 0x8000) >> 12);
        w->keys[FENSTER_KEYCODES[HIWORD(lParam) & 0x1ff]] = !((lParam >> 31) & 1);
    } break;
    case WM_SIZE:
        w->width = LOWORD(lParam);
        w->height = HIWORD(lParam);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(win, msg, wParam, lParam);
    }
    return 0;
}

bool window_open(window_t* w, const window_desc_t* desc) {
    HINSTANCE hInstance = GetModuleHandle(NULL);
    WNDCLASSEX wc = {0};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = 0;
    wc.lpfnWndProc = window_wndproc;
    wc.hInstance = hInstance;
    wc.lpszClassName = desc->title;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClassEx(&wc);
    w->win = CreateWindowEx(WS_EX_CLIENTEDGE, desc->title, desc->title,
                            WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
                            desc->width, desc->height, NULL, NULL, hInstance, NULL);

    if (w->win == NULL) return false;
    SetWindowLongPtr(w->win, GWLP_USERDATA, (LONG_PTR)w);
    ShowWindow(w->win, SW_NORMAL);
    UpdateWindow(w->win);
    w->width = desc->width;
    w->height = desc->height;
    w->title = desc->title;
    return true;
}

void window_close(window_t *w) { (void)w; }

bool window_loop(window_t *w) {
    MSG msg;
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) return false;
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return true;
}

bool window_key_down(window_t *w, int key) {
    return w->keys[key] != 0;
}

bool window_key_pressed(window_t *w, int key) {
    bool pressed = w->keys[key] && !w->prev_keys[key];
    w->prev_keys[key] = w->keys[key];
    return pressed;
}

bool window_key_released(window_t *w, int key) {
    bool released = !w->keys[key] && w->prev_keys[key];
    w->prev_keys[key] = w->keys[key];
    return released;
}

void window_toggle_fullscreen(window_t *w) {
    static RECT saved_rect;
    HWND hwnd = w->win;
    if (!w->fullscreen) {
        // Save current window position and size
        GetWindowRect(hwnd, &saved_rect);
        // Remove window borders and set fullscreen
        SetWindowLong(hwnd, GWL_STYLE, WS_POPUP | WS_VISIBLE);
        MONITORINFO mi = { sizeof(mi) };
        GetMonitorInfo(MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST), &mi);
        SetWindowPos(hwnd, HWND_TOP,
            mi.rcMonitor.left, mi.rcMonitor.top,
            mi.rcMonitor.right - mi.rcMonitor.left,
            mi.rcMonitor.bottom - mi.rcMonitor.top,
            SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
        w->fullscreen = true;
    } else {
        // Restore window borders and position
        SetWindowLong(hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_VISIBLE);
        SetWindowPos(hwnd, HWND_NOTOPMOST,
            saved_rect.left, saved_rect.top,
            saved_rect.right - saved_rect.left,
            saved_rect.bottom - saved_rect.top,
            SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
        w->fullscreen = false;
    }
}

void window_sleep(int64_t ms) { Sleep(ms); }

int64_t window_time() {
    LARGE_INTEGER freq, count;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&count);
    return (int64_t)(count.QuadPart * 1000.0 / freq.QuadPart);
}

#else

static int FENSTER_KEYCODES[124] = {XK_BackSpace,8,XK_Delete,127,XK_Down,18,XK_End,5,XK_Escape,27,XK_Home,2,XK_Insert,26,XK_Left,20,XK_Page_Down,4,XK_Page_Up,3,XK_Return,10,XK_Right,19,XK_Tab,9,XK_Up,17,XK_apostrophe,39,XK_backslash,92,XK_bracketleft,91,XK_bracketright,93,XK_comma,44,XK_equal,61,XK_grave,96,XK_minus,45,XK_period,46,XK_semicolon,59,XK_slash,47,XK_space,32,XK_a,65,XK_b,66,XK_c,67,XK_d,68,XK_e,69,XK_f,70,XK_g,71,XK_h,72,XK_i,73,XK_j,74,XK_k,75,XK_l,76,XK_m,77,XK_n,78,XK_o,79,XK_p,80,XK_q,81,XK_r,82,XK_s,83,XK_t,84,XK_u,85,XK_v,86,XK_w,87,XK_x,88,XK_y,89,XK_z,90,XK_0,48,XK_1,49,XK_2,50,XK_3,51,XK_4,52,XK_5,53,XK_6,54,XK_7,55,XK_8,56,XK_9,57};

bool window_open(window_t* w, const window_desc_t* desc) {
    w->dpy = XOpenDisplay(NULL);
    int screen = DefaultScreen(w->dpy);
    w->win = XCreateSimpleWindow(
        w->dpy,
        RootWindow(w->dpy, screen), 0, 0, desc->width,
        desc->height, 0, BlackPixel(w->dpy, screen),
        WhitePixel(w->dpy, screen)
    );
    XSelectInput(w->dpy, w->win,
                ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask |
                    ButtonReleaseMask | PointerMotionMask);
    XStoreName(w->dpy, w->win, desc->title);
    w->title = desc->title;
    // Setup WM_DELETE_WINDOW protocol
    Atom wm_delete = XInternAtom(w->dpy, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(w->dpy, w->win, &wm_delete, 1);
    XMapWindow(w->dpy, w->win);
    XSync(w->dpy, w->win);
    return true;
}

void window_close(window_t* w) { XCloseDisplay(w->dpy);}

bool window_loop(window_t* w) {
    XEvent ev;
    Atom wm_delete = XInternAtom(w->dpy, "WM_DELETE_WINDOW", False);
    while (XPending(w->dpy)) {
        XNextEvent(w->dpy, &ev);
        switch (ev.type) {
        case ButtonPress:
        case ButtonRelease:
            w->mouse = (ev.type == ButtonPress);
            break;
        case MotionNotify:
            w->x = ev.xmotion.x, w->y = ev.xmotion.y;
            break;
        case KeyPress:
        case KeyRelease: {
            int m = ev.xkey.state;
            int k = XkbKeycodeToKeysym(w->dpy, ev.xkey.keycode, 0, 0);
            for (unsigned int i = 0; i < 124; i += 2) {
                if (FENSTER_KEYCODES[i] == k) {
                    w->keys[FENSTER_KEYCODES[i + 1]] = (ev.type == KeyPress);
                    break;
                }
            }
            w->mod = (!!(m & ControlMask)) | (!!(m & ShiftMask) << 1) |
                    (!!(m & Mod1Mask) << 2) | (!!(m & Mod4Mask) << 3);
        } break;
        case ClientMessage:
            if ((Atom)ev.xclient.data.l[0] == wm_delete) {
                return false;
            }
            break;
        }
    }
    return true;
}

void window_toggle_fullscreen(window_t *w) {
    static int is_fullscreen = 0;
    Display *dpy = w->dpy;
    Window win = w->win;
    Atom wm_state = XInternAtom(dpy, "_NET_WM_STATE", False);
    Atom fullscreen = XInternAtom(dpy, "_NET_WM_STATE_FULLSCREEN", False);
    XEvent xev = {0};
    xev.type = ClientMessage;
    xev.xclient.window = win;
    xev.xclient.message_type = wm_state;
    xev.xclient.format = 32;
    xev.xclient.data.l[0] = is_fullscreen ? 0 : 1; // 1 = add, 0 = remove
    xev.xclient.data.l[1] = fullscreen;
    xev.xclient.data.l[2] = 0;
    xev.xclient.data.l[3] = 1;
    xev.xclient.data.l[4] = 0;
    XSendEvent(dpy, DefaultRootWindow(dpy), False,
        SubstructureNotifyMask, &xev);
    is_fullscreen = !is_fullscreen;
}

bool window_key_down(window_t *w, int key) {
    return w->keys[key] != 0;
}

bool window_key_pressed(window_t *w, int key) {
    bool pressed = w->keys[key] && !w->prev_keys[key];
    w->prev_keys[key] = w->keys[key];
    return pressed;
}

bool window_key_released(window_t *w, int key) {
    bool released = !w->keys[key] && w->prev_keys[key];
    w->prev_keys[key] = w->keys[key];
    return released;
}

void window_sleep(int64_t ms) {
    struct timespec ts;
    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000000;
    nanosleep(&ts, NULL);
}
int64_t window_time(void) {
    struct timespec time;
    clock_gettime(CLOCK_REALTIME, &time);
    return time.tv_sec * 1000 + (time.tv_nsec / 1000000);
}
#endif

#endif //WINDOW_IMPL

