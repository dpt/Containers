/* int-kv.h -- declare functions for keys or values which are ints */

#ifndef INT_KV_H
#define INT_KV_H

#include "kv.h"
#include "kv-common.h"

kv_len intkv_len;
kv_compare intkv_compare;
#define intkv_destroy free
#define intkv_nodestroy kv_nodestroy
kv_fmt intkv_fmt;
#define intkv_fmt_destroy kv_fmtdestroy
#define intkv_fmt_nodestroy kv_nofmtdestroy

#endif /* INT_KV_H */
