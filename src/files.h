/**
 * @file files.h
 * File module. Responsible for file access. Knows how to work with zip files
 * and asset directories.
 */

#ifndef FILES_H
#define FILES_H

#include <stdbool.h>
#include <stdio.h>

/**
 * Opens a file from inside asset directory or zip file.
 *
 * @param filename Name of file to open
 * @param mode File access mode
 * @return FILE* File stream pointer if successful, NULL otherwise.
 */
FILE* files_open(const char* filename, const char* mode);

/**
 * Opens and reads entire file as a string.
 *
 * @param filename Name of file to open
 * @return const char* File content if successful, NULL otherwise.
 */
const char* files_read(const char* filename);

/**
 * Check filename extension.
 *
 * @param filename Filename to check
 * @param ext Extension to look for
 * @return true if match, false otherwise
 */
bool files_check_extension(const char* filename, const char* ext);

/**
 * Recursively walk given directory and invoke callback on all files.
 *
 * @param directory Directory to walk
 * @param callback Callback function to invoke on files
 */
void files_walk_directory(char* directory, void(callback)(const char*));

#endif
