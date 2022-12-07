#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <emscripten.h>

#include "../configuration.h"
#include "../core.h"
#include "../event.h"
#include "../graphics.h"
#include "../log.h"
#include "../platform.h"
#include "../sounds.h"

static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;
static SDL_Texture* render_buffer_texture = NULL;
static uint32_t* render_buffer = NULL;
static SDL_Rect display_rect;

static void sdl_handle_events(void);

int platform_main(int argc, char* argv[]) {
    core_init();
    emscripten_set_main_loop(core_main_loop, 0, 1);

    return 0;
}

void platform_init(void) {
    // Get platform version info
    SDL_version version;
    SDL_GetVersion(&version);

    const SDL_version* mix_version = Mix_Linked_Version();

    char buffer[128];
    snprintf(
        buffer,
        sizeof(buffer),
        "platform init (Emscripten %i.%i.%i, SDL %i.%i.%i, SDL Mixer %i.%i.%i)",
        __EMSCRIPTEN_major__, __EMSCRIPTEN_minor__, __EMSCRIPTEN_tiny__,
        version.major, version.minor, version.patch,
        mix_version->major, mix_version->minor, mix_version->patch
    );

    log_info(buffer);

    const int window_width = config->resolution.width * 3;
    const int window_height = config->resolution.height * 3;

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        log_fatal("Error initializing SDL");
    }

    if (Mix_OpenAudio(11025, MIX_DEFAULT_FORMAT, 1, 2048) < 0) {
        log_fatal("Error intializing SDL Mixer");
    }

    window = SDL_CreateWindow(
        NULL,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        window_width,
        window_height,
        SDL_WINDOW_RESIZABLE
    );

    if (!window) {
        log_fatal("Error creating SDL window");
    }

    renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED
    );

    if (!renderer) {
        log_fatal("Error creating SDL renderer");
    }

    SDL_SetRenderDrawBlendMode(
        renderer,
        SDL_BLENDMODE_BLEND
    );

    render_buffer = calloc(config->resolution.width * config->resolution.height, sizeof(uint32_t));

    if (!render_buffer) {
        log_fatal("Error creating frame buffer.");
    }

    render_buffer_texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_STREAMING,
        config->resolution.width,
        config->resolution.height
    );

    if (!render_buffer_texture) {
        log_fatal("Error creating SDL frame buffer texture");
    }

    display_rect.x = 0;
    display_rect.y = 0;
    display_rect.w = window_width;
    display_rect.h = window_height;

    SDL_ShowCursor(SDL_DISABLE);
}

void platform_destroy(void) {
    SDL_DestroyTexture(render_buffer_texture);
    free(render_buffer);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    Mix_Quit();
    SDL_Quit();
}

void platform_update(void) {
    sdl_handle_events();
}

void platform_draw(void) {
    texture_t* render_texture = graphics_get_render_texture();
    uint32_t* palette = graphics_palette_get();

    // Convert core render buffer from indexed to rgba
    for (int i = 0; i < render_texture->width * render_texture->height; i++) {
        render_buffer[i] = palette[render_texture->pixels[i]];
    }

    // Maintain aspect ratio and center in window
    int window_width;
    int window_height;

    SDL_GetWindowSize(window, &window_width, &window_height);

    float window_aspect = window_width / (float)window_height;
    float buffer_aspect = config->resolution.width / (float)config->resolution.height;

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
        render_buffer,
        render_texture->width * sizeof(uint32_t)
    );

    SDL_RenderCopy(
        renderer,
        render_buffer_texture,
        NULL,
        &display_rect
    );

    SDL_RenderPresent(renderer);
}

static float clamp(float x, float min, float max) {
    if (x < min) return min;
    if (x > max) return max;
    return x;
}

static void sdl_handle_events(void) {
    SDL_Event sdl_event;
    event_t event;

    float aspect_width = config->resolution.width / (float)display_rect.w;
    float aspect_height = config->resolution.height / (float)display_rect.h;

    while (SDL_PollEvent(&sdl_event)) {
        switch (sdl_event.type) {
            case SDL_QUIT:
                event.type = EVENT_QUIT;
                event_post(&event);
                break;

            case SDL_KEYDOWN:
                event.type = EVENT_KEYDOWN;
                event.key.type = EVENT_KEYDOWN;
                event.key.code = (key_code_t)sdl_event.key.keysym.scancode;
                event.key.symbol = (key_symbol_t)sdl_event.key.keysym.sym;
                event_post(&event);
                break;

            case SDL_KEYUP:
                event.type = EVENT_KEYUP;
                event.key.type = EVENT_KEYUP;
                event.key.code = (key_code_t)sdl_event.key.keysym.scancode;
                event.key.symbol = (key_symbol_t)sdl_event.key.keysym.sym;
                event_post(&event);
                break;

            case SDL_MOUSEMOTION:
                event.type = EVENT_MOUSEMOTION;
                event.motion.type = EVENT_MOUSEMOTION;
                event.motion.x = (sdl_event.motion.x - display_rect.x) * aspect_width;
                event.motion.y = (sdl_event.motion.y - display_rect.y) * aspect_height;
                event.motion.rel_x = (sdl_event.motion.xrel - display_rect.x) * aspect_width;
                event.motion.rel_y = (sdl_event.motion.yrel - display_rect.y) * aspect_height;

                event.motion.x = clamp(event.motion.x, 0, config->resolution.width - 1);
                event.motion.y = clamp(event.motion.y, 0, config->resolution.height - 1);
                event.motion.rel_x = clamp(event.motion.rel_x, 0, config->resolution.width - 1);
                event.motion.rel_y = clamp(event.motion.rel_y, 0, config->resolution.height - 1);

                event_post(&event);
                break;

            case SDL_MOUSEBUTTONDOWN:
                event.type = EVENT_MOUSEDOWN;
                event.button.type = EVENT_MOUSEDOWN;
                event.button.button = sdl_event.button.button;
                event_post(&event);
                break;

            case SDL_MOUSEBUTTONUP:
                event.type = EVENT_MOUSEUP;
                event.button.type = EVENT_MOUSEUP;
                event.button.button = sdl_event.button.button;
                event_post(&event);
                break;
        }
    }
}

void platform_play_sound(sound_t* sound) {
    Mix_Chunk* chunk = Mix_QuickLoad_RAW((uint8_t*)sound->pcm, sound->frame_count * sound->channel_count * sizeof(sample_t));
    Mix_PlayChannel(-1, chunk, 0);
}
