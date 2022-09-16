#include <SDL2/SDL.h>

#include "../arguments.h"
#include "../assets.h"
#include "../core.h"
#include "../event.h"
#include "../graphics.h"
#include "../log.h"
#include "../platform.h"
#include "../script.h"

#define FPS 60
#define FRAME_TIME_LENGTH (1000 / FPS)

static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;
static SDL_Texture* render_buffer_texture = NULL;
static uint32_t render_buffer[RENDER_BUFFER_WIDTH * RENDER_BUFFER_HEIGHT];
static int ticks_last_frame;

void sdl_handle_events(void);
void sdl_fix_frame_rate(void);

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
    // Get platform version info
    SDL_version version;
    SDL_GetVersion(&version);
    char buffer[32];
    snprintf(
        buffer,
        sizeof(buffer),
        "platform init (SDL %i.%i.%i)",
        version.major, version.minor, version.patch
    );

    log_info(buffer);

    const int window_width = RENDER_BUFFER_WIDTH * 3;
    const int window_height = RENDER_BUFFER_HEIGHT * 3;

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        log_fatal("Error initializing SDL");
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

    render_buffer_texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_STREAMING,
        RENDER_BUFFER_WIDTH,
        RENDER_BUFFER_HEIGHT
    );

    if (!render_buffer_texture) {
        log_fatal("Error creating SDL frame buffer texture");
    }
}

void platform_destroy(void) {
    SDL_DestroyTexture(render_buffer_texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void platform_update(void) {
    sdl_handle_events();
    sdl_fix_frame_rate();
}

void platform_draw(void) {
    texture_t* render_texture = graphics_get_render_texture();
    uint32_t* palette = graphics_palette_get();

    // Convert core render buffer from indexed to rgba
    for (int i = 0; i < render_texture->width * render_texture->height; i++) {
        render_buffer[i] = palette[render_texture->pixels[i]];
    }

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
        NULL
    );

    SDL_RenderPresent(renderer);
}

void sdl_handle_events(void) {
    SDL_Event sdl_event;
    event_t event;

    int width = 0;
    int height = 0;
    SDL_GetWindowSize(window, &width, &height);

    float aspect_width = RENDER_BUFFER_WIDTH / (float)width;
    float aspect_height = RENDER_BUFFER_HEIGHT / (float)height;

    char* filename;

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
                event.motion.x = sdl_event.motion.x * aspect_width;
                event.motion.y = sdl_event.motion.y * aspect_height;
                event.motion.rel_x = sdl_event.motion.xrel * aspect_width;
                event.motion.rel_y = sdl_event.motion.yrel * aspect_height;

                event_post(&event);
                break;

            case SDL_DROPFILE:
                filename = sdl_event.drop.file;

                // TODO: Handle drag and drop

                SDL_free(filename);

                break;
        }
    }
}

void sdl_fix_frame_rate(void) {
    int time_to_wait = FRAME_TIME_LENGTH - (SDL_GetTicks() - ticks_last_frame);
    if (0 < time_to_wait && time_to_wait < FRAME_TIME_LENGTH) {
        SDL_Delay(time_to_wait);
    }

    ticks_last_frame = SDL_GetTicks();
}
