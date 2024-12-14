/* err.c - Error handling infrastructure.
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

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#define ERR_C
#include "err.h"


/* Wrap a few functions to be ERR-compliant. */
ERR_F err_calloc(void **rtn_ptr, size_t nmemb, size_t size) {
  ERR_ASSRT(rtn_ptr, ERR_ERR_PARAM);
  ERR_ASSRT(nmemb > 0, ERR_ERR_PARAM);
  ERR_ASSRT(size > 0, ERR_ERR_PARAM);
  ERR_ASSRT(*rtn_ptr = calloc(nmemb, size), ERR_ERR_NOMEM);

  return ERR_OK;
}  /* err_calloc */


ERR_F err_strdup(char **rtn_str, const char *src_str) {
  ERR_ASSRT(src_str, ERR_ERR_PARAM);
  ERR_ASSRT(rtn_str, ERR_ERR_PARAM);

  size_t size = strlen(src_str) + 1;
  ERR_ASSRT(*rtn_str = malloc(size), ERR_ERR_NOMEM);

  memcpy(*rtn_str, src_str, size);

  return ERR_OK;
}  /* err_strdup */


ERR_F err_atol(const char *in_str, long *rtn_value) {
  int base = 10;

  if (in_str[0] == '0' && (in_str[1] == 'x' || in_str[1] == 'X')) {
    base = 16;
    in_str += 2;  /* Step past the "0x". */
  }

  errno = 0;  /* Best practice when using strtol. */
  char *p = NULL;
  long value = strtol(in_str, &p, base);
  /* Check for error. */
  if (errno != 0 || p == in_str || p == NULL || *p != '\0') {
    ERR_THROW(ERR_ERR_BAD_NUMBER, in_str);
  }

  *rtn_value = value;
  return ERR_OK;
}  /* err_atol */


/* Thanks to Claude.ai for helping me write this vararg code. */

char *err_vasprintf(const char *format, va_list args) {
  va_list args_copy;
  va_copy(args_copy, args);

  /* Get required size */
  int size = vsnprintf(NULL, 0, format, args) + 1;
  if (size < 0) {
    va_end(args_copy);
    return NULL;
  }

  /* Allocate buffer */
  char *str = malloc(size);
  if (str == NULL) {
    va_end(args_copy);
    return NULL;
  }

  /* Format string */
  int result = vsnprintf(str, size, format, args_copy);
  va_end(args_copy);

  if (result < 0) {
    free(str);
    return NULL;
  }

  return str;
}  /* err_vasprintf */


ERR_F err_asprintf(char **rtn_str, const char *format, ...) {
  va_list args;
  va_start(args, format);
  char *result = err_vasprintf(format, args);
  va_end(args);

  ERR_ASSRT(result, ERR_ERR_NOMEM);
  *rtn_str = result;

  return ERR_OK;
}  /* err_asprintf */


err_t *err_throw_v(const char *file, int line, const char *func, char *code, const char *format, ...) {
  err_t *err = NULL;
  err = (err_t *)calloc(1, sizeof(err_t));
  if (err == NULL) {
    fprintf(stderr, "err_throw: calloc error, aborting.\n");  fflush(stderr);
    abort();
  }

  if (format != NULL) {
    va_list args;
    va_start(args, format);
    err->mesg = err_vasprintf(format, args);
    va_end(args);

    if (err->mesg == NULL) {
      fprintf(stderr, "err_throw: message formatting error, aborting.\n");  fflush(stderr);
      abort();
    }
  } else {
    err->mesg = NULL;
  }

  err->file = file;
  err->line = line;
  err->func = func;
  err->code = code;
  err->stacktrace = NULL;

  return err;
}  /* err_throw_v */


err_t *err_rethrow_v(const char *file, int line, const char *func, err_t *in_err, const char *format, ...) {
  err_t *new_err = NULL;
  new_err = (err_t *)calloc(1, sizeof(err_t));
  if (new_err == NULL) {
    fprintf(stderr, "err_rethrow_v: calloc error, aborting.\n"); fflush(stderr);
    abort();
  }

  if (format != NULL) {
    va_list args;
    va_start(args, format);
    new_err->mesg = err_vasprintf(format, args);
    va_end(args);

    if (new_err->mesg == NULL) {
      fprintf(stderr, "err_rethrow_v: message formatting error, aborting.\n");  fflush(stderr);
      abort();
    }
  } else {
    new_err->mesg = NULL;
  }

  new_err->file = file;
  new_err->line = line;
  new_err->func = func;
  new_err->code = in_err->code;
  new_err->stacktrace = in_err;

  return new_err;
}  /* err_throw_v */


void err_print(err_t *err, FILE *stream) {
  while (err != NULL) {
    fprintf(stream, "[%s:%d %s()]: Code: %s, Mesg: %s\n",
      err->file, err->line, err->func, err->code,
      (err->mesg == NULL) ? ("(no mesg)") : (err->mesg));
    if (err->stacktrace != NULL) {
      fprintf(stream, "----------------\n");
    }
    fflush(stream);

    err = err->stacktrace;
  }
}  /* err_print */


void err_dispose(err_t *err) {
  while (err != NULL) {
    err_t *next_err = err->stacktrace;
    if (err->mesg != NULL) {
      free(err->mesg);
      err->mesg = NULL;
    }
    free(err);
    err = next_err;
  }
}  /* err_dispose */
