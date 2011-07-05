/* container-test.c */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "memento.h"

#include "types.h"

#include "char-kv.h"
#include "int-kv.h"
#include "string-kv.h"

#include "icontainer.h"
#include "icontainer-key.h"
#include "icontainer-value.h"
#include "icontainer-maker.h"

#include "container-orderedarray.h"
#include "container-linkedlist.h"
#include "container-bstree.h"
#include "container-dstree.h"
#include "container-trie.h"
//#include "container-critbit.h"
//#include "container-patricia.h"

#include "testdata.h"

#include "container-test.h"

/* ----------------------------------------------------------------------- */

/* A statically allocated string key. */
static const icontainer_key_t static_string_key =
{
  stringkv_len, stringkv_compare,
  { stringkv_nodestroy, stringkv_fmt, stringkv_fmt_nodestroy }
};

/* A statically allocated int key. */
static const icontainer_key_t static_int_key =
{
  intkv_len, intkv_compare,
  { intkv_nodestroy, intkv_fmt, intkv_fmt_nodestroy }
};

/* A statically allocated char key. */
static const icontainer_key_t static_char_key =
{
  charkv_len, charkv_compare,
  { charkv_nodestroy, charkv_fmt, charkv_fmt_nodestroy }
};

/* A statically allocated string value. */
static const icontainer_value_t static_string_value =
{
  NULL /* default value */, { stringkv_nodestroy, stringkv_fmt, stringkv_fmt_nodestroy }
};

/* ----------------------------------------------------------------------- */

#define BLURT(m) printf(NAME ": -- " m "\n")
#define BLURT1(m,a) printf(NAME ": -- " m "\n", a)
#define BLURT2(m,a,b) printf(NAME ": -- " m "\n", a, b)
#define BLURT3(m,a,b,c) printf(NAME ": -- " m "\n", a, b, c)

/* ----------------------------------------------------------------------- */

#define NAME "stringtest"

static error stringtest(icontainer_maker *maker, int viz)
{
  const int     max = NELEMS(testdata);
  error         err;
  icontainer_t *cont;
  int           i;
  int           j;
  int           found;
  int           matched;
  const item_t *item;

  BLURT("Create cont");

  err = maker(&cont, &static_string_key, &static_string_value);
  if (err)
    goto failure;

  BLURT("Insert-Delete");

  for (j = 0; j < max; j++)
  {
    //BLURT1("inserting %d elems", j + 1);

    for (i = 0; i <= j; i++)
    {
      err = cont->insert(cont, testdata[i].key, testdata[i].value);
      if (err)
        return err;
      //BLURT2("insert: j=%d: now has %d nodes", j, cont->count(cont));
    }

    //cont->show_viz(cont, stdout);

    for (i = 0; i <= j; i++)
    {
      cont->remove(cont, testdata[i].key);
      //BLURT2("remove: j=%d: now has %d nodes", j, cont->count(cont));
    }

    assert(cont->count(cont) == 0);
    //BLURT1("delete: now has %d nodes", cont->count(cont));
  }

  // todo: pass in a vector of randomised testdata pointers

  BLURT("Insert test values");

  for (i = 0; i < max; i++)
  {
    err = cont->insert(cont, testdata[i].key, testdata[i].value);
    if (err)
      return err;

    //BLURT("Look up every key");

    found   = 0;
    matched = 0;
    for (j = 0; j <= i; j++)
    {
      const char *value;

      value = cont->lookup(cont, testdata[j].key);
      if (value == NULL)
      {
        BLURT1("lookup didn't find %s", testdata[j].key);
      }
      else
      {
        found++;

        if (value != testdata[j].value)
          BLURT1("values didn't match for key %s", testdata[j].key);
        else
          matched++;
      }
    }

    if (found < i + 1 || matched < i + 1)
    {
      BLURT3("%s%d of %d keys found", found < i + 1 ? "*** " : "", found, i + 1);
      BLURT3("%s%d of %d keys matched", matched < i + 1 ? "*** " : "", matched, i + 1);
    }
  }

  BLURT("The fifth element contains:");

  if ((item = cont->select(cont, 5)) == NULL)
    printf(NAME ": -- (null)\n");
  else
    printf(NAME ": -- '%s' : '%s'\n", item->key, item->value);

  BLURT("Dump");

  cont->show(cont, stdout);

  if (viz)
  {
    BLURT("Dump viz");

    cont->show_viz(cont, stdout);
  }

  BLURT("Remove every other test value");

  for (i = 0; i < max; i += 2)
    cont->remove(cont, testdata[i].key);

  BLURT("Remove remaining test values");

  for (i = 1; i < max; i += 2)
    cont->remove(cont, testdata[i].key);

  BLURT("Dump");

  cont->show(cont, stdout);

  BLURT("Destroy");

  cont->destroy(cont);

  return error_OK;


failure:

  BLURT1("error %lu\n", err);

  return err;
}

#undef NAME

/* ----------------------------------------------------------------------- */

static const struct
{
  int         key;
  const char *value;
}
inttestdata[] =
{
  { 0x00000000, "John"   },
  { 0x55555555, "Paul"   },
  { 0x0F0F0F0F, "George" },
  { 0x77777777, "Ringo"  },
};

#define NAME "inttest"

static error inttest(icontainer_maker *maker, int viz)
{
  const int     max = NELEMS(inttestdata);
  error         err;
  icontainer_t *cont;
  int           i;
  int           j;
  int           found;
  int           matched;
  const item_t *item;

  BLURT("Create cont");

  err = maker(&cont, &static_int_key, &static_string_value);
  if (err)
    goto failure;

  BLURT("Insert test values");

  for (i = 0; i < max; i++)
  {
    err = cont->insert(cont, &inttestdata[i].key, inttestdata[i].value);
    if (err)
      goto failure;

    BLURT1("Count of elements = %d elements", cont->count(cont));

    //BLURT("Look up every key");

    found   = 0;
    matched = 0;
    for (j = 0; j <= i; j++)
    {
      const char *value;

      value = cont->lookup(cont, &inttestdata[j].key);
      if (value == NULL)
      {
        BLURT1("lookup didn't find %d", inttestdata[j].key);
      }
      else
      {
        found++;

        if (value != inttestdata[j].value)
          BLURT1("values didn't match for key %d", inttestdata[j].key);
        else
          matched++;
      }
    }

    if (found < i + 1 || matched < i + 1)
    {
      BLURT3("%s%d of %d keys found", found < i + 1 ? "*** " : "", found, i + 1);
      BLURT3("%s%d of %d keys matched", matched < i + 1 ? "*** " : "", matched, i + 1);
    }
  }

  BLURT("The fifth element contains:");

  if ((item = cont->select(cont, 5)) == NULL)
    printf(NAME ": -- (null)\n");
  else
    printf(NAME ": -- %x : '%s'\n", (unsigned int) item->key, item->value);

  BLURT("Dump");

  cont->show(cont, stdout);

  if (viz)
  {
    BLURT("Dump viz");

    cont->show_viz(cont, stdout);
  }

  BLURT("Remove every other test value");

  for (i = 0; i < max; i += 2)
    cont->remove(cont, &inttestdata[i].key);

  BLURT("Remove remaining test values");

  for (i = 1; i < max; i += 2)
    cont->remove(cont, &inttestdata[i].key);

  BLURT("Dump");

  cont->show(cont, stdout);

  BLURT("Destroy");

  cont->destroy(cont);

  return error_OK;


failure:

  BLURT1("error %lu\n", err);

  return err;
}

#undef NAME


/* ----------------------------------------------------------------------- */

#define E(c) { c - 'A' + 1, #c }

static const struct
{
  char        key;
  const char *value;
}
chartestdata[] =
{
  E('A'),
  E('S'),
  E('E'),
  E('R'),
  E('C'),
  E('H'),
  E('I'),
  E('N'),
  E('G'),
  E('X'),
  E('M'),
  E('P'),
  E('L'),
};

#define NAME "chartest"

static error chartest(icontainer_maker *maker, int viz)
{
  const int     max = NELEMS(chartestdata);
  error         err;
  icontainer_t *cont;
  int           i;
  int           j;
  int           found;
  int           matched;
  const item_t *item;

  BLURT("Create cont");

  err = maker(&cont, &static_char_key, &static_string_value);
  if (err)
    goto failure;

  BLURT("Insert test values");

  for (i = 0; i < max; i++)
  {
    err = cont->insert(cont, &chartestdata[i].key, chartestdata[i].value);
    if (err)
      goto failure;

    BLURT1("Count of elements = %d elements", cont->count(cont));

    //BLURT("Look up every key");

    found   = 0;
    matched = 0;
    for (j = 0; j <= i; j++)
    {
      const char *value;

      value = cont->lookup(cont, &chartestdata[j].key);
      if (value == NULL)
      {
        BLURT1("lookup didn't find %d", chartestdata[j].key);
      }
      else
      {
        found++;

        if (value != chartestdata[j].value)
          BLURT1("values didn't match for key %d", chartestdata[j].key);
        else
          matched++;
      }
    }

    if (found < i + 1 || matched < i + 1)
    {
      BLURT3("%s%d of %d keys found", found < i + 1 ? "*** " : "", found, i + 1);
      BLURT3("%s%d of %d keys matched", matched < i + 1 ? "*** " : "", matched, i + 1);
    }
  }

  BLURT("The fifth element contains:");

  if ((item = cont->select(cont, 5)) == NULL)
    printf(NAME ": -- (null)\n");
  else
    printf(NAME ": -- %d : '%s'\n", *((char *) item->key), item->value);

  BLURT("Dump");

  cont->show(cont, stdout);

  if (viz)
  {
    BLURT("Dump viz");

    cont->show_viz(cont, stdout);
  }

  BLURT("Remove every other test value");

  for (i = 0; i < max; i += 2)
    cont->remove(cont, &chartestdata[i].key);

  BLURT("Remove remaining test values");

  for (i = 1; i < max; i += 2)
    cont->remove(cont, &chartestdata[i].key);

  BLURT("Dump");

  cont->show(cont, stdout);

  BLURT("Destroy");

  cont->destroy(cont);

  return error_OK;


failure:

  BLURT1("error %lu\n", err);

  return err;
}

#undef NAME

/* ----------------------------------------------------------------------- */

static error test_icontainer_type(icontainer_maker *maker, int viz)
{
  static const struct
  {
    error     (*test)(icontainer_maker *maker, int viz);
    const char *name;
  }
  tests[] =
  {
    { chartest,   "char test"   },
    { inttest,    "int test"    },
    { stringtest, "string test" },
  };

  error err;
  int   i;

  printf(">> test maker %p\n", maker);

  for (i = 0; i < NELEMS(tests); i++)
  {
    printf("> %s\n", tests[i].name);
    err = tests[i].test(maker, viz);
    if (err)
      goto failure;
  }

  printf("--------\n");

  return error_OK;


failure:

  printf("test_container: error %ld\n", err);

  return err;
}

int test_container(int viz)
{
  static const struct
  {
    icontainer_maker *maker;
    const char       *name;
  }
  makers[] =
  {
    { container_create_orderedarray, "ordered array" },
    { container_create_linkedlist,   "linked list"   },
    { container_create_bstree,       "bstree"        },
    { container_create_dstree,       "dstree"        },
    { container_create_trie,         "trie"          },
    //{ container_create_critbit,      "critbit"       },
    //{ container_create_patricia,     "patricia"      },
  };

  error err;
  int   i;

  for (i = 0; i < NELEMS(makers); i++)
  {
    printf(">> test_container with '%s'\n", makers[i].name);

    err = test_icontainer_type(makers[i].maker, viz);
    if (err)
      goto failure;
  }

  return EXIT_SUCCESS;


failure:

  printf("test_container: error %ld\n", err);

  return EXIT_FAILURE;
}
