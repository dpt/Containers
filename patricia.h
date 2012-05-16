/* patricia.h -- associative array implemented as patricia trie */

/* A PATRICIA trie is a form of trie where one-way branching is replaced with
 * a bit index and terminal nodes point back to previous nodes. */

#ifndef PATRICIA_H
#define PATRICIA_H

#include <stdio.h>

#include "errors.h"
#include "item.h"

/* ----------------------------------------------------------------------- */

#define T patricia_t

typedef struct patricia T;

/* ----------------------------------------------------------------------- */

/* Destroy the specified key. */
typedef void (patricia_destroy_key)(void *key);

/* Destroy the specified value. */
typedef void (patricia_destroy_value)(void *value);

/* As in the hash library, if NULL is passed in for the destroy functions
 * when a malloc'd string is assumed.
 *
 * Keys and values passed in (e.g. 'default_value' here, 'key' and 'value' to
 * patricia_insert below) are then owned by this data structure.
 *
 * NULL can be passed in for destroy_key and destroy_value to use default
 * routines suitable for strings.
 */
error patricia_create(const void             *default_value,
                      patricia_destroy_key   *destroy_key,
                      patricia_destroy_value *destroy_value,
                      T                     **t);
void patricia_destroy(T *t);

/* ----------------------------------------------------------------------- */

const void *patricia_lookup(const T *t, const void *key, size_t keylen);

error patricia_insert(T          *t,
                      const void *key,
                      size_t      keylen,
                      const void *value);

void patricia_remove(T *t, const void *key, size_t keylen);

const item_t *patricia_select(T *t, int k);

int patricia_count(T *t);

/* ----------------------------------------------------------------------- */

typedef error (patricia_found_callback)(const item_t *item,
                                        void         *opaque);

error patricia_lookup_prefix(const T                 *t,
                             const void              *prefix,
                             size_t                   prefixlen,
                             patricia_found_callback *cb,
                             void                    *opaque);

/* ----------------------------------------------------------------------- */

typedef error (patricia_walk_callback)(const void *key,
                                       const void *value,
                                       int         level,
                                       void       *opaque);

error patricia_walk(const T                *t,
                    patricia_walk_callback *cb,
                    void                   *opaque);

/* ----------------------------------------------------------------------- */

/* To dump the data meaningfully patricia_show must call back to the client
 * to get the opaque keys and values turned into printable strings. These
 * strings may or may not be dynamically allocated so patricia_show_destroy
 * is provided to destroy them once finished with. */

typedef const char *(patricia_show_key)(const void *key);
typedef const char *(patricia_show_value)(const void *value);
typedef void (patricia_show_destroy)(char *doomed);

error patricia_show(const T               *t,
                    patricia_show_key     *key,
                    patricia_show_destroy *key_destroy,
                    patricia_show_value   *value,
                    patricia_show_destroy *value_destroy,
                    FILE                  *f);

/* ----------------------------------------------------------------------- */

/* Dump in format which can be fed into Graphviz. */
error patricia_show_viz(const T               *t,
                        patricia_show_key     *key,
                        patricia_show_destroy *key_destroy,
                        patricia_show_value   *value,
                        patricia_show_destroy *value_destroy,
                        FILE                  *f);

/* ----------------------------------------------------------------------- */

#undef T

#endif /* PATRICIA_H */
