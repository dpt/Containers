/* orderedarray.h -- associative array implemented as ordered array */

#ifndef ORDEREDARRAY_H
#define ORDEREDARRAY_H

#include <stdio.h>

#include "errors.h"
#include "utils.h"
#include "item.h"

/* ----------------------------------------------------------------------- */

#define T orderedarray_t

typedef struct orderedarray T;

/* ----------------------------------------------------------------------- */

/* Compare two keys (as for qsort). */
typedef int (orderedarray_compare)(const void *a, const void *b);

/* Destroy the specified key. */
typedef void (orderedarray_destroy_key)(void *key);

/* Destroy the specified value. */
typedef void (orderedarray_destroy_value)(void *value);

/* As in the hash library, if NULL is passed in for the compare or destroy
 * functions when a malloc'd string is assumed.
 *
 * Keys and values passed in (e.g. 'default_value' here, 'key' and 'value' to
 * orderedarray_insert below) are then owned by this data structure.
 *
 * NULL can be passed in for compare, destroy_key and destroy_value to
 * use default routines suitable for strings.
 */
error orderedarray_create(const void                  *default_value,
                          orderedarray_compare        *compare,
                          orderedarray_destroy_key    *destroy_key,
                          orderedarray_destroy_value  *destroy_value,
                          T                          **t);
void orderedarray_destroy(T *t);

/* ----------------------------------------------------------------------- */

const void *orderedarray_lookup(T *t, const void *key);

error orderedarray_insert(T *t, const void *key, const void *value);

void orderedarray_remove(T *t, const void *key);

const item_t *orderedarray_select(T *t, int k);

int orderedarray_count(T *t);

/* ----------------------------------------------------------------------- */

typedef error (orderedarray_walk_callback)(const void *key,
                                           const void *value,
                                           void       *opaque);

error orderedarray_walk(const T                    *t,
                        orderedarray_walk_callback *cb,
                        void                       *opaque);

/* ----------------------------------------------------------------------- */

/* To dump the data meaningfully orderedarray_show must call back to the
 * client to get the opaque keys and values turned into printable strings.
 * These strings may or may not be dynamically allocated so
 * orderedarray_show_destroy is provided to destroy them once finished with.
 * */

typedef const char *(orderedarray_show_key)(const void *key);
typedef const char *(orderedarray_show_value)(const void *value);
typedef void (orderedarray_show_destroy)(char *doomed);

error orderedarray_show(const T                   *t,
                        orderedarray_show_key     *key,
                        orderedarray_show_destroy *key_destroy,
                        orderedarray_show_value   *value,
                        orderedarray_show_destroy *value_destroy,
                        FILE                      *f);

/* ----------------------------------------------------------------------- */

#undef T

#endif /* ORDEREDARRAY_H */
