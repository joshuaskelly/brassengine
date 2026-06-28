
/**
 * SDL3 platform implementation
 */

#include <stdbool.h>
#include <stdio.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_error.h>

#include "../extensions/sdl3-extensions.h"

#include "../../arguments.h"
#include "../../assets.h"
#include "../../configuration.h"
#include "../../core.h"
#include "../../event.h"
#include "../../graphics.h"
#include "../../input.h"
#include "../../log.h"
#include "../../math.h"
#include "../../platform.h"

#include "../../modules/platforms/sdl3.h"

#define FPS 60
#define FRAME_TIME_LENGTH (1000 / FPS)

static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;
static SDL_Texture* render_buffer_texture = NULL;
static uint32_t* pixels = NULL;
static int ticks_last_frame;
static SDL_FRect display_rect;

static void sdl_init(void);
static void sdl_destroy(void);
static void sdl_handle_events(void);
static void sdl_fix_frame_rate(void);
static void sdl_pixels_resize(int width, int height);

void sound_init(void);
void sound_reload(void);
void sound_destroy(void);
void sound_get_version(char* s);

static void log_platform_info(void);

int platform_main(int argc, char* argv[]) {
    if (arguments_check("-v") || arguments_check("--version")) {
       log_info(ENGINE_COPYRIGHT);

       return 0;
    }

    core_init();
    core_run();
    core_destroy();

    return 0;
}

void platform_init(void) {
    sound_init();
    sdl_init();
    log_platform_info();
}

void platform_destroy(void) {
    sound_destroy();
    sdl_destroy();
}

void platform_reload(void) {
    sound_reload();
}

void platform_update(void) {
    sdl_handle_events();
    sdl_fix_frame_rate();
}

void platform_draw(void) {
    texture_t* render_texture = graphics_render_texture_get();
    uint32_t* palette = graphics_palette_get();

    // Convert core render buffer from indexed to rgba pixels
    for (int i = 0; i < render_texture->width * render_texture->height; i++) {
        pixels[i] = palette[render_texture->pixels[i]];
    }

    // Maintain aspect ratio and center in window
    int width;
    int height;
    graphics_resolution_get(&width, &height);

    int window_width;
    int window_height;

    SDL_GetWindowSizeInPixels(window, &window_width, &window_height);

    float window_aspect = window_width / (float)window_height;
    float buffer_aspect = width / (float)height * config->display.aspect;

    display_rect.w = window_width;
    display_rect.h = window_height;

    if (buffer_aspect < window_aspect) {
        display_rect.w = display_rect.h * buffer_aspect;
    }
    else {
        display_rect.h = display_rect.w / buffer_aspect;
    }

    display_rect.x = (window_width - display_rect.w) / 2;
    display_rect.y = (window_height - display_rect.h) / 2;

    SDL_UpdateTexture(
        render_buffer_texture,
        NULL,
        pixels,
        render_texture->width * sizeof(uint32_t)
    );

    SDL_RenderClear(renderer);

    SDL_RenderTexture(
        renderer,
        render_buffer_texture,
        NULL,
        &display_rect
    );

    SDL_RenderPresent(renderer);
}

bool platform_handle_event(event_t* event) {
    if (event->type == EVENT_GRAPHICSRESOLUTIONCHANGED) {
        int width = event->graphics_resolution_change.width;
        int height = event->graphics_resolution_change.height;

        sdl_pixels_resize(width, height);

        SDL_DestroyTexture(render_buffer_texture);

        render_buffer_texture = SDL_CreateTexture(
            renderer,
            SDL_PIXELFORMAT_RGBA32,
            SDL_TEXTUREACCESS_STREAMING,
            width,
            height
        );

        if (!render_buffer_texture) {
            log_fatal("Error creating SDL frame buffer texture");
        }

        return true;
    }

    return false;
}

void platform_mouse_grabbed_set(bool grabbed) {
    SDL_SetWindowRelativeMouseMode(window, grabbed);
}

bool platform_mouse_grabbed_get(void) {
    return SDL_GetWindowRelativeMouseMode(window);
}

void platform_open_module(void* arg) {
    open_sdl3_platform_module(arg, window);
}

static void sdl_init(void) {
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMEPAD)) {
        log_fatal("Error initializing SDL");
    }

    const int width = config->resolution.width;
    const int height = config->resolution.height;
    const int default_window_scale = 3;

    // Create SDL window
    window = SDL_CreateWindow(
        NULL,
        width * default_window_scale,
        height * default_window_scale,
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY
    );

    if (!window) {
        log_fatal("Error creating SDL window");
    }

    SDL_SetWindowSizeInPixels(
        window,
        width * default_window_scale,
        height * default_window_scale
    );

    // Create SDL renderer
    renderer = SDL_CreateRenderer(
        window,
        NULL
    );

    if (!renderer) {
        log_fatal("Error creating SDL renderer");
    }

    sdl_pixels_resize(width, height);

    // Create render buffer texture
    render_buffer_texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_STREAMING,
        width,
        height
    );

    SDL_SetTextureScaleMode(render_buffer_texture, SDL_SCALEMODE_PIXELART);

    if (!render_buffer_texture) {
        log_fatal("Error creating SDL frame buffer texture");
    }

    // Hide cursor by default
    SDL_HideCursor();
}

static void sdl_destroy(void) {
    SDL_DestroyTexture(render_buffer_texture);
    free(pixels);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

static void sdl_handle_events(void) {
    SDL_Event sdl_event;
    event_t event;
    SDL_Gamepad* controller = NULL;

    int width;
    int height;
    graphics_resolution_get(&width, &height);

    float aspect_width = width / (float)display_rect.w;
    float aspect_height = height / (float)display_rect.h;
    float ratio = SDL_GetWindowPixelScale(window);

    while (SDL_PollEvent(&sdl_event)) {
        switch (sdl_event.type) {
            case SDL_EVENT_QUIT:
                event.type = EVENT_QUIT;
                event_post(&event);
                break;

            case SDL_EVENT_KEY_DOWN:
                event.type = EVENT_KEYDOWN;
                event.key.type = EVENT_KEYDOWN;
                event.key.code = (key_code_t)sdl_event.key.scancode;
                event.key.symbol = (key_symbol_t)sdl_event.key.key;
                event_post(&event);
                break;

            case SDL_EVENT_KEY_UP:
                event.type = EVENT_KEYUP;
                event.key.type = EVENT_KEYUP;
                event.key.code = (key_code_t)sdl_event.key.scancode;
                event.key.symbol = (key_symbol_t)sdl_event.key.key;
                event_post(&event);
                break;

            case SDL_EVENT_MOUSE_MOTION:
                event.type = EVENT_MOUSEMOTION;
                event.motion.type = EVENT_MOUSEMOTION;
                event.motion.x = (sdl_event.motion.x / ratio - display_rect.x) * aspect_width;
                event.motion.y = (sdl_event.motion.y / ratio - display_rect.y) * aspect_height;
                event.motion.motion_x = (sdl_event.motion.xrel / ratio) * aspect_width;
                event.motion.motion_y = (sdl_event.motion.yrel / ratio) * aspect_height;

                event.motion.x = clamp(event.motion.x, 0, width - 1);
                event.motion.y = clamp(event.motion.y, 0, height - 1);

                event_post(&event);
                break;

            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                event.type = EVENT_MOUSEDOWN;
                event.button.type = EVENT_MOUSEDOWN;
                event.button.button = sdl_event.button.button;
                event_post(&event);
                break;

            case SDL_EVENT_MOUSE_BUTTON_UP:
                event.type = EVENT_MOUSEUP;
                event.button.type = EVENT_MOUSEUP;
                event.button.button = sdl_event.button.button;
                event_post(&event);
                break;

            case SDL_EVENT_MOUSE_WHEEL:
                event.type = EVENT_MOUSEWHEEL;
                event.wheel.wheel_x = sdl_event.wheel.x;
                event.wheel.wheel_y = sdl_event.wheel.y;
                event_post(&event);
                break;

            case SDL_EVENT_GAMEPAD_ADDED:
                controller = SDL_OpenGamepad(sdl_event.gdevice.which);
                int id = SDL_GetJoystickID(SDL_GetGamepadJoystick(controller));
                input_controller_connect(id);
                break;

            case SDL_EVENT_GAMEPAD_REMOVED:
                input_controller_disconnect(sdl_event.gdevice.which);
                SDL_CloseGamepad(SDL_GetGamepadFromID(sdl_event.gdevice.which));
                break;

            case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
                event.type = EVENT_CONTROLLERBUTTONDOWN;
                event.controller_button.which = sdl_event.gdevice.which;
                event.controller_button.button = sdl_event.gbutton.button;
                event_post(&event);
                break;

            case SDL_EVENT_GAMEPAD_BUTTON_UP:
                event.type = EVENT_CONTROLLERBUTTONUP;
                event.controller_button.which = sdl_event.gdevice.which;
                event.controller_button.button = sdl_event.gbutton.button;
                event_post(&event);
                break;

            case SDL_EVENT_GAMEPAD_AXIS_MOTION:
                event.type = EVENT_CONTROLLERAXISMOTION;
                event.controller_axis.which = sdl_event.gdevice.which;
                event.controller_axis.axis = sdl_event.gaxis.axis;
                if (sdl_event.gaxis.value < 0) {
                    event.controller_axis.value = sdl_event.gaxis.value / 32768.0f;
                }
                else {
                    event.controller_axis.value = sdl_event.gaxis.value / 32767.0f;
                }
                event_post(&event);
                break;
        }
    }
}

static void sdl_fix_frame_rate(void) {
    int time_to_wait = FRAME_TIME_LENGTH - (SDL_GetTicks() - ticks_last_frame);
    if (0 < time_to_wait && time_to_wait < FRAME_TIME_LENGTH) {
        SDL_Delay(time_to_wait);
    }

    ticks_last_frame = SDL_GetTicks();
}

static void sdl_pixels_resize(int width, int height) {
    if (sizeof(pixels) == width * height * sizeof(uint32_t)) return;

    // Free if not NULL
    if (pixels) {
        free(pixels);
        pixels = NULL;
    }

    pixels = calloc(width * height, sizeof(uint32_t));

    if (!pixels) {
        log_fatal("Error creating frame buffer.");
    }
}

static void log_platform_info(void) {
    // Get SDL version info
    const int sdl_version = SDL_GetVersion();

    // Get sound system version info
    char sound_version[32];
    sound_get_version(sound_version);

    // Build info string
    char buffer[128];
    snprintf(
        buffer,
        sizeof(buffer),
        "platform init (SDL %i.%i.%i, %s)",
        SDL_VERSIONNUM_MAJOR(sdl_version),
        SDL_VERSIONNUM_MINOR(sdl_version),
        SDL_VERSIONNUM_MICRO(sdl_version),
        sound_version
    );

    log_info(buffer);
}
