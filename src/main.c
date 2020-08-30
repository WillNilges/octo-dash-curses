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
  int refresh;
  int border;
  int scale;

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

  config_lookup_int(&cfg, "refresh", &refresh);

  config_lookup_int(&cfg, "border", &border);

  config_lookup_int(&cfg, "scale", &scale);

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
    char *state = get_value(job, "state");

    move(1, border);
    attron(A_STANDOUT);
  	printw("Yo, what's cookin!?");
    attroff(A_STANDOUT);

    move(3, border);
    attron(A_BOLD);
    printw("  Print name: ");
    attroff(A_BOLD);
    printw(name);

    move(4, border);
    attron(A_BOLD);
    printw("       Owner: ");
    attroff(A_BOLD);
    printw(user);

    move(5, border);
    clrtoeol();
    attron(A_BOLD);
    printw("Time elapsed: ");
    attroff(A_BOLD);
    if (strcmp(time_spent, "null") != 0){
      char *total_time_spent=format_time(time_spent);
      printw(total_time_spent);
    } else printw("N/A");

    move(6, border);
    attron(A_BOLD);
    printw("    Progress: ");
    attroff(A_BOLD);
    if (strcmp(percent_complete, "null") != 0){
        float float_percent = atof(percent_complete);
        int rounded_percent = (int) round(float_percent);
        printw("%i", rounded_percent);
        printw("%%\n");

        // Make a progress bar
        move(7, border);
        printw("[");
        for (int i = 0; i < (float_percent/100)*scale; i++) {
            printw("#");
        }
        for (int i = 0; i < scale-((float_percent/100)*scale); i++) {
            printw(" ");
        }
        printw("]");

        move(8, border);
        if (strcmp(state, "Printing") == 0)
            printw("Printing...");
        else if (/*strcmp(state, "Operational") == 0 &&*/ atoi(percent_complete) >= 100)
            printw("Done!");
        else
            printw(state);
    } else printw("N/A");


  	refresh(); // Update the screen

    // Clean up.
    free(user);
    free(job);
    free(time_spent);
    free(percent_complete);
    free(name);
    free(state);
    sleep(refresh); // Wait a bit to do it again.
  }
	endwin();			/* End curses mode		  */

  return 0;
}
