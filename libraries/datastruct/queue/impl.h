/* --------------------------------------------------------------------------
 *    Name: impl.h
 * Purpose: Queue implemented as a circular buffer
 * ----------------------------------------------------------------------- */

/* This implementation keeps one entry spare to allow detection of full
 * vs empty (which would otherwise be signified by identical tests). */

#include <stddef.h>

struct queue_t
{
  char  *head;
  char  *tail;
  int    nelems;
  size_t width;
  char   buffer[1];
};

