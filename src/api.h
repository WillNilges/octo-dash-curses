#ifndef UTIL_H
#define UTIL_H

#include <string.h>
#include <curl/curl.h>
#include <stdlib.h>
#include <ncurses.h>
#include <unistd.h>

#include "jsmn.h"
#include "data.h"

// curl wants this for some fucking reason
struct string {
  char *ptr;
  size_t len;
};

struct Duration {
    int hr;
    int min;
    int sec;
};

// CURL string nonsense

void init_string(struct string *s);

size_t writefunc(void* ptr, size_t size, size_t nmemb, struct string* s);

struct Duration format_time(char* time_in_seconds);

static int jsoneq(const char* json, jsmntok_t* tok, const char* s);

char* get_value(char* json_blob, char* seek);

char* call_octoprint(char* api_call, const char* key);

int check_alive(char* reply);

#endif
