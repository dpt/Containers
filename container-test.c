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
#include "container-patricia.h"

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

#define LOG(m) printf(NAME ": -- " m "\n")
#define LOG1(m,a) printf(NAME ": -- " m "\n", a)
#define LOG2(m,a,b) printf(NAME ": -- " m "\n", a, b)
#define LOG3(m,a,b,c) printf(NAME ": -- " m "\n", a, b, c)

/* ----------------------------------------------------------------------- */

static int viz_counter = 0;

static error viz_show(icontainer_t *cont,
                      const char   *groupname,
                      const char   *testname)
{
  static char filename[256];
  FILE       *f;

  sprintf(filename, "%s-%s-%d.gv", groupname, testname, viz_counter++);

  f = fopen(filename, "w");
  if (f == NULL)
    return error_NOT_FOUND;

  cont->show_viz(cont, f);

  fclose(f);

  return error_OK;
}

/* ----------------------------------------------------------------------- */

#define NAME "stringtest"

static int prefixes_seen = 0;

static error stringtest_lookup_prefix_callback(const item_t *item, void *opaque)
{
  int *count = opaque;

  (void) printf(NAME ": -- '%s' : '%s'\n", item->key, item->value);

  prefixes_seen++;

  (*count)++;

  return error_OK;
}

static error stringtest(icontainer_maker *maker, const char *testname)
{
  const int     max = NELEMS(testdata);
  error         err;
  icontainer_t *cont;
  int           i;
  int           j;
  int           found;
  int           matched;
  const item_t *item;
  int           prefixcounts[26];

  LOG("Create cont");

  err = maker(&cont, &static_string_key, &static_string_value);
  if (err)
    goto failure;

  LOG("Insert-Delete");

  for (j = 0; j < max; j++)
  {
    LOG1("inserting %d elems", j + 1);

    for (i = 0; i <= j; i++)
    {
      err = cont->insert(cont, testdata[i].key, testdata[i].value);
      if (err)
        return err;
      // LOG2("insert: j=%d: now has %d nodes", j, cont->count(cont));
    }

    //cont->show_viz(cont, stdout);

    LOG1("deleting %d elems", j + 1);

    for (i = 0; i <= j; i++)
    {
      cont->remove(cont, testdata[i].key);
      // LOG2("remove: j=%d: now has %d nodes", j, cont->count(cont));
    }

    if (cont->count(cont) != 0)
    {
      LOG1("*** %d nodes counted but expected 0", cont->count(cont));
    }
  }

  // todo: pass in a vector of randomised testdata pointers

  LOG("Insert test values");

  for (i = 0; i < max; i++)
  {
    err = cont->insert(cont, testdata[i].key, testdata[i].value);
    if (err)
      return err;

    //LOG("Look up every key");

    found   = 0;
    matched = 0;
    for (j = 0; j <= i; j++)
    {
      const char *value;

      value = cont->lookup(cont, testdata[j].key);
      if (value == NULL)
      {
        LOG1("lookup didn't find %s", testdata[j].key);
      }
      else
      {
        found++;

        if (value != testdata[j].value)
          LOG1("values didn't match for key %s", testdata[j].key);
        else
          matched++;
      }
    }

    if (found < i + 1 || matched < i + 1)
    {
      LOG3("%s%d of %d keys found", found < i + 1 ? "*** " : "", found, i + 1);
      LOG3("%s%d of %d keys matched", matched < i + 1 ? "*** " : "", matched, i + 1);
    }
  }

  LOG("The fifth element contains:");

  if ((item = cont->select(cont, 5)) == NULL)
    printf(NAME ": -- (null)\n");
  else
    printf(NAME ": -- '%s' : '%s'\n", (const char *) item->key, (const char *) item->value);


  LOG("Look up keys by their prefix");

  /* prepare a count of testdata prefixes */

  memset(prefixcounts, 0, sizeof(prefixcounts));

  for (i = 0; i < max; i++)
    prefixcounts[(int) testdata[i].key[0] - 'A']++;

  prefixes_seen = 0;

  for (i = 'A'; i <= 'Z'; i++)
  {
    char prefix[2];
    int  count;

    prefix[0] = (char) i;
    prefix[1] = '\0';

    LOG1("Enumerate keys beginning with '%s':", prefix);

    count = 0;
    err = cont->lookup_prefix(cont, prefix, stringtest_lookup_prefix_callback, &count);
    if (err == error_OK || err == error_NOT_FOUND)
    {
      int expected = prefixcounts[i - 'A'];
      if (count != expected)
        LOG2("*** Incorrect number of keys seen! (got %d but expected %d)", count, expected);
      else
        LOG("ok!");
    }
    else if (err)
    {
      goto failure;
    }
  }

  if (prefixes_seen != NELEMS(testdata))
    LOG2("*** Incorrect number of prefixes seen! (got %d but expected %d)", prefixes_seen, NELEMS(testdata));
  else
    LOG("ok!");


  LOG("Enumerate keys which don't exist");

  for (i = 0; i < 256; i++)
  {
    char prefix[2];
    int  count;

    if (i >= 'A' && i <= 'Z')
      continue; /* these exist - skip them */

    prefix[0] = (char) i;
    prefix[1] = '\0';

    LOG1("Enumerate keys beginning with '<%d>':", i);

    count = 0;
    err = cont->lookup_prefix(cont, prefix, stringtest_lookup_prefix_callback, &count);
    if (err == error_OK || err == error_NOT_FOUND)
    {
      int expected = 0;
      if (count != expected)
        LOG2("*** Incorrect number of keys seen! (got %d but expected %d)", count, expected);
      else
        LOG("ok!");
    }
    else if (err)
    {
      goto failure;
    }
  }


  LOG("Look up a key which doesn't exist");

  err = cont->lookup_prefix(cont, "Gooseberries", stringtest_lookup_prefix_callback, NULL);
  if (err != error_NOT_FOUND)
  {
    LOG("lookup_prefix did _not_ return 'not found'!");
    goto failure;
  }

  LOG("Dump");

  cont->show(cont, stdout);

  LOG("Dump viz");

  viz_show(cont, testname, "dump");

  LOG("Remove every other test value");

  for (i = 0; i < max; i += 2)
    cont->remove(cont, testdata[i].key);

  LOG("Remove remaining test values");

  for (i = 1; i < max; i += 2)
    cont->remove(cont, testdata[i].key);

  LOG("Dump");

  cont->show(cont, stdout);

  LOG("Destroy");

  cont->destroy(cont);

  return error_OK;


failure:

  LOG1("error %lu\n", err);

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

static error commonprefixtest(icontainer_maker *maker, const char *testname)
{
  const int     max = NELEMS(commonprefixstrings);
  error         err;
  icontainer_t *cont;
  int           i;

  LOG("Create cont");

  err = maker(&cont, &static_string_key, &static_string_value);
  if (err)
    goto failure;

  LOG("Insert test values");

  for (i = 0; i < max; i++)
  {
    err = cont->insert(cont, commonprefixstrings[i].key, commonprefixstrings[i].value);
    if (err)
      return err;
  }

  LOG("Dump");

  cont->show(cont, stdout);

  LOG("Dump viz");

  viz_show(cont, testname, "dump");

  LOG("Destroy");

  cont->destroy(cont);

  return error_OK;


failure:

  LOG1("error %lu\n", err);

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

static error inttest(icontainer_maker *maker, const char *testname)
{
  const int     max = NELEMS(inttestdata);
  error         err;
  icontainer_t *cont;
  int           i;
  int           j;
  int           found;
  int           matched;
  const item_t *item;

  LOG("Create cont");

  err = maker(&cont, &static_int_key, &static_string_value);
  if (err)
    goto failure;

  LOG("Insert test values");

  for (i = 0; i < max; i++)
  {
    err = cont->insert(cont, &inttestdata[i].key, inttestdata[i].value);
    if (err)
      goto failure;

    LOG1("Count of elements = %d elements", cont->count(cont));

    //LOG("Look up every key");

    found   = 0;
    matched = 0;
    for (j = 0; j <= i; j++)
    {
      const char *value;

      value = cont->lookup(cont, &inttestdata[j].key);
      if (value == NULL)
      {
        LOG1("lookup didn't find %d", inttestdata[j].key);
      }
      else
      {
        found++;

        if (value != inttestdata[j].value)
          LOG1("values didn't match for key %d", inttestdata[j].key);
        else
          matched++;
      }
    }

    if (found < i + 1 || matched < i + 1)
    {
      LOG3("%s%d of %d keys found", found < i + 1 ? "*** " : "", found, i + 1);
      LOG3("%s%d of %d keys matched", matched < i + 1 ? "*** " : "", matched, i + 1);
    }
  }

  LOG("Select all keys in order");

  for (i = 0; i <= max; i++) /* note <= */
  {
    if ((item = cont->select(cont, i)) == NULL)
      printf(NAME ": -- (null)\n");
    else
      printf(NAME ": -- %x : '%s'\n", (unsigned int) item->key, (const char *) item->value);
  }

  LOG("Look up keys by their prefix");

  /* Prefix lookup tests aren't exactly relevant here. The key for 'dave'
   * won't get found. Since int key objects are always four bytes long we hit
   * the 'not found' case. A 'prefix' in this case is always as long as a
   * full key. */

  for (i = 0; i < max; i++)
  {
    int prefix;

    LOG1("Enumerate keys beginning with '%d':", i);

    prefix = i;

    err = cont->lookup_prefix(cont, &prefix, inttest_lookup_prefix_callback, NULL);
    if (err == error_NOT_FOUND)
      ;
    else if (err)
      goto failure;
  }

  LOG("Look up a key which doesn't exist");

  err = cont->lookup_prefix(cont, "Gooseberries", inttest_lookup_prefix_callback, NULL);
  if (err != error_NOT_FOUND)
  {
    LOG("lookup_prefix did _not_ return 'not found'!");
    goto failure;
  }

  LOG("Dump");

  cont->show(cont, stdout);

  LOG("Dump viz");

  viz_show(cont, testname, "dump");

  LOG("Remove every other test value");

  for (i = 0; i < max; i += 2)
    cont->remove(cont, &inttestdata[i].key);

  LOG("Remove remaining test values");

  for (i = 1; i < max; i += 2)
    cont->remove(cont, &inttestdata[i].key);

  LOG("Dump");

  cont->show(cont, stdout);

  LOG("Destroy");

  cont->destroy(cont);

  return error_OK;


failure:

  LOG1("error %lu\n", err);

  return err;
}

#undef NAME


/* ----------------------------------------------------------------------- */

#define E(c) { c - 'A' + 1, #c }

static const struct
{
  unsigned char key;
  const char   *value;
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
  {   0,   "0" },
  { 255, "255" },
};

#define NAME "chartest"

static error chartest(icontainer_maker *maker, const char *testname)
{
  const int     max = NELEMS(chartestdata);
  error         err;
  icontainer_t *cont;
  int           i;
  int           j;
  int           found;
  int           matched;
  const item_t *item;

  LOG("Create cont");

  err = maker(&cont, &static_char_key, &static_string_value);
  if (err)
    goto failure;

  LOG("Insert test values");

  for (i = 0; i < max; i++)
  {
    err = cont->insert(cont, &chartestdata[i].key, chartestdata[i].value);
    if (err)
      goto failure;

    LOG1("Count of elements = %d elements", cont->count(cont));

    viz_show(cont, testname, "insert");

    //LOG("Look up every key");

    found   = 0;
    matched = 0;
    for (j = 0; j <= i; j++)
    {
      const char *value;

      value = cont->lookup(cont, &chartestdata[j].key);
      if (value == NULL)
      {
        LOG1("lookup didn't find %d", chartestdata[j].key);
      }
      else
      {
        found++;

        if (value != chartestdata[j].value)
          LOG1("values didn't match for key %d", chartestdata[j].key);
        else
          matched++;
      }
    }

    if (found < i + 1 || matched < i + 1)
    {
      LOG3("%s%d of %d keys found", found < i + 1 ? "*** " : "", found, i + 1);
      LOG3("%s%d of %d keys matched", matched < i + 1 ? "*** " : "", matched, i + 1);
    }
  }

  LOG("The fifth element contains:");

  if ((item = cont->select(cont, 5)) == NULL)
    printf(NAME ": -- (null)\n");
  else
    printf(NAME ": -- %d : '%s'\n", *((char *) item->key), item->value);

  LOG("Dump");

  cont->show(cont, stdout);

  LOG("Dump viz");

  viz_show(cont, testname, "dump");

  LOG("Remove every other test value");

  for (i = 0; i < max; i += 2)
    cont->remove(cont, &chartestdata[i].key);

  LOG("Remove remaining test values");

  for (i = 1; i < max; i += 2)
    cont->remove(cont, &chartestdata[i].key);

  LOG("Dump");

  cont->show(cont, stdout);

  LOG("Destroy");

  cont->destroy(cont);

  return error_OK;


failure:

  LOG1("error %lu\n", err);

  return err;
}

#undef NAME

/* ----------------------------------------------------------------------- */

static error test_icontainer_type(icontainer_maker *maker,
                                  const char       *safemakername)
{
  static const struct
  {
    error     (*test)(icontainer_maker *maker, const char *testname);
    const char *name;
    const char *safename;
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

  for (i = 0; i < NELEMS(tests); i++)
  {
    char testname[256];

    printf("> test '%s' (%d of %d)\n", tests[i].name,
           i + 1,
           NELEMS(tests));

    sprintf(testname, "%s-%s", safemakername, tests[i].safename);

    viz_counter = 0;

    err = tests[i].test(maker, testname);
    if (err)
      goto failure;

    printf("<\n\n");
  }

  return error_OK;


failure:

  printf("test_container: error %ld\n", err);

  return err;
}

int test_container(int viz) // viz ignored now
{
  static const struct
  {
    icontainer_maker *maker;
    const char       *name;
    const char       *safename; /* safe for filenames: no spaces etc. */
  }
  makers[] =
  {
    { container_create_orderedarray, "ordered array", "orderedarray" },
    { container_create_linkedlist,   "linked list",   "linkedlist"   },
    { container_create_bstree,       "bstree",        "bstree"       },
    { container_create_dstree,       "dstree",        "dstree"       },
    { container_create_trie,         "trie",          "trie"         },
    { container_create_critbit,      "critbit",       "critbit"      },
    { container_create_patricia,     "patricia",      "patricia"     },
  };

  error err;
  int   i;

  for (i = 0; i < NELEMS(makers); i++)
  {
    printf(">> container '%s' (%d of %d)\n", makers[i].name,
           i + 1,
           NELEMS(makers));

    err = test_icontainer_type(makers[i].maker, makers[i].safename);
    if (err)
      goto failure;

    printf("<<\n\n");
  }

  return EXIT_SUCCESS;


failure:

  printf("test_container: error %ld\n", err);

  return EXIT_FAILURE;
}
