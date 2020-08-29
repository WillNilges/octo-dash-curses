#ifndef UTIL_H
#define UTIL_H

#include <string.h>
#include "jsmn.h"

// typedef struct string_t {
//   char *ptr;
//   size_t len;
// } string;

// Boneless string, becaue I don't feel like figuring out how long these
// strings are.
struct string {
  char *ptr;
  size_t len;
};

void init_string(struct string *s);

size_t writefunc(void *ptr, size_t size, size_t nmemb, struct string *s);

static int jsoneq(const char *json, jsmntok_t *tok, const char *s);

char *get_value(char *json_blob, char *seek);

char *call_octoprint(char *api_call, char *key);

#endif
