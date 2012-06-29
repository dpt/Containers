/* --------------------------------------------------------------------------
 *    Name: char.h
 * Purpose: Functions for keys or values which are chars
 * ----------------------------------------------------------------------- */

#ifndef CHAR_KV_H
#define CHAR_KV_H

#include "keyval/kv.h"
#include "keyval/common.h"

kv_len charkv_len;
kv_compare charkv_compare;
#define charkv_destroy free
#define charkv_nodestroy kv_nodestroy
kv_hash charkv_hash;
kv_fmt charkv_fmt;
#define charkv_fmt_destroy kv_fmtdestroy
#define charkv_fmt_nodestroy kv_nofmtdestroy

#endif /* CHAR_KV_H */

