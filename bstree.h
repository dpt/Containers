/* bstree.h -- associative array implemented as binary search tree */

#ifndef BSTREE_H
#define BSTREE_H

#include <stdio.h>

#include "errors.h"
#include "item.h"

/* ----------------------------------------------------------------------- */

#define T bstree_t

typedef struct bstree T;

/* ----------------------------------------------------------------------- */

/* Compare two keys (as for qsort). */
typedef int (bstree_compare)(const void *a, const void *b);

/* Destroy the specified key. */
typedef void (bstree_destroy_key)(void *key);

/* Destroy the specified value. */
typedef void (bstree_destroy_value)(void *value);

/* As in the hash library, if NULL is passed in for the compare or destroy
 * functions when a malloc'd string is assumed.
 *
 * Keys and values passed in (e.g. 'default_value' here, 'key' and 'value' to
 * bstree_insert below) are then owned by this data structure.
 *
 * NULL can be passed in for compare, destroy_key and destroy_value to use
 * default routines suitable for strings.
 */
error bstree_create(const void            *default_value,
                    bstree_compare        *compare,
                    bstree_destroy_key    *destroy_key,
                    bstree_destroy_value  *destroy_value,
                    T                    **t);
void bstree_destroy(T *t);

/* ----------------------------------------------------------------------- */

const void *bstree_lookup(T *t, const void *key);

error bstree_insert(T *t, const void *key, size_t keylen, const void *value);

void bstree_remove(T *t, const void *key);

const item_t *bstree_select(T *t, int k);

int bstree_count(T *t);

/* ----------------------------------------------------------------------- */

typedef error (bstree_found_callback)(const item_t *item,
                                      void         *opaque);

error bstree_lookup_prefix(const T               *t,
                           const void            *prefix,
                           size_t                 prefixlen,
                           bstree_found_callback *cb,
                           void                  *opaque);

/* ----------------------------------------------------------------------- */

typedef unsigned int bstree_walk_flags;

#define bstree_WALK_ORDER_MASK (3u << 0)
#define bstree_WALK_IN_ORDER   (0u << 0)
#define bstree_WALK_PRE_ORDER  (1u << 0)
#define bstree_WALK_POST_ORDER (2u << 0)

#define bstree_WALK_LEAVES     (1u << 2)
#define bstree_WALK_BRANCHES   (1u << 3)
#define bstree_WALK_ALL        (bstree_WALK_LEAVES | bstree_WALK_BRANCHES)

typedef error (bstree_walk_callback)(const item_t *item,
                                     int           level,
                                     void         *opaque);

error bstree_walk(const T              *t,
                  bstree_walk_flags     flags,
                  bstree_walk_callback *cb,
                  void                 *opaque);

/* ----------------------------------------------------------------------- */

/* To dump the data meaningfully bstree_show must call back to the client to
 * get the opaque keys and values turned into printable strings. These
 * strings may or may not be dynamically allocated so bstree_show_destroy is
 * provided to destroy them once finished with. */

typedef const char *(bstree_show_key)(const void *key);
typedef const char *(bstree_show_value)(const void *value);
typedef void (bstree_show_destroy)(char *doomed);

error bstree_show(const T             *t,
                  bstree_show_key     *key,
                  bstree_show_destroy *key_destroy,
                  bstree_show_value   *value,
                  bstree_show_destroy *value_destroy,
                  FILE                *f);

/* ----------------------------------------------------------------------- */

#undef T

#endif /* BSTREE_H */
