#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/time.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sched.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>

#include "tracing.h"



/*
 * Global data structure to record the data
 * The output is per-task.
 * To minimize interference, you should dump it to /dev/shm (shared memory)
 */


/*
 * Task related parameters, time unit is milli second
 */
int              period = 0;       /* period (we assume deadline equals period) */
long long        duration = 0;     /* task execution duration */
int              priority = 0;     /* task priority */
int              count = 0;        /* number of jobs to execute (duration / period) */
volatile int32_t gs32JobID = 0;    /* job index */
int32_t          gs32PID;
char             gacFileName[100];

void usage(void)
{
	printf("calib_tsc for determining how many TSC ticks in One Second\n");
	printf("./calib_tsc [period in ms] [count] [priority]\n");
	printf("Recommand Setting ./calib_tsc 1000 101 98\n");
	return;
}


/*
 * each job's work
 * record start time
 */
static void work(int sig, siginfo_t *extra, void *cruft)
{
	uint64_t u64elapsed_tsc;
    /* We have reached the count. Print res and quit */
	if (count != 0 && gs32JobID >= count) {

		u64elapsed_tsc = get_tscbyidx(count - 1) - get_tscbyidx(0);
		printf("Elaspsed_TSC[%12" PRId64 "] [%lf]TSC/Sec\n", 
				u64elapsed_tsc,
				(double)u64elapsed_tsc / (((double)period / 1000.0) * (count - 1))
			  );



		dump_tracing_to_file(gacFileName);
        exit(1);
    }

	/* We wont do anything important rather than get the start time    */
	/* Basically, we can simply compute the Interval between two shots */

	add_record_evt(CAL_EVENT, CAL_EVT_RLS_JOB, gs32JobID);
	gs32JobID++;

	return;
}


static void exit_dump(int sig, siginfo_t *extra, void *cruft)
{
	printf("SIGINT Rcved! Dump File\n");
	dump_tracing_to_file(gacFileName);
	/* sleep(1); */
	exit(1);
	return;
}

/*
 * Set affinity of the task, alwasy pin it to core 0
 */
#if 0
static void set_cpuaffinity(void)
{
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(0, &mask);
    if (sched_setaffinity(0, sizeof(cpu_set_t), &mask) < 0)
	{
        perror("sched_setaffinity");
        exit(EXIT_FAILURE);
    }
	return;
}
#endif

static void set_sched(void)
{

    struct sched_param sched;
    sched.sched_priority = priority;
    if (sched_setscheduler(getpid(), SCHED_FIFO, &sched) < 0)
	{
        perror("sched_setscheduler");
        exit(EXIT_FAILURE);
    }
	return;
}

int main(int argc, char *argv[])
{
    struct sigaction sa;
    sigset_t allsigs;
    struct itimerspec timerspec;
    struct timespec now;
    timer_t timer;
    struct sigevent timer_event;

	if(argc != 4)
	{
		usage();
		exit(EXIT_FAILURE);
	}
/*  We do not need a Wcet this time */
	period =	atoi(argv[1]);
	count =		atoi(argv[2]);
	priority =  atoi(argv[3]);

    printf("Period[%d] count[%d] priority[%d]\n",
			period,
			count,
		   	priority);
 
	sprintf(gacFileName, "Calib.trace");

	/* Set SCHED_FIFO */
	set_sched();

	/* INIT Tracing */
	init_tracing();
	gs32PID = getpid();

	/* Bind a sighandler, using SIGRTMIN */
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = work;

    if (sigaction(SIGRTMIN, &sa, NULL) < 0) {
        perror("sigaction error");
        exit(EXIT_FAILURE);
    }

	/* Bind another sighander, using SIGINT */
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = exit_dump;

    if (sigaction(SIGINT, &sa, NULL) < 0) {
        perror("sigaction error");
        exit(EXIT_FAILURE);
    }
	

	/* We don't need an external trigger */
	/* raise(SIGSTOP); */

    /* the timer */
	/* Correct the Timer Interval Error Regardless of rdtsc*/
    timerspec.it_interval.tv_sec  =  period / 1000;
    timerspec.it_interval.tv_nsec = (period % 1000) * 1000000;

    /* the start time */
    /* if(clock_gettime(CLOCK_MONOTONIC, &now) < 0) { */
    if(clock_gettime(CLOCK_REALTIME, &now) < 0) {
        perror("clock_gettime");
        exit(EXIT_FAILURE);
    }

    // Start one second from now.
    timerspec.it_value.tv_sec  = now.tv_sec + 1;
    timerspec.it_value.tv_nsec = now.tv_nsec;

    timer_event.sigev_notify   = SIGEV_SIGNAL;
    timer_event.sigev_signo    = SIGRTMIN;
    timer_event.sigev_value.sival_ptr = (void *)&timer;

    if (timer_create(CLOCK_REALTIME, &timer_event, &timer) < 0) {
        perror("timer_create");
        exit(EXIT_FAILURE);
    }

    if (timer_settime(timer, TIMER_ABSTIME, &timerspec, NULL) < 0) {
        perror("timer_settime");
        exit(EXIT_FAILURE);
    }

    sigemptyset(&allsigs);


    while(1) {
        sigsuspend(&allsigs);
    }
    
    exit(EXIT_SUCCESS);
}

