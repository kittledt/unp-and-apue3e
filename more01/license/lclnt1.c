
/*********************************************
    license  client end,  version 1.0

    link


*/

#include<stdio.h>

int main(int argc, char *argv[])
{
    setup();                        //start

    if(get_ticket()!= 0)
        exit(0)

    do_regular_work();              //pair
    release_ticket();

    shutdown();                     //end

}

do_regular_work()
{
    printf("sleep is running - licensed software\n");
    sleep(10);

}
