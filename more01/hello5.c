


/*     use curses lib         */


#include<stdio.h>
#include<curses.h>

#define LEFTEDGE 10
#define RIGHTEDGE 30
#define TOPEDGE 10
#define BOTTUMEDGE 30
#define ROW 10

int main()
{
    char* message = "Hello";
    char* blank = "       ";
    int diry = 1;
    int dirx = 1;
    int posy = LEFTEDGE;
    int posx = TOPEDGE;
    initscr();
    clear();
    while(1)
    {
        move(posx, posy);
        addstr(message);            //draw string
        move(LINES-1,COLS-1);       //park the cursor
        refresh();
        usleep(100000);
        move(posx, posy);
        addstr(blank);              //erase string
        posy+=diry;
        posx+=dirx;
        if(posy>=RIGHTEDGE)
            diry = -1;
        if(posy<=LEFTEDGE)
            diry = 1;

         if(posx>=BOTTUMEDGE)
            dirx = -1;
        if(posx<=TOPEDGE)
            dirx = 1;
    }
    endwin();
    return 0;
}
