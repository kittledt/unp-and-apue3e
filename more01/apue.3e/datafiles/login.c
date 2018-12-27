/*****

    simulate login(1)

     get name and password, compare with  /etc/shadow,


     run: sudo  ./login
     must has root

*/

#define _XOPEN_SOURCE
#include <stdio.h>
#include <shadow.h>
#include <unistd.h>
#include <string.h>

int main (int argc, char **argv)
{
    char name[32] = "", *pwd;
     struct spwd *p;
     size_t namelen = 0;

     printf("input name：");
     fgets(name, 32, stdin);
     pwd = getpass("input password：");

    namelen = strlen(name);
     name[namelen - 1] = 0;
     printf("name= %s\n",name);
     p = getspnam(name);
     if (!p) {
        fprintf(stderr, "name or password error！\n");
         return -1;
     }

     // getspnam(3) return sp_pwdp can be used to  crpyt(3)  salt
     if (!strcmp(crypt(pwd, p->sp_pwdp), p->sp_pwdp)) {
         printf("password  correct!\n");
     } else {
         fprintf(stderr, "password error！\n");
     }

     return 0;
}
