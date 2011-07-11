/* critbit.h -- associative array implemented as critbit tree */

/* A critbit tree is a form of trie where one-way branching is replaced with
 * a bit index. */

#ifndef CRITBIT_H
#define CRITBIT_H

#include <stdio.h>

#include "types.h"
#include "item.h"

/* ----------------------------------------------------------------------- */

#define T critbit_t

typedef struct critbit T;

/* ----------------------------------------------------------------------- */

/* Compare two keys (as for qsort). */
typedef int (critbit_compare)(const void *a, const void *b);

/* Destroy the specified key. */
typedef void (critbit_destroy_key)(void *key);

/* Destroy the specified value. */
typedef void (critbit_destroy_value)(void *value);

/* As in the hash library, if NULL is passed in for the compare or destroy
 * functions when a malloc'd string is assumed.
 *
 * Keys and values passed in (e.g. 'default_value' here, 'key' and 'value' to
 * critbit_insert below) are then owned by this data structure.
 *
 * NULL can be passed in for bit, compare, destroy_key and destroy_value to
 * use default routines suitable for strings.
 */
error critbit_create(const void            *default_value,
                     critbit_compare       *compare,
                     critbit_destroy_key   *destroy_key,
                     critbit_destroy_value *destroy_value,
                     T                     **t);
void critbit_destroy(T *t);

/* ----------------------------------------------------------------------- */

const void *critbit_lookup(const T *t, const void *key, size_t keylen);

error critbit_insert(T          *t,
                     const void *key,
                     size_t      keylen,
                     const void *value);

void critbit_remove(T *t, const void *key, size_t keylen);

const item_t *critbit_select(T *t, int k);

int critbit_count(T *t);

/* ----------------------------------------------------------------------- */

typedef unsigned int critbit_walk_flags;

#define critbit_WALK_ORDER_MASK (3u << 0)
#define critbit_WALK_IN_ORDER   (0u << 0)
#define critbit_WALK_PRE_ORDER  (1u << 0)
#define critbit_WALK_POST_ORDER (2u << 0)

#define critbit_WALK_LEAVES     (1u << 2)
#define critbit_WALK_BRANCHES   (1u << 3)
#define critbit_WALK_ALL        (critbit_WALK_LEAVES | critbit_WALK_BRANCHES)

typedef error (critbit_walk_callback)(const void *key,
                                      const void *value,
                                      int         level,
                                      void       *opaque);

error critbit_walk(const T               *t,
                   critbit_walk_flags     flags,
                   critbit_walk_callback *cb,
                   void                  *opaque);

/* ----------------------------------------------------------------------- */

/* To dump the data meaningfully critbit_show must call back to the client to
 * get the opaque keys and values turned into printable strings. These
 * strings may or may not be dynamically allocated so critbit_show_destroy is
 * provided to destroy them once finished with. */

typedef const char *(critbit_show_key)(const void *key);
typedef const char *(critbit_show_value)(const void *value);
typedef void (critbit_show_destroy)(char *doomed);

error critbit_show(const T              *t,
                   critbit_show_key     *key,
                   critbit_show_destroy *key_destroy,
                   critbit_show_value   *value,
                   critbit_show_destroy *value_destroy,
                   FILE                 *f);

/* ----------------------------------------------------------------------- */

/* Dump in format which can be fed into Graphviz. */
error critbit_show_viz(const T              *t,
                       critbit_show_key     *key,
                       critbit_show_destroy *key_destroy,
                       critbit_show_value   *value,
                       critbit_show_destroy *value_destroy,
                       FILE                 *f);

/* ----------------------------------------------------------------------- */

#undef T

#endif /* CRITBIT_H */
