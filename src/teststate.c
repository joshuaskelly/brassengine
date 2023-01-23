#include "console.h"
#include "log.h"
#include "script.h"
#include "teststate.h"

void test_init(void) {
    script_setup();
    console_init();
}

bool test_handle_event(event_t* event) {
    if (console_handle_event(event)) return true;

    return script_handle_event(event);
}

void test_update(void) {
    script_update();
    console_update();
}

void test_draw(void) {
    script_draw();
    console_draw();
}

state_t test_state = {
    test_init,
    test_handle_event,
    test_update,
    test_draw
};
