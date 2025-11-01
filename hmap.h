/* hmap.h - simple hashmap. */

/* This work is dedicated to the public domain under CC0 1.0 Universal:
 * http://creativecommons.org/publicdomain/zero/1.0/
 * 
 * To the extent possible under law, Steven Ford has waived all copyright
 * and related or neighboring rights to this work. In other words, you can 
 * use this code for any purpose without any restrictions.
 * This work is published from: United States.
 * Project home: https://github.com/fordsfords/hmap
 */

#ifndef HMAP_H
#define HMAP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "err.h"

/* Linked list of entries for handling collisions */
typedef struct hmap_entry_s hmap_entry_t;  /* Forward definition. */
struct hmap_entry_s {
    void *key;
    size_t key_size;
    void *value;
    hmap_entry_t *next;
    uint32_t bucket;  /* Bucket that this entry is under. */
};

typedef struct hmap_s hmap_t;
struct hmap_s {
    size_t table_size;
    uint32_t seed;
    hmap_entry_t **table;
    int num_entries;
};


#ifdef HMAP_C
#  define ERR_CODE(err__code) ERR_API char *err__code = #err__code
#else
#  define ERR_CODE(err__code) ERR_API extern char *err__code
#endif

ERR_CODE(HMAP_ERR_PARAM);
ERR_CODE(HMAP_ERR_NOMEM);
ERR_CODE(HMAP_ERR_NOTFOUND);

#undef ERR_CODE


uint32_t hmap_murmur3_32(const void *key, size_t len, uint32_t seed);

ERR_F hmap_create(hmap_t **rtn_hmap, size_t table_size);

ERR_F hmap_delete(hmap_t *hmap);

ERR_F hmap_write(hmap_t *hmap, const void *key, size_t key_size, void *val);

ERR_F hmap_lookup(hmap_t *hmap, const void *key, size_t key_size, void **rtn_val);

ERR_F hmap_swrite(hmap_t *hmap, const char *key, void *val);

ERR_F hmap_slookup(hmap_t *hmap, const char *key, void **rtn_val);

ERR_F hmap_next(hmap_t *hmap, hmap_entry_t **in_entry);

#ifdef __cplusplus
}
#endif

#endif  /* HMAP_H */
