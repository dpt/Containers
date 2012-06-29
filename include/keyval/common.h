/* --------------------------------------------------------------------------
 *    Name: common.h
 * Purpose: Functions common across key and value types
 * ----------------------------------------------------------------------- */

#ifndef KV_COMMON_H
#define KV_COMMON_H

#include "keyval/kv.h"

/* Function of type kv_destroy which does nothing.
 * Used when the type has been allocated statically. */
kv_destroy kv_nodestroy;

/* Function of type kv_fmt_destroy which frees its argument.
 * Used when the type has been allocated with malloc. */
kv_fmt_destroy kv_fmtdestroy;

/* Function of type kv_fmt_destroy which does nothing.
 * Used when the type has been allocated statically. */
kv_fmt_destroy kv_nofmtdestroy;

#endif /* KV_COMMON_H */

