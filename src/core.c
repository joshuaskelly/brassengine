#include <stdbool.h>

#include "assets.h"
#include "configuration.h"
#include "console.h"
#include "core.h"
#include "event.h"
#include "graphics.h"
#include "input.h"
#include "log.h"
#include "platform.h"
#include "script.h"
#include "time.h"

static bool is_running = true;

static void handle_events(void);

void core_init(void) {
    console_init();

    log_info("%s", ENGINE_COPYRIGHT);

    configuration_init();
    time_init();
    assets_init();
    platform_init();
    graphics_init();
    input_init();
    script_init();
}

void core_destroy(void) {
    input_destroy();
    script_destroy();
    assets_destroy();
    graphics_destroy();
    platform_destroy();
    time_destroy();
    configuration_destroy();
    console_destroy();
}

void core_run(void) {
    while (is_running) {
        core_main_loop();
    }
}

void core_main_loop(void) {
    platform_update();
    input_update();
    handle_events();
    script_update();
    console_update();

    script_draw();
    console_draw();
    platform_draw();

    time_update();
}

void core_reload(void) {
    time_reload();
    assets_reload();
    script_reload();
}

/**
 * Process all events in the event queue.
 */
static void handle_events() {
    event_t event;

    while (event_poll(&event)) {
        // Core handles quit event
        if (event.type == EVENT_QUIT) {
            is_running = false;
            continue;
        }

        if (event.type == EVENT_RELOAD) {
            core_reload();
            continue;
        }

        input_handle_event(&event);

        if (event.type == EVENT_KEYDOWN) {
            if (event.key.code == KEYCODE_GRAVE) {
                console_buffer_toggle();
                return;
            }
        }

        if (console_handle_event(&event)) return;

        script_handle_event(&event);
    }
}
