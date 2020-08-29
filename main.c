#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #include <ncurses.h> // TODO :)
// #include "jsmn.h"
#include "util.c" // TODO: Is this how project structures work?

int main(void)
{
  // Set up variables for making API calls.
  char *key = getenv("OCTOKEY");
  char *addr = getenv("OCTOADDR");

  if (key == NULL) {
    printf("We need an API key, fool! Come back when you have one!\n");
    return 1;
  }

  if (addr == NULL) {
    printf("We need an address, fool! Come back when you have one!\n");
    return 1;
  }

  // Let's get some basic info about what's printing.
  char *call = "/api/job"; // Or whatever api call you wanna make.

  // Construct the curl URL
  char address[strlen(addr) + strlen(call)];
  strcpy(address, addr);
  strcat(address, call);
  char *job = call_octoprint(address, key);
  char *user = get_value(job, "user");
  printf("Username: %s\n", user);
  char *name = get_value(job, "name");
  printf("Currently Printing: %s\n", name);

  char *time_spent = get_value(job, "printTime");
  printf("Time spent: %s\n", time_spent);

  char *percent_complete = get_value(job, "completion");
  printf("Percent completion: %s\n", percent_complete);

  // Clean up and exit.
  free(user);
  free(job);
  free(name);
  return 0;
}
