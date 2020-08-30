#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <libconfig.h>
#include <ncurses.h>
#include "util.h"

int main(void)
{
  // Set up variables for making API calls
  // Grab some configs
  config_t cfg;
  config_setting_t *setting;
  const char *addr;
  const char *key;

  config_init(&cfg);

  /* Read the file. If there is an error, report it and exit. */
  if(! config_read_file(&cfg, "api.cfg"))
  {
    fprintf(stderr, "%s:%d - %s\n", config_error_file(&cfg),
            config_error_line(&cfg), config_error_text(&cfg));
    config_destroy(&cfg);
    return(EXIT_FAILURE);
  }

  // Get url
  config_lookup_string(&cfg, "url", &addr);

  // Get key
  config_lookup_string(&cfg, "key", &key);

  // Let's get some basic info about what's printing.
  char *call = "/api/job";

  // Construct the curl URL
  char address[strlen(addr) + strlen(call)];
  strcpy(address, addr);
  strcat(address, call);

  initscr(); // Start ncurses.
  for(;;) {
    char *job = call_octoprint(address, key);
    if (strcmp(job, "-1") == 0) {
        endwin();
        printf("Error: Can't contact the OctoPrint server.");
        return 1;
    }
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

    move(6, 1);
    attron(A_BOLD);
    printw("    Progress: ");
    attroff(A_BOLD);
    float float_percent = atof(percent_complete);
    printw("%i", (int) round(float_percent));
    printw("%%");

  	refresh();			/* Print it on to the real screen */

    // Clean up and exit.
    free(user);
    free(job);
    free(time_spent);
    free(percent_complete);
    free(name);
    sleep(10);
  }
	endwin();			/* End curses mode		  */

  return 0;
}
