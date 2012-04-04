/* icontainer.h -- interface to container compliant data structures */

/* The individual data structure libraries I've implemented don't expose
 * exactly the same interface. 'icontainer' provides an alternative
 * consistent interface which glue libraries can provide.
 *
 * This ought to allow a single user of the icontainer interface to swap in
 * one data structure implementation for another without needing to alter
 * their code, much like you can do in C++ with STL. (That at least is the
 * idea, but whether it will work in practice remains to be seen).
 *
 * By writing this in C it'll work on RISC OS and possibly be smaller than
 * templated code (also the real point is that it's a learning exercise).
 * But it will be slower as the callback arrangement means nothing will be
 * inlined.
 */

#ifndef ICONTAINER_H
#define ICONTAINER_H

#include <stdio.h>

#include "errors.h"

#include "item.h"

#define T icontainer_t

typedef struct icontainer T;

/* ----------------------------------------------------------------------- */

/* Search for keyed element. */
typedef const void *(*icontainer_lookup)(const T    *c,
                                         const void *key);

/* Insert new element. */
typedef error (*icontainer_insert)(T          *c,
                                   const void *key,
                                   const void *value);

/* Remove keyed element. */
typedef void (*icontainer_remove)(T          *c,
                                  const void *key);

/* Select K'th largest element. */
typedef const item_t *(*icontainer_select)(const T *c,
                                           int      k);

/* A function which is called back with a found item. */
typedef error (*icontainer_found_callback)(const item_t *item,
                                           void          *opaque);

/* Search for elements with the specified key prefix. */
typedef error (*icontainer_lookup_prefix)(const T                   *c,
                                          const void                *prefix,
                                          icontainer_found_callback  cb,
                                          void                      *opaque);

/* Return number of elements in container. */
typedef int (*icontainer_count)(const T *c);

/* Display container's contents. */
typedef error (*icontainer_show)(const T *c,
                                 FILE    *f);

/* Display container's contents in Graphviz 'dot' input format. */
typedef error (*icontainer_show_viz)(const T *c,
                                     FILE    *f);

/* Destroy specified container. */
typedef void (*icontainer_destroy)(T *doomed);

struct icontainer
{
  icontainer_lookup        lookup;
  icontainer_insert        insert;
  icontainer_remove        remove;
  icontainer_select        select;
  icontainer_lookup_prefix lookup_prefix;
  icontainer_count         count;
  icontainer_show          show;
  icontainer_show_viz      show_viz;
  icontainer_destroy       destroy;
};

/* ----------------------------------------------------------------------- */

#undef T

#endif /* ICONTAINER_H */
