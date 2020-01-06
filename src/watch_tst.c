/* Main.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <zookeeper/zookeeper.h>

static zhandle_t *zh;

/* Watcher function -- 
 * empty for this example, not something you should do in real code */
void global_watcher(zhandle_t *zzh, int type, int state, const char *path,
             void *watcherCtx)
{
	//printf("The Global Watch Has Been Called.\n");
	return;
}

/* Specialized Watcher for wexists() */
void my_watcher2(zhandle_t *zzh, int type, int state, const char *path,
             void *watcherCtx)
{
	if(ZOO_DELETED_EVENT == type)
	{
		printf("[Watch2] Node Has Been Deleted.\n");
	}
	return;
}

int main(int argc, char* argv[])
{
	char   appId[64];
	int    rc;
	struct Stat stat;

	strcpy(appId, "example.foo_test");

	//zoo_set_debug_level(ZOO_LOG_LEVEL_DEBUG);

	zh = zookeeper_init("192.168.0.11:2181", global_watcher, 10001, 0, 0, 0);
	if (!zh)
	{
		return errno;
	}
	
	rc = zoo_wexists(zh, "/xyz", my_watcher2, 0, &stat);
	if (ZOK == rc)
	{
		printf("Node Exists.\n");
	}
	else if(ZNONODE == rc)
	{
		printf("No Node.\n");
	}
	
	getchar();

	zookeeper_close(zh);
	return 0;
}
