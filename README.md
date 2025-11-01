# hmap
A simple implementation of a hashmap based on Austin Appleby's
[murmur3](https://github.com/aappleby/smhasher) algorithm.
Many thanks to Austin for putting his reference code in the public domain.
I am also placing this implementation in
the public domain using CC0; see [License](#license).


## Table of contents

<!-- mdtoc-start -->
&bull; [hmap](#hmap)  
&nbsp;&nbsp;&nbsp;&nbsp;&bull; [Table of contents](#table-of-contents)  
&nbsp;&nbsp;&nbsp;&nbsp;&bull; [Introduction](#introduction)  
&nbsp;&nbsp;&nbsp;&nbsp;&bull; [API](#api)  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&bull; [Functions](#functions)  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&bull; [`ERR_F hmap_create(hmap_t **rtn_hmap, size_t table_size)`](#err_f-hmap_createhmap_t-rtn_hmap-size_t-table_size)  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&bull; [`ERR_F hmap_delete(hmap_t *hmap)`](#err_f-hmap_deletehmap_t-hmap)  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&bull; [`ERR_F hmap_write(hmap_t *hmap, const void *key, size_t key_size, void *val)`](#err_f-hmap_writehmap_t-hmap-const-void-key-size_t-key_size-void-val)  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&bull; [`ERR_F hmap_lookup(hmap_t *hmap, const void *key, size_t key_size, void **rtn_val)`](#err_f-hmap_lookuphmap_t-hmap-const-void-key-size_t-key_size-void-rtn_val)  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&bull; [`ERR_F hmap_swrite(hmap_t *hmap, const char *key, void *val)`](#err_f-hmap_swritehmap_t-hmap-const-char-key-void-val)  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&bull; [`ERR_F hmap_slookup(hmap_t *hmap, const char *key, void **rtn_val)`](#err_f-hmap_slookuphmap_t-hmap-const-char-key-void-rtn_val)  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&bull; [`ERR_F hmap_next(hmap_t *hmap, hmap_entry_t **in_entry)`](#err_f-hmap_nexthmap_t-hmap-hmap_entry_t-in_entry)  
&nbsp;&nbsp;&nbsp;&nbsp;&bull; [Example](#example)  
&nbsp;&nbsp;&nbsp;&nbsp;&bull; [Implementation Notes](#implementation-notes)  
&nbsp;&nbsp;&nbsp;&nbsp;&bull; [Development Tips](#development-tips)  
&nbsp;&nbsp;&nbsp;&nbsp;&bull; [License](#license)  
<!-- TOC created by '../mdtoc/mdtoc.pl README.md' (see https://github.com/fordsfords/mdtoc) -->
<!-- mdtoc-end -->


## Introduction

I like to write software and release it to public domain (CC0).
Sometimes I want to use a hash map in my code, and I don't like to have to include their license.
Fortunately, Austin Appleby's excellent murmur3 is in the public domain,
so with a little help from Claude.ai I was able to get this working in a couple hours.

This implementation emphasizes simplicity and clarity over advanced features. For a more full-featured hashmap with an MIT license, see https://github.com/tidwall/hashmap.c.

- Simple, lightweight implementation with no external dependencies
- Uses MurmurHash3 for efficient key hashing
- Supports arbitrary byte arrays as keys (not limited to strings)
- Iterator functionality for traversing all entries
- Public domain (CC0) licensed
- Not thread-safe (by design, for simplicity)

Thanks to Claude.ai for some help with the code and much help with the doc.
See https://blog.geeky-boy.com/2024/12/claude-as-coders-assistant.html for details.


## API

The library uses the "err" system for error handling.
All functions return `ERR_OK` for success or an error object on failure.
See https://github.com/fordsfords/err for details.

There are two sets of write/lookup APIs, one that allows an
arbitrary byte array as the key,
and the other assumes the key is a normal C string.

### Functions

#### `ERR_F hmap_create(hmap_t **rtn_hmap, size_t table_size)`
Creates a new hash map.
- Parameters:
  - `rtn_hmap`: Pointer to store the created hash map
  - `table_size`: Initial size of the hash table (preferably a prime number)
- Returns: `ERR_OK` on success, `HMAP_ERR_PARAM` or `HMAP_ERR_NOMEM` on failure
- Notes: The table size remains fixed; the map does not automatically resize
Choose a table_size that will accomodate expected growth.

#### `ERR_F hmap_delete(hmap_t *hmap)`
Deletes the hash map and frees all associated memory.
- Parameters:
  - `hmap`: The hash map to delete
- Notes: Does not free the values stored in the map; that's the caller's responsibility

#### `ERR_F hmap_write(hmap_t *hmap, const void *key, size_t key_size, void *val)`
Stores a key-value pair in the map.
- Parameters:
  - `hmap`: The hash map
  - `key`: Pointer to the key data
  - `key_size`: Size of the key in bytes
  - `val`: Pointer to the value (only the pointer is stored)
- Notes: 
  - If the key already exists, the value is updated
  - The key is copied, but the value pointer is stored as-is

#### `ERR_F hmap_lookup(hmap_t *hmap, const void *key, size_t key_size, void **rtn_val)`
Retrieves a value from the map.
- Parameters:
  - `hmap`: The hash map
  - `key`: Pointer to the key string
  - `key_size`: Size of the key in bytes
  - `rtn_val`: Pointer to store the found value
- Returns: `ERR_OK` if found, `HMAP_ERR_NOTFOUND` if the key doesn't exist

#### `ERR_F hmap_swrite(hmap_t *hmap, const char *key, void *val)`
Stores a key-value pair in the map. Key must be a C string.
- Parameters:
  - `hmap`: The hash map
  - `key`: Pointer to the key string
  - `val`: Pointer to the value (only the pointer is stored)
- Notes: 
  - If the key already exists, the value is updated
  - The key is copied, but the value pointer is stored as-is

#### `ERR_F hmap_slookup(hmap_t *hmap, const char *key, void **rtn_val)`
Retrieves a value from the map. Key must be a C string.
- Parameters:
  - `hmap`: The hash map
  - `key`: Pointer to the key data
  - `rtn_val`: Pointer to store the found value
- Returns: `ERR_OK` if found, `HMAP_ERR_NOTFOUND` if the key doesn't exist

#### `ERR_F hmap_next(hmap_t *hmap, hmap_entry_t **in_entry)`
Iterates through all entries in the map.
- Parameters:
  - `hmap`: The hash map
  - `in_entry`: Entry pointer (set to NULL to start iteration)
- Notes: Returns entries in arbitrary order based on hash distribution

## Example

See [example.c](example.c).


## Implementation Notes

- Not thread-safe (by design, for simplicity)
- Uses MurmurHash3 algorithm for hash generation
- Collision resolution through chaining (linked lists)
- Fixed-size hash table (no automatic resizing)
- Keys are copied, values are stored by reference


## Development Tips

* bld.sh - builds the test program.
* tst.sh - calls "bld.sh" and runs the test programs.


## License

I want there to be NO barriers to using this code, so I am releasing it to the public domain.  But "public domain" does not have an internationally agreed upon definition, so I use CC0:

This work is dedicated to the public domain under CC0 1.0 Universal:
http://creativecommons.org/publicdomain/zero/1.0/

To the extent possible under law, Steven Ford has waived all copyright
and related or neighboring rights to this work. In other words, you can 
use this code for any purpose without any restrictions.
This work is published from: United States.
Project home: https://github.com/fordsfords/hamp
