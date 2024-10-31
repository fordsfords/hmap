/* hmap_test.c */
/*
# This code and its documentation is Copyright 2024-2024 Steven Ford, http://geeky-boy.com
# and licensed "public domain" style under Creative Commons "CC0": http://creativecommons.org/publicdomain/zero/1.0/
# To the extent possible under law, the contributors to this project have
# waived all copyright and related or neighboring rights to this work.
# In other words, you can use this code for any purpose without any
# restrictions.  This work is published from: United States.  The project home
# is https://github.com/fordsfords/hmap
*/

#include <stdio.h>
#include <string.h>
#if ! defined(_WIN32)
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#endif
#include "hmap.h"

#if defined(_WIN32)
#define MY_SLEEP_MS(msleep_msecs) Sleep(msleep_msecs)
#else
#define MY_SLEEP_MS(msleep_msecs) usleep((msleep_msecs)/1000)
#endif

#define E(e_test) do { \
  if ((e_test) == -1) { \
    fprintf(stderr, "ERROR [%s:%d]: '%s' returned -1\n", __FILE__, __LINE__, #e_test); \
    exit(1); \
  } \
} while (0)

#define ASSRT(assrt_cond) do { \
  if (! (assrt_cond)) { \
    fprintf(stderr, "ERROR [%s:%d]: assert '%s' failed\n", __FILE__, __LINE__, #assrt_cond); \
    exit(1); \
  } \
} while (0)


/* Options */
int o_testnum;


char usage_str[] = "Usage: test_hmap [-h] [-t testnum]";
void usage(char *msg) {
  if (msg) fprintf(stderr, "\n%s\n\n", msg);
  fprintf(stderr, "%s\n", usage_str);
  exit(1);
}  /* usage */

void help() {
  printf("%s\n"
    "where:\n"
    "  -h - print help\n"
    "  -t testnum - Specify which test to run [1].\n"
    "For details, see https://github.com/fordsfords/hmap\n",
    usage_str);
  exit(0);
}  /* help */


void parse_cmdline(int argc, char **argv) {
  int i;

  /* Since this is Unix and Windows, don't use getopts(). */
  for (i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-h") == 0) {
      help();  exit(0);

    } else if (strcmp(argv[i], "-t") == 0) {
      if ((i + 1) < argc) {
        i++;
        o_testnum = atoi(argv[i]);
      } else { fprintf(stderr, "Error, -t requires test number\n");  exit(1); }

    } else { fprintf(stderr, "Error, unknown option '%s'\n", argv[i]);  exit(1); }
  }  /* for i */
}  /* parse_cmdline */


void test1() {
  int a, b, c;
  uint8_t k[5] = {1, 0, 2, 3, 4};
  void *v;
  hmap_t *hmap;
  hmap_node_t *n;
  uint32_t bucket;

  a = 1; b = 2; c = 3;

  E(hmap_create(&hmap, 1));
  ASSRT(hmap->table_size == 1);
  ASSRT(hmap->seed == 42);

  E(hmap_write(hmap, k, sizeof(k), &a));
  ASSRT(hmap->table_size == 1);
  n = hmap->table[0];
  ASSRT(n->value == &a);
  ASSRT(n->next == NULL);
  ASSRT(n->key_size == sizeof(k));
  ASSRT(memcmp(n->key, k, sizeof(k)) == 0);

  ASSRT(hmap_lookup(hmap, k, sizeof(k), &v) == 0);
  ASSRT(v == &a);
  ASSRT(hmap_lookup(hmap, "foobar", sizeof(k), &v) != 0);

  E(hmap_write(hmap, k, sizeof(k), &b));
  ASSRT(hmap->table_size == 1);
  n = hmap->table[0];
  ASSRT(n->value == &b);
  ASSRT(n->next == NULL);
  ASSRT(n->key_size == sizeof(k));
  ASSRT(memcmp(n->key, k, sizeof(k)) == 0);

  ASSRT(hmap_lookup(hmap, k, sizeof(k), &v) == 0);
  ASSRT(v == &b);
  ASSRT(hmap_lookup(hmap, "foobar", sizeof(k), &v) != 0);

  k[4] = 0;
  E(hmap_write(hmap, k, sizeof(k), &c));
  ASSRT(hmap->table_size == 1);
  n = hmap->table[0];
  ASSRT(n->value == &c);
  ASSRT(n->next != NULL);
  ASSRT(n->key_size == sizeof(k));
  ASSRT(memcmp(n->key, k, sizeof(k)) == 0);
  n = n->next;
  ASSRT(n->value == &b);
  ASSRT(n->next == NULL);
  ASSRT(n->key_size == sizeof(k));
  ASSRT(memcmp(n->key, k, sizeof(k)) != 0);

  ASSRT(hmap_lookup(hmap, k, sizeof(k), &v) == 0);
  ASSRT(v == &c);
  ASSRT(hmap_lookup(hmap, "foobar", sizeof(k), &v) != 0);
  k[4] = 4;  /* Return key to original value. */
  ASSRT(hmap_lookup(hmap, k, sizeof(k), &v) == 0);
  ASSRT(v == &b);
  ASSRT(hmap_lookup(hmap, "foobar", sizeof(k), &v) != 0);

  /*********************************************************
   * Leak memory! But I don't care.
   *********************************************************/

  E(hmap_create(&hmap, 7919));  /* prime number. */
  ASSRT(hmap->table_size == 7919);
  ASSRT(hmap->seed == 42);

  E(hmap_write(hmap, k, sizeof(k), &a));
  ASSRT(hmap->table_size == 7919);
  bucket = hmap_murmur3_32(k, sizeof(k), hmap->seed) % hmap->table_size;
  ASSRT(bucket > 1);  /* True for this key. */
  n = hmap->table[bucket];
  ASSRT(n->value == &a);
  ASSRT(n->next == NULL);
  ASSRT(n->key_size == sizeof(k));
  ASSRT(memcmp(n->key, k, sizeof(k)) == 0);

  ASSRT(hmap_lookup(hmap, k, sizeof(k), &v) == 0);
  ASSRT(v == &a);
  ASSRT(hmap_lookup(hmap, "foobar", sizeof(k), &v) != 0);

  E(hmap_write(hmap, k, sizeof(k), &b));
  ASSRT(hmap->table_size == 7919);
  ASSRT(bucket == hmap_murmur3_32(k, sizeof(k), hmap->seed) % hmap->table_size);
  n = hmap->table[bucket];
  ASSRT(n->value == &b);
  ASSRT(n->next == NULL);
  ASSRT(n->key_size == sizeof(k));
  ASSRT(memcmp(n->key, k, sizeof(k)) == 0);

  ASSRT(hmap_lookup(hmap, k, sizeof(k), &v) == 0);
  ASSRT(v == &b);
  ASSRT(hmap_lookup(hmap, "foobar", sizeof(k), &v) != 0);

  k[4] = 0;
  E(hmap_write(hmap, k, sizeof(k), &c));
  ASSRT(hmap->table_size == 7919);
  ASSRT(bucket != hmap_murmur3_32(k, sizeof(k), hmap->seed) % hmap->table_size);
  ASSRT(n->next == NULL);  /* Previous bucket. */
  bucket = hmap_murmur3_32(k, sizeof(k), hmap->seed) % hmap->table_size;
  n = hmap->table[bucket];
  ASSRT(n->value == &c);
  ASSRT(n->next == NULL);
  ASSRT(n->key_size == sizeof(k));
  ASSRT(memcmp(n->key, k, sizeof(k)) == 0);

  ASSRT(hmap_lookup(hmap, k, sizeof(k), &v) == 0);
  ASSRT(v == &c);
  ASSRT(hmap_lookup(hmap, "foobar", sizeof(k), &v) != 0);
  k[4] = 4;  /* Return key to original value. */
  ASSRT(hmap_lookup(hmap, k, sizeof(k), &v) == 0);
  ASSRT(v == &b);
  ASSRT(hmap_lookup(hmap, "foobar", sizeof(k), &v) != 0);
}  /* test1 */


int main(int argc, char **argv) {
  parse_cmdline(argc, argv);

  if (o_testnum == 0 || o_testnum == 1) {
    test1();
    printf("test1: success\n");
  }

  return 0;
}  /* main */
