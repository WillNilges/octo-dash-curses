#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <ncurses.h>
#include <locale.h>
#include <string.h>
#include <unistd.h>

#include "data.h"

struct big_num {
    wchar_t num_data[70];
};

extern const struct big_num big_numbers[11];

WINDOW *create_newwin(int height, int width, int starty, int startx);

void destroy_win(WINDOW *local_win);

void open_error_win();

void draw_big_num(const wchar_t* big_num, int y, int x);

#endif