#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "console.h"
#include "event.h"
#include "graphics.h"
#include "log.h"
#include "script.h"
#include "time.h"

#include "renderers/draw.h"

static char input[2048] = "";
static char last_key;
static double last_time = 0;
static bool shift_down = false;

static void execute(void);

void console_init(void) {
}

char get_char(key_event_t* key) {
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
    }
}

bool handle_key_down(event_t* event) {
    switch (event->key.code) {
        case KEYCODE_BACKSPACE: {
            size_t length = strlen(input);
            if (length > 0) {
                input[length - 1] = '\0';
            }

            return true;
        }

        case KEYCODE_RETURN: {
            execute();
            return true;
        }

        case KEYCODE_LSHIFT:
        case KEYCODE_RSHIFT: {
            shift_down = true;
            return true;
        }

        default:
            break;
    }

    char c = get_char(&event->key);
    strcat(input, &c);

    return true;
}

bool handle_key_up(event_t* event) {
    switch (event->key.code) {
        case KEYCODE_LSHIFT:
        case KEYCODE_RSHIFT: {
            shift_down = false;
            return true;
        }

        default:
            break;
    }

    return false;
}

bool console_handle_event(event_t* event) {
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
    texture_t* render_texture = graphics_get_render_texture();
    graphics_texture_clear(render_texture, 0);

    draw_text("> ", 0, 0);
    draw_text(input, 16, 0);

    // Draw cursor
    bool show_cursor = (int)time_since_init() % 500 > 250;
    draw_text(show_cursor ? "\xdb" : " ", strlen(input) * 8 + 16, 0);
}

static void execute(void) {
    script_evaluate(input);
    input[0] = '\0';
}