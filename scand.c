#include <inttypes.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <unistd.h>

#define DELAY_S			5

#define STAT_ENTRIES_NUMBER	10
#define STAT_IOWAIT_POS		4
#define STAT_IDLE_POS		3

#define MEM_SWAPTOTAL_POS	14
#define MEM_SWAPFREE_POS	15
#define MEM_TOTAL_POS		0
#define MEM_AVAILABLE_POS	2

#define BUF_SIZE		256

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
		fscanf(f_stat, "%"SCNu64, &cpu_times[i]); 
		sum += cpu_times[i];
	}

	uint64_t diff_sum = sum - prev_sum;
	uint64_t diff_idle = (cpu_times[STAT_IDLE_POS] + cpu_times[STAT_IOWAIT_POS])
			- (prev_cpu_times[STAT_IDLE_POS] + prev_cpu_times[STAT_IOWAIT_POS]);

	prev_sum = sum;
	for (size_t i = 0; i < STAT_ENTRIES_NUMBER; ++i)
		prev_cpu_times[i] = cpu_times[i];

	fclose(f_stat);

	return (double)(diff_sum - diff_idle) / diff_sum * 100;
}

static uint64_t get_cpu_iowait(void)
{
	uint64_t iowait_time;

	FILE *f_stat = fopen("/proc/stat", "r");

	// skip 'cpu ' in /proc/stat
	fseek(f_stat, 4, SEEK_SET);

	for (size_t i = 0; i < STAT_IOWAIT_POS + 1; ++i)
		fscanf(f_stat, "%"SCNu64, &iowait_time); 

	fclose(f_stat);

	return iowait_time;
}

static double get_mem_swap(void)
{
	static char buf[BUF_SIZE];
	uint64_t swap_total, swap_free;

	FILE *f_mem = fopen("/proc/meminfo", "r");

	// skip unnecessary lines	
	for (size_t i = 0; i < MEM_SWAPTOTAL_POS; ++i)
		fgets(buf, BUF_SIZE, f_mem);

	fscanf(f_mem, "%s%"SCNu64"%s", buf, &swap_total, buf);
	fscanf(f_mem, "%s%"SCNu64"%s", buf, &swap_free, buf);

	fclose(f_mem);
	
	return (double)swap_free / swap_total;
}

static double get_mem_total(void)
{
	static char buf[BUF_SIZE];
	uint64_t mem_avail, mem_total;

	FILE *f_mem = fopen("/proc/meminfo", "r");

	fscanf(f_mem, "%s%"SCNu64"%s", buf, &mem_total, buf);

	// skip unnecessary lines	
	fgets(buf, BUF_SIZE, f_mem);

	fscanf(f_mem, "%s%"SCNu64"%s", buf, &mem_avail, buf);

	fclose(f_mem);
	
	return (double)mem_avail / mem_total;
}

static uint64_t get_disk_ioproc(void)
{
}

static uint64_t get_disk_opcount(void)
{
}

static uint64_t get_disk_iowait(void)
{
}

static void term(void)
{
	closelog();
}

void main(void)
{
	init();

	get_mem_swap();
	while (1) {
		syslog(LOG_DEBUG, "%lf\n", get_cpu_stat());
		syslog(LOG_DEBUG, "%"PRIu64, get_cpu_iowait());
		sleep(DELAY_S);
	}

	term();
}
