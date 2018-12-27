/**************
    stream control ,  1second,  read 10 bytes, and write 10 bytes.

*/
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>

#include <sys/types.h>
#include <sys/stat.h>

#define BUFSIZE    10

static volatile int loop = 0;		//set and checked by  SIG hander and main loop

static void alarm_handler (int s)
{
     alarm(1);
     loop = 0;
 }

int main (int argc, char **argv)
{
 int fd = -1;
 char buf[BUFSIZE] = "";
 ssize_t readsize = -1;
 ssize_t writesize = -1;
 size_t off = 0;

 if (argc < 2)
 {
     fprintf(stderr, "Usage %s <filepath>\n", argv[0]);
     return 1;
 }

 do {
     fd = open(argv[1], O_RDONLY);
     if (fd < 0) {
         if (EINTR != errno) {				// system call read() was interupt by SIG, then re-do
             perror("open()");				// not interupt by SIG, return .
             goto e_open;
         }
     }
 } while (fd < 0);

 loop = 1;
 signal(SIGALRM, alarm_handler);
 alarm(1);

 while(1) {
     while (loop) {
         pause();		//pause main loop, woken up by SIG alarm in 1 second
     }
     loop = 1;

     while ((readsize = read(fd, buf, BUFSIZE)) < 0) {
         if (readsize < 0) {
             if (EINTR == errno) {			// system call read() was interupt by SIG, then re-do
                 continue;
             }
             perror("read()");
             goto e_read;
         }
     }
     if (!readsize) {
         break;				// go to while(1)
     }

     off = 0;
     do {
         writesize = write(1, buf + off, readsize);
         off += writesize;
         readsize -= writesize;
     } while (readsize > 0);
 }

     close(fd);

     return 0;

 e_read:
     close(fd);
 e_open:
     return 1;
 }

