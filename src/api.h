#ifndef UTIL_H
#define UTIL_H

#include <string.h>
#include <curl/curl.h>
#include <stdlib.h>
#include <ncurses.h>
#include <unistd.h>

#include "data.h"

#include <cjson/cJSON.h>

// String struct for curl writefunc. Problems start here.
struct string {
  char *ptr;
  size_t len;
};

struct Duration {
    int hr;
    int min;
    int sec;
};

void init_string(struct string *s);

size_t writefunc(void* ptr, size_t size, size_t nmemb, struct string* s);

void octoprint_comm_init(CURL* curl);

char* octoprint_comm_recv(CURL* curl, char* api_call, const char* key);

void octoprint_comm_end(CURL* curl);

char* call_octoprint(char* api_call, const char* key);

int check_alive(char* reply);

struct Duration format_time(int time_seconds);

#endif
