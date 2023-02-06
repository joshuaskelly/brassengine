#include <stddef.h>

#include <sys/time.h>

#include "time.h"

static double startup_time = 0;
static double current_time = 0;
static double previous_time = 0;
static double delta_time = 0;

double time_millis_get(void) {
    struct timeval ts;
    gettimeofday(&ts, NULL);
    return (ts.tv_sec * 1000) + ((double)ts.tv_usec / 1000.0f);
}

void time_init(void) {
    startup_time = time_millis_get();
    current_time = time_millis_get();
}

void time_destroy(void) {

}

void time_update(void) {
    previous_time = current_time;
    current_time = time_millis_get();
    delta_time = current_time - previous_time;
}

void time_reload(void) {
    startup_time = time_millis_get();
    current_time = time_millis_get();
}

double time_delta_time(void) {
    return delta_time;
}

double time_since_init(void) {
    return current_time - startup_time;
}
