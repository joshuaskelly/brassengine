/**
 * @file arguments.h
 * @brief Handles CLI arguments given from user.
 */

#ifndef ARGUMENTS_H
#define ARGUMENTS_H

/**
 * @brief Set command line arguments.
 *
 * @param argc Argument count. Will always be at least one.
 * @param argv Argument vector. The command line arguments given from user.
 */
void arguments_set(int argc, char* argv[]);

/**
 * @brief Returns number of arguments given from user.
 */
int arguments_count(void);

/**
 * @brief Returns array of command line arguments given from user. First arg
 * is always the file path of this program.
 */
const char** arguments_vector(void);

/**
 * @brief Checks if arg was given by user.
 *
 * @param arg Arg to check.
 * @return Index of given arg if present, zero otherwise.
 */
int arguments_check(char* arg);

#endif
