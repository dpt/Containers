/* --------------------------------------------------------------------------
 *    Name: count.c
 * Purpose: Queue
 * ----------------------------------------------------------------------- */

#include <stddef.h>

#include "datastruct/queue.h"

#include "impl.h"

int queue_count(const queue_t *q)
{
  char      *head, *tail;
  ptrdiff_t  c;

  head = q->head;
  tail = q->tail;

  if (head >= tail)
    c = head - tail;
  else
    c = head + q->nelems * q->width - tail;

  return c / q->width;
}

