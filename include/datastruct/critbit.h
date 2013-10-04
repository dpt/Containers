/* --------------------------------------------------------------------------
 *    Name: critbit.h
 * Purpose: Associative array implemented as a critbit tree
 * ----------------------------------------------------------------------- */

/* A critbit tree is a form of trie where one-way branching is replaced with
 * a bit index. */

/* This implementation is based on Bernstein binary crit-bit tree via the
 * paper at imperialviolet.org:
 * http://www.imperialviolet.org/2008/09/29/critbit-trees.html
 */

#ifndef CRITBIT_H
#define CRITBIT_H

#include <stdio.h>

#include "base/errors.h"
#include "item.h"

/* ----------------------------------------------------------------------- */

#define T critbit_t

typedef struct critbit T;

/* ----------------------------------------------------------------------- */

/* Destroy the specified key. */
typedef void (critbit_destroy_key)(void *key);

/* Destroy the specified value. */
typedef void (critbit_destroy_value)(void *value);

/* As in the hash library, if NULL is passed in for the destroy functions
 * when a malloc'd string is assumed.
 *
 * Keys and values passed in (e.g. 'default_value' here, 'key' and 'value' to
 * critbit_insert below) are then owned by this data structure.
 *
 * NULL can be passed in for destroy_key and destroy_value to use default
 * routines suitable for strings.
 */
error critbit_create(const void            *default_value,
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

typedef error (critbit_found_callback)(const item_t *item,
                                       void         *opaque);

error critbit_lookup_prefix(const T                *t,
                            const void             *prefix,
                            size_t                  prefixlen,
                            critbit_found_callback *cb,
                            void                   *opaque);

/* ----------------------------------------------------------------------- */

typedef error (critbit_walk_callback)(const void *key,
                                      const void *value,
                                      int         level,
                                      void       *opaque);

error critbit_walk(const T               *t,
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

