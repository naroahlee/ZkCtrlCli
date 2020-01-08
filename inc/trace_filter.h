/* trace_filter.h */

#ifndef __TRACE_FILTER_H__
#define __TRACE_FILTER_H__

#include <stdint.h>
#include "tracing.h"

#define MAX_FILTER_NUM        (20)

#define EVENT_FILTER_TYPE     (1)
#define EVENT_FILTER_EVENTID  (1 << 1)

typedef union cont_t
{
	EVENT_TYPE   u16type;
	EVENT_ID     u16evtID;
} cont_s;

typedef uint32_t EVENT_FILTER_OPT;
typedef struct event_filter_t
{
	/* option for filter */
	EVENT_FILTER_OPT s32filopt;
	cont_s			 cont;
} event_filter;

typedef struct event_filter_list_t
{
	int32_t	     s32cnt;
	event_filter strFilterList[MAX_FILTER_NUM];
} event_filter_list;

#endif 
