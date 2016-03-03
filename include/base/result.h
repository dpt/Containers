/* result.h -- generic function return values */

#ifndef BASE_RESULT_H
#define BASE_RESULT_H

typedef int result_t;

/* ----------------------------------------------------------------------- */

/* DPTLib bases */
#define result_BASE_GENERIC                     0x0000
#define result_BASE_STREAM                      0x0100
#define result_BASE_ATOM                        0x0200
#define result_BASE_HASH                        0x0300
#define result_BASE_PICKLE                      0x0400
#define result_BASE_TAGDB                       0x0500
#define result_BASE_FILENAMEDB                  0x0600
#define result_BASE_TEST                        0x0700
#define result_BASE_PACKER                      0x0800
#define result_BASE_LAYOUT                      0x0900

/* Non-DPTLib bases */
#define result_BASE_MMPLAYER                    0xF000
#define result_BASE_CONTAINER                   0xF100
#define result_BASE_CONTAINER_HASH              0xF200
#define result_BASE_CONTAINER_QUEUE             0xF300

/* ----------------------------------------------------------------------- */

#define result_OK                               (result_BASE_GENERIC     + 0)   /* No error */
#define result_OOM                              (result_BASE_GENERIC     + 1)   /* Out of memory */
#define result_FILE_NOT_FOUND                   (result_BASE_GENERIC     + 2)
#define result_BAD_ARG                          (result_BASE_GENERIC     + 3)   /* An argument was unacceptable */
#define result_BUFFER_OVERFLOW                  (result_BASE_GENERIC     + 4)
#define result_STOP_WALK                        (result_BASE_GENERIC     + 5)   /* Used to cancel callbacks */
#define result_PARSE_ERROR                      (result_BASE_GENERIC     + 6)
#define result_TOO_BIG                          (result_BASE_GENERIC     + 7)
#define result_NOT_IMPLEMENTED                  (result_BASE_GENERIC     + 8)   /* Function not implemented */
#define result_NOT_FOUND                        (result_BASE_GENERIC     + 9)   /* Item not found */
#define result_EXISTS                           (result_BASE_GENERIC     + 10)  /* Item already exists */
#define result_CLASHES                          (result_BASE_GENERIC     + 11)  /* Key clashes with existing one */
#define result_NULL_ARG                         (result_BASE_GENERIC     + 12)  /* An argument was NULL */

/* Stream result codes are in io/stream.h */

/* Atom result codes are in datastruct/atom.h */

/* Hash result codes are in datastruct/hash.h */

/* Pickle result codes are in databases/pickle.h */

/* TagDB result codes are in databases/tag-db.h */

/* FilenameDB result codes are in databases/filename-db.h */

#define result_TEST_PASSED                      (result_BASE_TEST        + 0)
#define result_TEST_FAILED                      (result_BASE_TEST        + 1)

/* Packer result codes are in geom/packer.h */

/* Layout result codes are in geom/layout.h */

/* ----------------------------------------------------------------------- */

#endif /* BASE_RESULT_H */
