/* Main.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <zookeeper/zookeeper.h>
#include "tracing.h"

static zhandle_t *zh;

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
	char   appId[64];
	int    rc;
	struct Stat stat;

	strcpy(appId, "example.foo_test");

	//zoo_set_debug_level(ZOO_LOG_LEVEL_DEBUG);
	init_tracing();

	zh = zookeeper_init("192.168.0.11:2181", global_watcher, 10001, 0, 0, 0);
	if (!zh)
	{
		return errno;
	}
	
	rc = zoo_wexists(zh, "/xyz", my_watcher2, 0, &stat);
	assert(ZOK == rc);
	
	pause();

	zookeeper_close(zh);

	return 0;
}
