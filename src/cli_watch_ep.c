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

/* For Debug: Copied from cli.c */
/*
static const char* state2String(int state){
  if (state == 0)
    return "CLOSED_STATE";
  if (state == ZOO_CONNECTING_STATE)
    return "CONNECTING_STATE";
  if (state == ZOO_ASSOCIATING_STATE)
    return "ASSOCIATING_STATE";
  if (state == ZOO_CONNECTED_STATE)
    return "CONNECTED_STATE";
  if (state == ZOO_READONLY_STATE)
    return "READONLY_STATE";
  if (state == ZOO_EXPIRED_SESSION_STATE)
    return "EXPIRED_SESSION_STATE";
  if (state == ZOO_AUTH_FAILED_STATE)
    return "AUTH_FAILED_STATE";

  return "INVALID_STATE";
}

static const char* type2String(int state){
  if (state == ZOO_CREATED_EVENT)
    return "CREATED_EVENT";
  if (state == ZOO_DELETED_EVENT)
    return "DELETED_EVENT";
  if (state == ZOO_CHANGED_EVENT)
    return "CHANGED_EVENT";
  if (state == ZOO_CHILD_EVENT)
    return "CHILD_EVENT";
  if (state == ZOO_SESSION_EVENT)
    return "SESSION_EVENT";
  if (state == ZOO_NOTWATCHING_EVENT)
    return "NOTWATCHING_EVENT";

  return "UNKNOWN_EVENT_TYPE";
}
*/

/* Watcher function -- Mod from cli.c */
void mywatcher(zhandle_t *zzh, int type, int state, const char *path,
             void *watcherCtx)
{
	/* For Further Debug: Comment if you are not debugging
    fprintf(stderr, "Watcher %s state = %s", type2String(type), state2String(state));
    if (path && strlen(path) > 0) {
      fprintf(stderr, " for path %s", path);
    }
    fprintf(stderr, "\n");
	*/

	if (type == ZOO_DELETED_EVENT)
	{
		/* Tracing the detection */
		add_record_evt(CAL_EVENT, CAL_EVT_WAT_TRI, 0);
		zookeeper_close(zh);
		dump_tracing_to_file("end.tracing");
		exit(0);	
	}
	else if (type == ZOO_SESSION_EVENT)
   	{
        if (state == ZOO_CONNECTED_STATE)
	   	{
			fprintf(stderr, "Now session id: 0x%llx\n",
                        (long long)zoo_client_id(zzh));

        }
	   	else if (state == ZOO_AUTH_FAILED_STATE)
	   	{
            fprintf(stderr, "Authentication failure. Shutting down...\n");
            zookeeper_close(zzh);
            zh=0;
        }
	   	else if (state == ZOO_EXPIRED_SESSION_STATE)
	   	{
            fprintf(stderr, "Session expired. Shutting down...\n");
            zookeeper_close(zzh);
            zh=0;
        }
    }
	return;
}

int main(int argc, char* argv[])
{
	int    rc;
	char   buffer[512];
	int	   bufferlen;
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

//	zoo_set_debug_level(ZOO_LOG_LEVEL_DEBUG);
	init_tracing();

	zh = zookeeper_init(acSrvs, mywatcher, s32expect_timeout, 0, 0, 0);
	if (!zh)
	{
		return errno;
	}

	/* I should not use wexists() to set the watch for existence */
	rc = zoo_get(zh,
		         "/myep",
				 1, 
				 buffer, 
				 &bufferlen,
				 &stat);


	assert(ZOK == rc);
	
	pause();

	zookeeper_close(zh);

	return 0;
}
