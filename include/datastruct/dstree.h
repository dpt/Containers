/* --------------------------------------------------------------------------
 *    Name: dstree.h
 * Purpose: Associative array implemented as digital search tree
 * ----------------------------------------------------------------------- */

/* A digital search tree (DST) uses the bits of the key to build a tree
 * structure. It is used for storing associative arrays where there are no
 * identical keys nor keys which are a prefix of others.
 *
 * It provides fast lookup, but the elements are not necessarily stored
 * sorted by key. */

#ifndef DSTREE_H
#define DSTREE_H

#include <stdio.h>

#include "base/result.h"
#include "item.h"

/* ----------------------------------------------------------------------- */

#define T dstree_t

typedef struct dstree T;

/* ----------------------------------------------------------------------- */

/* Destroy the specified key. */
typedef void (dstree_destroy_key)(void *key);

/* Destroy the specified value. */
typedef void (dstree_destroy_value)(void *value);

/* As in the hash library, if NULL is passed in for the compare or destroy
 * functions when a malloc'd string is assumed.
 *
 * Keys and values passed in (e.g. 'default_value' here, 'key' and 'value' to
 * dstree_insert below) are then owned by this data structure.
 *
 * NULL can be passed in for bit, compare, destroy_key and destroy_value to
 * use default routines suitable for strings.
 */
result_t dstree_create(const void            *default_value,
                       dstree_destroy_key    *destroy_key,
                       dstree_destroy_value  *destroy_value,
                       T                    **t);
void dstree_destroy(T *t);

/* ----------------------------------------------------------------------- */

const void *dstree_lookup(T *t, const void *key, size_t keylen);

result_t dstree_insert(T *t, const void *key, size_t keylen, const void *value);

void dstree_remove(T *t, const void *key, size_t keylen);
void dstree_remove2(T *t, const void *key, size_t keylen); /* alternative */

const item_t *dstree_select(T *t, int k);

int dstree_count(T *t);

/* ----------------------------------------------------------------------- */

typedef result_t (dstree_found_callback)(const item_t *item,
                                         void         *opaque);

result_t dstree_lookup_prefix(const T               *t,
                              const void            *prefix,
                              size_t                 prefixlen,
                              dstree_found_callback *cb,
                              void                  *opaque);

/* ----------------------------------------------------------------------- */

typedef result_t (dstree_walk_callback)(const item_t *item,
                                        int           level,
                                        void         *opaque);

result_t dstree_walk(const T              *t,
                     dstree_walk_callback *cb,
                     void                 *opaque);

/* ----------------------------------------------------------------------- */

/* To dump the data meaningfully dstree_show must call back to the client to
 * get the opaque keys and values turned into printable strings. These
 * strings may or may not be dynamically allocated so dstree_show_destroy is
 * provided to destroy them once finished with. */

typedef const char *(dstree_show_key)(const void *key);
typedef const char *(dstree_show_value)(const void *value);
typedef void (dstree_show_destroy)(char *doomed);

result_t dstree_show(const T             *t,
                     dstree_show_key     *key,
                     dstree_show_destroy *key_destroy,
                     dstree_show_value   *value,
                     dstree_show_destroy *value_destroy,
                     FILE                *f);

/* ----------------------------------------------------------------------- */

/* Dump in format which can be fed into Graphviz. */
result_t dstree_show_viz(const T             *t,
                         dstree_show_key     *key,
                         dstree_show_destroy *key_destroy,
                         dstree_show_value   *value,
                         dstree_show_destroy *value_destroy,
                         FILE                *f);

/* ----------------------------------------------------------------------- */

#undef T

#endif /* DSTREE_H */

