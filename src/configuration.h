/**
 * @file configuration.h
 * Configuration module.
 */

#ifndef CONFIGURATION_H
#define CONFIGURATION_H

/**
 * Initialize config system. Called once during application start.
 */
void configuration_init(void);

/**
 * Destroy config system. Called once during application shutdown.
 */
void configuration_destroy(void);

int configuration_resolution_width_get(void);

int configuration_resolution_height_get(void);

#endif