#ifndef LOG_H
#define LOG_H

/**
 * @brief Logs a message to stdout.
 *
 * @param message string to be displayed
 */
void log_info(const char* message);

/**
 * @brief Logs a message to stderr.
 *
 * @param message string to be displayed
 */
void log_error(const char* message);

/**
 * @brief Logs a message to stderr and immediately terminates program.
 *
 * @param message string to be displayed
 */
void log_fatal(const char* message);

#endif