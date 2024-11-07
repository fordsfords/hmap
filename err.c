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
#include "err.h"


err_t *err_throw(char *file, int line, int code, char *mesg)
{
  err_t *err = NULL;
  err = (err_t *)calloc(1, sizeof(err_t));
  if (err == NULL) {
    fprintf(stderr, "err_throw: calloc error, aborting.\n"); fflush(stderr);
    abort();
  }

  if (mesg != NULL) {
    err->mesg = strdup(mesg);
    if (err->mesg == NULL) {
      fprintf(stderr, "err_throw: strdup error, aborting.\n"); fflush(stderr);
      abort();
    }
  } else {
    err->mesg = NULL;
  }

  err->file = file;
  err->line = line;
  err->code = code;
  err->stacktrace = NULL;

  return err;
}  /* err_throw */


err_t *err_rethrow(char *file, int line, err_t *in_err, int code, char *mesg)
{
  err_t *new_err = NULL;
  new_err = (err_t *)calloc(1, sizeof(err_t));
  if (new_err == NULL) {
    fprintf(stderr, "err_rethrow: calloc error, aborting.\n"); fflush(stderr);
    abort();
  }

  if (mesg != NULL) {
    new_err->mesg = strdup(mesg);
    if (mesg == NULL) {
      fprintf(stderr, "err_rethrow: strdup error, aborting.\n"); fflush(stderr);
      abort();
    }
  } else {
    new_err->mesg = NULL;
  }

  new_err->file = file;
  new_err->line = line;
  new_err->code = code;
  new_err->stacktrace = in_err;

  return new_err;
}  /* err_rethrow */


void err_print(err_t *err, FILE *stream)
{
  while (err != NULL) {
    fprintf(stream, "File: %s\nLine: %d\nCode: %d\nMesg: %s\n",
      err->file, err->line, err->code,
      (err->mesg == NULL) ? ("(no mesg)") : (err->mesg));
    if (err->stacktrace != NULL) {
      fprintf(stream, "----------------\n");
    }
    fflush(stream);

    err = err->stacktrace;
  }
}  /* err_print */


void err_dispose(err_t *err)
{
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


char *err_asprintf(const char *format, ...) {
  va_list args1;
  va_list args2;
  va_start(args1, format);
  va_copy(args2, args1);  /* Make a copy since we need to use it twice */

  int size = vsnprintf(NULL, 0, format, args1) + 1;  /* +1 for null terminator */
  va_end(args1);

  if (size < 0) {
    va_end(args2);
    return NULL;
  }

  /* Allocate buffer. */
  char *str = malloc(size);
  if (str == NULL) {
    va_end(args2);
    return NULL;
  }

  int result = vsnprintf(str, size, format, args2);
  va_end(args2);

  if (result < 0) {
    free(str);
    return NULL;
  }

  return str;
}  /* err_asprintf */
