#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <libconfig.h>
#include <ncurses.h>
#include "util.h"
#include <locale.h>

typedef struct big_num {
    wchar_t num_data[70];
} big_num;

big_num big_numbers[11] = {
    {L" ██████  /██  ████ /██ ██ ██ /████  ██ / ██████  /         /         /"},
    {L" ██      /███      / ██      / ██      / ██      /         /         /"},
    {L"██████   /     ██  / █████   /██       /███████  /         /         /"},
    {L"██████   /     ██  / █████   /     ██  /██████   /         /         /"},
    {L"██   ██  /██   ██  /███████  /     ██  /     ██  /         /         /"},
    {L"███████  /██       /███████  /     ██  /███████  /         /         /"},
    {L" ██████  /██       /███████  /██    ██ / ██████  /         /         /"},
    {L"███████  /     ██  /    ██   /   ██    /   ██    /         /         /"},
    {L" █████   /██   ██  / █████   /██   ██  / █████   /         /         /"},
    {L" █████   /██   ██  / ██████  /     ██  / █████   /         /         /"},
    {L"██  ██   /   ██    /  ██     / ██      /██  ██   /         /         /"},
};

int main(void)
{
    // Set up config variables, API variables
    config_t cfg;
    const char *ADDR;
    const char *KEY;
    const char *DASHBOARD_MESSAGE;
    const char *NO_PRINT_MESSSAGE;

    int refresh;
    int border;
    int scale;

    // Grab some configs
    config_init(&cfg);

    // Read the config file. If there is an error, report it and exit.
    if(! config_read_file(&cfg, "api.cfg"))
    {
        fprintf(stderr, "%s:%d - %s\n", config_error_file(&cfg),
        config_error_line(&cfg), config_error_text(&cfg));
        config_destroy(&cfg);
        return(EXIT_FAILURE);
    }

    // Dump those configs!
    config_lookup_string(&cfg, "url", &ADDR);
    config_lookup_string(&cfg, "key", &KEY);
    config_lookup_string(&cfg, "dashboard_message", &DASHBOARD_MESSAGE);
    config_lookup_string(&cfg, "no_print_message", &NO_PRINT_MESSSAGE);
    config_lookup_int(&cfg, "refresh", &refresh);
    config_lookup_int(&cfg, "border", &border);
    config_lookup_int(&cfg, "scale", &scale);

    // Let's get some basic info about what's printing.
    char *job_call = "/api/job";

    // Construct the curl URL
    char job_address[strlen(ADDR) + strlen(job_call)];
    strcpy(job_address, ADDR);
    strcat(job_address, job_call);

    //Specify the API path to use
    char *printer_call = "/api/printer";

    // Construct the curl URL
    char printer_address[strlen(ADDR) + strlen(printer_call)];
    strcpy(printer_address, ADDR);
    strcat(printer_address, printer_call);

    // Check if the octoprint server is alive.
    // If it's not then don't let the user open odc.
    char *printer = call_octoprint(printer_address, KEY);
    if (check_alive(printer) == 1)
    {
        printf("Error: Can't contact the OctoPrint server.\0");
        return 1;
    }

    setlocale(LC_ALL, "");
    initscr(); // Start ncurses.
    curs_set(0);           // Don't show terminal cursor

    // Set up color
    start_color();
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);
    init_pair(3, COLOR_GREEN, COLOR_BLACK);
    init_pair(4, COLOR_BLUE, COLOR_BLACK);
    init_pair(5, COLOR_BLACK, COLOR_RED);
    init_pair(6, COLOR_BLACK, COLOR_YELLOW);
    init_pair(7, COLOR_BLACK, COLOR_GREEN);
    init_pair(8, COLOR_BLACK, COLOR_BLUE);

    // Get bounds of display
    int max_row,max_col;
    getmaxyx(stdscr, max_row, max_col);
    
    for(;;)
    {
        /* === DATA COLLECTION === */

        // Get some basic info about what's printing
        char *job = call_octoprint(job_address, KEY);

        // Make sure that we don't have some kind of error message
        if (check_alive(job) == 1)
            open_error_win();
        
        // Get basic data we want to display
        char *user = get_value(job, "user");
        char *name = get_value(job, "name");
        char *time_spent = get_value(job, "printTime"); // In seconds
        char *percent_complete = get_value(job, "completion"); // In percent
        char *state = get_value(job, "state");

        // Get some basic info about the print head and print bed
        char *printer = call_octoprint(printer_address, KEY);
        if (check_alive(printer) == 1)
            open_error_win();

        // TODO: Might want to make this (tool0) customizable, since there can be
        // multiple tools.

        // Get print head temps
        char *print_head = get_value(printer, "tool0");
        char *print_head_actual_temp = get_value(print_head, "actual");
        char *print_head_target_temp = get_value(print_head, "target");

        // Get print bed temps
        char *bed = get_value(printer, "bed");
        char *bed_actual_temp = get_value(bed, "actual");
        char *bed_target_temp = get_value(bed, "target");

        // Print dashboard title
        move(1, 0);
        clrtoeol();
        move(1, max_col/2 - strlen(DASHBOARD_MESSAGE)/2);
        attron(A_STANDOUT);
        if (strcmp(state, "Printing\n") == 0)
            printw(DASHBOARD_MESSAGE);
        else 
            printw(NO_PRINT_MESSSAGE);
        attroff(A_STANDOUT);

        // Set up the titles of the data we just acquired
        const char *PRINT_NAME = "  Print name: ";
        const char *OWNER      = "       Owner: ";
        const char *PRINT_HEAD = "  Print head: ";
        const char *BED        = "         Bed: ";

        // Show the file name of the current print
        move(3, border);
        attron(A_BOLD);
        printw(PRINT_NAME);
        attroff(A_BOLD);
        if (strcmp(name, "null") != 0)
            printw(name);
        else
            printw("N/A");

        // Show the username of whoever started the print
        move(4, border);
        attron(A_BOLD);
        printw(OWNER);
        attroff(A_BOLD);
        if (strcmp(user, "null") != 0)
            printw(user);
        else
            printw("N/A");

        // Display print head temps
        move(6, border);
        clrtoeol();
        attron(A_BOLD);
        printw(PRINT_HEAD);
        attroff(A_BOLD);
        printw(print_head_actual_temp);
        int spacing = border + strlen(PRINT_HEAD) + strlen(print_head_actual_temp) - 1;
        move(6, spacing);
        printw("/");
        printw(print_head_target_temp);
        spacing += strlen(print_head_target_temp);
        move(6, spacing);
        printw(" °C");

        // Display bed temps
        move(7, border);
        clrtoeol();
        attron(A_BOLD);
        printw(BED);
        attroff(A_BOLD);
        printw(bed_actual_temp);
        spacing = border + strlen(BED) + strlen(bed_actual_temp) - 1;
        move(7, spacing);
        printw("/");
        printw(bed_target_temp);
        spacing += strlen(bed_target_temp);
        move(7, spacing);
        printw(" °C");


        // Display the progress bar and progress bar accessories
        int progress_bar_start = (max_col / 2) - (scale / 2);
        int progress_bar_y = 12;
        int prog_zone;
        if (strcmp(percent_complete, "null") != 0)
        {
            // Print printer state
            move(progress_bar_y - 1, progress_bar_start + 1);
            clrtoeol();
            if (atoi(percent_complete) >= 100)
                printw("Done!");
            else
                printw(state);

            // Display time elapsed printing
            move(11, (max_col / 2) + (scale / 2) - 7);
            if (strcmp(time_spent, "null") != 0){
                struct Duration parsed_time_spent = format_time(time_spent);
                printw(
                    "%02d:%02d:%02d",
                    parsed_time_spent.hr,
                    parsed_time_spent.min,
                    parsed_time_spent.sec
                );
            } else printw("N/A");

            // Display percent complete
            move(progress_bar_y + 1, (max_col / 2) + (scale / 2) - 3);
            float float_percent = atof(percent_complete);
            int rounded_percent = (int) round(float_percent);
            printw("%03d", rounded_percent);
            printw("%%\n");

            // Draw a progress bar
            move(progress_bar_y, progress_bar_start);
            clrtoeol();
            printw("[");
            for (int i = 0; i < (float_percent / 100) * scale; i++)
            {
                if (float_percent >= 00) prog_zone = 5;
                if (float_percent >= 25) prog_zone = 6;
                if (float_percent >= 50) prog_zone = 7;
                if (float_percent >= 75) prog_zone = 8;
                attron(COLOR_PAIR(prog_zone));
                addch(' ');
                attroff(COLOR_PAIR(prog_zone));
            }
            
            for (int i = 0; i < scale - ((float_percent / 100) * scale); i++)
                printw(" ");

            char quarter_tick = '|';

            move(progress_bar_y, progress_bar_start + border + (scale / 4));
            addch(quarter_tick);

            move(progress_bar_y, progress_bar_start + border + (scale / 2));
            addch(quarter_tick);

            move(progress_bar_y, progress_bar_start + border + (scale * 3/4));
            addch(quarter_tick);

            move(progress_bar_y, progress_bar_start + border + scale - 1);
            printw("]");

            // Big nums (fuck yea)
            int big_num_x = max_col-48;
            int big_num_y = 15;
            char fmtted_percent_complete[10];
            snprintf(fmtted_percent_complete, 10, "%03d", rounded_percent);

            for (int digit = 0; digit < 3; digit++)
                draw_big_num(big_numbers[fmtted_percent_complete[digit] - '0'].num_data, big_num_y, big_num_x + (10*(digit+1)));
            draw_big_num(big_numbers[10].num_data, big_num_y, big_num_x + 40);
        }
        else
        {
            move(progress_bar_y - 1, (max_col / 2) - (strlen("Progress: ") / 2));
            attron(A_BOLD);
            printw("Progress: ");
            attroff(A_BOLD);
            move(progress_bar_y - 1, (max_col / 2));
            printw("N/A");
        }

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
    endwin(); // End curses mode
    return 0;
}
