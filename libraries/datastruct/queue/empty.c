/* --------------------------------------------------------------------------
 *    Name: empty.c
 * Purpose: Queue implemented as a circular buffer
 * ----------------------------------------------------------------------- */

#include "datastruct/queue.h"

#include "impl.h"

int queue_empty(const queue_t *q)
{
  return q->head == q->tail;
}

