
/*

    2 signal:
        1. SIGALRM  for anamation,  200ms interval. need reset after call handler
        2. SIGIO for keyboard input, need open fd aio_read()
        3.


*/

#include<stdio.h>
#include<curses.h>
#include<signal.h>
#include <sys/time.h>
#include<aio.h>

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

struct aiocb  kbcbuf;   // aio control block

void on_alarm_move_msg(int);     //timer handler
void on_IO_kbd_input(int );         // SIGIO handler fro keyboard input

void setup_aio_buffer();       //open SIGIO in fd

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


    int delay;
    int newdelay;


    initscr();
    crmode();       //no buffer input char
    noecho();
    clear();


    row = 10;
    col = 1;
    dir = 1;
    delay = 500;        //0.2 second

    setup_aio_buffer();        //initialize aio ctrl buff
    signal(SIGIO, on_IO_kbd_input);     //keyboard SIGIO handler
    aio_read(&kbcbuf);          //place a read request



    signal(SIGALRM, on_alarm_move_msg);
    set_ticker(delay);          // delay  =  frame speed

    move(row, col);
    addstr(message);            //draw string


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


/*  handler called when aio_read() has stuff to read */
void on_IO_kbd_input(int signum)
{
    int c = getch();
    char *cp = (char*)kbcbuf.aio_buf;   // cast to char*

    /*check for error */
    if (aio_error(&kbcbuf)!= 0)
        perror("reading failed");
    else
    /*get number of chars read */
        if(aio_return(&kbcbuf) == 1)
        {
            c = *cp;

            if(c == 'q'|| c == EOF)            //be careful   char and string
            {   printf("qqqqqqqqqqqqqq\n");
                done = 1;
            }
            else if(c == ' ')
            {   printf("revers       \n");
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

    /*place a new request , means reset for new input */
    aio_read(&kbcbuf);

}

void setup_aio_buffer()
{
    static char input[1];   //1 char for input
    /*describe what to read */
    kbcbuf.aio_fildes = 0;      //standard input
    kbcbuf.aio_buf = input;      //buffer
    kbcbuf.aio_nbytes = 1;      //number to read
    kbcbuf.aio_offset = 0;      //offset in file

    /*describe what to do when read ready */
    kbcbuf.aio_sigevent.sigev_notify = SIGEV_SIGNAL;
    kbcbuf.aio_sigevent.sigev_signo = SIGIO;       //send this signal to process
}
