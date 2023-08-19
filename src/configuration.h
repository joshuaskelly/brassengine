/**
 * @file configuration.h
 * Configuration module.
 */

#ifndef CONFIGURATION_H
#define CONFIGURATION_H

extern struct config {
    struct {
        int width;
        int height;
    } resolution;

    struct {
        float aspect;
    } display;

    struct {
        struct {
            int foreground;
            int background;
            int cursor;
        } colors;
        char* prompt;
    } console;
}* config;

/**
 * Initialize config system. Called once during application start.
 */
void configuration_init(void);

/**
 * Destroy config system. Called once during application shutdown.
 */
void configuration_destroy(void);

#endif
