/* Main.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <zookeeper/zookeeper.h>


static zhandle_t *zh;

/* Watcher function -- 
 * empty for this example, not something you should do in real code */
void watcher(zhandle_t *zzh, int type, int state, const char *path,
             void *watcherCtx)
{
	return;
}

int main(int argc, char* argv[])
{
	char   buffer[512];
	char   appId[64];
	int    rc;
	//int    buflen= sizeof(buffer);
	//struct Stat stat;

	strcpy(appId, "example.foo_test");

	//zoo_set_debug_level(ZOO_LOG_LEVEL_DEBUG);

	zh = zookeeper_init("192.168.0.11:2181", watcher, 10000, 0, 0, 0);
	if (!zh)
	{
		return errno;
	}
	
	/* Create an ephemeral node */
	rc = zoo_create(zh,
					"/xyz",
					"value",
					5,
					//&ALLOW_ALL,
					&ZOO_OPEN_ACL_UNSAFE,
					ZOO_EPHEMERAL,
					buffer,
					sizeof(buffer)-1);
	if (rc)
	{
		fprintf(stderr, "Error %d for %d\n", rc, __LINE__);
	}

	/*
	rc = zoo_get(zh, "/xyz", 0, buffer, &buflen, &stat);
	if (rc)
	{
		fprintf(stderr, "Error %d for %d\n", rc, __LINE__);
	}
	printf("Res = %s\n", buffer);
	*/
	
	getchar();

	zookeeper_close(zh);
	return 0;
}
