/*
Copyright (c) Arne Koenig 2025
Redistribution and use in source and binary forms, with or without modification, are permitted.
THIS SOFTWARE IS PROVIDED 'AS-IS', WITHOUT ANY EXPRESS OR IMPLIED WARRANTY. IN NO EVENT WILL THE AUTHORS BE HELD LIABLE FOR ANY DAMAGES ARISING FROM THE USE OF THIS SOFTWARE.
*/

/*
Quick and dirty bare bones gamepad wrapper for windows and linux(tested on linux mint).
This header tries to do the bare minimum, to get gamedpad input working on both platforms.
I only own a logitech controller, so further testing might be required.
As you might guess by now, this code is far from production ready, but I hope,
it can serve as a starting point.
All functions except gp_create() assume a valid gp_context*, it is up to you, to make sure it is.
Since controllers do not have a common layout you might have to create mapping lookup tables.
You can find many layouts here: https://github.com/mdqinc/SDL_GameControllerDB,
but it is up to you, to implement it.
*/


#ifndef GAMEPAD_H
#define GAMEPAD_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

#define GP_MAX_GAMEPADS 4
#define GP_MAX_AXES 8
#define GP_MAX_BUTTONS 16

typedef struct gp_context gp_context;

typedef struct gp_state {
    float axes[GP_MAX_AXES];
    bool buttons[GP_MAX_BUTTONS];
    bool connected;
} gp_state;

gp_context* gp_create(void);
void gp_destroy(gp_context* ctx);
void gp_update(gp_context* ctx);
const gp_state* gp_get_state(gp_context* ctx, int index);
void gp_set_deadzone(gp_context* ctx, float deadzone);
bool gp_set_vibration(gp_context* ctx, int index, float left_motor, float right_motor);

#ifdef __cplusplus
}
#endif

#endif //GAMEPAD_H

#ifdef GAMEPAD_IMPL

#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Xinput.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#pragma comment(lib, "xinput9_1_0.lib") // Or xinput1_4.lib depending on SDK

struct gp_context {
    gp_state states[GP_MAX_GAMEPADS];
    float deadzone;
};

gp_context* gp_create(void) {
    gp_context* ctx = (gp_context*)malloc(sizeof(gp_context));
    if (ctx) {
        memset(ctx, 0, sizeof(gp_context));
        ctx->deadzone = 0.1f; // Default deadzone
    }
    return ctx;
}

void gp_destroy(gp_context* ctx) {
    free(ctx);
}

void gp_set_deadzone(gp_context* ctx, float deadzone) {
    ctx->deadzone = deadzone;
}

// Apply radial deadzone filtering
static float apply_deadzone(float value, float deadzone) {
    if (fabsf(value) < deadzone) {
        return 0.0f;
    }
    // Rescale to full range after deadzone
    float sign = (value > 0) ? 1.0f : -1.0f;
    return sign * ((fabsf(value) - deadzone) / (1.0f - deadzone));
}

void gp_update(gp_context* ctx) {
    //Maybe: track number of connected gamepads
    // and only update these, or have a check interval...
    for (DWORD i = 0; i < GP_MAX_GAMEPADS; ++i) {
        XINPUT_STATE xstate;
        DWORD result = XInputGetState(i, &xstate);

        gp_state* gp = &ctx->states[i];
        if (result == ERROR_SUCCESS) {
            gp->connected = true;

            // Buttons
            WORD b = xstate.Gamepad.wButtons;
            gp->buttons[0] = (b & XINPUT_GAMEPAD_A) != 0;
            gp->buttons[1] = (b & XINPUT_GAMEPAD_B) != 0;
            gp->buttons[2] = (b & XINPUT_GAMEPAD_X) != 0;
            gp->buttons[3] = (b & XINPUT_GAMEPAD_Y) != 0;
            gp->buttons[4] = (b & XINPUT_GAMEPAD_LEFT_SHOULDER) != 0;
            gp->buttons[5] = (b & XINPUT_GAMEPAD_RIGHT_SHOULDER) != 0;
            gp->buttons[6] = (b & XINPUT_GAMEPAD_BACK) != 0;
            gp->buttons[7] = (b & XINPUT_GAMEPAD_START) != 0;
            gp->buttons[8] = (b & XINPUT_GAMEPAD_LEFT_THUMB) != 0;
            gp->buttons[9] = (b & XINPUT_GAMEPAD_RIGHT_THUMB) != 0;

            // Axes
            float lx = xstate.Gamepad.sThumbLX / 32768.0f;
            float ly = xstate.Gamepad.sThumbLY / 32768.0f;
            float rx = xstate.Gamepad.sThumbRX / 32768.0f;
            float ry = xstate.Gamepad.sThumbRY / 32768.0f;

            gp->axes[0] = apply_deadzone(lx, ctx->deadzone);
            gp->axes[1] = apply_deadzone(ly, ctx->deadzone);
            gp->axes[2] = apply_deadzone(rx, ctx->deadzone);
            gp->axes[3] = apply_deadzone(ry, ctx->deadzone);

            gp->axes[4] = xstate.Gamepad.bLeftTrigger / 255.0f;
            gp->axes[5] = xstate.Gamepad.bRightTrigger / 255.0f;
        } else {
            gp->connected = false;
        }
    }
}

const gp_state* gp_get_state(gp_context* ctx, int index) {
    if (index < 0 || index >= GP_MAX_GAMEPADS) {
        return NULL;
    }
    return &ctx->states[index];
}

bool gp_set_vibration(gp_context* ctx, int index, float left_motor, float right_motor) {
    if (index < 0 || index >= GP_MAX_GAMEPADS) return false;

    XINPUT_VIBRATION vibration;
    vibration.wLeftMotorSpeed = (WORD)(left_motor * 65535.0f);
    vibration.wRightMotorSpeed = (WORD)(right_motor * 65535.0f);

    DWORD result = XInputSetState(index, &vibration);
    return result == ERROR_SUCCESS;
}

#elif defined(__linux__) // linux

#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <linux/input.h>
#include <linux/input-event-codes.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <stdint.h>

struct gp_context {
    int fds[GP_MAX_GAMEPADS];
    gp_state states[GP_MAX_GAMEPADS];
    float deadzone;
};

static int is_gamepad(const char* path) {
    int fd = open(path, O_RDONLY | O_NONBLOCK);
    if (fd < 0) return 0;

    unsigned long evbits[EV_MAX / (sizeof(unsigned long) * 8) + 1];
    memset(evbits, 0, sizeof(evbits));
    if (ioctl(fd, EVIOCGBIT(0, sizeof(evbits)), evbits) < 0) {
        close(fd);
        return 0;
    }

    int is_gp = (evbits[EV_KEY / (sizeof(unsigned long) * 8)] & (1UL << (EV_KEY % (sizeof(unsigned long) * 8)))) &&
                (evbits[EV_ABS / (sizeof(unsigned long) * 8)] & (1UL << (EV_ABS % (sizeof(unsigned long) * 8))));

    close(fd);
    return is_gp;
}

gp_context* gp_create(void) {
    gp_context* ctx = (gp_context*)malloc(sizeof(gp_context));
    if (!ctx) return NULL;
    memset(ctx, 0, sizeof(gp_context));

    ctx->deadzone = 0.1f;

    DIR* dir = opendir("/dev/input");
    if (!dir) return ctx;

    struct dirent* entry;
    int index = 0;
    while ((entry = readdir(dir)) && index < GP_MAX_GAMEPADS) {
        if (strncmp(entry->d_name, "event", 5) != 0) continue;

        char path[PATH_MAX];
        snprintf(path, sizeof(path), "/dev/input/%s", entry->d_name);

        if (!is_gamepad(path)) continue;

        int fd = open(path, O_RDONLY | O_NONBLOCK);
        if (fd < 0) continue;

        ctx->fds[index] = fd;
        ctx->states[index].connected = true;
        index++;
    }

    closedir(dir);
    return ctx;
}

void gp_destroy(gp_context* ctx) {
    for (int i = 0; i < GP_MAX_GAMEPADS; ++i) {
        if (ctx->fds[i] > 0) close(ctx->fds[i]);
    }
    free(ctx);
}

static float apply_deadzone(float value, float deadzone) {
    if (value > -deadzone && value < deadzone) return 0.0f;
    return value;
}

#define BTN_LOOKUP_TABLE_SIZE (KEY_MAX + 1)

static const uint8_t btn_lookup[BTN_LOOKUP_TABLE_SIZE] = {
    [BTN_SOUTH]  = 1,
    [BTN_EAST]   = 2,
    [BTN_NORTH]  = 3,
    [BTN_WEST]   = 4,
    [BTN_TL]     = 5,
    [BTN_TR]     = 6,
    [BTN_SELECT] = 7,
    [BTN_START]  = 8,
    [BTN_THUMBL] = 9,
    [BTN_THUMBR] = 10,
    [BTN_MODE]   = 11,
};

static int map_button_code(uint16_t code) {
    if (code < BTN_LOOKUP_TABLE_SIZE) {
        int mapped = btn_lookup[code];
        return (mapped > 0) ? mapped - 1 : -1;
    }
    return -1;
}

void gp_update(gp_context* ctx) {
    if (!ctx) return;

    struct input_event ev;
    for (int i = 0; i < GP_MAX_GAMEPADS; ++i) {
        int fd = ctx->fds[i];
        if (fd <= 0) continue;

        while (read(fd, &ev, sizeof(ev)) == sizeof(ev)) {
            if (ev.type == EV_KEY) {
                int btn_index = map_button_code(ev.code);
                if (btn_index >= 0 && btn_index < GP_MAX_BUTTONS) {
                    ctx->states[i].buttons[btn_index] = ev.value;
                }
            } else if (ev.type == EV_ABS && ev.code < GP_MAX_AXES) {
                struct input_absinfo abs;
                if (ioctl(fd, EVIOCGABS(ev.code), &abs) == 0) {
                    float val = (float)(ev.value - abs.minimum) / (abs.maximum - abs.minimum) * 2.0f - 1.0f;
                    ctx->states[i].axes[ev.code] = apply_deadzone(val, ctx->deadzone);
                }
            }
        }
    }
}

const gp_state* gp_get_state(gp_context* ctx, int index) {
    if (index < 0 || index >= GP_MAX_GAMEPADS) return NULL;
    return &ctx->states[index];
}

void gp_set_deadzone(gp_context* ctx, float deadzone) {
    ctx->deadzone = deadzone;
}

bool gp_set_vibration(gp_context* ctx, int index, float left_motor, float right_motor) {
    //TODO
    return false;
}

#else
#error Unsupported platform!
#endif

#endif //GAMEPAD_IMPL
