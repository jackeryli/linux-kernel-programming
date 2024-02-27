// SPDX-License-Identifier: BSD-3-Clause
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h> /* signal handler*/
#include <errno.h>

#define EXEC 1

#define MAXARGS 10
#define NCOMMAND 2
#define DELIMITER " \n"

int my_cd(char**);
int my_exit(char**);

/**
 * ref: https://brennan.io/2015/01/16/write-a-shell-in-c
*/
char *builtin_str[] = {
	"cd",
	"exit"
};

int (*builtin_func[]) (char**) = {
	&my_cd,
	&my_exit
};


struct cmd {
	int type;
};

struct execcmd {
	int type;
	char *argv[MAXARGS];
};

int
my_cd(char** argv)
{
	if(argv[1] == NULL)
		return EINVAL;
	
	if(argv[2] != NULL)
		return EINVAL;
	
	if(chdir(argv[1]) != 0)
		return -1;
	
	return 0;
}

int
my_exit(char** argv)
{
	if(argv[1] != NULL){
		return EINVAL;
	}
	return 0;
}

int
my_execv(char *c, char** argv)
{
	for(int i = 0; i < NCOMMAND; i++){
		if(strcmp(c, builtin_str[i]) == 0)
			return (*builtin_func[i])(argv);
	}
	fprintf(stderr, "shell: unknown commands\n");
	return -1;
}

/**
 * ref: xv6/user/ulib.c
*/
char*
gets(char* buf, int max)
{
	int i, cc;
	char c;

	for(i=0; i+1 < max;){
		cc = read(0, &c, 1);
		if(cc < 1)
			break;
		buf[i++] = c;
		if(c == '\n' || c == '\r')
			break;
	}
	buf[i] = '\0';
	return buf;
}

struct cmd*
execcmd(void)
{
	struct execcmd *cmd;

	cmd = malloc(sizeof(*cmd));
	memset(cmd, 0, sizeof(*cmd));
	cmd->type = EXEC;
	return (struct cmd*)cmd;
}

/**
 * ref: xv6/user/sh.c
*/
int
getcmd(char* buf, int nbuf)
{
	fprintf(stderr, "$ ");
	memset(buf, 0, nbuf);
	gets(buf, nbuf);
	if(buf[0] == 0)
		return -1;
	return 0;
}

struct cmd*
parsecmd(char *s)
{
	char* token;
	int i = 0;
	struct execcmd* cmd;
	struct cmd *ret;

	ret = execcmd();
	cmd = (struct execcmd*)ret;

	token = strtok(s, DELIMITER);

	while(token != NULL){
		cmd->argv[i++] = token;
		token = strtok(NULL, DELIMITER);
		if(i > MAXARGS)
			fprintf(stderr, "shell: exceed argmax.\n");
	}
	cmd->argv[i] = 0;

	return ret;
}

void
runcmd(struct cmd* cmd)
{
	struct execcmd *ecmd;

	if(cmd == 0){
		fprintf(stderr, "shell: cmd = 0\n");
		exit(1);
	}

	switch(cmd->type){
	default:
		exit(1);
	case EXEC:
		ecmd = (struct execcmd*)cmd;
		//fprintf(stderr, "len=%ld, command=%s", strlen(ecmd->argv[0]), ecmd->argv[0]);
		int ret;
		ret = my_execv(ecmd->argv[0], ecmd->argv);
		if(ret != 0){
			fprintf(stderr, "shell: %s\n", strerror(ret));
			exit(1);
		}
		break;
	}
}

void
signal_handler(int sig)
{
	if(sig == SIGINT){
		exit(1);
	}
}

int
main(void)
{
	static char buf[100];
	int pid, wpid;
	int status;

	signal(SIGINT, signal_handler);

	while(getcmd(buf, sizeof(buf)) >= 0){
		
		pid = fork();
		if(pid < 0){
			fprintf(stderr, "shell: failed to fork.\n");
			exit(1);
		}
		
		if(pid == 0) {
			// child
			runcmd(parsecmd(buf));
		} else {
			for(;;){
				wpid = waitpid(pid, &status, WUNTRACED);

				if(WIFEXITED(status) || WIFSIGNALED(status))
					break;
			}
		}
		
	}

	return 0;
}
