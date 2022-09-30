#include <stdbool.h>

#include "assets.h"
#include "core.h"
#include "event.h"
#include "graphics.h"
#include "input.h"
#include "log.h"
#include "platform.h"
#include "script.h"
#include "state.h"
#include "teststate.h"

static state_t current_state;
static bool is_running = true;

void handle_events(void);

void core_init(void) {
    log_info("%s\n", ENGINE_COPYRIGHT);

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
}

void core_run(void) {
    while (is_running) {
        core_main_loop();
    }
}

void core_main_loop(void) {
    platform_update();
    handle_events();
    current_state.update();
    current_state.draw();
    platform_draw();
}

void core_set_state(state_t* state) {
    current_state = *state;
    current_state.init();
}

/**
 * Process all events in the event queue.
 */
void handle_events() {
    event_t event;

    while (event_poll(&event)) {
        // Core handles quit event
        if (event.type == EVENT_QUIT) {
            is_running = false;
            continue;
        }

        input_handle_event(&event);

        // Current state handles all others
        current_state.handle_event(&event);
    }
}
