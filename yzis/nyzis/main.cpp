
#include <curses.h>
#include <signal.h>
#include <stdlib.h>

#include "nyzis.h"

static void finish(int sig);
void nyz_init_screen(void);

int
main(int argc, char *argv[])
{

    (void) signal(SIGINT, finish);      /* arrange interrupts to terminate */
    nyz_init_screen();


    YZBuffer *buffer = new YZBuffer();
    NYZView *view = new NYZView(buffer, LINES);


    /* event loop */
    for (;;)
    {
        int c = getch();     /* refresh, accept single keystroke of input */

        /* process the command keystroke */
    }

    finish(0);               /* we're done */

}


void nyz_init_screen(void)
{
    (void) initscr();      /* initialize the curses library */
    keypad(stdscr, TRUE);  /* enable keyboard mapping */
    (void) nonl();         /* tell curses not to do NL->CR/NL on output */
    (void) cbreak();       /* take input chars one at a time, no wait for \n */
    (void) echo();         /* echo input - in color */

    if (has_colors())
    {
        start_color();

        /*
         * Simple color assignment, often all we need.  Color pair 0 cannot
         * be redefined.  This example uses the same value for the color
         * pair as for the foreground color, though of course that is not
         * necessary:
         */
        init_pair(1, COLOR_RED,     COLOR_BLACK);
        init_pair(2, COLOR_GREEN,   COLOR_BLACK);
        init_pair(3, COLOR_YELLOW,  COLOR_BLACK);
        init_pair(4, COLOR_BLUE,    COLOR_BLACK);
        init_pair(5, COLOR_CYAN,    COLOR_BLACK);
        init_pair(6, COLOR_MAGENTA, COLOR_BLACK);
        init_pair(7, COLOR_WHITE,   COLOR_BLACK);
    }
}

static void finish(int sig)
{
    endwin();

    /* do your non-curses wrapup here */

    exit(0);
}

 
