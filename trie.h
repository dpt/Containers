/* trie.h -- associative array implemented as trie */

/* A trie uses the bits of the key to build a tree structure. It is used for
 * storing associative arrays where there are no identical keys nor keys
 * which are a prefix of others.
 *
 * It provides fast lookup, and the elements are stored sorted by key. */

#ifndef TRIE_H
#define TRIE_H

#include <stdio.h>

#include "types.h"
#include "item.h"

/* ----------------------------------------------------------------------- */

#define T trie_t

typedef struct trie T;

/* ----------------------------------------------------------------------- */

/* Compare two keys (as for qsort). */
typedef int (trie_compare)(const void *a, const void *b);

/* Destroy the specified key. */
typedef void (trie_destroy_key)(void *key);

/* Destroy the specified value. */
typedef void (trie_destroy_value)(void *value);

/* As in the hash library, if NULL is passed in for the compare or destroy
 * functions when a malloc'd string is assumed.
 *
 * Keys and values passed in (e.g. 'default_value' here, 'key' and 'value' to
 * trie_insert below) are then owned by this data structure.
 *
 * NULL can be passed in for bit, compare, destroy_key and destroy_value to
 * use default routines suitable for strings.
 */
error trie_create(const void          *default_value,
                  trie_compare        *compare,
                  trie_destroy_key    *destroy_key,
                  trie_destroy_value  *destroy_value,
                  T                  **t);
void trie_destroy(T *t);

/* ----------------------------------------------------------------------- */

const void *trie_lookup(T *t, const void *key, size_t keylen);

error trie_insert(T *t, const void *key, size_t keylen, const void *value);

void trie_remove(T *t, const void *key, size_t keylen);

const item_t *trie_select(T *t, int k);

int trie_count(T *t);

/* ----------------------------------------------------------------------- */

typedef unsigned int trie_walk_flags;

#define trie_WALK_ORDER_MASK (3u << 0)
#define trie_WALK_IN_ORDER   (0u << 0)
#define trie_WALK_PRE_ORDER  (1u << 0)
#define trie_WALK_POST_ORDER (2u << 0)

#define trie_WALK_LEAVES     (1u << 2)
#define trie_WALK_BRANCHES   (1u << 3)
#define trie_WALK_ALL        (trie_WALK_LEAVES | trie_WALK_BRANCHES)

typedef error (trie_walk_callback)(const void *key,
                                   const void *value,
                                   int         level,
                                   void       *opaque);

error trie_walk(const T            *t,
                trie_walk_flags     flags,
                trie_walk_callback *cb,
                void               *opaque);

/* ----------------------------------------------------------------------- */

/* To dump the data meaningfully trie_show must call back to the client to
 * get the opaque keys and values turned into printable strings. These
 * strings may or may not be dynamically allocated so trie_show_destroy is
 * provided to destroy them once finished with. */

typedef const char *(trie_show_key)(const void *key);
typedef const char *(trie_show_value)(const void *value);
typedef void (trie_show_destroy)(char *doomed);

error trie_show(const T           *t,
                trie_show_key     *key,
                trie_show_destroy *key_destroy,
                trie_show_value   *value,
                trie_show_destroy *value_destroy,
                FILE              *f);

/* ----------------------------------------------------------------------- */

/* Dump in format which can be fed into Graphviz. */
error trie_show_viz(const T           *t,
                    trie_show_key     *key,
                    trie_show_destroy *key_destroy,
                    trie_show_value   *value,
                    trie_show_destroy *value_destroy,
                    FILE              *f);

/* ----------------------------------------------------------------------- */

#undef T

#endif /* TRIE_H */
