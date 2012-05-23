/* queue.c -- queue implemented as circular buffer */

/* This implementation keeps one entry spare to allow detection of full
 * vs empty (which would otherwise be signified by identical tests). */

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "base/memento/memento.h"

#include "base/types.h"

#include "datastruct/queue.h"

struct queue_t
{
  char  *head;
  char  *tail;
  int    nelems;
  size_t width;
  char   buffer[1];
};

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

void queue_destroy(queue_t *doomed)
{
  free(doomed);
}

error queue_enqueue(queue_t *q, const void *value)
{
  if (queue_full(q))
    return error_QUEUE_FULL;

  memcpy(q->head, value, q->width);

  if (q->head + q->width == q->buffer + q->nelems * q->width)
    q->head = q->buffer;
  else
    q->head += q->width;

  return error_OK;
}

error queue_dequeue(queue_t *q, void *value)
{
  if (queue_empty(q))
    return error_QUEUE_EMPTY;

  memcpy(value, q->tail, q->width);

  if (q->tail + q->width == q->buffer + q->nelems * q->width)
    q->tail = q->buffer;
  else
    q->tail += q->width;

  return error_OK;
}

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

int queue_full(const queue_t *q)
{
  ptrdiff_t head, tail;

  head = q->head - q->buffer;
  tail = q->tail - q->buffer;

  return ((head + q->width) % (q->nelems * q->width)) == tail;
}

int queue_empty(const queue_t *q)
{
  return q->head == q->tail;
}
