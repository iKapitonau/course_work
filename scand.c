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

uint64_t get_mem_swap_total(void)
{
	uint64_t swap_total;
	char localbuf[BUF_SIZE];

	FILE *f_mem = fopen("/proc/meminfo", "r");

	for (size_t i = 0; i < MEM_ENTRIES_NUMBER; ++i) {
		fgets(buf, BUF_SIZE, f_mem);
		if (strstr(buf, "SwapTotal") != NULL) {
			sscanf(buf, "%s%"SCNu64"%s", localbuf, &swap_total, localbuf);
			break;
		}
	}

	fclose(f_mem);
	
	return swap_total;
}

uint64_t get_mem_swap_free(void)
{
	uint64_t swap_free;
	char localbuf[BUF_SIZE];

	FILE *f_mem = fopen("/proc/meminfo", "r");

	for (size_t i = 0; i < MEM_ENTRIES_NUMBER; ++i) {
		fgets(buf, BUF_SIZE, f_mem);
		if (strstr(buf, "SwapFree") != NULL) {
			sscanf(buf, "%s%"SCNu64"%s", localbuf, &swap_free, localbuf);
			break;
		}
	}

	fclose(f_mem);
	
	return swap_free;
}

uint64_t get_mem_ram_total(void)
{
	uint64_t mem_total;
	char localbuf[BUF_SIZE];

	FILE *f_mem = fopen("/proc/meminfo", "r");

	for (size_t i = 0; i < MEM_ENTRIES_NUMBER; ++i) {
		fgets(buf, BUF_SIZE, f_mem);
		if (strstr(buf, "MemTotal") != NULL) {
			sscanf(buf, "%s%"SCNu64"%s", localbuf, &mem_total, localbuf);
			break;
		}
	}

	fclose(f_mem);
	
	return mem_total;
}

uint64_t get_mem_ram_free(void)
{
	uint64_t mem_avail;
	char localbuf[BUF_SIZE];

	FILE *f_mem = fopen("/proc/meminfo", "r");

	for (size_t i = 0; i < MEM_ENTRIES_NUMBER; ++i) {
		fgets(buf, BUF_SIZE, f_mem);
		if (strstr(buf, "MemAvailable") != NULL) {
			sscanf(buf, "%s%"SCNu64"%s", localbuf, &mem_avail, localbuf);
			break;
		}
	}

	fclose(f_mem);
	
	return mem_avail;
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

void main(void)
{
}
