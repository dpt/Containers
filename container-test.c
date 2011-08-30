/* container-test.c */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "memento.h"

#include "errors.h"
#include "utils.h"

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
#include "container-critbit.h"
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
  NULL /* default value */,
  { stringkv_nodestroy, stringkv_fmt, stringkv_fmt_nodestroy }
};

/* ----------------------------------------------------------------------- */

#define BLURT(m) printf(NAME ": -- " m "\n")
#define BLURT1(m,a) printf(NAME ": -- " m "\n", a)
#define BLURT2(m,a,b) printf(NAME ": -- " m "\n", a, b)
#define BLURT3(m,a,b,c) printf(NAME ": -- " m "\n", a, b, c)

/* ----------------------------------------------------------------------- */

#define NAME "stringtest"

static int prefixes_seen = 0;

static error stringtest_lookup_prefix_callback(const item_t *item, void *opaque)
{
  NOT_USED(opaque);

  (void) printf(NAME ": -- '%s' : '%s'\n", item->key, item->value);
  
  prefixes_seen++;

  return error_OK;
}

static error stringtest(icontainer_maker *maker, FILE *vizout)
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
    BLURT1("inserting %d elems", j + 1);

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

  BLURT("Look up keys by their prefix");
  
  prefixes_seen = 0;

  for (i = 'A'; i <= 'Z'; i++)
  {
    char prefix[2];

    BLURT1("Enumerate keys beginning with '%c':", i);

    prefix[0] = (char) i;
    prefix[1] = '\0';

    err = cont->lookup_prefix(cont, prefix, stringtest_lookup_prefix_callback, NULL);
    if (err == error_NOT_FOUND)
      BLURT("Prefix not found");
    else if (err)
      goto failure;
  }
  
  BLURT1("prefixes_seen = %d", prefixes_seen);
  if (prefixes_seen != NELEMS(testdata))
    BLURT1("*** incorrect number of prefixes seen! (expected %d)", NELEMS(testdata));
  else
    BLURT("ok!");

  BLURT("Look up a key which doesn't exist");

  err = cont->lookup_prefix(cont, "Gooseberries", stringtest_lookup_prefix_callback, NULL);
  if (err != error_NOT_FOUND)
  {
    BLURT("lookup_prefix did _not_ return 'not found'!");
    goto failure;
  }

  BLURT("Dump");

  cont->show(cont, stdout);

  if (vizout)
  {
    BLURT("Dump viz");

    cont->show_viz(cont, vizout);
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

/* Insert a set of strings with common prefixes. We can use this to check
 * that crit-bit and PATRICIA correctly build trees which factor out the
 * common part. */

static const struct
{
  const char *key;
  const char *value;
}
commonprefixstrings[] =
{
  { "A man, a plan, a canal - Panama!", "one" },
  { "A man, a plan, a cat, a canal – Panama!", "two" },
  { "A man, a plan, a cat, a ham, a yak, a yam, a hat, a canal – Panama!", "three" }
};

#define NAME "commonprefixtest"

static error commonprefixtest(icontainer_maker *maker, FILE *vizout)
{
  const int     max = NELEMS(commonprefixstrings);
  error         err;
  icontainer_t *cont;
  int           i;

  BLURT("Create cont");

  err = maker(&cont, &static_string_key, &static_string_value);
  if (err)
    goto failure;

  BLURT("Insert test values");

  for (i = 0; i < max; i++)
  {
    err = cont->insert(cont, commonprefixstrings[i].key, commonprefixstrings[i].value);
    if (err)
      return err;
  }

  BLURT("Dump");

  cont->show(cont, stdout);

  if (vizout)
  {
    BLURT("Dump viz");

    cont->show_viz(cont, vizout);
  }

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
  { 0x00000001, "Paul"   },
  { 0x00000002, "George" },
  { 0x00000003, "Ringo"  },
  { 0x7fffff04, "Dave"   },
};

#define NAME "inttest"

static error inttest_lookup_prefix_callback(const item_t *item, void *opaque)
{
  NOT_USED(opaque);

  printf(NAME ": -- %x : '%s'\n", (unsigned int) item->key, item->value);

  return error_OK;
}

static error inttest(icontainer_maker *maker, FILE *vizout)
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

  BLURT("Look up keys by their prefix");

  /* Prefix lookup tests aren't exactly relevant here. The key for 'dave'
   * won't get found. Since int key objects are always four bytes long we hit
   * the 'not found' case. A 'prefix' in this case is always as long as a
   * full key. */

  for (i = 0; i < 5; i++)
  {
    int prefix;

    BLURT1("Enumerate keys beginning with '%d':", i);

    prefix = i;

    err = cont->lookup_prefix(cont, &prefix, inttest_lookup_prefix_callback, NULL);
    if (err == error_NOT_FOUND)
      ;
    else if (err)
      goto failure;
  }

  BLURT("Look up a key which doesn't exist");

  err = cont->lookup_prefix(cont, "Gooseberries", inttest_lookup_prefix_callback, NULL);
  if (err != error_NOT_FOUND)
  {
    BLURT("lookup_prefix did _not_ return 'not found'!");
    goto failure;
  }

  BLURT("Dump");

  cont->show(cont, stdout);

  if (vizout)
  {
    BLURT("Dump viz");

    cont->show_viz(cont, vizout);
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

static error chartest(icontainer_maker *maker, FILE *vizout)
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

  if (vizout)
  {
    BLURT("Dump viz");

    cont->show_viz(cont, vizout);
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

static error test_icontainer_type(icontainer_maker *maker,
                                  const char       *filesafemakername,
                                  int               viz)
{
  static const struct
  {
    error     (*test)(icontainer_maker *maker, FILE *vizout);
    const char *name;
    const char *filesafename;
  }
  tests[] =
  {
    { chartest,         "char test",                 "char"         },
    { inttest,          "int test",                  "int"          },
    { stringtest,       "string test",               "string"       },
    { commonprefixtest, "common prefix string test", "commonprefix" },
  };

  error err;
  int   i;

  printf(">> test maker %p\n", maker);

  for (i = 0; i < NELEMS(tests); i++)
  {
    FILE *f = NULL;

    printf("> %s\n", tests[i].name);

    if (viz)
    {
      char vizfilename[100];

      sprintf(vizfilename, "%s-%s.gv", filesafemakername,
              tests[i].filesafename);

      f = fopen(vizfilename, "w");
    }

    err = tests[i].test(maker, f);
    if (err)
      goto failure;

    fclose(f);
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
    const char       *filesafename;
  }
  makers[] =
  {
    { container_create_orderedarray, "ordered array", "orderedarray" },
    { container_create_linkedlist,   "linked list",   "linkedlist"   },
    { container_create_bstree,       "bstree",        "bstree"       },
    { container_create_dstree,       "dstree",        "dstree"       },
    { container_create_trie,         "trie",          "trie"         },
    { container_create_critbit,      "critbit",       "critbit"      },
//    { container_create_patricia,     "patricia",      "patricia"     },
  };

  error err;
  int   i;

  for (i = 0; i < NELEMS(makers); i++)
  {
    printf(">> test_container with '%s'\n", makers[i].name);

    err = test_icontainer_type(makers[i].maker, makers[i].filesafename, viz);
    if (err)
      goto failure;
  }

  return EXIT_SUCCESS;


failure:

  printf("test_container: error %ld\n", err);

  return EXIT_FAILURE;
}
