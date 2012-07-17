/* --------------------------------------------------------------------------
 *    Name: errors.h
 * Purpose: Error type and constants
 * ----------------------------------------------------------------------- */

#ifndef ERRORS_H
#define ERRORS_H

typedef unsigned long int error;

/* Generic errors */

#define error_OK                    0ul /* No error */
#define error_OOM                   1ul /* Out of memory */
#define error_NOT_IMPLEMENTED       2ul /* Function not implemented */
#define error_NOT_FOUND             3ul /* Item not found */
#define error_EXISTS                4ul /* Item already exists */
#define error_STOP_WALK             5ul /* Callback was cancelled */

/* Data structure errors */

#define error_CLASHES             100ul /* Key would clash with existing one */

#define error_QUEUE_FULL          110ul
#define error_QUEUE_EMPTY         111ul

#define error_HASH_END            120ul
#define error_HASH_BAD_CONT       121ul

/* Container errors */

#define error_KEYLEN_REQUIRED     200ul
#define error_KEYCOMPARE_REQUIRED 201ul
#define error_KEYHASH_REQIURED    202ul

/* Test errors */

#define error_TEST_FAILED         300ul

#endif /* ERRORS_H */

