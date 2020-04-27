#include <inttypes.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <unistd.h>

#define DELAY_S	5
#define STAT_ENTRIES_NUMBER 10

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

static double get_cpu_stat(void)
{
	static uint64_t prev_cpu_times[STAT_ENTRIES_NUMBER];
	static uint64_t cpu_times[STAT_ENTRIES_NUMBER];
	static uint64_t prev_sum;

	uint64_t sum = 0;

	FILE *f_stat = fopen("/proc/stat", "r");

	// skip 'cpu ' in /proc/stat
	fseek(f_stat, 4, SEEK_SET);	

	for (size_t i = 0; i < STAT_ENTRIES_NUMBER; ++i) {
		fscanf(f_stat, "%"SCNu32, &cpu_times[i]); 
		sum += cpu_times[i];
	}

	fclose(f_stat);
	
	uint64_t diff_sum = sum - prev_sum;
	uint64_t diff_idle = (cpu_times[3] + cpu_times[4]) - (prev_cpu_times[3] + prev_cpu_times[4]);

	prev_sum = sum;
	for (size_t i = 0; i < STAT_ENTRIES_NUMBER; ++i)
		prev_cpu_times[i] = cpu_times[i];

	return (double)(diff_sum - diff_idle) / diff_sum * 100;
}

static void term(void)
{
	closelog();
}

void main(void)
{
	init();

	while (1) {
		syslog(LOG_DEBUG, "%lf\n", get_cpu_stat());
		sleep(DELAY_S);
	}

	term();
}
