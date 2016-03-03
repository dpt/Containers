/* debug.h -- debugging and logging macros */

#ifndef BASE_DEBUG_H
#define BASE_DEBUG_H

#include <stdio.h>

/* Log a message - available in all builds. */
#define logf_info(fmt, ...)    fprintf(stderr, "(info) " fmt "\n", ##__VA_ARGS__)
#define logf_warning(fmt, ...) fprintf(stderr, "(warning) " fmt "\n", ##__VA_ARGS__)
#define logf_error(fmt, ...)   fprintf(stderr, "(ERROR) %s:%d " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#define logf_fatal(fmt, ...)   fprintf(stderr, "(FATAL) %s:%d " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#define logf_abort(fmt, ...)   fprintf(stderr, "(ABORT) %s:%d " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)

/* Log a debug message - available only in debug builds. */
#ifndef NDEBUG
#  define logf_debug(fmt, ...) fprintf(stderr, "(debug) " fmt "\n", ##__VA_ARGS__)
#  define check(err)           do { if (err) { logf_error("err=%x", err); goto failure; } } while (0)
#  define sentinel             do { logf_error("sentinel"); goto failure; } while (0)
#else
#  define logf_debug(fmt, ...)
#  define check(err)
#  define sentinel
#endif

#define haltf(...)             do { logf_abort(__VA_ARGS__); abort(); } while (0)

#endif /* BASE_DEBUG_H */
