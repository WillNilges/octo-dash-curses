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

#define JOB_PATH "/api/job"
#define PRINTER_PATH "/api/printer"

int main(void)
{

    // Set up config variables, API variables
    config_t cfg;
    const char* ADDR;
    const char* KEY;
    const char* DASHBOARD_MESSAGE;
    const char* NO_PRINT_MESSSAGE;

    int REFRESH;
    int BORDER;
    int SCALE;

    // Strings for json blobs
    char* printer;
    char* job;
    CURL* curl;

    // Job stuff
    cJSON* job_json;

    char* name;
    char* user;
    char* state;
    double percent_complete;
    int time_spent;

    // Printer stuff
    cJSON* printer_json;

    double print_head_actual_temp;
    double print_head_target_temp;
    double bed_actual_temp;
    double bed_target_temp;

    /* === CONFIG PARSING === */
    config_init(&cfg);

    // Read the config file. If there is an error, report it and exit.
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
    config_lookup_string(&cfg, "no_print_message", &NO_PRINT_MESSSAGE);
    config_lookup_int(&cfg, "refresh", &REFRESH);
    config_lookup_int(&cfg, "border", &BORDER);
    config_lookup_int(&cfg, "scale", &SCALE);

    // Assemble API URLs
    char job_address[strlen(ADDR) + strlen(JOB_PATH) + 1];
    snprintf(
        job_address,
        strlen(ADDR) + strlen(JOB_PATH) + 1,
        "%s%s",
        ADDR,
        JOB_PATH
    );

    char printer_address[strlen(ADDR) + strlen(PRINTER_PATH) + 1];
    snprintf(
        printer_address,
        strlen(ADDR) + strlen(PRINTER_PATH) + 1,
        "%s%s",
        ADDR,
        PRINTER_PATH
    );

    // Set up libcurl
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();

    // Check if the octoprint server is alive
    // If it's not then don't let the user open odc
    printer = octoprint_comm_recv(curl, printer_address, KEY);
    if (!printer)
    {
        fprintf(stderr, "Error: Can't contact the OctoPrint server.\n");
        exit(1);
    }

    job = octoprint_comm_recv(curl, job_address, KEY);
    

    printf(job);

    printf(printer);

    #if 1 // For debugging. Don't ask.
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
    
    timeout(0);

    while(getch() != 'q')
    {
        /* === DATA COLLECTION === */
        job = octoprint_comm_recv(curl, job_address, KEY);
        job_json = cJSON_Parse(job);

        user = cJSON_GetArrayItem(cJSON_GetArrayItem(job_json, 0), 5)->valuestring; 
        name = cJSON_GetArrayItem(cJSON_GetArrayItem(cJSON_GetArrayItem(job_json, 0), 3), 2)->valuestring;
        state = cJSON_GetArrayItem(job_json, 2)->valuestring;
        percent_complete = cJSON_GetArrayItem(cJSON_GetArrayItem(job_json, 1), 0)->valuedouble;
        time_spent = cJSON_GetArrayItem(cJSON_GetArrayItem(job_json, 1), 2)->valuedouble;

        printer = octoprint_comm_recv(curl, printer_address, KEY);
        printer_json = cJSON_Parse(printer);

        bed_actual_temp = cJSON_GetArrayItem(cJSON_GetArrayItem(cJSON_GetArrayItem(printer_json, 2), 0), 0)->valuedouble;
        bed_target_temp = cJSON_GetArrayItem(cJSON_GetArrayItem(cJSON_GetArrayItem(printer_json, 2), 0), 2)->valuedouble;

        print_head_actual_temp = cJSON_GetArrayItem(cJSON_GetArrayItem(cJSON_GetArrayItem(printer_json, 2), 1), 0)->valuedouble;
        print_head_target_temp = cJSON_GetArrayItem(cJSON_GetArrayItem(cJSON_GetArrayItem(printer_json, 2), 1), 2)->valuedouble;


        /* === RENDER DASHBOARD === */
        int current_line = BORDER/2;

        // Print dashboard title
        move(current_line, 0);
        clrtoeol();
        move(current_line, max_col/2 - strlen(DASHBOARD_MESSAGE)/2);
        attron(A_STANDOUT);
        if (strcmp(state, "Printing") == 0)
            printw(DASHBOARD_MESSAGE);
        else 
            printw(NO_PRINT_MESSSAGE);
        attroff(A_STANDOUT);

        // Show the file name of the current print
        current_line += 2;
        move(current_line, BORDER);
        attron(A_BOLD);
        printw(PRINT_NAME);
        attroff(A_BOLD);
        if (strcmp(state, "Printing") == 0)
            printw(name);
        else
            printw("N/A");

        // Show the username of whoever started the print
        current_line++;
        move(current_line, BORDER);
        attron(A_BOLD);
        printw(OWNER);
        attroff(A_BOLD);
        if (strcmp(state, "Printing") == 0)
            printw(user);
        else
            printw("N/A");

        // Display print head temps
        current_line += 2;
        move(current_line, BORDER);
        clrtoeol();
        attron(A_BOLD);
        printw(PRINT_HEAD);
        attroff(A_BOLD);
        printw(
            TEMP_FORMATTING" / "TEMP_FORMATTING" °C",
            print_head_actual_temp, print_head_target_temp
        );

        // Display bed temps
        current_line++;
        move(current_line, BORDER);
        clrtoeol();
        attron(A_BOLD);
        printw(BED);
        attroff(A_BOLD);
        printw(
            TEMP_FORMATTING" / "TEMP_FORMATTING" °C",
            bed_actual_temp, bed_target_temp
        );

        // Display the progress bar and progress bar accessories
        int progress_bar_start = (max_col / 2) - (SCALE / 2);
        current_line += 3;
        int prog_zone;
        if (!percent_complete) percent_complete = 0.0;
            // Print printer state
            move(current_line - 1, progress_bar_start + 1);
            clrtoeol();
            if (percent_complete >= 100)
                printw("Done!");
            else
                printw(state);

            // Display time elapsed printing
            if (time_spent)
            {
                move(current_line - 1, (max_col / 2) + (SCALE / 2) - 7);
                struct Duration parsed_time_spent = format_time(time_spent);
                printw(
                    "%02d:%02d:%02d",
                    parsed_time_spent.hr,
                    parsed_time_spent.min,
                    parsed_time_spent.sec
                );
            }
            else
            {
                move(current_line - 1, (max_col / 2) + (SCALE / 2) - strlen("No print is queued.") + 1);
                printw("No print is queued.");
            }

            // Display percent complete
            move(current_line + 1, (max_col / 2) + (SCALE / 2) - 3);
            int rounded_percent = (int) round(percent_complete);
            printw("%03d", rounded_percent);
            printw("%%\n");

            // Draw a progress bar
            //Move to the start of the bar and clear the line to re-draw the bar
            move(current_line, progress_bar_start);
            clrtoeol();
            attron(A_BOLD);
            addch('[');
            attroff(A_BOLD);

            // Draw the progress into the print
            for (int i = 1; i <= (percent_complete / 100) * SCALE; i++)
            {
                if (percent_complete >= 00) prog_zone = 1;
                if (percent_complete >= 25) prog_zone = 2;
                if (percent_complete >= 50) prog_zone = 3;
                if (percent_complete >= 75) prog_zone = 4;
                attron(COLOR_PAIR(prog_zone));
                wchar_t bar_tick[] = {L'█', L'\0'};
                mvaddwstr(current_line, progress_bar_start + i, bar_tick);
                attroff(COLOR_PAIR(prog_zone));
            }
            
            for (int i = 0; i < SCALE - ((percent_complete / 100) * SCALE); i++)
                printw(" ");

            // Draw tick marks on the quarters
            attron(COLOR_PAIR(prog_zone));
            attron(A_BOLD);
            wchar_t quarter_tick[] = {L'▚', L'\0'};
            mvaddwstr(current_line, progress_bar_start + (SCALE * 0.25), quarter_tick);
            mvaddwstr(current_line, progress_bar_start + (SCALE * 0.50), quarter_tick);
            mvaddwstr(current_line, progress_bar_start + (SCALE * 0.75), quarter_tick);
            attron(A_BOLD);
            attroff(COLOR_PAIR(prog_zone));

            // Cap off the progress bar
            move(current_line, progress_bar_start + BORDER + SCALE - 1);
            printw("]");

            // Print the percent complete in bold
            int big_num_x = max_col/2 - 28; // 28 *roughly* centers the percentage.
            current_line += 3;
            char fmtted_percent_complete[10];
            snprintf(fmtted_percent_complete, 10, "%03d", rounded_percent);

            for (int digit = 0; digit < 3; digit++)
                draw_big_num(big_numbers[fmtted_percent_complete[digit] - '0'].num_data, current_line, big_num_x + (10*(digit+1)));
            draw_big_num(big_numbers[10].num_data, current_line, big_num_x + 40);
        // }
        // else
        // {   
        //     move(current_line - 1, (max_col / 2) - (strlen("Progress: ") / 2));
        //     attron(A_BOLD);
        //     printw("Progress: ");
        //     attroff(A_BOLD);
        //     move(current_line - 1, (max_col / 2));
        //     printw("N/A");
        // }
        
        current_line += 6;
        move(current_line, BORDER);
        clrtoeol();
        attroff(A_BOLD);
        printw("(refreshing every %d seconds)", REFRESH);
        move(current_line, max_col - 18 - BORDER);
        printw("press 'q' to quit.");

        refresh(); // Update the screen

        sleep(REFRESH); // Wait a bit to do it again.
    
        // Clean up cJSON
        cJSON_Delete(job_json);
        cJSON_Delete(printer_json);

        // Delete those dangling pointers!
        free(printer);
        free(job);
    }
    endwin(); // End curses mode
    #endif

    // Clean up config
    config_destroy(&cfg);

    // Clean up curl
    curl_easy_cleanup(curl);
    curl_global_cleanup();
    return 0;
}
