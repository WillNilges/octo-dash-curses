#include "graphics.h"

const struct big_num big_numbers[11] = {
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

void draw_big_num(wchar_t* big_num, int y, int x) {
    int rows = 0;
    int cols = 0;
    for (int i = 0; i < 70; i++) {
        if (big_num[i] != '/') {
            wchar_t big_str[] = {big_num[i], L'\0'};
            mvaddwstr(y, x + ++cols, big_str);
        } else {
            y++;
            cols = 0;
        }
    }
}