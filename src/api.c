#include "api.h"

/* API */

// Function to dump curl responses into strings.
size_t writefunc(void *ptr, size_t size, size_t nmemb, struct string *s) {
  size_t new_len = s->len + size*nmemb;
  s->ptr = realloc(s->ptr, new_len+1);
  if (s->ptr == NULL) {
    fprintf(stderr, "realloc() failed\n");
    exit(EXIT_FAILURE);
  }
  memcpy(s->ptr+s->len, ptr, size*nmemb);
  s->ptr[new_len] = '\0';
  s->len = new_len;

  return size*nmemb;
}

char* octoprint_comm_recv(CURL* curl, char* api_call, const char* key) {
    struct string s;
    CURLcode res;
    struct curl_slist* list;

    // Set up string
    s.len = 0;
    s.ptr = malloc(1);
    if (!s.ptr) 
    {
        fprintf(stderr, "String malloc failed!");
        return NULL;
    }

    // Set up data object for curl arguments
    list = NULL;

    // Prepare API key
    char keybase[32+11] = "X-Api-Key: ";
    strcat(keybase, key);

    // Curl arguments
    curl_easy_setopt(curl, CURLOPT_URL, api_call);
    list = curl_slist_append(list, keybase);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);
    
    // Perform the CURL
    res = curl_easy_perform(curl);
    if (res != 0) {
        fprintf(stderr, "Error: CURL failed.");
        return NULL;
    }

    // Clean up and return result (remember to free that pointer later!)
    curl_slist_free_all(list);
    return s.ptr;
}

// Checks contents of API reply for every possible issue.
// Returns 0 if OK and 1 if not OK.
// TODO: Add more issues as they arise :P
int check_alive(char* reply) {
    // Forgive me father for I have sinned
    if (
        strstr(reply, "OctoPrint is currently not running")  ||
        strstr(reply, "Can't contact the OctoPrint server.") ||
        strstr(reply, "Error: Malformed API key.")           ||
        strstr(reply, "403 Forbidden")                       ||
        strstr(reply, "Offline")
    ) return 1;
    return 0;
}

/* MISC */

// Parses a time in seconds into a struct for hours, minutes, and seconds. 
struct Duration format_time(int time_seconds) {
    struct Duration parsed_time;

    int seconds = time_seconds % 60;
    int minutes = (time_seconds / 60) % 60;
    int hours = time_seconds / 3600;

    parsed_time.hr = hours;
    parsed_time.min = minutes;
    parsed_time.sec = seconds;
    return parsed_time;
}