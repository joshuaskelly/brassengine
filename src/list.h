/**
 * @file list.h
 * Linked list implementation.
 */

#ifndef LIST_H
#define LIST_H

#include <stdbool.h>
#include <stdint.h>

typedef struct list_node {
    void* item;
    struct list_node* next;
    struct list_node* prev;
} list_node_t;

typedef struct list {
    list_node_t* head;
    list_node_t* tail;
    size_t count;
} list_t;

/**
 * Creates an empty list.
 */
list_t* list_new(void);

/**
 * Frees a list.
 *
 * @param list List to free.
 */
void list_free(list_t* list);

/**
 * Adds an item to the end of the list.
 *
 * @param list The list to add to.
 * @param entry The entry to be added.
 */
void list_add(list_t* list, void* item);

/**
 * Removes an item from the list.
 *
 * @param list List to remove from.
 * @param item Item to remove.
 */
void list_remove(list_t* list, void* item);

/**
 * Gets first item in list.
 *
 * @param list List to get from.
 * @return void* First item.
 */
void* list_first(list_t* list);

/**
 * Gets last item in list.
 *
 * @param list List to get from.
 * @return void* Last item.
 */
void* list_last(list_t* list);

/**
 * Calls the given function on each item in list.
 *
 * @param list The list to iterate over.
 * @param func The callback function to call on each item.
 */
void list_for_each(list_t* list, void(func)(void*));

/**
 * Sorts list using given comparison function.
 *
 * @param list List to sort.
 * @param comp Comparison function. Returns a negative number if the first item
 * is less than the second item, a positive number if the first item is greater
 * than the second item, and zero otherwise.
 */
void list_sort(list_t* list, int(comp)(const void*, const void*));

/**
 * Copy contents of list into given array.
 *
 * @param list List to copy from.
 * @param array Array to copy to. The array should be correctly sized.
 */
void list_to_array(list_t* list, void* array[]);

typedef struct list_iterator {
    list_t* list;
    list_node_t* current;
} list_iterator_t;

/**
 * Creates a list iterator.
 *
 * @param list List to get iterator for.
 * @return list_iterator_t*
 */
list_iterator_t* list_iterator_new(list_t* list);

/**
 * Frees a list iterator.
 *
 * @param iterator Iterator to free.
 */
void list_iterator_free(list_iterator_t* iterator);

/**
 * Gets first item.
 *
 * @param iterator Iterator to get item from.
 * @return void* First item.
 */
void* list_iterator_begin(list_iterator_t* iterator);

/**
 * Checks if iterator is done.
 *
 * @param iterator Iterator to check.
 * @return true If iteration is complete, false otherwise.
 */
bool list_iterator_done(list_iterator_t* iterator);

/**
 * Gets next item.
 *
 * @param iterator Iterator to get next from.
 * @return void* Next item.
 */
void* list_iterator_next(list_iterator_t* iterator);

#endif
