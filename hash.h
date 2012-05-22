/* --------------------------------------------------------------------------
 *    Name: hash.h
 * Purpose: Interface of Hash library
 * ----------------------------------------------------------------------- */

/**
 * \file Hash (interface).
 *
 * Hash is an associative array.
 *
 * The interface presently forces you to malloc all keys, and values passed
 * in, yourself.
 */

#ifndef HASH_H
#define HASH_H

#include <stdio.h>

#include "errors.h"
#include "item.h"

#define T hash_t

typedef struct hash T;

/* ----------------------------------------------------------------------- */

/**
 * A function called to hash the specified key.
 */
typedef unsigned int (hash_fn)(const void *a);

/**
 * A function called to compare the two specified keys.
 */
typedef int (hash_compare)(const void *a, const void *b);

/**
 * A function called to destroy the specified key.
 */
typedef void (hash_destroy_key)(void *key);

/**
 * A function called to destroy the specified value.
 */
typedef void (hash_destroy_value)(void *value);

/**
 * Create a hash.
 *
 * \param      default_value Value to return for failed lookups.
 * \param      nbins         Suggested number of hash bins to allocate.
 * \param      fn            Function to hash keys.
 * \param      compare       Function to compare keys.
 * \param      destroy_key   Function to destroy a key.
 * \param      destroy_value Function to destroy a value.
 * \param[out] hash          Created hash.
 *
 * \return Error indication.
 */
error hash_create(const void         *default_value,
                  int                 nbins,
                  hash_fn            *fn,
                  hash_compare       *compare,
                  hash_destroy_key   *destroy_key,
                  hash_destroy_value *destroy_value,
                  T                 **hash);

/**
 * Destroy a hash.
 *
 * \param doomed Hash to destroy.
 */
void hash_destroy(T *doomed);

/* ----------------------------------------------------------------------- */

/**
 * Return the value associated with the specified key.
 *
 * \param hash Hash.
 * \param key  Key to look up.
 *
 * \return Value associated with the specified key.
 */
const void *hash_lookup(T *hash, const void *key);

/**
 * Insert the specified key:value pair into the hash.
 *
 * The hash takes ownership of the key and value pointers. It will call the
 * destroy functions passed to hash_create when the keys and values are to
 * be destroyed.
 *
 * \param hash   Hash.
 * \param key    Key to insert.
 * \param keylen Length of key.
 * \param value  Associated value.
 *
 * \return Error indication.
 */
error hash_insert(T          *hash,
                  const void *key,
                  size_t      keylen,
                  const void *value);

/**
 * Remove the specified key from the hash.
 *
 * \param hash Hash.
 * \param key  Key to remove.
 */
void hash_remove(T *hash, const void *key);

/**
 * Return the count of items stored in the hash.
 *
 * \param hash Hash.
 *
 * \return Count of items in the hash.
 */
int hash_count(T *hash);

/* ----------------------------------------------------------------------- */

/**
 * A function called for every key:value pair in the hash.
 *
 * Return an error to halt the walk operation.
 */
typedef error (hash_walk_callback)(const item_t *item,
                                   void         *opaque);

/**
 * Walk the hash, calling the specified routine for every element.
 *
 * \param hash   Hash.
 * \param cb     Callback routine.
 * \param opaque Opaque pointer to pass to callback routine.
 *
 * \return Error indication.
 * \retval error_OK If the walk completed successfully.
 */
error hash_walk(const T *hash, hash_walk_callback *cb, void *opaque);

/* ----------------------------------------------------------------------- */

/**
 * Walk the hash, returning each element in turn.
 *
 * \param      hash             Hash.
 * \param      continuation     Continuation value. Zero for initial call.
 * \param[out] nextcontinuation Next continuation value.
 * \param[out] key              Pointer to receive key.
 * \param[out] value            Pointer to receive value.
 *
 * \return Error indication.
 * \retval error_OK       If an element was found.
 * \retval error_HASH_END If no elements remain.
 */
error hash_walk_continuation(T           *hash,
                             int          continuation,
                             int         *nextcontinuation,
                             const void **key,
                             const void **value);

/* ----------------------------------------------------------------------- */

/* To dump the data meaningfully hash_show must call back to the client to
 * get the opaque keys and values turned into printable strings. These
 * strings may or may not be dynamically allocated so hash_show_destroy is
 * provided to destroy them once finished with. */

typedef const char *(hash_show_key)(const void *key);
typedef const char *(hash_show_value)(const void *value);
typedef void (hash_show_destroy)(char *doomed);

error hash_show(const T           *t,
                hash_show_key     *key,
                hash_show_destroy *key_destroy,
                hash_show_value   *value,
                hash_show_destroy *value_destroy,
                FILE              *f);

/* ----------------------------------------------------------------------- */

#undef T

#endif /* HASH_H */
