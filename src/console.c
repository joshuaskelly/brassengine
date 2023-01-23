#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "console.h"
#include "event.h"
#include "graphics.h"
#include "log.h"
#include "time.h"

#include "renderers/draw.h"

static char input[2048] = "";
static char last_key;
static double last_time = 0;

static void execute(void);

void console_init(void) {
}

bool console_handle_event(event_t* event) {
    if (event->type != EVENT_KEYDOWN) return false;

    if (event->key.code == KEYCODE_BACKSPACE) {
        size_t length = strlen(input);
        if (length > 0) {
            input[length - 1] = '\0';
        }

        return true;
    }

    if (event->key.code == KEYCODE_RETURN) {
        execute();
        return true;
    }

    strcat(input, &event->key.symbol);

    return true;
}

void console_update(void) {
}

void console_draw(void) {
    texture_t* render_texture = graphics_get_render_texture();
    graphics_texture_clear(render_texture, 0);

    draw_text("> ", 0, 0);
    draw_text(input, 16, 0);
}

static void execute(void) {
    log_info(input);
    input[0] = '\0';
}