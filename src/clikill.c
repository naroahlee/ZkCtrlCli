#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/types.h>

#include "tracing.h"

void rand_sleep(void)
{
	int usec;
	time_t t;
	srand((unsigned) time(&t));
	usec = (int)(((double)rand() / RAND_MAX) * 1000000);
	usleep(usec);
	return;
}

int main(int argc, char* argv[])
{
	int s32pid = 0;
	if(2 != argc)
	{
		fprintf(stderr, "Argument Error!\n");
		exit(-1);
	}
	s32pid = atoi(argv[1]);

	init_tracing();

	rand_sleep();
	kill(s32pid, SIGKILL);
	add_record_evt(CAL_EVENT, CAL_EVT_KIL_CLI, 0);

	dump_tracing_to_file("start.tracing");
	return 0;
}
