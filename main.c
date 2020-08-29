#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #include <ncurses.h> // TODO :)
#include "jsmn.h"
#include "util.c" // TODO: Is this how project structures work?

int main(void)
{
  // Set up variables for making API calls.
  char *key = getenv("OCTOKEY");
  char *addr = getenv("OCTOADDR");

  // Let's see if we can get the username of whoever is currently printing,
  // and what they are currently printing.
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
  
  // Clean up and exit.
  free(user);
  free(job);
  free(name);
  return 0;
}
