#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <unistd.h>

#define DELAY_MS	1000

static void init(void)
{
	pid_t pid = fork();

	if (pid < 0) 
		exit(EXIT_FAILURE);

	if (pid > 0)
		exit(EXIT_SUCCESS);

	umask(0);

	openlog("scand", LOG_PID | LOG_CONS, LOG_DAEMON);

	pid_t sid = setsid();

	if (chdir("/") < 0)
		exit(EXIT_FAILURE);

	close(STDERR_FILENO);
	close(STDOUT_FILENO);
	close(STDIN_FILENO);
}

static void term(void)
{
	closelog();
}

void main(void)
{
	init();

	while (1) {

		sleep(DELAY_MS);
	}

	term();
}
