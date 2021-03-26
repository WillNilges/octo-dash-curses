#include "api.h"

/* API */

void init_string(struct string *s) {
  s->len = 0;
  s->ptr = malloc(s->len+1); // I need to free this.
  if (s->ptr == NULL) {
    fprintf(stderr, "malloc() failed\n");
    exit(EXIT_FAILURE);
  }
  s->ptr[0] = '\0';
}

// TODO: Free string

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

// Query the octoprint server for data. This is a stupid CURL request, and does not check
// if it was actually successful.
char* call_octoprint(char* api_call, const char* key) {
    CURL* curl;
    CURLcode res;

    struct curl_slist* list = NULL;

    // curl_global_init(); // TODO: FIX MEMORY LEAKS
    curl = curl_easy_init();
    if(curl) {
        struct string s;
        init_string(&s);

        curl_easy_setopt(curl, CURLOPT_URL, api_call);
        if (strlen(key) != 32) { // I believe all OctoPrint API keys are exactly 32 bytes long. 
            printf("%s\n", MALFORMED); // I'm lazy, so this is how I'm gonna report errors.
            curl_slist_free_all(list);
            curl_easy_cleanup(curl);
            return (char* ) MALFORMED;
        }
        char keybase[32+11] = "X-Api-Key: "; // Memory really do be like that.
        strcat(keybase, key);
        list = curl_slist_append(list, keybase);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);
        res = curl_easy_perform(curl);
        if (res != 0) {
            printf("API call returned nonzero value.");
            curl_slist_free_all(list);
            curl_easy_cleanup(curl);
            return (char* ) ERROR;
        }
        /* always cleanup */
        curl_slist_free_all(list);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
        return s.ptr; // Fuck you.
    }
    curl_easy_cleanup(curl);
    curl_global_cleanup();
    return 0;
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