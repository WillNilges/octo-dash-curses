#include <curl/curl.h>
// #include "jsmn.h"


/* STRING STUFF */

// Boneless string, becaue I don't feel like figuring out how long these
// strings are.
struct string {
  char *ptr;
  size_t len;
};

void init_string(struct string *s) {
  s->len = 0;
  s->ptr = malloc(s->len+1);
  if (s->ptr == NULL) {
    fprintf(stderr, "malloc() failed\n");
    exit(EXIT_FAILURE);
  }
  s->ptr[0] = '\0';
}

// Function to dump curl responses into strings.
size_t writefunc(void *ptr, size_t size, size_t nmemb, struct string *s)
{
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

/* JSON STUFF */
static int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
  if (tok->type == JSMN_STRING && (int)strlen(s) == tok->end - tok->start &&
      strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
    return 0;
  }
  return -1;
}

void get_value(char * info, char *json_blob, char *seek, int levels){
  int r;
  jsmn_parser p;
  jsmntok_t t[128];

  jsmn_init(&p);
  r = jsmn_parse(&p, json_blob, strlen(json_blob), t, sizeof(t) / sizeof(t[0]));
  for (int i = 1; i < r; i++) {
    if (jsoneq(json_blob, &t[i], seek) == 0) {
      // char data[1000]; // I'm guessing this'll be less than 1000 characters.
      snprintf(info, 1000, "%.*s\n", t[i + 1].end - t[i + 1].start,
             json_blob + t[i + 1].start);
    }
  }
}

/* OCTOPRINT STUFF */

// Query the octoprint server for data.
char * call_octoprint(char *api_call, char *key){
  CURL *curl;
  CURLcode res;

  struct curl_slist *list = NULL;

  curl = curl_easy_init();
  if(curl) {
    struct string s;
    init_string(&s);

    curl_easy_setopt(curl, CURLOPT_URL, api_call);
    // printf(key);
    char keybase[32+11] = "X-Api-Key: "; // Memory really do be like that.
    strcat(keybase, key);
    list = curl_slist_append(list, keybase);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);
    res = curl_easy_perform(curl);

    // printf("%s\n", s.ptr);
    // free(s.ptr);

    /* always cleanup */
    curl_slist_free_all(list);
    curl_easy_cleanup(curl);
    return s.ptr;
  }
  return 0;
}
