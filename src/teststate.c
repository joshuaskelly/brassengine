#include "teststate.h"
#include "log.h"
#include "script.h"

void test_init(void) {
    log_info("test state init\n");

    script_setup();
}

bool test_handle_event(event_t* event) {
    switch (event->type) {
        case EVENT_KEYDOWN:
            if (event->key.code == KEYCODE_ESCAPE) {
                event_t quit_event;
                quit_event.type = EVENT_QUIT;
                event_post(&quit_event);
                return true;
            }
            break;

        default:
            break;
    }

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
