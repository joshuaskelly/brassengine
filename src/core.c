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
    assets_init();
    platform_init();
    time_init();
    graphics_init();
    input_init();
    script_init();

    log_info(" ");
}

void core_destroy(void) {
    input_destroy();
    script_destroy();
    platform_destroy();
    assets_destroy();
    graphics_destroy();
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
    platform_reload();
    assets_reload();
    graphics_reload();
    script_reload();

    log_info(" ");
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

        if (console_handle_event(&event)) return;

        input_handle_event(&event);
    }
}
