/* cli_watch_ep.c */
/* A Zookeeper Client which watchess the ephemeral    */
/* Znode /myep created by another client              */
/* After that client dies for whatever reason         */
/* The watcher will get a notification.               */
/* It dumps the tracing file which contains timestamp */

/* Usage: */
/* ./cli_watch_ep [ZkSrv1:Port,ZkSrv2:Port]          */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <zookeeper/zookeeper.h>
#include "tracing.h"

static zhandle_t *zh;
const int s32expect_timeout = 1000;

/* Watcher function -- 
 * empty for this example, not something you should do in real code */
void global_watcher(zhandle_t *zzh, int type, int state, const char *path,
             void *watcherCtx)
{
	return;
}

/* Specialized Watcher for wexists() */
void my_watcher2(zhandle_t *zzh, int type, int state, const char *path,
             void *watcherCtx)
{
	add_record_evt(CAL_EVENT, CAL_EVT_WAT_TRI, 0);
	zookeeper_close(zh);
	dump_tracing_to_file("end.tracing");
	exit(0);	
	return;
}

int main(int argc, char* argv[])
{
	int    rc;
	struct Stat stat;
	char   acSrvs[256];

	/* Parameter Check */
	if(1 == argc)
	{
		strcpy(acSrvs, "localhost:2181");
	}
	else if (2 == argc)
	{
		strcpy(acSrvs, argv[1]);
	}
	else
	{
		exit(-1);
	}

	//zoo_set_debug_level(ZOO_LOG_LEVEL_DEBUG);
	init_tracing();

	zh = zookeeper_init(acSrvs, global_watcher, s32expect_timeout, 0, 0, 0);
	if (!zh)
	{
		return errno;
	}
	
	rc = zoo_wexists(zh, "/myep", my_watcher2, 0, &stat);
	assert(ZOK == rc);
	
	pause();

	zookeeper_close(zh);

	return 0;
}
