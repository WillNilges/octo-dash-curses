#include <curl/curl.h>
#include <stdlib.h>
#include <ncurses.h>
#include "util.h"
#include <unistd.h>

/* WHAT CAN POSSIBLY GO WRONG? */
const char *ERROR = "Can't contact the OctoPrint server.";
const char *MALFORMED = "Error: Malformed API key.";

/* STRING STUFF */

void init_string(struct string *s) {
  s->len = 0;
  s->ptr = malloc(s->len+1);
  if (s->ptr == NULL) {
    fprintf(stderr, "malloc() failed\n");
    exit(EXIT_FAILURE);
  }
  s->ptr[0] = '\0';
}

// Function to dump curl responses into strings.
size_t writefunc(void *ptr, size_t size, size_t nmemb, struct string *s) {
  size_t new_len = s->len + size*nmemb;
  s->ptr = realloc(s->ptr, new_len+1);
  if (s->ptr == NULL) {
    fprintf(stderr, "realloc() failed\n");
    exit(EXIT_FAILURE);
  }
  memcpy(s->ptr+s->len, ptr, size*nmemb);
  s->ptr[new_len] = '\0';
  s->len = new_len;

  return size*nmemb;
}

struct Duration format_time(char *time_seconds) {
  int int_time_seconds = atoi(time_seconds);

  struct Duration parsed_time;

  int seconds = int_time_seconds % 60;
  int minutes = (int_time_seconds / 60) % 60;
  int hours = int_time_seconds / 3600;

  parsed_time.hr = hours;
  parsed_time.min = minutes;
  parsed_time.sec = seconds;
  return parsed_time;
}

/* JSON STUFF */

static int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
  if (tok->type == JSMN_STRING && (int)strlen(s) == tok->end - tok->start &&
      strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
    return 0;
  }
  return -1;
}

// Parse through a JSON blob recursively to acquire a specific key.
char *get_value(char *json_blob, char *seek) {
  // Set up all the shit we need
  int r;
  jsmn_parser p;
  jsmntok_t t[128];
  char *data;

  // Initialize JSMN
  jsmn_init(&p);
  r = jsmn_parse(&p, json_blob, strlen(json_blob), t, sizeof(t) / sizeof(t[0]));
  for (int i = 1; i < r; i++) {
    // If we find what we're looking for, grab it and return it.
    if (jsoneq(json_blob, &t[i], seek) == 0) {
      data = (char *) malloc(1000); // I'm guessing this'll be less than 1kb.
      snprintf(data, 1000, "%.*s\n", t[i + 1].end - t[i + 1].start,
             json_blob + t[i + 1].start);
      return data;
    }

    // If not, but we do find another object, check to see if what we're
    // looking for is in that object.
    else if ((&t[i])->type == JSMN_OBJECT) {
      // I Love code duplication.
      data = (char *) malloc(1000); // I'm guessing this'll be less than 1kb.
      snprintf(data, 1000, "%.*s\n", t[i + 1].end - t[i + 1].start,
             json_blob + t[i + 1].start);
      char *keep_looking = get_value(data, seek);
      free(data);
    }
  }
  // If the JSON doesn't contain the key you're looking for, then return this.
  char *fail = "Couldn't find anything!";
  return fail;
}

/* OCTOPRINT STUFF */

// Query the octoprint server for data. This is a stupid CURL request, and does not check
// if it was actually successful.
char *call_octoprint(char *api_call, const char *key) {
  CURL *curl;
  CURLcode res;

  struct curl_slist *list = NULL;

  curl = curl_easy_init();
  if(curl) {
    struct string s;
    init_string(&s);

    curl_easy_setopt(curl, CURLOPT_URL, api_call);
    if (strlen(key) != 32) { // I believe all OctoPrint API keys are exactly 32 bytes long. 
      printf("%s\n", MALFORMED); // I'm lazy, so this is how I'm gonna report errors.
      curl_slist_free_all(list);
      curl_easy_cleanup(curl);
      return (char *) MALFORMED;
    }
    char keybase[32+11] = "X-Api-Key: "; // Memory really do be like that.
    strcat(keybase, key);
    list = curl_slist_append(list, keybase);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);
    res = curl_easy_perform(curl);
    if (res != 0) {
      printf("API call returned nonzero value.");
      curl_slist_free_all(list);
      curl_easy_cleanup(curl);
      return (char *) ERROR;
    }
    /* always cleanup */
    curl_slist_free_all(list);
    curl_easy_cleanup(curl);
    return s.ptr;
  }
  return 0;
}

// Checks contents of API reply for every possible issue.
// Returns 0 if OK and 1 if not OK.
// TODO: Add more issues as they arise :P
int check_alive(char *reply) {
  // Forgive me father for I have sinned
  if (
    strstr(reply, "OctoPrint is currently not running")  ||
    strstr(reply, "Can't contact the OctoPrint server.") ||
    strstr(reply, "Error: Malformed API key.")           ||
    strstr(reply, "403 Forbidden")                       ||
    strstr(reply, "Offline")
  ) return 1;
  return 0;
}

// Some dumb ncurses shit

WINDOW *create_newwin(int height, int width, int starty, int startx)
{	WINDOW *local_win;

	local_win = newwin(height, width, starty, startx);
	box(local_win, 0 , 0);		/* 0, 0 gives default characters 
					 * for the vertical and horizontal
					 * lines			*/
	wrefresh(local_win);		/* Show that box 		*/

	return local_win;
}

void destroy_win(WINDOW *local_win)
{	
	/* box(local_win, ' ', ' '); : This won't produce the desired
	 * result of erasing the window. It will leave it's four corners 
	 * and so an ugly remnant of window. 
	 *
   * The parameters taken are 
   * 1. win: the window on which to operate
   * 2. ls: character to be used for the left side of the window 
   * 3. rs: character to be used for the right side of the window 
   * 4. ts: character to be used for the top side of the window 
   * 5. bs: character to be used for the bottom side of the window 
   * 6. tl: character to be used for the top left corner of the window 
   * 7. tr: character to be used for the top right corner of the window 
   * 8. bl: character to be used for the bottom left corner of the window 
   * 9. br: character to be used for the bottom right corner of the window
   */
	wborder(local_win, ' ', ' ', ' ',' ',' ',' ',' ',' ');
	wrefresh(local_win);
	delwin(local_win);
}

void open_error_win() {
  WINDOW *error_win;
  char *retry = "Retrying in %d seconds.";
  int startx, starty, width, height;
  int ch;
  height = 6;
  width = 40;
  starty = (LINES - height) / 2;	/* Calculating for a center placement */
  startx = (COLS - width) / 2;	/* of the window		*/
  refresh();
  error_win = create_newwin(height, width, starty, startx);

  // Just gonna leave this mess here because
  // I can't decide on a theme.

  wattron(error_win, A_BOLD);
  // wattron(error_win, A_STANDOUT);
  wattron(error_win, COLOR_PAIR(1));
  mvwprintw(error_win, 0, 1, "Error:");
  // wattroff(error_win, A_STANDOUT);
  // wattron(error_win, A_BOLD);
  mvwprintw(error_win, 2, (width-strlen(ERROR))/2, ERROR);
  wattroff(error_win, A_BOLD);
  wattroff(error_win, COLOR_PAIR(1));

  // Blocking loop to wait about 10 seconds to try contacting
  // OctoPrint again. We'll have to see how well this performs.
  for (int i = 0; i < 10; i++) {
    mvwprintw(error_win, 3, (width-strlen(retry))/2, retry, 10-i);
    wrefresh(error_win);
    refresh();
    sleep(1);
  }
  destroy_win(error_win);
}

// void draw_big_num(char* big_num, int y, int x) {
//     if (big_num == NULL) return;
    
//     // int pos = 0;
//     // while (big_num[pos] != '\0') {
//     //     if (big_num[pos] != '/') {
//     //         addch(big_num[pos]);
//     //     } else {
//     //         move(y + ++rows, x);
//     //     }
//     //     pos++;
//     // }

//     move(y, x);
//     int rows = 0;
//     for (size_t i = 0; i < strlen(big_num); i++) {
//     char c = big_num[i];
//     if (c != '/') {
//         addch(c);
//     } else {
//         move(y + ++rows, x);
//     }
//     pos++;
//     }
// }

void draw_big_num(char* big_num, int y, int x) {
    int rows = 0;
    move(y, x);
    for (int i = 0; i < 70; i++) {
        if (big_num[i] != '/') {
            addch(big_num[i]);
        } else {
            move(y + ++rows, x);
        }
    }
}