/* --------------------------------------------------------------------------
 *    Name: create.c
 * Purpose: Queue implemented as a circular buffer
 * ----------------------------------------------------------------------- */

#include <stddef.h>

#include "base/memento/memento.h"

#include "datastruct/queue.h"

#include "impl.h"

/* Note: The parameter 'length' in the header is called 'width' here. */
queue_t *queue_create(int nelems, size_t width)
{
  queue_t *q;

  nelems++; /* increase to account for the entry we keep spare */

  q = malloc(offsetof(queue_t, buffer) + nelems * width);
  if (q == NULL)
    return NULL;

  q->head   = q->tail = &q->buffer[0];
  q->nelems = nelems;
  q->width  = width;

  return q;
}

