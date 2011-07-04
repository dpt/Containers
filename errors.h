/* errors.h */

#ifndef ERRORS_H
#define ERRORS_H

typedef unsigned long int error;

#define error_OK                  0
#define error_EXISTS              1
#define error_NOT_FOUND           2
#define error_OOM                 3
#define error_STOP_WALK           4

#define error_DICT_NAME_EXISTS    100
#define error_DICT_OUT_OF_RANGE   101

#define error_KEYLEN_REQUIRED     200
#define error_KEYCOMPARE_REQUIRED 201

#define error_QUEUE_FULL          300
#define error_QUEUE_EMPTY         301

#define error_TEST_FAILED         400

#endif /* ERRORS_H */
