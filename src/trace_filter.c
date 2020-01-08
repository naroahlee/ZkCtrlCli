/* trace_filter.c                */
/* Filter and Sort Data          */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <inttypes.h>
#include <unistd.h>
#include "tracing.h"
#include "trace_filter.h"

static event_filter_list gstrFilterList;

static void init_filter_list(void)
{
	gstrFilterList.s32cnt = 0;
	return;
}

static void add_filter_to_list(event_filter* pstrFilter)
{
	int32_t s32cur;
	s32cur = gstrFilterList.s32cnt;
	memcpy((void*)&gstrFilterList.strFilterList[s32cur], (void *)pstrFilter, sizeof(event_filter));
	gstrFilterList.s32cnt++;
	return;
}

static void echo_filter_list(void)
{
	int s32i = 0;
	event_filter* pstrTemp;
	for(s32i = 0; s32i < gstrFilterList.s32cnt; s32i++)
	{
		pstrTemp = &gstrFilterList.strFilterList[s32i];
		switch(pstrTemp->s32filopt)
		{
		case EVENT_FILTER_TYPE:
			printf("FILTER_TYPE[%8s]:  EVENT_TYPE[%4d]\n", "EVN_TYPE", pstrTemp->cont.u16type);
			break;
		case EVENT_FILTER_EVENTID:
			printf("FILTER_TYPE[%8s]:  EVENT_ID  [%4d]\n", "EVN_ID"  , pstrTemp->cont.u16evtID);
			break;
		default:
			fprintf(stderr, "Error Filter Type[%d]!\n", pstrTemp->s32filopt);
			exit(EXIT_FAILURE);
		}	
	}
	return;
}

static int is_match_filter_each(event_filter* pstrFilter, event_record* pstrEvt)
{
	switch(pstrFilter->s32filopt)
	{
	case EVENT_FILTER_TYPE:
		if(pstrFilter->cont.u16type  == pstrEvt->u16type)
		{
			return 1;
		}
		break;
	case EVENT_FILTER_EVENTID:
		if(pstrFilter->cont.u16evtID == pstrEvt->u16eid)
		{
			return 1;
		}
		break;
	default:
		fprintf(stderr, "Error Filter Type[%d]!\n", pstrFilter->s32filopt);
		exit(EXIT_FAILURE);
		return 0;
	}	
	return 0;
}

static int is_match_filter_all(event_record* pstrEvt)
{
	int s32i;
	for(s32i = 0; s32i < gstrFilterList.s32cnt; s32i++)
	{
		/* AND-Filter */
		if(0 == is_match_filter_each(&gstrFilterList.strFilterList[s32i], pstrEvt))
		{
			return 0;
		}
	}
	return 1;
}

int cmpEventTsc(const void *pA, const void *pB)
{
	event_record *pstrA;
	event_record *pstrB;
	pstrA = (event_record *)pA;
	pstrB = (event_record *)pB;
	if(pstrA->u64tsc > pstrB->u64tsc)
	{
		return 1;
	}	
	else if(pstrA->u64tsc < pstrB->u64tsc)
	{
		return -1;
	}	
	else
	{
		return 0;
	}	
}

size_t getFileSize(FILE* fp)
{
	size_t s32size;
	fseek(fp, 0L, SEEK_END);
	s32size = ftell(fp);
	fseek(fp, 0L, SEEK_SET);
	return s32size;
}

#define OPTSTR "y:e:o:rp"


int main(int argc, char* argv[])
{
	FILE* fpin;
	FILE* fpout;
	int s32i; 
	int s32cnt;
	size_t s32total;
	/* uint64_t u64start; */
	event_record* pstrRecList = NULL;
	event_record  strTemp;
	event_record* pstrTemp;
	event_filter  strFilter;
	char acOutFile[50] = "out.dat";
	int s32issort = 0;
	int s32isout  = 0;
	int s32isprint= 0;
	int opt;

	init_filter_list();
    while ((opt = getopt(argc, argv, OPTSTR)) != -1) {
        switch (opt) {
        case 'y':
			strFilter.s32filopt = EVENT_FILTER_TYPE;
            strFilter.cont.u16type   = atoi(optarg);
			add_filter_to_list(&strFilter);
            break;
        case 'e':
			strFilter.s32filopt = EVENT_FILTER_EVENTID;
            strFilter.cont.u16evtID  = atoi(optarg);
			add_filter_to_list(&strFilter);
            break;
		/* Speicify Sort */
        case 'r':
			s32issort = 1;
            break;
		case 'p':
			s32isprint = 1;
			break;
		/* Speicify Output File */
        case 'o':
			s32isout  = 1;
			strcpy(acOutFile, optarg);
			break;
		case ':':
			fprintf(stderr, "Missing Argument!\n");
			exit(EXIT_FAILURE);
			break;
        default:
			fprintf(stderr, "Error Arg[-%c]!\n", opt);
			exit(EXIT_FAILURE);
            break;
        }
    }

	echo_filter_list();
	/* printf("argc[%d], optind[%d]\n", argc, optind); */
	if(1 != argc - optind)
	{
		fprintf(stderr, "Argument Err!\n");
		exit(EXIT_FAILURE);
	}
	
	fpin = fopen(argv[optind], "rb");
	if(NULL == fpin)
	{
		fprintf(stderr, "Open File [%s] Error!\n", argv[optind]);
		exit(-1);
	}
	
	s32total = getFileSize(fpin);
	pstrRecList = (event_record*)malloc(s32total);

	/* Read and Filter */
	/* fread((void *)pstrRecList, s32total, 1, fpin); */
	s32cnt = 0;
	while(1 == fread((void *)&strTemp, sizeof(event_record), 1, fpin))
	{
		if(1 == is_match_filter_all(&strTemp))
		{
			memcpy((void*)&pstrRecList[s32cnt], 
				   (void*)&strTemp, 
				   sizeof(event_record));

			s32cnt++;
		}
	}

	fclose(fpin);
	
	/* quick sort base on tsc */
	if(1 == s32issort)
	{
		printf("Sort ON!\n");
		qsort(pstrRecList, s32cnt, sizeof(event_record), cmpEventTsc);
	}
	/* u64start = pstrTemp->u64tsc; */

	if(1 == s32isprint)
	{
		pstrTemp = pstrRecList;
		for(s32i = 0; s32i < s32cnt; s32i++, pstrTemp++)
		{
			switch(pstrTemp->u16type)
			{
			case PLT_EVENT:
				printf("[PLT] JID[%4d] [%25s]: TSC[%12" PRId64 "] [%7.2lf]ms\n", 
						pstrTemp->s32jid,
						getEventNamefromID(pstrTemp->u16eid),
						pstrTemp->u64tsc,
						(double)(pstrTemp->u64tsc) / KHZ);
				break;
			case CTL_EVENT:
				printf("[CTL] JID[%4d] [%25s]: TSC[%12" PRId64 "] [%7.2lf]ms\n", 
						pstrTemp->s32jid,
						getEventNamefromID(pstrTemp->u16eid),
						pstrTemp->u64tsc,
						(double)(pstrTemp->u64tsc) / KHZ);
				break;
			case ACU_EVENT:
				printf("[ACU] JID[%4d] [%25s]: TSC[%12" PRId64 "] [%7.2lf]ms\n", 
						pstrTemp->s32jid,
						getEventNamefromID(pstrTemp->u16eid),
						pstrTemp->u64tsc,
						(double)(pstrTemp->u64tsc) / KHZ);
				break;
			case CAL_EVENT:
				printf("[CAL] JID[%4d] [%25s]: TSC[%12" PRId64 "] [%7.2lf]ms\n", 
						pstrTemp->s32jid,
						getEventNamefromID(pstrTemp->u16eid),
						pstrTemp->u64tsc,
						(double)(pstrTemp->u64tsc) / KHZ);
				break;
			default:
				printf("TYPE [%d] ERROR!\n", pstrTemp->u16type);
				break;
			}
		}
	}

	/* Writing binary File */
	if(1 == s32isout)
	{
		fpout = fopen(acOutFile, "wb");
		if(NULL == fpout)
		{
			fprintf(stderr, "Open File [%s] Error!\n", acOutFile);
			exit(-1);
		}
		
		fwrite((void *)pstrRecList, sizeof(event_record), s32cnt, fpout);
		fclose(fpout);
		printf("Total Size Write: [%d]Byte\n", (int32_t)(s32cnt * sizeof(event_record)));
		printf("Total Event:      [%d]\n", s32cnt);
	}


	/* Post Processing */
	free(pstrRecList);
	
	return 0;
}
