/* cli_create_ep.c */
/* A Zookeeper Client which creates an ephemeral node */
/* @ Znode /myep                                      */
/* After creating the node, main went into pause()    */
/* However, the background thread of zookeeper_mt lib */
/* will continue working.                             */

/* Usage: */
/* ./cli_create_ep [ZkSrv1:Port,ZkSrv2:Port]          */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <zookeeper/zookeeper.h>


static zhandle_t *zh;

const int s32expect_timeout = 1000;

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
	char   acSrvs[256];
	int    rc;

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

	/* zoo_set_debug_level(ZOO_LOG_LEVEL_DEBUG); */

	zh = zookeeper_init(acSrvs, watcher, s32expect_timeout, 0, 0, 0);
	if (!zh)
	{
		return errno;
	}
	
	/* Create an ephemeral node */
	rc = zoo_create(zh,
					"/myep",
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
	
	pause();

	zookeeper_close(zh);
	return 0;
}
