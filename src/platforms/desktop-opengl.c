#include <stdbool.h>
#include <string.h>

#include <pthread.h>

#include <SDL.h>
#include <SDL_mixer.h>
#include <GL/glew.h>
#include <SDL_opengl.h>
#include <SDL_error.h>

#include "../arguments.h"
#include "../configuration.h"
#include "../core.h"
#include "../event.h"
#include "../files.h"
#include "../graphics.h"
#include "../input.h"
#include "../log.h"
#include "../math.h"
#include "../platform.h"
#include "../sounds.h"
#include "../time.h"

#include "../modules/platforms/desktop.h"

#define FPS 60
#define FRAME_TIME_LENGTH (1000 / FPS)

static SDL_Window* window = NULL;
static uint32_t* render_buffer = NULL;
static int ticks_last_frame;
static SDL_Rect display_rect;

static char* fragment_shader_source = NULL;

#define OPENGL_VERSION_MAJOR 2
#define OPENGL_VERSION_MINOR 0

static GLuint shader_program;
static GLint position = -1;
static GLint texture_coordinates = -1;
static GLint screen_texture = -1;
static GLint output_size = -1;
static GLint frame_count = -1;

static GLuint vertex_buffer_object = 0;
static GLuint index_buffer_object = 0;
static GLuint texture = 0;

static Mix_Chunk chunks[MIX_CHANNELS];
static bool audio_disabled = false;

static void sdl_handle_events(void);
static void sdl_fix_frame_rate(void);
static void load_shader_program(void);

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
        "platform init (SDL %i.%i.%i, SDL Mixer %i.%i.%i, OpenGL ES %i.%i)",
        version.major, version.minor, version.patch,
        mix_version->major, mix_version->minor, mix_version->patch,
        OPENGL_VERSION_MAJOR, OPENGL_VERSION_MINOR
    );

    log_info(buffer);

    const int window_width = config->resolution.width * 3;
    const int window_height = config->resolution.height * 3;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER) != 0) {
        log_fatal("Error initializing SDL");
    }

    if (Mix_OpenAudioDevice(11025, AUDIO_U8, 1, 256, NULL, 0) < 0) {
        log_error("Error intializing SDL Mixer");
        log_error(SDL_GetError());
        log_info("Sound playback will be disabled");
        audio_disabled = true;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, OPENGL_VERSION_MAJOR);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, OPENGL_VERSION_MINOR);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);

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

    // Create vertex buffer object
    glGenBuffers(1, &vertex_buffer_object);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object);
    glBufferData(GL_ARRAY_BUFFER, 4 * 4 * sizeof(GLfloat), vertex_data, GL_STATIC_DRAW);

    // Create index buffer object
    glGenBuffers(1, &index_buffer_object);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_object);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof(GLuint), index_data, GL_STATIC_DRAW);

    // Create texture
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, config->resolution.width, config->resolution.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, render_buffer);

    // Set texture paramters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    display_rect.x = 0;
    display_rect.y = 0;
    display_rect.w = window_width;
    display_rect.h = window_height;

    SDL_ShowCursor(SDL_DISABLE);
}

void platform_destroy(void) {
    glDeleteProgram(shader_program);
    if (fragment_shader_source) free(fragment_shader_source);
    free(render_buffer);
    SDL_DestroyWindow(window);
    Mix_Quit();
    SDL_Quit();
}

void platform_reload(void) {
    glDeleteProgram(shader_program);
    if (fragment_shader_source) free(fragment_shader_source);
    load_shader_program();
    Mix_HaltChannel(-1);
    Mix_Volume(-1, MIX_MAX_VOLUME);
}

void platform_update(void) {
    sdl_handle_events();
    sdl_fix_frame_rate();
}

void platform_draw(void) {
    texture_t* render_texture = graphics_render_texture_get();
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
    float buffer_aspect = config->resolution.width / (float)config->resolution.height * config->display.aspect;

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

static void sdl_handle_events(void) {
    SDL_Event sdl_event;
    event_t event;
    SDL_GameController* controller = NULL;

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

            case SDL_MOUSEWHEEL:
                event.type = EVENT_MOUSEWHEEL;
                event.wheel.wheel_x = sdl_event.wheel.x;
                event.wheel.wheel_y = sdl_event.wheel.y;
                event_post(&event);
                break;

            case SDL_WINDOWEVENT_RESIZED:
            case SDL_WINDOWEVENT_SIZE_CHANGED:
                display_rect.w = sdl_event.window.data1;
                display_rect.h = sdl_event.window.data2;
                break;

            case SDL_CONTROLLERDEVICEADDED:
                controller = SDL_GameControllerOpen(sdl_event.cdevice.which);
                int id = SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(controller));
                input_controller_connect(id);
                break;

            case SDL_CONTROLLERDEVICEREMOVED:
                input_controller_disconnect(sdl_event.cdevice.which);
                SDL_GameControllerClose(SDL_GameControllerFromInstanceID(sdl_event.cdevice.which));
                break;

            case SDL_CONTROLLERBUTTONDOWN:
                event.type = EVENT_CONTROLLERBUTTONDOWN;
                event.controller_button.which = sdl_event.cdevice.which;
                event.controller_button.button = sdl_event.cbutton.button;
                event_post(&event);
                break;

            case SDL_CONTROLLERBUTTONUP:
                event.type = EVENT_CONTROLLERBUTTONUP;
                event.controller_button.which = sdl_event.cdevice.which;
                event.controller_button.button = sdl_event.cbutton.button;
                event_post(&event);
                break;

            case SDL_CONTROLLERAXISMOTION:
                event.type = EVENT_CONTROLLERAXISMOTION;
                event.controller_axis.which = sdl_event.cdevice.which;
                event.controller_axis.axis = sdl_event.caxis.axis;
                if (sdl_event.caxis.value < 0) {
                    event.controller_axis.value = sdl_event.caxis.value / 32768.0f;
                }
                else {
                    event.controller_axis.value = sdl_event.caxis.value / 32767.0f;
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

void platform_sound_play(sound_t* sound, int channel, bool looping) {
    if (audio_disabled) return;

    if (channel >= MIX_CHANNELS) {
        log_error("Error playing sound: channel %i does not exist", channel);
        return;
    }

    // Search for a free channel if channel not specified
    if (channel == -1) {
        for (int i = 0; i < MIX_CHANNELS; i++) {
            if (!Mix_Playing(i)) {
                channel = i;
                break;
            }
        }
    }

    if (channel == -1) {
        log_error("Error playing sound: no free channels available");
        return;
    }

    size_t size = sound->frame_count * sound->channel_count * sizeof(sample_t);

    // Configure chunk with sound data
    Mix_Chunk* chunk = &chunks[channel];
    chunk->allocated = 0;
    chunk->alen = size;
    chunk->abuf = (uint8_t*)sound->pcm;
    chunk->volume = 128;

    int loops = looping ? -1 : 0;

    Mix_PlayChannel(channel, chunk, loops);
}

void platform_sound_stop(int channel) {
    if (channel < -1 || channel >= MIX_CHANNELS) {
        log_error("Error stopping channel: channel %i does not exist", channel);
        return;
    }

    Mix_HaltChannel(channel);
}

void platform_sound_volume(int channel, float volume) {
    if (channel < -1 || channel >= MIX_CHANNELS) {
        log_error("Error stopping channel: channel %i does not exist", channel);
        return;
    }

    volume = clamp(volume, 0.0f, 1.0f);

    Mix_Volume(channel, volume * MIX_MAX_VOLUME);
}

/**
 * Log any diagnostic info for given shader.
 *
 * @param shader Shader object to get info for
 */
static void log_shader_info(GLuint shader) {
    if (!glIsShader(shader)) {
        return;
    }

    // Get log length
    GLint max_length = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &max_length);

    // Get log data
    char* log = (char*)malloc(sizeof(char) * max_length);
    GLint length = 0;
    glGetShaderInfoLog(shader, max_length, &length, log);

    if (length > 0) {
        // Trim extra newline
        if (log[length - 1] == '\n') {
            log[length - 1] = '\0';
        }

        log_error(log);
    }

    free(log);
}

/**
 * Log any diagnostic info for given program.
 *
 * @param program Program object to get info for
 */
static void log_program_info(GLuint program) {
    if (glIsProgram(program)) {
        return;
    }

    // Get log length
    GLint max_length = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &max_length);

    // Get log data
    char* log = (char*)malloc(sizeof(char) * max_length);
    GLint length = 0;
    glGetProgramInfoLog(program, max_length, &length, log);

    if (length > 0) {
        // Trim extra newline
        if (log[length - 1] == '\n') {
            log[length - 1] = '\0';
        }

        log_error(log);
    }

    free(log);
}

/**
 * Compile given shader and log diagnostic info if fails
 *
 * @param shader Shader object
 * @param source Source
 * @return true If compile was successful, false otherwise
 */
static bool compile_shader(GLuint shader, const GLchar* source) {
    const GLchar* source_array[] = {
        source
    };

    glShaderSource(shader, 1, source_array, NULL);
    glCompileShader(shader);

    GLint compile_success = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_success);
    if (compile_success != GL_TRUE) {
        log_error("Error compiling shader");
        log_shader_info(shader);

        return false;
    }

    return true;
}

static const char* default_fragment_shader =
    "#version 100\n"
    "precision mediump float;"
    "uniform sampler2D screen_texture;"
    "varying mediump vec2 uv;"
    "void main() {"
    "    gl_FragColor = texture2D(screen_texture, uv);"
    "}";

static const char* vertex_shader_source =
    "#version 100\n"
    "attribute vec2 position;"
    "attribute vec2 texture_coordinates;"
    "varying vec2 uv;"
    "void main() {"
    "    uv = texture_coordinates;"
    "    gl_Position = vec4(position.x, position.y, 0, 1);"
    "}";

static void load_shader_program(void) {
    shader_program = glCreateProgram();
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);

    // Vertex shader
    if (!compile_shader(vertex_shader, vertex_shader_source)) {
        log_fatal("Error compiling vertex shader");
    }

    glAttachShader(shader_program, vertex_shader);

    // Fragment shader
    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

    bool use_default = true;
    fragment_shader_source = (char*)files_read("shader.frag");
    if (fragment_shader_source) {
        use_default = !compile_shader(fragment_shader, fragment_shader_source);
    }

    if (use_default) {
        if (!compile_shader(fragment_shader, default_fragment_shader)) {
            log_fatal("Error compiling default fragment shader");
        }
    }

    glAttachShader(shader_program, fragment_shader);

    glLinkProgram(shader_program);

    GLint compile_success = GL_FALSE;
    glGetProgramiv(shader_program, GL_LINK_STATUS, &compile_success);
    if (compile_success != GL_TRUE) {
        log_program_info(shader_program);
        log_fatal("Error linking program");
    }

    position = glGetAttribLocation(shader_program, "position");
    texture_coordinates = glGetAttribLocation(shader_program, "texture_coordinates");
    screen_texture = glGetUniformLocation(shader_program, "screen_texture");
    output_size = glGetUniformLocation(shader_program, "output_size");
    frame_count = glGetUniformLocation(shader_program, "frame_count");
}

void platform_mouse_grabbed_set(bool grabbed) {
    SDL_SetRelativeMouseMode(grabbed);
}

bool platform_mouse_grabbed_get(void) {
    return SDL_GetRelativeMouseMode();
}

void platform_open_module(void* arg) {
    open_desktop_platform_module(arg, window);
}

struct thread {
    pthread_t pthread;
};

thread_t* platform_thread_new(void* (function)(void*), void* args) {
    thread_t* thread = (thread_t*)malloc(sizeof(thread_t));
    pthread_create(&thread->pthread, NULL, function, args);

    return thread;
}

void platform_thread_free(thread_t* thread) {
    if (thread == NULL) return;

    free(thread);
}

void platform_thread_detatch(thread_t* thread) {
    pthread_detach(thread->pthread);
}

void* platform_thread_join(thread_t* thread) {
    void* result = NULL;
    pthread_join(thread->pthread, result);
    free(thread);

    return result;
}

void platform_thread_exit(void* result) {
    pthread_exit(result);
}

struct thread_lock {
    pthread_mutex_t mutex;
};

thread_lock_t* platform_thread_lock_new(void) {
    thread_lock_t* lock = NULL;
    lock = (thread_lock_t*)malloc(sizeof(thread_lock_t));

    pthread_mutex_init(&lock->mutex, NULL);

    return lock;
}

void platform_thread_lock_free(thread_lock_t* lock) {
    free(lock);
}

void platform_thread_lock_lock(thread_lock_t* lock) {
    pthread_mutex_lock(&(lock->mutex));
}

void platform_thread_lock_unlock(thread_lock_t* lock) {
    pthread_mutex_unlock(&(lock->mutex));
}

struct thread_condition {
    pthread_cond_t cond;
};

thread_condition_t* platform_thread_condition_new(void) {
    thread_condition_t* condition = (thread_condition_t*)malloc(sizeof(thread_condition_t));

    pthread_cond_init(&(condition->cond), NULL);

    return condition;
}

void platform_thread_condition_free(thread_condition_t* condition) {
    if (condition == NULL) return;

    pthread_cond_destroy(&(condition->cond));
    free(condition);
}

void platform_thread_condition_wait(thread_condition_t* condition, thread_lock_t* lock) {
    pthread_cond_wait(&(condition->cond), &(lock->mutex));
}

void platform_thread_condition_notify(thread_condition_t* condition) {
    pthread_cond_signal(&(condition->cond));
}
