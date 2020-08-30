#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include "util.h"

int main(void)
{
  initscr();			/* Start curses mode 		  */
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

  for(;;) {

    char *job = call_octoprint(address, key);
    char *user = get_value(job, "user");
    char *name = get_value(job, "name");
    char *time_spent = get_value(job, "printTime"); // In seconds
    char *percent_complete = get_value(job, "completion"); // In percent

    move(1, 1);
    attron(A_STANDOUT);
  	printw("Yo, what's cookin!?");
    attroff(A_STANDOUT);

    move(3, 1);
    attron(A_BOLD);
    printw("  Print name: ");
    attroff(A_BOLD);
    printw(name);

    move(4, 1);
    attron(A_BOLD);
    printw("       Owner: ");
    attroff(A_BOLD);
    printw(user);

    move(5, 1);
    clrtoeol();
    attron(A_BOLD);
    printw("Time elapsed: ");
    attroff(A_BOLD);
    if (strcmp(time_spent, "null") != 0) {
      char *total_time_spent=format_time(time_spent);
      printw(total_time_spent);
    } else printw("N/A");
    // move(5, 30);
    // printw(time_spent);
    // printw(time_spent);

    move(6, 1);
    attron(A_BOLD);
    printw("    Progress: ");
    attroff(A_BOLD);
    printw(percent_complete);
    printw("%");

  	refresh();			/* Print it on to the real screen */

    // Clean up and exit.
    free(user);
    free(job);
    free(time_spent);
    free(percent_complete);
    free(name);
    sleep(2);
  }
	endwin();			/* End curses mode		  */

  return 0;
}
