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
#include <zookeeper/zookeeper.h>




int main(int argc, char* argv[])
{
	printf("ZOO_EXPIRED_SESSION_STATE = %5d\n", ZOO_EXPIRED_SESSION_STATE);
	printf("ZOO_AUTH_FAILED_STATE     = %5d\n", ZOO_AUTH_FAILED_STATE);
	printf("ZOO_CONNECTING_STATE      = %5d\n", ZOO_CONNECTING_STATE);
	printf("ZOO_ASSOCIATING_STATE     = %5d\n", ZOO_ASSOCIATING_STATE);
	printf("ZOO_CONNECTED_STATE       = %5d\n", ZOO_CONNECTED_STATE);
	printf("ZOO_READONLY_STATE        = %5d\n", ZOO_READONLY_STATE);
	printf("ZOO_NOTCONNECTED_STATE    = %5d\n", ZOO_NOTCONNECTED_STATE);
	printf("\n");
	printf("ZOO_CREATED_EVENT         = %5d\n", ZOO_CREATED_EVENT);
	printf("ZOO_DELETED_EVENT         = %5d\n", ZOO_DELETED_EVENT);
	printf("ZOO_CHANGED_EVENT         = %5d\n", ZOO_CHANGED_EVENT);
	printf("ZOO_CHILD_EVENT           = %5d\n", ZOO_CHILD_EVENT);
	printf("ZOO_SESSION_EVENT         = %5d\n", ZOO_SESSION_EVENT);
	printf("ZOO_NOTWATCHING_EVENT     = %5d\n", ZOO_NOTWATCHING_EVENT);
	return 0;
}
