#include <stdbool.h>
#include <string.h>

#include <SDL.h>
#include <SDL_mixer.h>
#include <gl\glew.h>
#include <SDL_opengl.h>
#include <gl\glu.h>

#include "../arguments.h"
#include "../configuration.h"
#include "../core.h"
#include "../event.h"
#include "../files.h"
#include "../graphics.h"
#include "../log.h"
#include "../platform.h"
#include "../sounds.h"
#include "../time.h"

#define FPS 60
#define FRAME_TIME_LENGTH (1000 / FPS)

static SDL_Window* window = NULL;
static uint32_t* render_buffer = NULL;
static int ticks_last_frame;
static SDL_Rect display_rect;

static char* shader_source = NULL;

static GLuint shader_program;
static GLint position = -1;
static GLint texture_coordinates = -1;
static GLint screen_texture = -1;
static GLint output_size = -1;
static GLint frame_count = -1;

static GLuint vertex_buffer_object = 0;
static GLuint index_buffer_object = 0;
static GLuint texture = 0;

static void sdl_handle_events(void);
static void sdl_fix_frame_rate(void);
static void load_shader_program(void);

static const char default_shader[] = "#version 140\nuniform sampler2D screen_texture;in vec2 uv;out vec4 color;void main() {color = texture(screen_texture, uv);}";

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

    const SDL_version* mix_version = Mix_Linked_Version();

    char buffer[128];

    snprintf(
        buffer,
        sizeof(buffer),
        "platform init (SDL %i.%i.%i, SDL Mixer %i.%i.%i)",
        version.major, version.minor, version.patch,
        mix_version->major, mix_version->minor, mix_version->patch
    );

    log_info(buffer);

    const int window_width = config->resolution.width * 3;
    const int window_height = config->resolution.height * 3;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        log_fatal("Error initializing SDL");
    }

    if (Mix_OpenAudio(11025, MIX_DEFAULT_FORMAT, 1, 2048) < 0) {
        log_fatal("Error intializing SDL Mixer");
    }

    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 1 );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );

    window = SDL_CreateWindow(
        NULL,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        window_width,
        window_height,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
    );

    if (!window) {
        log_fatal("Error creating SDL window");
    }

    SDL_GLContext context = SDL_GL_CreateContext(window);
    if (!context) {
        log_fatal("Error creating OpenGL context");
    }

    render_buffer = calloc(config->resolution.width * config->resolution.height, sizeof(uint32_t));

    if (!render_buffer) {
        log_fatal("Error creating frame buffer.");
    }

    glewExperimental = GL_TRUE;
    GLenum glewError = glewInit();
    if (glewError != GLEW_OK) {
        log_fatal("Error initializing GLEW");
    }

    load_shader_program();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    GLfloat vertex_data[] = {
        -1.0f, -1.0f, 0.0f, 1.0f,
        1.0f, -1.0f, 1.0f, 1.0f,
        1.0f,  1.0f, 1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f, 0.0f
    };

    GLuint index_data[] = {
        0, 1, 2, 3
    };

    glGenBuffers(1, &vertex_buffer_object);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object);
    glBufferData(GL_ARRAY_BUFFER, 4 * 4 * sizeof(GLfloat), vertex_data, GL_STATIC_DRAW);

    glGenBuffers(1, &index_buffer_object);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_object);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof(GLuint), index_data, GL_STATIC_DRAW);

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, config->resolution.width, config->resolution.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, render_buffer);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    display_rect.x = 0;
    display_rect.y = 0;
    display_rect.w = window_width;
    display_rect.h = window_height;

    SDL_ShowCursor(SDL_DISABLE);
}

void platform_destroy(void) {
    glDeleteProgram(shader_program);
    free(shader_source);
    free(render_buffer);
    SDL_DestroyWindow(window);
    Mix_Quit();
    SDL_Quit();
}

void platform_reload(void) {
    glDeleteProgram(shader_program);
    free(shader_source);
    load_shader_program();
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

    glViewport(
        display_rect.x,
        display_rect.y,
        display_rect.w,
        display_rect.h
    );

    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shader_program);

    glTexSubImage2D(
        GL_TEXTURE_2D,          // target
        0,                      // level
        0,                      // x offset
        0,                      // y offset
        render_texture->width,  // width
        render_texture->height, // height
        GL_RGBA,                // format
        GL_UNSIGNED_BYTE,       // type,
        render_buffer
    );

    glBindTexture(GL_TEXTURE_2D, texture);

    glUniform1i(screen_texture, 0);
    glUniform1i(frame_count, time_frames_get());
    glUniform2f(output_size, window_width, window_height);

    glEnableVertexAttribArray(texture_coordinates);
    glEnableVertexAttribArray(position);

    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object);

    glVertexAttribPointer(position, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4, NULL);
    glVertexAttribPointer(texture_coordinates, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4, (void*)(sizeof(GLfloat) * 2));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_object);
    glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_INT, NULL);

    glDisableVertexAttribArray(texture_coordinates);
    glDisableVertexAttribArray(position);

    glUseProgram(0);

    SDL_GL_SwapWindow(window);
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
                event.motion.motion_x = (sdl_event.motion.xrel) * aspect_width;
                event.motion.motion_y = (sdl_event.motion.yrel) * aspect_height;

                event.motion.x = clamp(event.motion.x, 0, config->resolution.width - 1);
                event.motion.y = clamp(event.motion.y, 0, config->resolution.height - 1);

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

            case SDL_WINDOWEVENT_RESIZED:
            case SDL_WINDOWEVENT_SIZE_CHANGED:
                display_rect.w = sdl_event.window.data1;
                display_rect.h = sdl_event.window.data2;
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

void platform_play_sound(sound_t* sound) {
    Mix_Chunk* chunk = Mix_QuickLoad_RAW((uint8_t*)sound->pcm, sound->frame_count * sound->channel_count * sizeof(sample_t));
    Mix_PlayChannel(-1, chunk, 0);
}

static void load_shader_program(void) {
    shader_program = glCreateProgram();
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);

    const GLchar* vertex_shader_source[] = {
        "#version 140\nin vec2 position;in vec2 texture_coordinates;out vec2 uv;void main() {uv = texture_coordinates;gl_Position = vec4(position.x, position.y, 0, 1);}"
    };

    glShaderSource(vertex_shader, 1, vertex_shader_source, NULL);

    glCompileShader(vertex_shader);

    GLint compile_success = GL_FALSE;
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &compile_success);
    if (compile_success != GL_TRUE) {
        log_fatal("Error compiling vertex shader");
    }

    glAttachShader(shader_program, vertex_shader);

    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

    shader_source = (char*)files_read("shader.frag");
    if (!shader_source) {
        shader_source = calloc(sizeof(default_shader), sizeof(char));
        strcpy(shader_source, default_shader);
    }

    const GLchar* fragment_shader_source[] = {
        shader_source
    };

    glShaderSource(fragment_shader, 1, fragment_shader_source, NULL);

    glCompileShader(fragment_shader);

    compile_success = GL_FALSE;
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &compile_success);
    if (compile_success != GL_TRUE) {
        log_fatal("Error compiling fragment shader");
    }

    glAttachShader(shader_program, fragment_shader);

    glLinkProgram(shader_program);

    compile_success = GL_FALSE;
    glGetProgramiv(shader_program, GL_LINK_STATUS, &compile_success);
    if (compile_success != GL_TRUE) {
        log_fatal("Error linking program");
    }

    position = glGetAttribLocation(shader_program, "position");
    texture_coordinates = glGetAttribLocation(shader_program, "texture_coordinates");
    screen_texture = glGetUniformLocation(shader_program, "screen_texture");
    output_size = glGetUniformLocation(shader_program, "output_size");
    frame_count = glGetUniformLocation(shader_program, "frame_count");
}

void platform_display_set_resolution(int width, int height) {
    free(render_buffer);

    render_buffer = calloc(width * height, sizeof(uint32_t));

    if (!render_buffer) {
        log_fatal("Error creating frame buffer.");
    }
}

void platform_display_set_size(int width, int height) {
    SDL_SetWindowSize(window, width, height);
}

void platform_display_set_fullscreen(bool fullscreen) {
    SDL_SetWindowFullscreen(window, fullscreen ? SDL_WINDOW_FULLSCREEN : 0);
}

void platform_display_set_title(const char* title) {
    SDL_SetWindowTitle(window, title);
}

void platform_mouse_set_grabbed(bool grabbed) {
    SDL_SetRelativeMouseMode(grabbed);
}

bool platform_mouse_get_grabbed(void) {
    return SDL_GetRelativeMouseMode();
}
