/* example.c - demonstrate use of "hmap" object. */

#include <stdio.h>
#include "err.h"
#include "hmap.h"


ERR_F hmap_example_app()
{
  hmap_t *hmap;
  ERR(hmap_create(&hmap, 5009));

  /* Arbitrary byte array as key. */
  char key[] = { 0x00, 0x01, 0x02, 0x03 };
  char value[] = "example value";

  ERR(hmap_write(hmap, key, sizeof(key), value));  /* Writes pointer to "value" array. */

  char *lookup_rtn;
  ERR(hmap_lookup(hmap, key, sizeof(key), (void *)&lookup_rtn));
  if (lookup_rtn == value) {  /* If the returned pointer is the same. */
    printf("Successfully saved and retrieved value.\n");
  }

  ERR(hmap_delete(hmap));


  /**********************************************
   * String keys, dynamic memory.
   *********************************************/

  ERR(hmap_create(&hmap, 5009));

  /* Use "err_asprintf()" to malloc string buffers. */
  char *skey = "abc";
  char *svalue;
  ERR(err_asprintf(&svalue, "Value for key %s", skey));
  ERR(hmap_swrite(hmap, skey, svalue));  /* Writes pointer to "value" array. */

  ERR(hmap_slookup(hmap, "abc", (void *)&value));
  if (strcmp(svalue, "Value for key abc") == 0) {
    printf("Successfully retrieved key %s, value %s\n", skey, svalue);
  }

  /* Cleanup by stepping through the hash, freeing values. */
  hmap_entry_t *entry = NULL;  /* Start at beginning. */
  do {
    ERR(hmap_next(hmap, &entry));
    if (entry) {
      printf("Freeing value %s (key %s)\n", (char *)entry->value, (char *)entry->key);
      free(entry->value);
    }
  } while (entry);

  ERR(hmap_delete(hmap));

  return ERR_OK;
}  /* hmap_example_app */


int main(int argc, char **argv)
{
  (void)argc;  (void)argv;  /* Unused parameters. */

  /* If error returns to outer-most main, abort to dump core.
   * Alternatively could use ERR_EXIT_ON_ERR(). */
  ERR_ABRT_ON_ERR(hmap_example_app(), stderr);

  return 0;
}  /* main */
