/**
 * @file core.h
 * Core engine module. Responsible for managing all engine subsytems and
 * the application lifecycle.
 */

#ifndef CORE_H
#define CORE_H

#define ENGINE_VERSION_MAJOR "0"
#define ENGINE_VERSION_MINOR "18"
#define ENGINE_VERSION_PATCH "0"

#define ENGINE_VERSION "Brass Engine " ENGINE_VERSION_MAJOR "." ENGINE_VERSION_MINOR "." ENGINE_VERSION_PATCH
#define ENGINE_COPYRIGHT ENGINE_VERSION "  Copyright (C) 2023 Joshua Skelton"

/**
 * Initialize all engine systems
 */
void core_init(void);

/**
 * Destroy all engine systems
 */
void core_destroy(void);

/**
 * Run engine
 */
void core_run(void);

/**
 * Engine main loop
 */
void core_main_loop(void);

/**
 * Reload engine systems
 */
void core_reload(void);

#endif
