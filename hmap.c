/* hmap.c - simple hashmap in C. */
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
#include "err.h"
#include "hmap.h"


/* Murmur3 32-bit hash function. */
uint32_t hmap_murmur3_32(const void *key, size_t key_len, uint32_t seed) {
  const uint8_t *data = (const uint8_t*)key;
  uint32_t h1 = seed;

  const uint32_t c1 = 0xcc9e2d51;
  const uint32_t c2 = 0x1b873593;
  const int r1 = 15;
  const int r2 = 13;

  /* Process 4-byte chunks. */
  int nblocks = key_len / 4;
  for (int i = 0; i < nblocks; i++) {
    uint32_t k1;
    memcpy(&k1, &data[i], sizeof(k1));  /* Key might not be mem aligned. */

    k1 *= c1;
    k1 = (k1 << r1) | (k1 >> (32 - r1));
    k1 *= c2;

    h1 ^= k1;
    h1 = (h1 << r2) | (h1 >> (32 - r2));
    h1 = h1 * 5 + 0xe6546b64;
  }

  /* Handle remaining bytes */
  uint32_t k1 = 0;
  int tail_size = key_len & 3;
  if (tail_size >= 3) k1 ^= data[nblocks * 4 + 2] << 16;
  if (tail_size >= 2) k1 ^= data[nblocks * 4 + 1] << 8;
  if (tail_size >= 1) {
    k1 ^= data[nblocks * 4];
    k1 *= c1;
    k1 = (k1 << r1) | (k1 >> (32 - r1));
    k1 *= c2;
    h1 ^= k1;
  }

  /* Finalization */
  h1 ^= key_len;
  h1 ^= (h1 >> 16);
  h1 *= 0x85ebca6b;
  h1 ^= (h1 >> 13);
  h1 *= 0xc2b2ae35;
  h1 ^= (h1 >> 16);

  return h1;
}  /* hmap_murmur3_32 */


ERR_F hmap_create(hmap_t **rtn_hmap, size_t table_size) {
  hmap_t *hmap;

  ERR_ASSRT(table_size != 0, HMAP_ERR_PARAM);

  hmap = calloc(1, sizeof(hmap_t));
  ERR_ASSRT(hmap, HMAP_ERR_NOMEM);

  (hmap)->table_size = table_size;
  (hmap)->seed = 42;  /* Could be made in input parameter. */
  (hmap)->num_entries = 0;
  (hmap)->table = calloc(table_size, sizeof(hmap_entry_t*));
  if (!(hmap)->table) {
    free(hmap);
    ERR_THROW(HMAP_ERR_NOMEM, "hmap->table");
  }

  *rtn_hmap = hmap;
  return ERR_OK;
}  /* hmap_create */


ERR_F hmap_delete(hmap_t *hmap) {
  uint32_t bucket;

  /* Step to each bucket and delete the list of entries. */
  for (bucket = 0; bucket < hmap->table_size; bucket++) {
    hmap_entry_t *entry = hmap->table[bucket];
    while (entry) {
      hmap_entry_t *next = entry->next;
      /* The application is responsible for freeing the value. */
      free(entry->key);
      free(entry);
      entry = next;
    }
  }

  free(hmap);
  return ERR_OK;
}  /* hmap_delete */


ERR_F hmap_write(hmap_t *hmap, const void *key, size_t key_size, void *val) {
  ERR_ASSRT(hmap, HMAP_ERR_PARAM);
  ERR_ASSRT(key, HMAP_ERR_PARAM);

  uint32_t bucket = hmap_murmur3_32(key, key_size, hmap->seed) % hmap->table_size;

  /* Search linked list.  */
  hmap_entry_t *entry = hmap->table[bucket];
  while (entry) {
    if (key_size == entry->key_size && memcmp(entry->key, key, key_size) == 0) {
      entry->value = val;
      return ERR_OK;
    }
    entry = entry->next;
  }

  /* Not found, create new entry. */
  hmap_entry_t *new_entry = calloc(1, sizeof(hmap_entry_t));
  ERR_ASSRT(new_entry, HMAP_ERR_NOMEM);

  new_entry->key = malloc(key_size);
  if (!new_entry->key) {
    free(new_entry);
    ERR_THROW(HMAP_ERR_NOMEM, "new_entry->key");
  }
  memcpy(new_entry->key, key, key_size);
  new_entry->key_size = key_size;
  new_entry->value = val;
  new_entry->bucket = bucket;

  /* Insert at head of list for this bucket */
  new_entry->next = hmap->table[bucket];
  hmap->table[bucket] = new_entry;
  hmap->num_entries ++;

  return ERR_OK;
}  /* hmap_write */


ERR_F hmap_lookup(hmap_t *hmap, const void *key, size_t key_size, void **rtn_val) {
  ERR_ASSRT(hmap, HMAP_ERR_PARAM);
  ERR_ASSRT(key, HMAP_ERR_PARAM);

  uint32_t bucket = hmap_murmur3_32(key, key_size, hmap->seed) % hmap->table_size;

  /* Search linked list */
  hmap_entry_t *entry = hmap->table[bucket];
  while (entry) {
    if (key_size == entry->key_size && memcmp(entry->key, key, key_size) == 0) {
      if (rtn_val) {
        *rtn_val = entry->value;
      }
      return ERR_OK;
    }
    entry = entry->next;
  }

  if (rtn_val) {
    *rtn_val = NULL;
  }
  ERR_THROW(HMAP_ERR_NOTFOUND, "key not found");
}  /* hmap_lookup */


ERR_F hmap_next(hmap_t *hmap, hmap_entry_t **in_entry) {
  uint32_t bucket;
  hmap_entry_t *next_entry;

  ERR_ASSRT(hmap, HMAP_ERR_PARAM);

  if (*in_entry == NULL) {
    /* If in_entry is NULL, user want's first entry in table. */
    bucket = 0;
    next_entry = hmap->table[bucket];
  } else {
    /* Next entry in list. */
    bucket = (*in_entry)->bucket;
    next_entry = (*in_entry)->next;
  }

  /* next_entry == NULL means we hit the end of a list;
   * check subsequent buckets till we find a non-empty one. */
  while (next_entry == NULL && bucket < hmap->table_size) {
    bucket++;
    if (bucket < hmap->table_size) {
      next_entry = hmap->table[bucket];
    }
  }

  *in_entry = next_entry;  /* If no more entries, it's NULL. */
  return ERR_OK;
}  /* hmap_next */
