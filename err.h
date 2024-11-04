/* err.h - simple error handler. */
/*
# This code and its documentation is Copyright 2024-2024 Steven Ford, http://geeky-boy.com
# and licensed "public domain" style under Creative Commons "CC0": http://creativecommons.org/publicdomain/zero/1.0/
# To the extent possible under law, the contributors to this project have
# waived all copyright and related or neighboring rights to this work.
# In other words, you can use this code for any purpose without any
# restrictions.  This work is published from: United States.  The project home
# is https://github.com/fordsfords/err
*/

#ifndef ERR_H
#define ERR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>


#define ERR_ABRT(err_code, err_msg) do { \
  fprintf(stderr, "ERR_ABRT: [%s:%d %s()] code=%s, '%s'\n", \
      __FILE__, __LINE__, __func__, err_code, err_msg); \
  fflush(stderr); \
  abort(); \
} while (0)


#define ERR_THROW(err_msg, err_code, err_ptr) do { \
  if (err_ptr) { \
    (err_ptr)->code = (err_code); \
    (err_ptr)->msg = strdup(err_msg); \
    (err_ptr)->file = __FILE__; \
    (err_ptr)->func = __func__; \
    (err_ptr)->line = __LINE__; \
    (err_ptr)->next = NULL; \
    return (err_ptr)->code; \
  } else { \
    ERR_ABRT(err_code, err_msg); \
  } \
} while (0)


#define ERR_ASSRT(err_cond, err_code, err_ptr) do { \
  int err_assrt_result = (err_cond); \
  if (! err_assrt_result) { \
    char *err_msg = err_asprintf("Assertion failed: %s", #err_cond); \
    if (err_msg) { \
      ERR_THROW(err_msg, err_code, err_ptr); \
      free(err_msg); \
    } else { \
      ERR_THROW("???", err_code, err_ptr); \
    } \
  } \
} while (0)


#define ERR_RTN_OK(err_ptr) do { \
  if (err_ptr) { err_ptr->code = ERR_OK; } \
  return ERR_OK; \
} while (0)


struct err_s;  /* forward definition. */
typedef struct err_s err_t;
struct err_s {
  char *code;
  const char *msg;
  const char *file;
  const char *func;
  int line;
  err_t *next;
};

#define ERR_OK NULL

#define ERR(err_code) extern char *err_code
ERR(ERR_CODE_PARAM);
ERR(ERR_CODE_NOMEM);
#undef ERR


char *err_asprintf(const char *format, ...);

#ifdef __cplusplus
}
#endif

#endif // ERR_H
