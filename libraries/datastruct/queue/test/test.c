/* test.c */

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "base/memento/memento.h"

#include "base/result.h"
#include "base/types.h"
#include "base/utils.h"

#include "datastruct/queue.h"

/* test int values */
static result_t queuetest1(void)
{
  static const int values1[] =
  {
    3, 33, 333, 3333, 33333, 333333, 3333333, 33333333
  };
  static const int values2[] =
  {
    6, 66, 666, 6666, 66666, 666666, 6666666, 66666666
  };

  result_t  err;
  queue_t  *q;
  int       i;
  int       v;

  printf("> queue test 1 - ints\n");

  q = queue_create(4, sizeof(int));
  if (q == NULL)
    return result_OOM;

  /* enqueue five values into the four long queue */

  for (i = 0; i < 5; i++)
  {
    printf("enqueue: %d (existing count=%d)\n", values1[i], queue_count(q));
    err = queue_enqueue(q, &values1[i]);
    if (err)
    {
      if (err == result_QUEUE_FULL && i == 4)
        printf("error: %x - expected (queue full)\n", err);
      else
        return err;
    }
  }

  /* dequeue five values from the four long queue */

  for (i = 0; i < 5; i++)
  {
    err = queue_dequeue(q, &v);
    if (err)
    {
      if (err == result_QUEUE_EMPTY && i == 4)
        printf("error: %x - expected (queue empty)\n", err);
      else
        return err;
    }
    else
    {
      printf("dequeue: %d (new count=%d)\n", v, queue_count(q));
      if (v != values1[i])
      {
        printf("values didn't match!\n");
        return result_TEST_FAILED;
      }
    }
  }

  /* enqueue all, dequeue one, enqueue one, ... */

  for (i = 0; i < 4; i++)
  {
    printf("enqueue: %d (existing count=%d)\n", values1[i], queue_count(q));
    err = queue_enqueue(q, &values1[i]);
    if (err)
      return err;
  }

  for (i = 0; i < NELEMS(values1); i++)
  {
    err = queue_dequeue(q, &v);
    if (err)
      return err;
    else
      printf("dequeue: %d (new count=%d)\n", v, queue_count(q));

    printf("enqueue: %d (existing count=%d)\n", values2[i], queue_count(q));
    err = queue_enqueue(q, &values2[i]);
    if (err)
      return err;
  }

  queue_destroy(q);

  return result_OK;
}

/* test wider-than-int values */
static result_t queuetest2(void)
{
  typedef struct testdata
  {
    int         num;
    const char *name;
  }
  testdata;

  static const testdata values1[] =
  {
    { 1, "John"   },
    { 2, "Paul"   },
    { 3, "George" },
    { 4, "Ringo"  },
    { 5, "Dave"   },
  };

  static const testdata values2[] =
  {
    { 11, "Gaz"   },
    { 12, "Mick"  },
    { 13, "Danny" },
    { 14, "Rob"   },
    { 15, "Dave"  },
  };

  result_t  err;
  queue_t  *q;
  int       i;
  testdata  v;

  printf("> queue test 2 - structs\n");

  q = queue_create(4, sizeof(testdata));
  if (q == NULL)
    return result_OOM;

  /* first part of testing omitted - same as above */

  /* enqueue all, dequeue one, enqueue one, ... */

  for (i = 0; i < 4; i++)
  {
    printf("enqueue: %d/%s (existing count=%d)\n", values1[i].num, values1[i].name, queue_count(q));
    err = queue_enqueue(q, &values1[i]);
    if (err)
      return err;
  }

  for (i = 0; i < NELEMS(values1); i++)
  {
    err = queue_dequeue(q, &v);
    if (err)
    {
      if (err == result_QUEUE_EMPTY && i == 4)
        printf("error: %x - expected\n", err);
      else
        return err;
    }
    else
    {
      printf("dequeue: %d/%s (new count=%d)\n", v.num, v.name, queue_count(q));
    }

    printf("enqueue: %d/%s (existing count=%d)\n", values2[i].num, values2[i].name, queue_count(q));
    err = queue_enqueue(q, &values2[i]);
    if (err)
      return err;
  }

  queue_destroy(q);

  return result_OK;
}

result_t queuetest(void)
{
  result_t e1, e2;

  printf(">> queue test\n");

  e1 = queuetest1();
  if (e1)
    printf("unexpected error: %x\n", e1);

  e2 = queuetest2();
  if (e2)
    printf("unexpected error: %x\n", e2);

  if (e1 || e2)
    return e1 != result_OK ? e1 : e2;

  printf("<< queue tests ok\n");

  return result_OK;
}
