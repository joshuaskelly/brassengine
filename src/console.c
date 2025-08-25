#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "configuration.h"
#include "console.h"
#include "event.h"
#include "graphics.h"
#include "log.h"
#include "script.h"
#include "time.h"
#include "collections/circular_buffer.h"

#define COMMAND_MAX_LENGTH 2048
static char command[COMMAND_MAX_LENGTH + 1] = "";

static circular_buffer_t* input;
static circular_buffer_t* output;
static int input_buffer_offset = 0;
static int output_buffer_offset = 0;
static int cursor_offset = 0;

static bool left_shift_down = false;
static bool right_shift_down = false;

static bool visible = false;

static void execute(void);
static void clear_input(void);
static void complete(void);

void console_init(void) {
    output = circular_buffer_new(80);
    input = circular_buffer_new(20);
}

void console_destroy(void) {
    circular_buffer_free(input);
    circular_buffer_free(output);
}

char get_char(key_event_t* key) {
    bool shift_down = left_shift_down || right_shift_down;

    switch (key->symbol) {
        case '`':
            return shift_down ?  '~' :  '`';
        case '1':
            return shift_down ?  '!' :  '1';
        case '2':
            return shift_down ?  '@' :  '2';
        case '3':
            return shift_down ?  '#' :  '3';
        case '4':
            return shift_down ?  '$' :  '4';
        case '5':
            return shift_down ?  '%' :  '5';
        case '6':
            return shift_down ?  '^' :  '6';
        case '7':
            return shift_down ?  '&' :  '7';
        case '8':
            return shift_down ?  '*' :  '8';
        case '9':
            return shift_down ?  '(' :  '9';
        case '0':
            return shift_down ?  ')' :  '0';
        case '-':
            return shift_down ?  '_' :  '-';
        case '=':
            return shift_down ?  '+' :  '=';
        case 'q':
            return shift_down ?  'Q' :  'q';
        case 'w':
            return shift_down ?  'W' :  'w';
        case 'e':
            return shift_down ?  'E' :  'e';
        case 'r':
            return shift_down ?  'R' :  'r';
        case 't':
            return shift_down ?  'T' :  't';
        case 'y':
            return shift_down ?  'Y' :  'y';
        case 'u':
            return shift_down ?  'U' :  'u';
        case 'i':
            return shift_down ?  'I' :  'i';
        case 'o':
            return shift_down ?  'O' :  'o';
        case 'p':
            return shift_down ?  'P' :  'p';
        case '[':
            return shift_down ?  '{' :  '[';
        case ']':
            return shift_down ?  '}' :  ']';
        case '\\':
            return shift_down ?  '|' :  '\\';
        case 'a':
            return shift_down ?  'A' :  'a';
        case 's':
            return shift_down ?  'S' :  's';
        case 'd':
            return shift_down ?  'D' :  'd';
        case 'f':
            return shift_down ?  'F' :  'f';
        case 'g':
            return shift_down ?  'G' :  'g';
        case 'h':
            return shift_down ?  'H' :  'h';
        case 'j':
            return shift_down ?  'J' :  'j';
        case 'k':
            return shift_down ?  'K' :  'k';
        case 'l':
            return shift_down ?  'L' :  'l';
        case ';':
            return shift_down ?  ':' :  ';';
        case '\'':
            return shift_down ?  '"' :  '\'';
        case 'z':
            return shift_down ?  'Z' :  'z';
        case 'x':
            return shift_down ?  'X' :  'x';
        case 'c':
            return shift_down ?  'C' :  'c';
        case 'v':
            return shift_down ?  'V' :  'v';
        case 'b':
            return shift_down ?  'B' :  'b';
        case 'n':
            return shift_down ?  'N' :  'n';
        case 'm':
            return shift_down ?  'M' :  'm';
        case ',':
            return shift_down ?  '<' :  ',';
        case '.':
            return shift_down ?  '>' :  '.';
        case '/':
            return shift_down ?  '?' :  '/';
        case ' ':
            return shift_down ?  ' ' :  ' ';
        default:
            break;
    }

    return '\0';
}

/**
 * Update current user command from current input history offset
 */
static void load_input_history(void) {
    char* s = circular_buffer_get(
        input,
        input->count - input_buffer_offset
    );

    strcpy(command, s);
    command[strlen(s)] = '\0';
}

static void scroll_up(void) {
    int height;
    graphics_resolution_get(NULL, &height);
    const int max_lines = (height / 2) / 8 - 1;

    if (output->count > max_lines) {
        output_buffer_offset--;

        if (max_lines - output_buffer_offset > output->count) {
            output_buffer_offset = -(output->count - max_lines);
        }
    }
}

static void scroll_down(void) {
    int height;
    graphics_resolution_get(NULL, &height);
    const int max_lines = (height / 2) / 8 - 1;

    if (output->count > max_lines) {
        output_buffer_offset = fminf(output_buffer_offset + 1, 0);
    }
}

/**
 * Handle key down events.
 *
 * @param event Event to handle.
 * @return true if handled, false otherwise
 */
static bool handle_key_down(event_t* event) {
    const size_t command_length = strlen(command);

    switch (event->key.code) {
        case KEYCODE_BACKSPACE: {
            // Don't allow backspacing beyond beginning of command
            if (command_length + cursor_offset <= 0) return true;

            int i = command_length + cursor_offset;
            memmove(command + i - 1, command + i, -cursor_offset + 1);

            return true;
        }

        case KEYCODE_DELETE: {
            if (command_length <= 0) return true;
            if (cursor_offset == 0) return true;

            int i = command_length + cursor_offset;
            memmove(command + i, command + i + 1, -cursor_offset + 1);
            cursor_offset += 1;

            return true;
        }

        case KEYCODE_TAB: {
            complete();
            return true;
        }

        case KEYCODE_RETURN: {
            execute();
            return true;
        }

        case KEYCODE_ESCAPE: {
            clear_input();
            return true;
        }

        case KEYCODE_PAGEDOWN: {
            scroll_down();
            return true;
        }

        case KEYCODE_PAGEUP: {
            scroll_up();
            return true;
        }

        case KEYCODE_UP: {
            if (input->count == 0) return true;

            input_buffer_offset = fminf(input_buffer_offset + 1, input->count);
            load_input_history();
            return true;
        }

        case KEYCODE_DOWN: {
            if (input->count == 0) return true;

            input_buffer_offset = fmaxf(input_buffer_offset - 1, 1);
            load_input_history();
            return true;
        }

        case KEYCODE_LEFT: {
            cursor_offset = fmaxf(cursor_offset - 1, -(int)command_length);
            return true;
        }

        case KEYCODE_RIGHT: {
            cursor_offset = fminf(cursor_offset + 1, 0);
            return true;
        }

        case KEYCODE_HOME: {
            cursor_offset = -strlen(command);
            return true;
        }

        case KEYCODE_END: {
            cursor_offset = 0;
            return true;
        }

        case KEYCODE_LSHIFT: {
            left_shift_down = true;
            return true;
        }

        case KEYCODE_RSHIFT: {
            right_shift_down = true;
            return true;
        }

        default:
            break;
    }

    if (command_length < COMMAND_MAX_LENGTH) {
        char c = get_char(&event->key);

        // Only insert printable characters into command
        if (c != '\0') {
            int i = command_length + cursor_offset;
            memmove(command + i + 1, command + i, -cursor_offset + 1);
            command[i] = c;
        }
    }

    return true;
}

static bool handle_mouse_wheel(event_t* event) {
    if (event->wheel.wheel_y == 0) {
        return false;
    }

    if (event->wheel.wheel_y > 0) {
        scroll_up();
    }
    else {
        scroll_down();
    }

    return true;
}

/**
 * Handle key up events.
 *
 * @param event Event to handle.
 * @return true if handled, false otherwise
 */
static bool handle_key_up(event_t* event) {
    switch (event->key.code) {
        case KEYCODE_LSHIFT: {
            left_shift_down = false;
            return true;
        }

        case KEYCODE_RSHIFT: {
            right_shift_down = false;
            return true;
        }

        default:
            break;
    }

    return false;
}

bool console_handle_event(event_t* event) {
    // Toggle console
    if (event->type == EVENT_KEYDOWN && event->key.code == KEYCODE_GRAVE) {
        console_buffer_toggle();
        return true;
    }

    // Don't handle events if console is hidden
    if (!visible) return false;

    switch (event->type) {
        case EVENT_KEYDOWN:
            return handle_key_down(event);

        case EVENT_KEYUP:
            return handle_key_up(event);

        case EVENT_MOUSEWHEEL:
            return handle_mouse_wheel(event);

        default:
            break;
    }

    return false;
}

void console_update(void) {
}

void console_draw(void) {
    if (!visible) return;

    // Preserve palette + transparent color
    color_t* palette = graphics_draw_palette_get();
    color_t background = palette[0];
    color_t foreground = palette[1];
    color_t transparent_color = graphics_transparent_color_get();

    // Set palette + transparent color
    palette[0] = config->console.colors.background;
    palette[1] = config->console.colors.foreground;
    graphics_transparent_color_set(config->console.colors.transparent);

    int width;
    int height;
    graphics_resolution_get(&width, &height);

    rect_t console_rect = {
        0,
        0,
        width,
        (height / 2) / 8 * 8
    };

    graphics_draw_filled_rectangle(
        console_rect.x,
        console_rect.y,
        console_rect.width,
        console_rect.height,
        config->console.colors.background
    );

    graphics_clipping_rectangle_set(&console_rect);

    int line = 0;
    const int max_lines = (console_rect.height / 8) - 1;

    // Draw console history
    if (output->count > 0) {
        int lines_to_draw = fminf(output->count, max_lines);

        const int history_begin = output->count - lines_to_draw + output_buffer_offset;
        const int history_end = output->count + output_buffer_offset;
        for (int i = history_begin; i < history_end; i++)  {
            char* s = circular_buffer_get(output, i);
            graphics_draw_text(s, 0, line * 8);
            line++;
        }
    }

    // Draw prompt
    int prompt_length = strlen(config->console.prompt);
    graphics_draw_text(config->console.prompt, 0, line * 8);

    // Draw input string
    graphics_draw_text(command, prompt_length * 8, line * 8);

    // Draw cursor
    palette[1] = config->console.colors.cursor;
    bool show_cursor = (int)time_since_init() % 500 > 250;
    if (show_cursor) {
        graphics_draw_text("\xdb", (strlen(command) + prompt_length + cursor_offset) * 8, line * 8);
    }

    // Restore palette + transparent color
    palette[0] = background;
    palette[1] = foreground;
    graphics_transparent_color_set(transparent_color);

    graphics_clipping_rectangle_set(NULL);
}

void console_buffer_write(const char* line) {
    // Copy line because
    char* s = (char*)malloc(sizeof(char) * strlen(line) + 1);
    strcpy(s, line);
    s[strlen(line)] = '\0';

    // Split on newlines
    char* token = strtok(s, "\n");
    while (token) {
        char* l = (char*)malloc(sizeof(char) * strlen(token) + 1);
        strcpy(l, token);
        l[strlen(token)] = '\0';

        circular_buffer_add(output, l);
        token = strtok(NULL, "\n");
    }

    free(s);
}

void console_buffer_clear(void) {
    circular_buffer_clear(output);
    output_buffer_offset = 0;
}

void console_buffer_toggle(void) {
    visible = !visible;
}

/**
 * Pushes current user command into input buffer.
 */
static void save_command_to_input_buffer(void) {
    size_t size = strlen(command);
    char* s = (char*)malloc(sizeof(char) * (size + 1));
    strcpy(s, command);
    s[size] = '\0';

    circular_buffer_add(input, s);
}

/**
 * Executes current user command.
 */
static void execute(void) {
    // Echo command
    log_info("%s%s", config->console.prompt, command);

    if (strlen(command) > 0) {
        // Evaluate command
        script_evaluate(command);

        // Save command
        save_command_to_input_buffer();
    }

    clear_input();
}

/**
 * Clears user input.
 */
static void clear_input(void) {
    // Reset input history position
    input_buffer_offset = 0;
    cursor_offset = 0;
    output_buffer_offset = 0;

    // Clear command
    command[0] = '\0';
}

static void complete(void) {
    script_complete(command);
    cursor_offset = 0;
    output_buffer_offset = 0;
}
