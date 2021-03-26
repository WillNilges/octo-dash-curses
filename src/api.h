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

char* octoprint_comm_recv(CURL* curl, char* api_call, const char* key);

int check_alive(char* reply);

struct Duration format_time(int time_seconds);

#endif
