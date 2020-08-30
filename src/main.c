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
  const char *dashboard_message;
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

  config_lookup_string(&cfg, "url", &addr);
  config_lookup_string(&cfg, "key", &key);
  config_lookup_string(&cfg, "dashboard_message", &dashboard_message);
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
  start_color();
  init_pair(1, COLOR_RED, COLOR_BLACK);
  init_pair(2, COLOR_YELLOW, COLOR_BLACK);
  init_pair(3, COLOR_GREEN, COLOR_BLACK);
  init_pair(4, COLOR_BLUE, COLOR_BLACK);
  init_pair(5, COLOR_BLACK, COLOR_RED);
  init_pair(6, COLOR_BLACK, COLOR_YELLOW);
  init_pair(7, COLOR_BLACK, COLOR_GREEN);
  init_pair(8, COLOR_BLACK, COLOR_BLUE);
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
  	printw(dashboard_message);
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
      char *parsed_time_spent=format_time(time_spent);
      printw(parsed_time_spent);
      free(parsed_time_spent);
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
        int prog_zone;
        for (int i = 0; i < (float_percent/100)*scale; i++) {
            if (float_percent >= 00) prog_zone = 1;
            if (float_percent >= 25) prog_zone = 2;
            if (float_percent >= 50) prog_zone = 3;
            if (float_percent >= 75) prog_zone = 4;
            attron(COLOR_PAIR(prog_zone));
            addch(ACS_CKBOARD);
            attroff(COLOR_PAIR(prog_zone));
        }
        for (int i = 0; i < scale-((float_percent/100)*scale); i++) {
            printw(" ");
        }
        if (float_percent >= 25) attron(COLOR_PAIR(4+prog_zone));
        move(7, border+(scale/4));
        addch('.');
        attroff(COLOR_PAIR(4+prog_zone));

        if (float_percent >= 50) attron(COLOR_PAIR(4+prog_zone));
        move(7, border+(scale/2));
        addch('.');
        attroff(COLOR_PAIR(4+prog_zone));
        if (float_percent >= 75) attron(COLOR_PAIR(4+prog_zone));
        move(7, border+(scale*3/4));
        addch('.');
        attroff(COLOR_PAIR(4+prog_zone));
        move(7, border+scale);
        printw("]");

        // Print printer state (TODO: Parse this better :/)
        move(8, border);
        if (/*strcmp(state, "Operational") == 0 &&*/ atoi(percent_complete) >= 100)
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
