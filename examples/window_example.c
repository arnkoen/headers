#define WINDOW_IMPL
#include "../window.h"


#define W 800
#define H 600

static int run() {
    window_t w = {0};
    window_open(&w, &(window_desc_t){
        .title = "hello",
        .width = W,
        .height = H,
    });

    int64_t now = window_time();
    while (window_loop(&w)) {
        int64_t time = window_time();
        if (time - now < 1000 / 60) {
        window_sleep(time - now);
        }
        now = time;

        if (window_key_pressed(&w, 'F')) {
            window_toggle_fullscreen(&w);
        }
    }
    window_close(&w);
    return 0;
}

#if defined(_WIN32)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow) {
    (void)hInstance, (void)hPrevInstance, (void)pCmdLine, (void)nCmdShow;
    return run();
}
#else
int main() { return run(); }
#endif
