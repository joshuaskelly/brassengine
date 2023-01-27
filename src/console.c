#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "circular_buffer.h"
#include "configuration.h"
#include "console.h"
#include "event.h"
#include "graphics.h"
#include "input.h"
#include "log.h"
#include "math.h"
#include "script.h"
#include "time.h"

#include "renderers/draw.h"

#define COMMAND_MAX_LENGTH 2048
static char command[COMMAND_MAX_LENGTH + 1] = "";

static circular_buffer_t* input;
static circular_buffer_t* output;
static int input_line = 0;
static int cursor_offset = 0;
static int history_offset = 0;

static bool visible = true;

static void execute(void);

void console_init(void) {
    output = circular_buffer_new(80);
    input = circular_buffer_new(10);
}

void console_destroy(void) {
    circular_buffer_free(input);
    circular_buffer_free(output);
}

char get_char(key_event_t* key) {
    bool shift_down = input_keyboard_is_key_code_down(KEYCODE_LSHIFT) || input_keyboard_is_key_code_down(KEYCODE_RSHIFT);

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

static void load_input_history(void) {
    char* s = circular_buffer_get(
        input,
        input->count - input_line
    );

    strncpy(command, s, strlen(s));
    command[strlen(s)] = '\0';
}

bool handle_key_down(event_t* event) {
    const size_t command_length = strlen(command);
    const int max_lines = (config->resolution.height / 2) / 8 - 1;

    switch (event->key.code) {
        case KEYCODE_BACKSPACE: {
            if (command_length <= 0) return true;

            int i = command_length + cursor_offset;
            memmove(command + i - 1, command + i, -cursor_offset + 1);

            return true;
        }

        case KEYCODE_RETURN: {
            execute();
            return true;
        }

        case KEYCODE_PAGEDOWN: {
            if (output->count > max_lines) {
                history_offset = min(history_offset + 1, 0);
            }

            return true;
        }

        case KEYCODE_PAGEUP: {
            if (output->count > max_lines) {
                history_offset--;

                if (max_lines - history_offset > output->count) {
                    history_offset = -(output->count - max_lines);
                }
            }

           return true;
        }

        case KEYCODE_UP: {
            if (input->count == 0) return true;

            input_line = min(input_line + 1, input->count);
            load_input_history();
            return true;
        }

        case KEYCODE_DOWN: {
            if (input->count == 0) return true;

            input_line = max(input_line - 1, 1);
            load_input_history();
            return true;
        }

        case KEYCODE_LEFT: {
            cursor_offset = max(cursor_offset - 1, -(int)command_length);
            return true;
        }

        case KEYCODE_RIGHT: {
            cursor_offset = min(cursor_offset + 1, 0);
            return true;
        }

        default:
            break;
    }

    if (command_length < COMMAND_MAX_LENGTH) {
        char c = get_char(&event->key);
        int i = command_length + cursor_offset;
        memmove(command + i + 1, command + i, -cursor_offset + 1);
        command[i] = c;
    }

    return true;
}

bool handle_key_up(event_t* event) {
    return false;
}

bool console_handle_event(event_t* event) {
    if (!visible) return false;

    switch (event->type) {
        case EVENT_KEYDOWN:
            return handle_key_down(event);

        case EVENT_KEYUP:
            return handle_key_up(event);

        default:
            break;
    }

    return true;
}

void console_update(void) {
}

void console_draw(void) {
    if (!visible) return;

    // Get palette
    color_t* palette = graphics_draw_palette_get();
    color_t background = palette[0];
    color_t foreground = palette[1];
    palette[0] = config->console.colors.background;
    palette[1] = config->console.colors.foreground;

    rect_t console_rect = {
        0,
        0,
        config->resolution.width,
        (config->resolution.height / 2) / 8 * 8
    };

    draw_filled_rectangle(
        console_rect.x,
        console_rect.y,
        console_rect.width,
        console_rect.height,
        config->console.colors.background
    );

    graphics_set_clipping_rectangle(&console_rect);

    int line = 0;
    const int max_lines = (console_rect.height / 8) - 1;

    // Draw console history
    if (output->count > 0) {
        int lines_to_draw = min(output->count, max_lines);

        const int history_begin = output->count - lines_to_draw + history_offset;
        const int history_end = output->count + history_offset;
        for (int i = history_begin; i < history_end; i++)  {
            char* s = circular_buffer_get(output, i);
            draw_text(s, 0, line * 8);
            line++;
        }
    }

    // Draw prompt
    int prompt_length = strlen(config->console.prompt);
    draw_text(config->console.prompt, 0, line * 8);

    // Draw input string
    draw_text(command, prompt_length * 8, line * 8);

    // Draw cursor
    palette[1] = config->console.colors.cursor;
    bool show_cursor = (int)time_since_init() % 500 > 250;
    if (show_cursor) {
        draw_text("\xdb", strlen(command) * 8 + prompt_length * 8 + cursor_offset * 8, line * 8);
    }

    // Restore palette
    palette[0] = background;
    palette[1] = foreground;

    graphics_set_clipping_rectangle(NULL);
}

void console_buffer_write(const char* line) {
    char* s = (char*)malloc(sizeof(char) * strlen(line) + 1);
    strncpy(s, line, strlen(line));
    s[strlen(line)] = '\0';

    circular_buffer_add(output, s);
}

void console_buffer_clear(void) {
    circular_buffer_clear(output);
    history_offset = 0;
}

void console_buffer_toggle(void) {
    visible = !visible;
}

static void save_command_to_input_buffer(void) {
    size_t size = strlen(command);
    char* s = malloc(sizeof(char) * (size + 1));
    strncpy(s, command, size);
    s[size] = '\0';

    circular_buffer_add(input, s);
}

static void execute(void) {
    // Echo command
    log_info("%s%s", "> ", command);

    // Evaluate command
    script_evaluate(command);

    // Save command
    save_command_to_input_buffer();

    // Reset input history position
    input_line = 0;
    cursor_offset = 0;
    history_offset = 0;

    // Clear command
    command[0] = '\0';
}
