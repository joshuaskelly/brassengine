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
#include "state.h"
#include "teststate.h"
#include "time.h"

static state_t current_state;
static bool is_running = true;

static void handle_events(void);

void core_init(void) {
    console_init();

    log_info("%s", ENGINE_COPYRIGHT);

    configuration_init();
    time_init();
    platform_init();
    graphics_init();
    assets_init();
    script_init();
    input_init();

    // Set initial state
    core_set_state(&test_state);
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
    current_state.update();
    current_state.draw();
    platform_draw();
    time_update();
}

void core_reload(void) {
    time_reload();
    assets_reload();
    script_reload();
}

void core_set_state(state_t* state) {
    current_state = *state;
    current_state.init();
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

        // Current state handles all others
        current_state.handle_event(&event);
    }
}
