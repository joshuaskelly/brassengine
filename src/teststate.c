#include "teststate.h"
#include "log.h"
#include "script.h"

void test_init(void) {
    script_setup();
}

bool test_handle_event(event_t* event) {
    return script_handle_event(event);
}

void test_update(void) {
    script_update();
}

void test_draw(void) {
    script_draw();
}

state_t test_state = {
    test_init,
    test_handle_event,
    test_update,
    test_draw
};
