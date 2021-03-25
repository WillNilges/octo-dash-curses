#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <libconfig.h>
#include <ncurses.h>
#include <locale.h>
#include <cjson/cJSON.h>

#include "api.h"
#include "graphics.h"

#define DOUBLE_SPACING (7)
#define TEMP_FORMATTING "%06.2f"

int main(void)
{
    // Set up config variables, API variables
    config_t cfg;
    const char* ADDR;
    const char* KEY;
    const char* DASHBOARD_MESSAGE;
    const char* NO_PRINT_MESSSAGE;

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
    char* job_call = "/api/job";
    char job_address[strlen(ADDR) + strlen(job_call)];
    strcpy(job_address, ADDR);
    strcat(job_address, job_call);

    // Specify the API path to use
    char* printer_call = "/api/printer";
    char printer_address[strlen(ADDR) + strlen(printer_call)];
    strcpy(printer_address, ADDR);
    strcat(printer_address, printer_call);

    // Check if the octoprint server is alive.
    // If it's not then don't let the user open odc.
    char* printer = call_octoprint(printer_address, KEY);
    if (check_alive(printer) == 1)
    {
        printf("Error: Can't contact the OctoPrint server.\n");
        return 1;
    }

    #if 1
    
    setlocale(LC_ALL, "");
    initscr();   // Start ncurses
    curs_set(0); // Don't show terminal cursor

    // Set up color
    start_color();
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);
    init_pair(3, COLOR_GREEN, COLOR_BLACK);
    init_pair(4, COLOR_BLUE, COLOR_BLACK);

    // Get bounds of display
    int max_row,max_col;
    getmaxyx(stdscr, max_row, max_col);
    
    while(1)
    {
        /* === DATA COLLECTION === */

        // Get some basic info about what's printing
        char* job = call_octoprint(job_address, KEY);
        cJSON* job_json = cJSON_Parse(job);
            cJSON* job_job_json = cJSON_GetObjectItem(job_json, "job");
                cJSON* file_json = cJSON_GetObjectItem(job_job_json, "file");
                    // cJSON* name_json = cJSON_GetObjectItem(file_json, "name");
                    char* name = cJSON_GetObjectItem(file_json, "name")->valuestring;
                // cJSON* user_json = cJSON_GetObjectItem(job_job_json, "user");
                    char* user = cJSON_GetObjectItem(job_job_json, "user")->valuestring;
            cJSON* state_json = cJSON_GetObjectItem(job_json, "state");
                char* state = state_json->valuestring;
            cJSON* progress_json = cJSON_GetObjectItem(job_json, "progress");
                // cJSON* completion_json = cJSON_GetObjectItem(progress_json, "completion");
                    double percent_complete = cJSON_GetObjectItem(progress_json, "completion")->valuedouble;
                // cJSON* print_time_json = cJSON_GetObjectItem(progress_json, "printTime");
                    int time_spent = cJSON_GetObjectItem(progress_json, "printTime")->valueint;
        
        // Get some basic info about the print head and print bed
        char* printer = call_octoprint(printer_address, KEY);
        cJSON* printer_json = cJSON_Parse(printer);
        cJSON* json_temp = cJSON_GetObjectItem(printer_json, "temperature");

        // TODO: Might want to make this (tool0) customizable, since there can be
        // multiple tools.

        // Get print head temps
        cJSON* json_print_head_temp = cJSON_GetObjectItem(json_temp, "tool0");
        cJSON* json_print_head_actual_temp = cJSON_GetObjectItem(json_print_head_temp, "actual");
        cJSON* json_print_head_target_temp = cJSON_GetObjectItem(json_print_head_temp, "target");
        double print_head_actual_temp = json_print_head_actual_temp->valuedouble;
        double print_head_target_temp = json_print_head_target_temp->valuedouble;


        // Get print bed temps
        cJSON* json_bed_temp = cJSON_GetObjectItem(json_temp, "bed");
        cJSON* json_bed_actual_temp = cJSON_GetObjectItem(json_bed_temp, "actual");
        cJSON* json_bed_target_temp = cJSON_GetObjectItem(json_bed_temp, "target");
        double bed_actual_temp = json_bed_actual_temp->valuedouble;
        double bed_target_temp = json_bed_target_temp->valuedouble;

        /* === RENDER DASHBOARD === */

        // Print dashboard title
        move(1, 0);
        clrtoeol();
        move(1, max_col/2 - strlen(DASHBOARD_MESSAGE)/2);
        attron(A_STANDOUT);
        if (strcmp(state, "Printing") == 0)
            printw(DASHBOARD_MESSAGE);
        else 
            printw(NO_PRINT_MESSSAGE);
        attroff(A_STANDOUT);

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

        int temperature_spacing;

        // Display print head temps
        move(6, border);
        clrtoeol();
        attron(A_BOLD);
        printw(PRINT_HEAD);
        attroff(A_BOLD);
        printw(
            TEMP_FORMATTING" / "TEMP_FORMATTING" °C",
            print_head_actual_temp, print_head_target_temp
        );

        // Display bed temps
        move(7, border);
        clrtoeol();
        attron(A_BOLD);
        printw(BED);
        attroff(A_BOLD);
        printw(
            TEMP_FORMATTING" / "TEMP_FORMATTING" °C",
            bed_actual_temp, bed_target_temp
        );

        // Display the progress bar and progress bar accessories
        int progress_bar_start = (max_col / 2) - (scale / 2);
        int progress_bar_y = 12;
        int prog_zone;
        if (percent_complete)
        {
            // Print printer state
            move(progress_bar_y - 1, progress_bar_start + 1);
            clrtoeol();
            if (percent_complete >= 100)
                printw("Done!");
            else
                printw(state);

            // Display time elapsed printing
            move(11, (max_col / 2) + (scale / 2) - 7);
            if (time_spent)
            {
                char unformatted_time[10];
                snprintf(unformatted_time, 10, "%d", time_spent);
                struct Duration parsed_time_spent = format_time(unformatted_time);
                printw(
                    "%02d:%02d:%02d",
                    parsed_time_spent.hr,
                    parsed_time_spent.min,
                    parsed_time_spent.sec
                );
            }
            else
                printw("N/A");

            // Display percent complete
            move(progress_bar_y + 1, (max_col / 2) + (scale / 2) - 3);
            int rounded_percent = (int) round(percent_complete);
            printw("%03d", rounded_percent);
            printw("%%\n");

            // Draw a progress bar
            //Move to the start of the bar and clear the line to re-draw the bar
            move(progress_bar_y, progress_bar_start);
            clrtoeol();
            addch('[');

            // Draw the progress into the print
            for (int i = 1; i <= (percent_complete / 100) * scale; i++)
            {
                if (percent_complete >= 00) prog_zone = 1;
                if (percent_complete >= 25) prog_zone = 2;
                if (percent_complete >= 50) prog_zone = 3;
                if (percent_complete >= 75) prog_zone = 4;
                attron(COLOR_PAIR(prog_zone));
                wchar_t bar_tick[] = {L'█', L'\0'};
                mvaddwstr(progress_bar_y, progress_bar_start + i, bar_tick);
                attroff(COLOR_PAIR(prog_zone));
            }
            
            for (int i = 0; i < scale - ((percent_complete / 100) * scale); i++)
                printw(" ");

            // Draw tick marks on the quarters
            attron(COLOR_PAIR(prog_zone));
            attron(A_BOLD);
            wchar_t quarter_tick[] = {L'▚', L'\0'};
            mvaddwstr(progress_bar_y, progress_bar_start + border + (scale / 4), quarter_tick);   // 25 %
            mvaddwstr(progress_bar_y, progress_bar_start + border + (scale / 2), quarter_tick);   // 50 %
            mvaddwstr(progress_bar_y, progress_bar_start + border + (scale * 3/4), quarter_tick); // 75 %
            attron(A_BOLD);
            attroff(COLOR_PAIR(prog_zone));

            // Cap off the progress bar
            move(progress_bar_y, progress_bar_start + border + scale - 1);
            printw("]");

            // Print the percent complete in bold
            int big_num_x = max_col/2 - 28; // 28 *roughly* centers the percentage.
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
        free(job);
        free(user);
        free(name);
        free(state);
        sleep(refresh); // Wait a bit to do it again.
    }
    endwin(); // End curses mode
    #endif
    return 0;
}
