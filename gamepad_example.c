#define GAMEPAD_IMPL
#include "gamepad.h"
#include <stdio.h>

int main(void) {
    gp_context* ctx = gp_create();
    gp_set_deadzone(ctx, 0.15f); //0.1 by default

    while (1) {
        gp_update(ctx);

        for (int i = 0; i < GP_MAX_GAMEPADS; ++i) {
            const gp_state* state = gp_get_state(ctx, i);
            if (state && state->connected) {
                printf("Pad %d: X=%.2f Y=%.2f A=%d\n", i, state->axes[0], state->axes[1], state->buttons[0]);

                if (state->buttons[0]) {
                    gp_set_vibration(ctx, i, 1.0f, 1.0f);
                } else {
                    gp_set_vibration(ctx, i, 0.0f, 0.0f);
                }
            }
        }
#ifdef _WIN32
        Sleep(16);
#else
        usleep(16000);
#endif
    }

    gp_destroy(ctx);
    return 0;
}
