/* queue.h -- queue implemented as circular buffer */

#ifndef QUEUE_H
#define QUEUE_H

#include <stdlib.h>

#include "errors.h"

typedef struct queue_t queue_t;

#define T queue_t

/* Creates a fixed-size queue 'nelems' long of 'length'-long objects. */
T *queue_create(int nelems, size_t length);
void queue_destroy(T *doomed);

/* Copies the specified value into the queue. */
error queue_enqueue(T *queue, const void *value);

/* Removes the next value from the queue. 'value' is assumed to point to a
 * buffer large enough to hold the returned value (which is the 'length'
 * specified to queue_create). */
error queue_dequeue(T *queue, void *value);

int queue_count(const T *queue);
int queue_full(const T *queue);
int queue_empty(const T *queue);

#undef T

#endif /* QUEUE_H */
