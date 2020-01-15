/* delayms_killpid.c */
/* Delay Serveral Milliseconds then Kill a process */

/* Usage */
/* ./delayms_killpid [msec] [pid] */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/types.h>

#include "tracing.h"

void rand_sleep(int s32msec)
{
	int s32usec;
	time_t t;
	srand((unsigned) time(&t));
	s32usec = (int)(((double)rand() / RAND_MAX) * s32msec * 1000);
	usleep(s32usec);
	return;
}

int main(int argc, char* argv[])
{
	int s32pid  = 0;
	int s32msec = 0;
	if(3 != argc)
	{
		fprintf(stderr, "Argument Error!\n");
		exit(-1);
	}
	s32msec = atoi(argv[1]);
	s32pid  = atoi(argv[2]);

	init_tracing();

	rand_sleep(s32msec);
	kill(s32pid, SIGKILL);
	add_record_evt(CAL_EVENT, CAL_EVT_KIL_CLI, 0);

	dump_tracing_to_file("start.tracing");
	return 0;
}
