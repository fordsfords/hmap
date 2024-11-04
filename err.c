/* err.c - support functions for simple error handler. */
/*
# This code and its documentation is Copyright 2024-2024 Steven Ford, http://geeky-boy.com
# and licensed "public domain" style under Creative Commons "CC0": http://creativecommons.org/publicdomain/zero/1.0/
# To the extent possible under law, the contributors to this project have
# waived all copyright and related or neighboring rights to this work.
# In other words, you can use this code for any purpose without any
# restrictions.  This work is published from: United States.  The project home
# is https://github.com/fordsfords/err
*/

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdint.h>
#include "err.h"

#define ERR(err_code) char *err_code = #err_code
ERR(ERR_CODE_PARAM);
ERR(ERR_CODE_NOMEM);
#undef ERR

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
