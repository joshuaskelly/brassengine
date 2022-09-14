/**
 * @file log.h
 * Logging module. Provides convenience functions for sending output to
 * the console.
 */

#ifndef LOG_H
#define LOG_H

/**
 * Logs a message to stdout.
 *
 * @param message string to be displayed
 * @param ... Optional format specifiers
 */
void log_info(const char* message, ...);

/**
 * Logs a message to stderr.
 *
 * @param message string to be displayed
 * @param ... Optional format specifiers
 */
void log_error(const char* message, ...);

/**
 * Logs a message to stderr and immediately terminates program.
 *
 * @param message string to be displayed
 * @param ... Optional format specifiers
 */
void log_fatal(const char* message, ...);

#endif
