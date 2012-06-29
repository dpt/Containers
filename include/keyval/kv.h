/* --------------------------------------------------------------------------
 *    Name: kv.h
 * Purpose: Types of callbacks used by keys and values
 * ----------------------------------------------------------------------- */

/* 'kv' here means 'key or value'. These are the interfaces of callback
 * functions which could apply to either. */

#ifndef KV_H
#define KV_H

#include <stdlib.h>

/* The key length returned from kv_len and passed into kv_bit is not (yet)
 * interpreted by the data structure implementations. It can be specified in
 * bytes, bits, or some other measure convienient to you and only your code
 * need be aware of the units. */

/* Signature of a function which returns the length of the specified key. */
typedef size_t (kv_len)(const void *key);

/* Signature of a function which compares two keys (like qsort() uses). */
typedef int (kv_compare)(const void *a, const void *b);

/* Signature of a function which hashes a key. */
typedef unsigned int (kv_hash)(const void *key);

/* Signature of a function which destroys an allocated key (same signature as
 * free()). */
typedef void (kv_destroy)(void *doomed);

/* Signature of a function which formats the specified key for printing. */
typedef const char *(kv_fmt)(const void *kv);

/* Signature of a function which destroys the specified formatted key. */
typedef void (kv_fmt_destroy)(char *doomed);

#endif /* KV_H */

