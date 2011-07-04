/* string-kv.h -- declare functions for keys or values which are strings */

#ifndef STRING_KV_H
#define STRING_KV_H

#include "kv.h"
#include "kv-common.h"

kv_len stringkv_len;
kv_compare stringkv_compare;
#define stringkv_destroy free;
#define stringkv_nodestroy kv_nodestroy
kv_fmt stringkv_fmt;
#define stringkv_fmt_destroy kv_fmtdestroy
#define stringkv_fmt_nodestroy kv_nofmtdestroy

#endif /* STRING_KV_H */
