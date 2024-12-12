# hmap
A simple implementation of a hashmap based on Austin Appleby's
[murmur3](https://github.com/aappleby/smhasher) algorithm.
Many thanks to Austin for putting his reference code in the public domain.
Full disclosure: [Claude.ai](https://claude.ai) helped me with the C code
and deserves some credit as well. I am also placing this implementation in
the public domain using CC0; see [License](#license).


## Table of contents

<!-- mdtoc-start -->
&bull; [hmap](#hmap)  
&nbsp;&nbsp;&nbsp;&nbsp;&bull; [Table of contents](#table-of-contents)  
&nbsp;&nbsp;&nbsp;&nbsp;&bull; [Introduction](#introduction)  
&nbsp;&nbsp;&nbsp;&nbsp;&bull; [API](#api)  
&nbsp;&nbsp;&nbsp;&nbsp;&bull; [Development](#development)  
&nbsp;&nbsp;&nbsp;&nbsp;&bull; [License](#license)  
<!-- TOC created by '../mdtoc/mdtoc.pl README.md' (see https://github.com/fordsfords/mdtoc) -->
<!-- mdtoc-end -->


## Introduction

I like to write software and release it to public domain (CC0).
Sometimes I want to use a hash map in my code, and I don't like to have to include their license.
Fortunately, Austin Appleby's excellent murmur3 is in the public domain,
so with a little help from Claude.ai I was able to get this working in a couple hours.

Note that it could be more efficient, and it certainly could be more functional.
E.g. it might be nice to remove individual records.
I emphasized short and simple over almost everything else.

For a more full-featured hashmap with an MIT license, see https://github.com/tidwall/hashmap.c


## API

The only non-obvious thing is the use of the "err" system for error handling.
APIs return ERR_OK for success, or an error object on error.
See https://github.com/fordsfords/err

A key is an arbitrary byte array, not a necessarily a valid C string (although a C string works fine).

````
#include "hmap.h"

ERR_F hmap_create(hmap_t **rtn_hmap, size_t table_size); - returns new hmap via rtn_hmap. Prefer table_size to be prime.

ERR_F hmap_delete(hmap_t *hmap); - deletes entire hmap object, freeing memory.

ERR_F hmap_write(hmap_t *hmap, const void *key, size_t key_size, void *val); - only the pointer "val" is stored in the map, not the content it is pointing at.

ERR_F hmap_lookup(hmap_t *hmap, const void *key, size_t key_size, void **rtn_val); - if not found, returns -1.

ERR_F hmap_next(hmap_t *hmap, hmap_entry_t **in_entry); - This is an iterator to step through all records in the hash.
Declare a "hmap_entry_t *entry" and set it to NULL to start at the beginning of the table. Each call to "hmap_next()"
updates your "in_entry" variable to point to the next node in the table, or NULL when you reach the end of the table.
````

NOTE: the "hmap_write()" function will overwrite an existing entry with the same key.

This code was not written to be thread-safe.

See "hmap_test.c" for example usages.


## Development

* bld.sh - builds the test program.
* tst.sh - calls "bld.sh" and runs the test programs.


## License

I want there to be NO barriers to using this code, so I am releasing it to the public domain.  But "public domain" does not have an internationally agreed upon definition, so I use CC0:

Copyright 2024-2024 Steven Ford http://geeky-boy.com and licensed
"public domain" style under
[CC0](http://creativecommons.org/publicdomain/zero/1.0/):
![CC0](https://licensebuttons.net/p/zero/1.0/88x31.png "CC0")

To the extent possible under law, the contributors to this project have
waived all copyright and related or neighboring rights to this work.
In other words, you can use this code for any purpose without any
restrictions.  This work is published from: United States.  The project home
is https://github.com/fordsfords/hmap

To contact me, Steve Ford, project owner, you can find my email address
at http://geeky-boy.com.  Can't see it?  Keep looking.
