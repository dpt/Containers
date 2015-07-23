/* --------------------------------------------------------------------------
 *    Name: queue.h
 * Purpose: Queue implemented as a circular buffer
 * ----------------------------------------------------------------------- */

#ifndef QUEUE_H
#define QUEUE_H

#include <stdlib.h>

#include "base/result.h"

#define T queue_t

typedef struct queue T;

/* ----------------------------------------------------------------------- */

#define result_QUEUE_EMPTY (result_BASE_CONTAINER_QUEUE + 0)
#define result_QUEUE_FULL  (result_BASE_CONTAINER_QUEUE + 1)

/* ----------------------------------------------------------------------- */

/* Creates a fixed-size queue 'nelems' long of 'length'-long objects. */
T *queue_create(int nelems, size_t length);
void queue_destroy(T *doomed);

/* Copies the specified value into the queue. */
result_t queue_enqueue(T *queue, const void *value);

/* Removes the next value from the queue. 'value' is assumed to point to a
 * buffer large enough to hold the returned value (which is the 'length'
 * specified to queue_create). */
result_t queue_dequeue(T *queue, void *value);

int queue_count(const T *queue);
int queue_full(const T *queue);
int queue_empty(const T *queue);

#undef T

#endif /* QUEUE_H */

