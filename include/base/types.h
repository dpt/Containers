/* --------------------------------------------------------------------------
 *    Name: types.h
 * Purpose: Various typedefs and utility macros
 * ----------------------------------------------------------------------- */

#ifndef TYPES_H
#define TYPES_H

typedef signed char    int8_t;
typedef signed short   int16_t;
typedef signed int     int32_t;

typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;

#ifdef _MSC_VER
#ifdef _WIN64
typedef __int64 intptr_t;
#else
typedef int intptr_t;
#endif
#endif

#define NELEMS(x) ((int) (sizeof(x) / sizeof(x[0])))

#define MIN(x,y) ((x) < (y) ? (x) : (y))
#define MAX(x,y) ((x) > (y) ? (x) : (y))

#define NOT_USED(x) ((x) = (x))

#ifdef _WIN32
#define INLINE __inline
#else
#define INLINE __inline__
#endif

#ifdef __GNUC__
#define likely(x)   __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)
#else
#define likely(x)   (x)
#define unlikely(x) (x)
#endif

#endif /* TYPES_H */

