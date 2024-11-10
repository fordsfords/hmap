/* err.h - Error handling infrastructure.
 * See https://github.com/fordsfords/err for documentation.
 *
 * This code and its documentation is Copyright 2019, 2019 Steven Ford,
 * http://geeky-boy.com and licensed "public domain" style under Creative
 * Commons "CC0": http://creativecommons.org/publicdomain/zero/1.0/
 * To the extent possible under law, the contributors to this project have
 * waived all copyright and related or neighboring rights to this work.
 * In other words, you can use this code for any purpose without any
 * restrictions. This work is published from: United States. The project home
 * is https://github.com/fordsfords/err
 */

#ifndef ERR_H
#define ERR_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/* For a Windows build of err.c, set the preprocessor symbol ERR_EXPORTS
 * if you want this subsystem to be available from a DLL.
 */
#if defined(_WIN32)
#  ifdef ERR_EXPORTS
#    define ERR_API __declspec(dllexport)
#  else
#    define ERR_API __declspec(dllimport)
#  endif
#else
#  define ERR_API
#endif

#define ERR_OK NULL

/* Simple macro to skip past the dir name of a full path (if any). */
#if defined(_WIN32)
#  define ERR_BASENAME(err__p)\
    ((strrchr(err__p, '\\') == NULL) ? (err__p) : (strrchr(err__p, '\\')+1))
#else
#  define ERR_BASENAME(err__p)\
    ((strrchr(err__p, '/') == NULL) ? (err__p) : (strrchr(err__p, '/')+1))
#endif

/* Applications that return an err_t should be declared with this macro. */
#define ERR_F __attribute__ ((__warn_unused_result__)) err_t *


/* Throwing an error means creating an err object and returning it. */
#define ERR_THROW(err__code, err__mesg) do { \
  return err_throw(__FILE__, __LINE__, err__code, err__mesg); \
} while (0)


/* Assert/throw combines sanity test with throw. */
#define ERR_ASSRT(err__cond_expr, err__code) do { \
  if (!(err__cond_expr)) { \
    return err_throw(__FILE__, __LINE__, err__code, #err__cond_expr); \
  } \
} while (0)


/* Shortcut abort-on-error macro. Prints stack trace to stderr. */
#define ERR_ABRT(err__funct_call, err__stream) do { \
  err_t *err__err = (err__funct_call); \
  if (err__err) { \
    err__err = err_rethrow(__FILE__, __LINE__, err__err, err__err->code, #err__funct_call); \
    fprintf(err__stream, "ERR_ABRT Failed!\nStack trace:\n----------------\n"); \
    err_print(err__err, err__stream); \
    fflush(err__stream); \
    abort(); \
  } \
} while (0)


/* Implicit re-throw. */
#define ERR(err__funct_call) do { \
  err_t *err__err = (err__funct_call); \
  if (err__err) { \
    return err_rethrow(__FILE__, __LINE__, err__err, err__err->code, #err__funct_call); \
  } \
} while (0)


/* Explicit re-throw. */
#define ERR_RETHROW(err__err, err__code) do { \
  return err_rethrow(__FILE__, __LINE__, err__err, err__err->code, \
                     "Re-throwing " #err__err); \
} while (0)



/* Internal structure of err object. Application is allowed to peek. */
typedef struct err_s err_t;  /* Forward def. */
struct err_s {
  int code;
  char *file;
  int line;
  char *mesg;  /* Separately malloced. */
  err_t *stacktrace;  /* Linked list. */
};


/* Print stack trace to an open FILE pointer (like stderr). */
ERR_API void err_print(err_t *err, FILE *stream);

/* If an error is handled and not re-thrown, the err object must be deleted. */
ERR_API void err_dispose(err_t *err);


/* These generally should not be called directly by applications. The
 * macro forms are preferred.
 */
ERR_API err_t *err_throw(char *file, int line, int code, char *msg);
ERR_API err_t *err_rethrow(char *file, int line, err_t *in_err, int code, char *msg);
char *err_asprintf(const char *format, ...);


#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif  /* ERR_H */
