/* hmap.h - simple hashmap. */
/*
# This code and its documentation is Copyright 2024-2024 Steven Ford, http://geeky-boy.com
# and licensed "public domain" style under Creative Commons "CC0": http://creativecommons.org/publicdomain/zero/1.0/
# To the extent possible under law, the contributors to this project have
# waived all copyright and related or neighboring rights to this work.
# In other words, you can use this code for any purpose without any
# restrictions.  This work is published from: United States.  The project home
# is https://github.com/fordsfords/hmap
*/

#ifndef HMAP_H
#define HMAP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "err.h"

/* Linked list node for handling collisions */
typedef struct hmap_node {
    void *key;
    size_t key_size;
    void *value;
    struct hmap_node *next;
    uint32_t bucket;  /* Bucket that this node is under. */
} hmap_node_t;

typedef struct {
    size_t table_size;
    uint32_t seed;
    hmap_node_t **table;
    int num_entries;
} hmap_t;


#define HMAP_ERR_PARAM 1
#define HMAP_ERR_NOMEM 2
#define HMAP_ERR_NOTFOUND 3


uint32_t hmap_murmur3_32(const void *key, size_t len, uint32_t seed);

ERR_F hmap_create(hmap_t **rtn_hmap, size_t table_size);

ERR_F hmap_delete(hmap_t *hmap);

ERR_F hmap_write(hmap_t *hmap, void *key, size_t key_size, void *val);

ERR_F hmap_lookup(hmap_t *hmap, void *key, size_t key_size, void **rtn_val);

ERR_F hmap_next(hmap_t *hmap, hmap_node_t **in_node);

#ifdef __cplusplus
}
#endif

#endif  /* HMAP_H */
