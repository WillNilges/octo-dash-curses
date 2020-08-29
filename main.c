#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jsmn.h"

// #include <ncurses.h>
#include "util.c" // TODO: Is this how project structures work?

int main(void)
{
  // Set up variables for making API calls.
  char *key = getenv("OCTOKEY");
  char *addr = getenv("OCTOADDR");

  // Query the current job
  char *call = "/api/job"; // Or whatever api call you wanna make.
  char address[strlen(addr) + strlen(call)];
  strcpy(address, addr);
  strcat(address, call);
  char *job = call_octoprint(address, key);
  // printf("%s\n", job);

  // int r;
  // jsmn_parser p;
  // jsmntok_t t[128]; /* We expect no more than 128 tokens */
  //
  // jsmn_init(&p);
  // r = jsmn_parse(&p, job, strlen(job), t, sizeof(t) / sizeof(t[0]));
  //
  // // Check that it's actually JSON. TODO: Put this in a function.
  // if (r < 0) {
  //   printf("Failed to parse JSON: %d\n", r);
  //   return 1;
  // }
  //
  // /* Assume the top-level element is an object */
  // if (r < 1 || t[0].type != JSMN_OBJECT) {
  //   printf("Object expected\n");
  //   return 1;
  // }

  char *user = get_value(job, "user");
  printf("here's the TOP LEVEL BITCH!: %s\n", user);
  free(user);


  // for (int i = 1; i < r; i++) {
    // if (jsoneq(job, &t[1], "job") == 0) {
      // char jobinfo[1000]; // I'm guessing this'll be less than 1000 characters.
      // snprintf(jobinfo,
      //          sizeof(jobinfo),
      //          "%.*s\n",
      //          t[1 + 1].end - t[1 + 1].start,
      //          job + t[1 + 1].start);

      // cursed
      // char info[1000];
      // get_value(jobinfo, "user");
      // printf("%s", info);
      // get_value(jobinfo, "file");
      // printf("%s", info);
      // printf("yeetus");

      // i++;
    // }
  // }

  free(job);
  return 0;
}
