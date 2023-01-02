#include <stdlib.h>

#include "list.h"
#include "log.h"

list_t* list_new(void) {
    list_t* list = (list_t*)malloc(sizeof(list_t));
    list->head = NULL;
    list->tail = NULL;
    list->count = 0;

    return list;
}

static void list_node_free(list_node_t* node) {
    node->item = NULL;
    node->next = NULL;
    node->prev = NULL;
    free(node);
    node = NULL;
}

void list_free(list_t* list) {
    // Empty list
    if (list->head == NULL) {
        free(list);
        list = NULL;

        return;
    }

    // Singleton list
    if (list->head == list->tail) {
        list_node_free(list->head);
        list->head = NULL;
        list->tail = NULL;
        list->count = 0;
        free(list);
        list = NULL;

        return;
    }

    // Two or more entries
    list_node_t* node = list->tail;
    for (int i = 0; i < list->count; i++) {
        list_node_t* next = node->prev;
        list_node_free(node);
        node = next;
    }

    list->head = NULL;
    list->tail = NULL;
    list->count = 0;
    free(list);
    list = NULL;
}

static list_node_t* list_node_new(void* item){
    list_node_t* node = (list_node_t*)malloc(sizeof(list_node_t));

    if (!node) {
        log_error("Failed to allocate memory for list element.");
        return NULL;
    }

    node->item = item;
    node->next = NULL;
    node->prev = NULL;

    return node;
}

void list_add(list_t* list, void* item) {
    list_node_t* node = list_node_new(item);

    // Empty list
    if (list->head == NULL) {
        list->head = node;
        list->tail = node;
        list->count = 1;

        return;
    }

    node->prev = list->tail;
    list->tail->next = node;
    list->tail = node;
    list->count++;
}

void list_remove(list_t* list, void* item) {
    // Remove first element
    if (list->head->item == item) {
        list_node_t* node = list->head;
        list->head = node->next;
        list->head->prev = NULL;
        list_node_free(node);
        list->count--;

        return;
    }

    // Remove last element
    if (list->tail->item == item) {
        list_node_t* node = list->tail;
        list->tail = node->prev;
        list->tail->next = NULL;
        list_node_free(node);
        list->count--;

        return;
    }

    list_node_t* node = list->head->next;
    for (int i = 1; i < list->count - 1; i++) {
        if (node->item == item) {
            node->prev->next = node->next;
            node->next->prev = node->prev;
            list_node_free(node);
            list->count--;

            return;
        }

        node = node->next;
    }
}

void* list_first(list_t* list) {
    return list->head->item;
}

void* list_last(list_t* list) {
    return list->tail->item;
}

void list_for_each(list_t* list, void(func)(void*)) {
    list_node_t* node = list->head;

    while (node) {
        func(node->item);
        node = node->next;
    }
}

/**
 * Inserts given node before the target.
 *
 * @param list List to insert into.
 * @param target Node to insert before.
 * @param node Node to insert.
 */
static void insert_before(list_t* list, list_node_t* target, list_node_t* node) {
    if (target == list->head) {
        list->head = node;
        node->next = target;
        node->prev = NULL;
        target->prev = node;
        list->count++;

        return;
    }

    node->prev = target->prev;
    node->next = target;
    target->prev->next = node;
    target->prev = node;
    list->count++;
}

/**
 * Removes node from list.
 *
 * @param list List to remove from.
 * @param node Node to remove.
 */
static void remove(list_t* list, list_node_t* node) {
    if (node == list->head) {
        list->head = node->next;
        list->head->prev = NULL;
        list->count--;

        return;
    }

    if (node == list->tail) {
        list->tail = node->prev;
        list->tail->next = NULL;
        list->count--;

        return;
    }

    node->prev->next = node->next;
    node->next->prev = node->prev;
    list->count--;
}

/**
 * Quicksort partition function. Picks the first node as the pivot and then
 * moves all items less than the pivot in front of the pivot.
 *
 * @param list List to sort in place.
 * @param comp Comparison function.
 * @param begin Fist list_node_t* in sublist.
 * @param end Last list_node_t* in sublist.
 * @return list_node_t* Pivot list_node_t*
 */
static list_node_t* partition(list_t* list, int(comp)(const void*, const void*), list_node_t* begin, list_node_t* end) {
    list_node_t* pivot = begin;
    list_node_t* current = begin->next;
    list_node_t* last = end->next;

    while (current != last) {
        list_node_t* next = current->next;

        if (comp(current->item, pivot->item) < 0) {
            remove(list, current);
            insert_before(list, pivot, current);
        }

        current = next;
    }

    return pivot;
}

/**
 * Recursive quicksort implementation.
 *
 * @param list List to sort in place.
 * @param comp Comparison function.
 * @param begin Fist list_node_t* in sublist.
 * @param end Last list_node_t* in sublist.
 */
static void quick_sort(list_t* list, int(comp)(const void*, const void*), list_node_t* begin, list_node_t* end) {
    if (!begin || !end) return;
    if (begin == end) return;
    if (end->next == begin) return;

    list_node_t* left_bound = begin->prev;
    list_node_t* right_bound = end->next;
    list_node_t* pivot = partition(list, comp, begin, end);

    quick_sort(list, comp, left_bound ? left_bound->next : list->head, pivot->prev);
    quick_sort(list, comp, pivot->next, right_bound ? right_bound->prev : list->tail);
}

void list_sort(list_t* list, int(comp)(const void*, const void*)) {
    if (list->count < 2) return;

    quick_sort(list, comp, list->head, list->tail);
}

void list_to_array(list_t* list, void* array[]) {
    list_node_t* current = list->head;
    for (int i = 0; i < list->count; i++) {
        array[i] = current->item;
        current = current->next;
    }
}

list_iterator_t* list_iterator_new(list_t* list) {
    list_iterator_t* iterator = (list_iterator_t*)malloc(sizeof(list_iterator_t));
    iterator->list = list;
    iterator->current = list->head;

    return iterator;
}

void list_iterator_free(list_iterator_t* iterator) {
    free(iterator);
    iterator = NULL;
}

void* list_iterator_begin(list_iterator_t* iterator) {
    return iterator->current->item;
}

bool list_iterator_done(list_iterator_t* iterator) {
    return iterator->current != NULL;
}

void* list_iterator_next(list_iterator_t* iterator) {
    iterator->current = iterator->current->next;

    if (iterator->current) {
        return iterator->current->item;
    }

    return NULL;
}
