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

/**
 * Read stdin into buf
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

	signal(SIGINT, signal_handler);

	while(getcmd(buf, sizeof(buf)) >= 0){
		printf("%s", buf);
	}

	return 0;
}
