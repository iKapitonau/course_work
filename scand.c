#include <inttypes.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>

#define DELAY_S			5

#define STAT_ENTRIES_NUMBER	10
#define STAT_IOWAIT_POS		4
#define STAT_IDLE_POS		3

#define MEM_ENTRIES_NUMBER	53
#define MEM_SWAPTOTAL_POS	14
#define MEM_SWAPFREE_POS	15
#define MEM_TOTAL_POS		0
#define MEM_AVAILABLE_POS	2

#define DISK_OPCOUNT_POS	8
#define DISK_IOTIME_POS		9

#define BUF_SIZE		256

static char buf[BUF_SIZE];

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

uint64_t get_cpu_useful(void)
{
	uint64_t sum = 0;

	FILE *f_stat = fopen("/proc/stat", "r");

	// skip 'cpu ' in /proc/stat
	fseek(f_stat, 4, SEEK_SET);

	for (size_t i = 0; i < STAT_ENTRIES_NUMBER; ++i) {
		uint64_t cpu_time;
		fscanf(f_stat, "%"SCNu64, &cpu_time); 
		if (i != 3 && i != 4)
			sum += cpu_time;
	}

	fclose(f_stat);

	return sum;
}

uint64_t get_cpu_idle(void)
{
	uint64_t idle_time;

	FILE *f_stat = fopen("/proc/stat", "r");

	// skip 'cpu ' in /proc/stat
	fseek(f_stat, 4, SEEK_SET);

	for (size_t i = 0; i < STAT_IDLE_POS + 1; ++i)
		fscanf(f_stat, "%"SCNu64, &idle_time); 

	fclose(f_stat);

	return idle_time;
}

uint64_t get_cpu_iowait(void)
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

double get_mem_swap(void)
{
	uint64_t swap_total, swap_free;
	char localbuf[BUF_SIZE];
	int cnt = 0;

	FILE *f_mem = fopen("/proc/meminfo", "r");

	for (size_t i = 0; i < MEM_ENTRIES_NUMBER; ++i) {
		fgets(buf, BUF_SIZE, f_mem);
		if (strstr(buf, "SwapTotal") != NULL) {
			sscanf(buf, "%s%"SCNu64"%s", localbuf, &swap_total, localbuf);
			++cnt;
		}
		if (strstr(buf, "SwapFree") != NULL) {
			sscanf(buf, "%s%"SCNu64"%s", localbuf, &swap_free, localbuf);
			++cnt;
		}
		if (cnt == 2)
			break;
	}

	fclose(f_mem);
	
	return (double)swap_free / swap_total;
}

double get_mem_ram(void)
{
	uint64_t mem_avail, mem_total;
	char localbuf[BUF_SIZE];
	int cnt = 0;

	FILE *f_mem = fopen("/proc/meminfo", "r");

	for (size_t i = 0; i < MEM_ENTRIES_NUMBER; ++i) {
		fgets(buf, BUF_SIZE, f_mem);
		if (strstr(buf, "MemTotal") != NULL) {
			sscanf(buf, "%s%"SCNu64"%s", localbuf, &mem_total, localbuf);
			++cnt;
		}
		if (strstr(buf, "MemAvailable") != NULL) {
			sscanf(buf, "%s%"SCNu64"%s", localbuf, &mem_avail, localbuf);
			++cnt;
		}
		if (cnt == 2)
			break;
	}

	fclose(f_mem);
	
	return (double)mem_avail / mem_total;
}

uint64_t get_disk_opcount(void)
{
	uint64_t tmp, opcount = 0;

	FILE *f_disk = fopen("/proc/diskstats", "r");
	
	do {
		fscanf(f_disk, "%"SCNu64"%"SCNu64"%s", &tmp, &tmp, buf);
		for (size_t i = 0; i < DISK_OPCOUNT_POS + 1; ++i)
			fscanf(f_disk, "%"SCNu64, &tmp);	
		opcount += tmp;
	} while (fgets(buf, BUF_SIZE, f_disk));
	
	fclose(f_disk);
	
	return opcount;
}

uint64_t get_disk_iotime(void)
{
	uint64_t tmp, iotime = 0;

	FILE *f_disk = fopen("/proc/diskstats", "r");
	
	do {
		fscanf(f_disk, "%"SCNu64"%"SCNu64"%s", &tmp, &tmp, buf);
		for (size_t i = 0; i < DISK_IOTIME_POS + 1; ++i)
			fscanf(f_disk, "%"SCNu64, &tmp);	
		iotime += tmp;
	} while (fgets(buf, BUF_SIZE, f_disk));
	
	fclose(f_disk);
	
	return iotime;
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
		syslog(LOG_DEBUG, "%lf\n", get_mem_swap());
		syslog(LOG_DEBUG, "%lf\n", get_mem_ram());
		sleep(DELAY_S);
	}

	term();
}
