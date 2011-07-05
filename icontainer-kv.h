/* icontainer-kv.h -- factored-out common interface of keys and values */

#ifndef ICONTAINER_KV_H
#define ICONTAINER_KV_H

/* Destroy the specified key. */
typedef void (*icontainer_kv_destroy)(void *kv);

/* Turn the specified key into something printable. */
typedef const char *(*icontainer_kv_show)(const void *kv);

/* Destroy the object 'show' returned. */
typedef void (*icontainer_kv_show_destroy)(char *doomed);

typedef struct icontainer_kv
{
  icontainer_kv_destroy      destroy;
  icontainer_kv_show         show;
  icontainer_kv_show_destroy show_destroy; /* NULL => nothing to destroy */
}
icontainer_kv_t;

#endif /* ICONTAINER_KV_H */
