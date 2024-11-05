/* hmape.h - simple hashmap. */
/*
# This code and its documentation is Copyright 2024-2024 Steven Ford, http://geeky-boy.com
# and licensed "public domain" style under Creative Commons "CC0": http://creativecommons.org/publicdomain/zero/1.0/
# To the extent possible under law, the contributors to this project have
# waived all copyright and related or neighboring rights to this work.
# In other words, you can use this code for any purpose without any
# restrictions.  This work is published from: United States.  The project home
# is https://github.com/fordsfords/hmap
*/

#ifndef EHMAP_H
#define EHMAP_H

#include <stdint.h>
#include "err.h"
#include "hmap.h"

#ifdef __cplusplus
extern "C" {
#endif

char *hmape_create(hmap_t **rtn_hmap, size_t table_size, err_t *err);

char *hmape_delete(hmap_t *hmap, err_t *err);

char *hmape_write(hmap_t *hmap, void *key, size_t key_size, void *val, err_t *err);

char *hmape_lookup(hmap_t *hmap, void *key, size_t key_size, void **rtn_val, err_t *err);

char *hmape_next(hmap_t *hmap, hmap_node_t **in_node, err_t *err);

#ifdef __cplusplus
}
#endif

#endif // EHMAP_H
