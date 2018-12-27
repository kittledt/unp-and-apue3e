
#include<stdio.h>
#include<curses.h>
#include<signal.h>
#include <sys/time.h>

#define LEFTEDGE 10
#define RIGHTEDGE 30
#define TOPEDGE 10
#define BOTTUMEDGE 30
#define ROW 10

    char* message = "Hello";
    char* blank = "       ";

    int row = 0;           //this 3 must global
    int col = 0;
    int dir;    // direction to go

int set_ticker(int n_msecs) /* n_msecs is m seconds */
{
    struct itimerval new_timeset;
    long n_sec, n_usecs;

    n_sec = n_msecs/1000;   /*The seconds */
    n_usecs = (n_msecs%1000) * 1000L;    /*The u seconds */

    new_timeset.it_interval.tv_sec = n_sec;
    new_timeset.it_interval.tv_usec = n_usecs;

    new_timeset.it_value.tv_sec = n_sec;
    new_timeset.it_value.tv_usec = n_usecs;

    return setitimer(ITIMER_REAL, &new_timeset, NULL);
}


int main()
{

    void move_msg(int);     //timer handler
    int delay;
    int newdelay;

    int c;      //user input

    initscr();
    crmode();       //no buffer input char
    noecho();
    clear();


    row = 10;
    col = 1;
    dir = 1;
    delay = 100;        //0.2 second


    move(row, col);
    addstr(message);            //draw string
    signal(SIGALRM, move_msg);
    set_ticker(delay);          // delay  =  frame speed


   while(1)
    {
        // user change direction and speed.
        newdelay = 0;
        c = getch();

        if(c == 'q')            //be careful   char and string
        {   //printf("qqqqqqqqqqqqqq\n")
            break;
        }
        if(c == ' ')
        {   //printf("               \n")
            dir = -dir;         //direction reverse
        }
        if(c == 'f'&& delay > 2)
        {
            newdelay = delay/2;
        }
        if(c == 's')
        {
            newdelay = delay*2;
        }

        if (newdelay > 0)
        {
            set_ticker(delay= newdelay);
        }


    }
    endwin();
    return 0;
}

void move_msg(int signum)
{
    signal(SIGALRM, move_msg);  //reset
    move(row, col);
    addstr(blank);            //draw string
    col+= dir;

    move(row, col);
    addstr(message);
    refresh();

    // handle border
    if(dir== -1 && col <= 0)
        dir = 1;
    else if(dir== 1 && col + strlen(message)>= COLS)
        dir = -1;

}
