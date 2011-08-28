/* trie.h -- associative array implemented as trie */

/* A trie uses the bits of the key to build a tree structure. It is used for
 * storing associative arrays where there are no identical keys nor keys
 * which are a prefix of others.
 *
 * It provides fast lookup, and the elements are stored sorted by key. */

#ifndef TRIE_H
#define TRIE_H

#include <stdio.h>

#include "errors.h"
#include "item.h"

/* ----------------------------------------------------------------------- */

#define T trie_t

typedef struct trie T;

/* ----------------------------------------------------------------------- */

/* Destroy the specified key. */
typedef void (trie_destroy_key)(void *key);

/* Destroy the specified value. */
typedef void (trie_destroy_value)(void *value);

/* As in the hash library, if NULL is passed in for the destroy function when
 * a malloc'd string is assumed.
 *
 * Keys and values passed in (e.g. 'default_value' here, 'key' and 'value' to
 * trie_insert below) are then owned by this data structure.
 *
 * NULL can be passed in for destroy_key and destroy_value to use default
 * routines suitable for strings.
 */
error trie_create(const void          *default_value,
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

typedef error (trie_found_callback)(const item_t *item,
                                    void         *opaque);

error trie_lookup_prefix(const T             *t,
                         const void          *prefix,
                         size_t               prefixlen,
                         trie_found_callback *cb,
                         void                *opaque);

/* ----------------------------------------------------------------------- */

typedef error (trie_walk_callback)(const item_t *item,
                                   int           level,
                                   void         *opaque);

error trie_walk(const T            *t,
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
