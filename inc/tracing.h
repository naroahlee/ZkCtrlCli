/* tracing.h */
#ifndef __TRACING__
#define __TRACING__

#include <stdint.h>
#include <string.h>
#define EVENT_BUF_MAX  (40960)
#define EVENT_NAME_LEN (40)

/* Three Components                   */
/* Plant in MATLAB                    */
/* Controller in Edge-Cloud           */
/* Acturator for handling concurrency */

#define PLT_EVENT    (1)
#define CTL_EVENT    (2)
#define ACU_EVENT    (3)
/* CAL_EVENT for Calibration the TSC  */
#define CAL_EVENT    (9)

/* ============== PLANT EVENT =============== */
#define PLT_EVT_SND_QUERY      (101)
#define PLT_EVT_RCV_CTLCMD     (102)
#define PLT_EVT_SND_SENDATA    (103)

/* ========== CONTROLLER EVENT ============== */
#define CTL_EVT_RCV_SENDATA    (201)
#define CTL_EVT_SND_CTLCMD     (202)

/* ========== ACTURATOR  EVENT ============== */
#define ACU_EVT_RCV_CTLCMD     (301)
#define ACU_EVT_RCV_QUERY      (302)
#define ACU_EVT_SND_CTLCMD     (303)

/* ========== CALIBTATION EVENT ============= */
#define CAL_EVT_RLS_JOB        (901)
#define CAL_EVT_KIL_CLI        (902)
#define CAL_EVT_WAT_TRI        (903)

#define HZ                 (2099963427)
#define KHZ                (2099963)
#define MHZ                (2099)

typedef uint16_t EVENT_ID;
typedef uint16_t EVENT_TYPE;

/* A 16 Byte Payload */
typedef struct event_record_t
{
	EVENT_TYPE u16type;
	EVENT_ID   u16eid;
	int32_t    s32jid;
	uint64_t   u64tsc;
} event_record;

typedef struct event_identify_t
{
	char     acEvent[EVENT_NAME_LEN];
	EVENT_ID u16eid;
} event_identify;

void init_tracing(void);

void add_record(event_record *pstrEvt);
void add_record_evt(EVENT_TYPE u16type, EVENT_ID u16eid, int32_t s32jid);

uint64_t get_tscbyidx(int32_t s32idx);

void dump_tracing_to_file(char* pcName);
char* getEventNamefromID(EVENT_ID u16ID);
int print_event_record(event_record* pstrTemp);

#endif
