/*

    2 signal:
        1. SIGALRM  for anamation,  200ms interval. need reset after call handler
        2. SIGIO for keyboard input, need open fd O_ASYNC
        3.open fd = STDIN_FILENO= 0,  fcntl(0, F_SETOWN,getpid()); link Process and STDIN file


*/

#include<stdio.h>
#include<curses.h>
#include<signal.h>
#include <sys/time.h>
#include<fcntl.h>

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
int done = 0;       // for loop control

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

    void on_alarm_move_msg(int);     //timer handler
    void on_IO_kbd_input(int );         // SIGIO handler fro keyboard input
    void enable_kbd_signal();       //open SIGIO in fd
    int delay;
    int newdelay;


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

    signal(SIGALRM, on_alarm_move_msg);
    set_ticker(delay);          // delay  =  frame speed

    signal(SIGIO, on_IO_kbd_input);
    enable_kbd_signal();        //turn on keyboard signal


   while(!done)
    {
        pause();
    }
    endwin();
    return 0;
}

void on_alarm_move_msg(int signum)
{
    signal(SIGALRM, on_alarm_move_msg);  //reset
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

void on_IO_kbd_input(int signum)
{
    int c = getch();

    if(c == 'q') //|| c == EOF)            //be careful   char and string
    {   printf("qqqqqqqqqqqqqq\n");
        done = 1;
    }
    else if(c == ' ')
    {   printf("               \n");
        dir = -dir;         //direction reverse
    }
    /*
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
    */
}

void enable_kbd_signal()
{
    int fd_flags;
    fcntl(0	, F_SETOWN,getpid());     //F_SETOWN set which process(or proc group) id can receive SIGIO/SIGURG signal
    fd_flags = fcntl(0	,F_GETFL);            //get flag
    fcntl(0	,F_SETFL,(fd_flags | O_ASYNC));  //set
}
