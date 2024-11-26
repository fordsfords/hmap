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
#include <stdarg.h>

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

/* The "err.c" module needs to define (and allocate space for)
 * error codes. Other modules just declare them extern. */
#ifdef ERR_C  /* If "err.c" is being compiled. */
#  define ERR_CODE(err__code) ERR_API char *err__code = #err__code  /* Define them. */
#else
#  define ERR_CODE(err__code) ERR_API extern char *err__code  /* Declare them. */
#endif

ERR_CODE(ERR_ERR_PARAM);
ERR_CODE(ERR_ERR_NOMEM);
ERR_CODE(ERR_ERR_INTERNAL);
#undef ERR_CODE


/* Applications that return an err_t should be declared with this macro. */
#define ERR_F __attribute__ ((__warn_unused_result__)) err_t *


/* Throwing an error means creating an err object and returning it. */
#define ERR_THROW(err__code, ...) do { \
    return err_throw_v(__FILE__, __LINE__, __func__, err__code, __VA_ARGS__); \
} while (0)


/* Explicit re-throw. */
#define ERR_RETHROW(err__err, ...) do { \
  return err_rethrow_v(__FILE__, __LINE__, __func__, err__err, __VA_ARGS__); \
} while (0)


/* Implicit re-throw. */
#define ERR(err__funct_call) do { \
  err_t *err__err = (err__funct_call); \
  if (err__err) { \
    return err_rethrow_v(__FILE__, __LINE__, __func__, err__err, err__err->code, #err__funct_call); \
  } \
} while (0)


/* Assert/throw combines sanity test with throw. */
#define ERR_ASSRT(err__cond_expr, err__code) do { \
  if (!(err__cond_expr)) { \
    return err_throw_v(__FILE__, __LINE__, __func__, err__code, #err__cond_expr); \
  } \
} while (0)


/* Prints stack trace to stderr and disposes err structure. */
#define ERR_WARN_ON_ERR(err__funct_call, err__stream) do { \
  err_t *err__err = (err__funct_call); \
  if (err__err) { \
    /* include ERR_ABRT() caller in stack trace. */ \
    err__err = err_rethrow_v(__FILE__, __LINE__, __func__, err__err, #err__funct_call); \
    fprintf(err__stream, "ERR_ABRT\nStack trace:\n----------------\n"); \
    err_print(err__err, err__stream); \
    fflush(err__stream); \
    err_dispose(err__err); \
  } \
} while (0)


/* Prints stack trace to stderr and calls "exit(1)". */
#define ERR_EXIT_ON_ERR(err__funct_call, err__stream) do { \
  err_t *err__err = (err__funct_call); \
  if (err__err) { \
    /* include ERR_ABRT() caller in stack trace. */ \
    err__err = err_rethrow_v(__FILE__, __LINE__, __func__, err__err, #err__funct_call); \
    fprintf(err__stream, "ERR_ABRT\nStack trace:\n----------------\n"); \
    err_print(err__err, err__stream); \
    fflush(err__stream); \
    exit(1); \
  } \
} while (0)


/* Prints stack trace to stderr and calls "abort()" to generate core file. */
#define ERR_ABRT_ON_ERR(err__funct_call, err__stream) do { \
  err_t *err__err = (err__funct_call); \
  if (err__err) { \
    /* include ERR_ABRT() caller in stack trace. */ \
    err__err = err_rethrow_v(__FILE__, __LINE__, __func__, err__err, #err__funct_call); \
    fprintf(err__stream, "ERR_ABRT\nStack trace:\n----------------\n"); \
    err_print(err__err, err__stream); \
    fflush(err__stream); \
    abort(); \
  } \
} while (0)


/* Internal structure of err object. Application is allowed to peek. */
typedef struct err_s err_t;  /* Forward def. */
struct err_s {
  char *code;
  const char *file;
  int line;
  const char *func;
  char *mesg;  /* Separately malloced. */
  err_t *stacktrace;  /* Linked list. */
};


/* Print stack trace to an open FILE pointer (like stderr). */
ERR_API void err_print(err_t *err, FILE *stream);

/* If an error is handled and not re-thrown, the err object must be deleted. */
ERR_API void err_dispose(err_t *err);

/* Helper functions for "sprintf()" style functions that malloc their own buffer. */
ERR_API char *err_vasprintf(const char *format, va_list args);
ERR_API char *err_asprintf(const char *format, ...);


/* These generally should not be called directly by applications. The
 * macro forms are preferred.
 */
ERR_API err_t *err_throw_v(const char *file, int line, const char *func, char *code, const char *format, ...);
ERR_API err_t *err_rethrow_v(const char *file, int line, const char *func, err_t *in_err, const char *format, ...);

ERR_API ERR_F err_strdup(char **dst_str, const char *src_str);
ERR_API ERR_F err_calloc(void **rtn_ptr, size_t nmemb, size_t size);


#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif  /* ERR_H */
