/* hmape.c - simple hashmap in C. */
/*
# This code and its documentation is Copyright 2024-2024 Steven Ford, http://geeky-boy.com
# and licensed "public domain" style under Creative Commons "CC0": http://creativecommons.org/publicdomain/zero/1.0/
# To the extent possible under law, the contributors to this project have
# waived all copyright and related or neighboring rights to this work.
# In other words, you can use this code for any purpose without any
# restrictions.  This work is published from: United States.  The project home
# is https://github.com/fordsfords/hmap
*/

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "hmape.h"


int hmape_create(hmap_t **rtn_hmap, size_t table_size, err_t *err) {
  if (err) { err->code = HMAP_OK; }
  int err_code = hmap_create(rtn_hmap, table_size);
  ERR_ASSRT(err_code == HMAP_OK, err_code, err);
  return err_code;
}  /* hmape_create */


int hmape_write(hmap_t *hmap, void *key, size_t key_size, void *val, err_t *err) {
  if (err) { err->code = HMAP_OK; }
  int err_code = hmap_write(hmap, key, key_size, val);
  ERR_ASSRT(err_code == HMAP_OK, err_code, err);
  return err_code;
}  /* hmape_write */


int hmape_lookup(hmap_t *hmap, void *key, size_t key_size, void **rtn_val, err_t *err) {
  if (err) { err->code = HMAP_OK; }
  int err_code = hmap_lookup(hmap, key, key_size, rtn_val);
  ERR_ASSRT(err_code == HMAP_OK, err_code, err);
  return err_code;
}  /* hmape_lookup */
