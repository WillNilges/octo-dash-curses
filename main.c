#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <ncurses.h>
#include "jsmn.h"

// Bibliography:
// https://stackoverflow.com/questions/2329571/c-libcurl-get-output-into-a-string

// Look ma, I'm a C++™ developer!
struct string {
  char *ptr;
  size_t len;
};

// Init a string struct, because I'm a masochist.
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

char * call_octoprint(char *api_call){
  CURL *curl;
  CURLcode res;

  struct curl_slist *list = NULL;

  curl = curl_easy_init();
  if(curl) {
    struct string s;
    init_string(&s);

    curl_easy_setopt(curl, CURLOPT_URL, api_call);
    list = curl_slist_append(list, "X-Api-Key: <key>");
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

int main(void)
{
  // initscr();			/* Start curses mode 		  */
  // printw("Hello World !!!");	/* Print Hello World		  */
  // refresh();			/* Print it on to the real screen */
  // getch();			/* Wait for user input */
  // printw();
  // endwin();			/* End curses mode		  */

  char *job = call_octoprint("3d.postave.us/api/job");
  printf("%s\n", job);



  free(job);
  return 0;
}
