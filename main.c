#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jsmn.h"

// #include <ncurses.h>
#include "util.c" // TODO: Is this how project structures work?

int main(void)
{
  char *key = getenv("OCTOKEY");
  char *addr = getenv("OCTOADDR");
  char *call = "/api/job"; // Or whatever api call you wanna make.
  char address[strlen(addr) + strlen(call)];
  strcpy(address, addr);
  strcat(address, call);
  char *job = call_octoprint("3d.postave.us/api/job", key);
  // printf("%s\n", job);

  int r;
  jsmn_parser p;
  jsmntok_t t[128]; /* We expect no more than 128 tokens */

  jsmn_init(&p);
  r = jsmn_parse(&p, job, strlen(job), t, sizeof(t) / sizeof(t[0]));

  for (int i = 1; i < r; i++) {
    if (jsoneq(job, &t[i], "job") == 0) {
      char jobinfo[1000]; // I'm guessing this'll be less than 1000 characters.
      snprintf(jobinfo, sizeof(jobinfo), "%.*s\n", t[i + 1].end - t[i + 1].start,
             job + t[i + 1].start);

      // cursed
      char info[1000];
      get_value(info, jobinfo, "user", 0);
      printf("%s", info);
      // printf("yeetus");

      // int r2;
      // jsmn_parser p2;
      // jsmntok_t t2[128];
      //
      // jsmn_init(&p2);
      // r2 = jsmn_parse(&p2, jobinfo, strlen(jobinfo), t2, sizeof(t2) / sizeof(t2[0]));
      // // printf("%.*s\n", t[i + 1].end - t[i + 1].start,
      // //        job + t[i + 1].start);
      // for (int j = 1; i < r2; i++) {
      //   if (jsoneq(jobinfo, &t2[i], "user") == 0) {
      //     printf("User: %.*s\n", t2[i + 1].end - t2[i + 1].start,
      //        jobinfo + t2[i + 1].start);
      //   }
      // }
      i++;
    }
  }

  free(job);
  return 0;
}
