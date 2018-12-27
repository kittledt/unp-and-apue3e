
/* prompt shell ver1.0

    use  fork  wait

    execvp();   exit

parent proc ignore SIGINT and SIGQUIT, only CTRL+D to quit
child proc havs SIGINT and SIGQUIT.

 */

 #include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <stddef.h>
#include <sys/wait.h>
#include <string.h>

#define DFL_PROMPT ">"
#define	is_delim(x) ((x)==' '||(x)=='/t')
#define YES 1
#define NO 0


char* next_cmd();
char** splitline(char*);
int execute(char**);

void freelist(char**);
void* emalloc(size_t);
void * erealloc(void *, size_t );
void fatal(char*, char*, int);

void setup();       //ignore signal

int main(int argc, char *argv[])
{
	char *cmdline, *prompt, **arglist;
	int result;

	prompt = (char*)DFL_PROMPT;
	setup();

	while((cmdline = next_cmd(prompt, stdin)) != NULL) {
		if((arglist = splitline(cmdline)) != NULL) {
			result = execute(arglist);
			freelist(arglist);
		}
		free(cmdline);
	}
	return 0;
}

void setup()
{
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
}

void fatal(char *s1, char *s2, int n)
{
	fprintf(stderr, "Error: %s, %s\n", s1, s2);
	exit(n);
}


int execute(char *argv[])
{
	int pid;
	int child_info = -1;
	if(argv[0] == NULL) {
		return 0;
	}else if((pid = fork()) == -1) {
		perror("fork");
	}else if(pid == 0) {
		signal(SIGINT, SIG_DFL);
		signal(SIGQUIT, SIG_DFL);
		execvp(argv[0], argv);
		perror("cannot execute command");
		exit(1);
	}else{
		if(wait(&child_info) == -1) {
			perror("Wait");
		}
	}
	return child_info;
}


// read a line until '\n'
// the reason do not use fgets is tha we could not know the length of the line
char* next_cmd(char *prompt, FILE *fp)
{
	char *buf;
	int bufspace = 0;
	int pos = 0;
	int c;

	printf("%s",prompt);

	while((c = getc(fp)) != EOF)
	{
		if(pos + 1 >= bufspace)         //1 for '\0'
		{
			if(bufspace == 0)           //1st malloc 1024 size
			{
				buf = emalloc(BUFSIZ);
			}
			else                        //expand buf
			{
				buf = erealloc(buf, bufspace + BUFSIZ);
			}
			bufspace += BUFSIZ;
			printf("bufspace = %d\n",bufspace);

		}
		if(c == '\n')           // end of command
		{
			break;
		}                       //
		buf[pos++] = c;
	}
	if(c == EOF && pos == 0)    //EOF and no input
	{
		return NULL;
	}

	buf[pos] = '\0';
	printf("buf = %s\n",buf);
	return buf;
}


char** splitline(char *line)
{
	char* newstr();
	char** args;
	int spots = 0;
	int bufspace = 0;
	int argnum = 0;
	char* cp = line;            //cp point to input string
	char* start;
	int len;

	if(line == NULL) {
		return NULL;
	}

	args = (char**)emalloc(BUFSIZ);
	bufspace = BUFSIZ;
	spots = BUFSIZ/sizeof(char*);

	while(*cp != '\0') {
		//only skip leading spaces
		while(is_delim(*cp)) {
			cp++;       //delimter , skip
		}
		if(*cp == '\0'){
			break;
		}
		if(argnum + 1 >= spots)
		{
			args = erealloc(args, bufspace + BUFSIZ);
			bufspace += BUFSIZ;
			spots += (BUFSIZ/sizeof(char*));
		}
		start = cp;
		len = 1;
		while(*++cp != '\0' && !(is_delim(*cp))) {
			len++;
		}
		args[argnum] = newstr(start, len);
		printf("args[%d] = %s\n",argnum,args[argnum]);
		argnum++;
	}
	args[argnum] = NULL;
	return args;
}

char* newstr(char* s, int len)
{
	char* rv = malloc(len + 1);
	rv[len] = '\0';
	strncpy(rv,s, len);
	return rv;
}

void freelist(char** list)
{
	char** cp = list;
	while(*cp) {
		free(*cp++);
	}
	free(list);
}

void* emalloc(size_t n)
{
	void* rv;
	if((rv = malloc(n)) == NULL) {
		fatal("out of memory","", 1);
	}
	return rv;
}
void * erealloc(void *p, size_t n)
{
 	void *rv;
 	if ( (rv = realloc(p,n)) == NULL )
 		fatal("realloc() failed","",1);
 	return rv;
}
