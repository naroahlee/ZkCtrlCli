/* tracing.h */
/* enable tracing feature */
#include <stdio.h>
#include <stdlib.h>
#include "tracing.h"

/* Event Record Buf List */
static event_record gstrRecList[EVENT_BUF_MAX];
/* only available in this file */
static       event_record* gpstrcuritem = &gstrRecList[0];
const static event_record* gpstrLast    = &gstrRecList[EVENT_BUF_MAX - 1];


static event_identify gstrEventList[] = 
{
	{"PLT_EVT_SND_QUERY"    , PLT_EVT_SND_QUERY  },
	{"PLT_EVT_RCV_CTLCMD"   , PLT_EVT_RCV_CTLCMD },
	{"PLT_EVT_SND_SENDATA"  , PLT_EVT_SND_SENDATA},

	{"CTL_EVT_RCV_SENDATA"  , CTL_EVT_RCV_SENDATA},
	{"CTL_EVT_SND_CTLCMD"   , CTL_EVT_SND_CTLCMD },

	{"ACU_EVT_RCV_CTLCMD"   , ACU_EVT_RCV_CTLCMD },
	{"ACU_EVT_RCV_QUERY"    , ACU_EVT_RCV_QUERY  },
	{"ACU_EVT_SND_CTLCMD"   , ACU_EVT_SND_CTLCMD },

	{"CAL_EVT_RLS_JOB"      , CAL_EVT_RLS_JOB    },
	{"CAL_EVT_KIL_CLI"      , CAL_EVT_KIL_CLI    },
	{"CAL_EVT_WAT_TRI"      , CAL_EVT_WAT_TRI    },
};

static int32_t getDefEventNum(void)
{
	return sizeof(gstrEventList) / sizeof(event_identify);
}

char* getEventNamefromID(EVENT_ID u16eid)
{
	int s32i = 0;
	for(s32i = 0; s32i < getDefEventNum(); s32i++)
	{
		if(u16eid == gstrEventList[s32i].u16eid)
		{
			return gstrEventList[s32i].acEvent;
		}
	}
	return NULL;
}

#ifdef __x86_64__
static __inline__ uint64_t rdtscp(void)
{
	uint32_t hi, lo;
	__asm__ __volatile__ ("rdtscp" : "=a"(lo), "=d"(hi));
	return (
			 ((uint64_t)lo) | 
			(((uint64_t)hi) << 32)
		   );
}
#endif

void init_tracing(void)
{
	/* rewind the pointer */
	gpstrcuritem = &gstrRecList[0];
	return;
}

uint64_t get_tscbyidx(int32_t s32idx)
{
	return gstrRecList[s32idx].u64tsc;
}

void add_record(event_record *pstrEvt)
{
	memcpy((void *)gpstrcuritem, (void *)pstrEvt, sizeof(event_record));
	/* Boundary Protection: Yeah, we will definately lost some data */
	gpstrcuritem = (gpstrcuritem != gpstrLast) ? gpstrcuritem + 1
											   : &gstrRecList[0];
	return;
}

__inline__ void add_record_evt(volatile EVENT_TYPE u16type, volatile EVENT_ID u16eid, volatile int32_t s32jid)
{
	uint64_t u64tmp;
	u64tmp = rdtscp();

	/* Write a Record */
	gpstrcuritem->u16type  = u16type;
	gpstrcuritem->u16eid   = u16eid;
	gpstrcuritem->s32jid   = s32jid;
	gpstrcuritem->u64tsc   = u64tmp;

	/* Boundary Protection: Yeah, we will definately lost some data */
	gpstrcuritem = (gpstrcuritem != gpstrLast) ? gpstrcuritem + 1
											   : &gstrRecList[0];
	
	return;
}

void dump_tracing_to_file(char* pcName)
{
	FILE* fp;
	event_record* pstrtemp;
	fp = fopen(pcName, "wb");
	if(NULL == fp)
	{
		fprintf(stderr, "[TRC] ERR: Dump File Error!\n");
		exit(EXIT_FAILURE);
	}

	pstrtemp = &gstrRecList[0];
	while(pstrtemp != gpstrcuritem)
	{
		fwrite((void *)pstrtemp, sizeof(event_record), 1, fp);
		pstrtemp++;
	}

	fclose(fp);
	return;
}

int print_event_record(event_record* pstrTemp)
{
	switch(pstrTemp->u16type)
	{
	case PLT_EVENT:
		printf("[PLT] JID[%3d] [%25s]: TSC[%10.2lf]ms\n", 
				pstrTemp->s32jid,
				getEventNamefromID(pstrTemp->u16eid),
				(double)(pstrTemp->u64tsc) / KHZ);
		break;
	case CTL_EVENT:
		printf("[CTL] JID[%3d] [%25s]: TSC[%10.2lf]ms\n", 
				pstrTemp->s32jid,
				getEventNamefromID(pstrTemp->u16eid),
				(double)(pstrTemp->u64tsc) / KHZ);
		break;
	case ACU_EVENT:
		printf("[ACU] JID[%3d] [%25s]: TSC[%10.2lf]ms\n", 
				pstrTemp->s32jid,
				getEventNamefromID(pstrTemp->u16eid),
				(double)(pstrTemp->u64tsc) / KHZ);
		break;
	default:
		printf("TYPE ERROR!\n");
		break;
	}
	return 0;
}
