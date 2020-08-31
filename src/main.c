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
    const char *ADDR;
    const char *KEY;
    const char *DASHBOARD_MESSAGE;
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

    config_lookup_string(&cfg, "url", &ADDR);
    config_lookup_string(&cfg, "key", &KEY);
    config_lookup_string(&cfg, "dashboard_message", &DASHBOARD_MESSAGE);
    config_lookup_int(&cfg, "refresh", &refresh);
    config_lookup_int(&cfg, "border", &border);
    config_lookup_int(&cfg, "scale", &scale);

    // Let's get some basic info about what's printing.
    char *job_call = "/api/job";

    // Construct the curl URL
    char job_address[strlen(ADDR) + strlen(job_call)];
    strcpy(job_address, ADDR);
    strcat(job_address, job_call);

    // Let's get some basic info about what's printing.
    char *printer_call = "/api/printer";

    // Construct the curl URL
    char printer_address[strlen(ADDR) + strlen(printer_call)];
    strcpy(printer_address, ADDR);
    strcat(printer_address, printer_call);

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
        char *job = call_octoprint(job_address, KEY);
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

        char *printer = call_octoprint(printer_address, KEY);
        if (strcmp(printer, "-1") == 0) {
            endwin();
            printf("Error: Can't contact the OctoPrint server.");
            return 1;
        }

        //TODO: Might want to make this customizable, since there can be
        // multiple tools.
        char *print_head = get_value(printer, "tool0");
        // So, the API call looks weird. This might be hard.
        char *print_head_actual_temp = get_value(print_head, "actual");
        char *print_head_target_temp = get_value(print_head, "target");

        char *bed = get_value(printer, "bed");
        // So, the API call looks weird. This might be hard.
        char *bed_actual_temp = get_value(bed, "actual");
        char *bed_target_temp = get_value(bed, "target");

        move(1, border);
        attron(A_STANDOUT);
        printw(DASHBOARD_MESSAGE);
        attroff(A_STANDOUT);

        // All the titles
        const char *PRINT_NAME = "  Print name: ";
        const char *OWNER      = "       Owner: ";
        const char *PRINT_HEAD = "  Print head: ";
        const char *BED        = "         Bed: ";

        move(3, border);
        attron(A_BOLD);
        printw(PRINT_NAME);
        attroff(A_BOLD);
        printw(name);

        move(4, border);
        attron(A_BOLD);
        printw(OWNER);
        attroff(A_BOLD);
        printw(user);

        // Info about the print head
        move(6, border);
        clrtoeol();
        attron(A_BOLD);
        printw(PRINT_HEAD);
        attroff(A_BOLD);
        printw(print_head_actual_temp);
        move(6, border + strlen(PRINT_HEAD) + strlen(print_head_actual_temp) - 1);
        printw("/");
        printw(print_head_target_temp);
        move(6, border + strlen(PRINT_HEAD) + strlen(print_head_target_temp) + strlen(print_head_actual_temp) - 1);
        printw(" °C");

        // Info about the print head
        move(7, border);
        clrtoeol();
        attron(A_BOLD);
        printw(BED);
        attroff(A_BOLD);
        printw(print_head_actual_temp);
        move(7, border + strlen(BED) + strlen(bed_actual_temp) - 1);
        printw("/");
        printw(print_head_target_temp);
        move(7, border + strlen(BED) + strlen(bed_target_temp) + strlen(bed_actual_temp) - 1);
        printw(" °C");

        // Time spent printing
        move(9, border);
        clrtoeol();
        attron(A_BOLD);
        printw("Time elapsed: ");
        attroff(A_BOLD);
        if (strcmp(time_spent, "null") != 0){
            char *parsed_time_spent=format_time(time_spent);
            printw(parsed_time_spent);
            free(parsed_time_spent);
        } else printw("N/A");

        // How far along we are.
        move(10, border);
        attron(A_BOLD);
        printw("    Progress: ");
        attroff(A_BOLD);
        if (strcmp(percent_complete, "null") != 0){
            float float_percent = atof(percent_complete);
            int rounded_percent = (int) round(float_percent);
            printw("%i", rounded_percent);
            printw("%%\n");

            // Make a progress bar
            move(11, border);
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
            move(11, border+(scale/4));
            addch('.');
            attroff(COLOR_PAIR(4+prog_zone));

            if (float_percent >= 50) attron(COLOR_PAIR(4+prog_zone));
            move(11, border+(scale/2));
            addch('.');
            attroff(COLOR_PAIR(4+prog_zone));
            if (float_percent >= 75) attron(COLOR_PAIR(4+prog_zone));
            move(11, border+(scale*3/4));
            addch('.');
            attroff(COLOR_PAIR(4+prog_zone));
            move(11, border+scale);
            printw("]");

            // Print printer state (TODO: Parse this better :/)
            move(12, border);
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
