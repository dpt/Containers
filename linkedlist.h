/* linkedlist.h -- associative array implemented as ordered linked list */

#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <stdio.h>

#include "errors.h"
#include "item.h"

/* ----------------------------------------------------------------------- */

#define T linkedlist_t

typedef struct linkedlist T;

/* ----------------------------------------------------------------------- */

/* Compare two keys (as for qsort). */
typedef int (linkedlist_compare)(const void *a, const void *b);

/* Destroy the specified key. */
typedef void (linkedlist_destroy_key)(void *key);

/* Destroy the specified value. */
typedef void (linkedlist_destroy_value)(void *value);

/* As in the hash library, if NULL is passed in for the compare or destroy
 * functions when a malloc'd string is assumed.
 *
 * Keys and values passed in (e.g. 'default_value' here, 'key' and 'value' to
 * linkedlist_insert below) are then owned by this data structure.
 *
 * NULL can be passed in for compare, destroy_key and destroy_value to
 * use default routines suitable for strings.
 */
error linkedlist_create(const void                *default_value,
                        linkedlist_compare        *compare,
                        linkedlist_destroy_key    *destroy_key,
                        linkedlist_destroy_value  *destroy_value,
                        T                        **t);
void linkedlist_destroy(T *t);

/* ----------------------------------------------------------------------- */

const void *linkedlist_lookup(T *t, const void *key);

error linkedlist_insert(T *t, const void *key, const void *value);

void linkedlist_remove(T *t, const void *key);

const item_t *linkedlist_select(T *t, int k);

int linkedlist_count(T *t);

/* ----------------------------------------------------------------------- */

typedef error (linkedlist_walk_callback)(const void *key,
                                         const void *value,
                                         void       *opaque);

error linkedlist_walk(const T                  *t,
                      linkedlist_walk_callback *cb,
                      void                     *opaque);

/* ----------------------------------------------------------------------- */

/* To dump the data meaningfully linkedlist_show must call back to the
 * client to get the opaque keys and values turned into printable strings.
 * These strings may or may not be dynamically allocated so
 * linkedlist_show_destroy is provided to destroy them once finished with.
 * */

typedef const char *(linkedlist_show_key)(const void *key);
typedef const char *(linkedlist_show_value)(const void *value);
typedef void (linkedlist_show_destroy)(char *doomed);

error linkedlist_show(const T                 *t,
                      linkedlist_show_key     *key,
                      linkedlist_show_destroy *key_destroy,
                      linkedlist_show_value   *value,
                      linkedlist_show_destroy *value_destroy,
                      FILE                    *f);

/* ----------------------------------------------------------------------- */

#undef T

#endif /* LINKEDLIST_H */
