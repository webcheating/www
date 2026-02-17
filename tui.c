#include <ncurses.h>
#include "tui.h" 

void tui_init(){
    initscr();
    noecho();
    cbreak();
}

void tui_draw(){
    clear();
    mvprintw(0, 0, "www web server ;333");
    refresh();
}

void tui_cleanup(){
    endwin();
}
