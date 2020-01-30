INCLUDE_DIR := ./inc
SRC_PATH := ./src
#CFLAG = -I${INCLUDE_DIR} -I/usr/local/include/zookeeper -Wall -O2
CFLAG = -I${INCLUDE_DIR} -Wall -O2 -DTHREADED

CC = /usr/bin/gcc

LDLIBS = -lrt -lzookeeper_mt

vpath %.c ${SRC_PATH}

ALL = cli_create_ep cli_watch_ep trace_filter calib_tsc delayms_killpid stat_lat show_const_table

.PHONY: all clean

all: ${ALL}
	cscope -Rb

clean:
	rm -rf ${ALL} *.o *.csv
	clear

%.o: %.c
	${CC} ${CFLAG} -c $^ -o $@

cli_create_ep: cli_create_ep.o
	${CC} ${CFLAG} $^ ${LDLIBS} -o $@

cli_watch_ep: cli_watch_ep.o tracing.o
	${CC} ${CFLAG} $^ ${LDLIBS} -o $@

stat_lat: stat_lat.o
	${CC} ${CFLAG} $^ ${LDLIBS} -o $@

delayms_killpid: delayms_killpid.o tracing.o
	${CC} ${CFLAG} $^ ${LDLIBS} -o $@

calib_tsc: calib_tsc.o tracing.o
	${CC} ${CFLAG} $^ ${LDLIBS} -o $@

trace_filter: trace_filter.o tracing.o
	${CC} ${CFLAG} $^           -o $@

show_const_table: show_const_table.o
	${CC} ${CFLAG} $^ ${LDLIBS} -o $@

EXP_CAL: calib_tsc trace_filter
	cp $^ /home/NFS_Share/ZkCtrlCli/Exp_Cal/

EXP_WATCHLAT: cli_create_ep cli_watch_ep delayms_killpid trace_filter stat_lat
	cp $^ /home/NFS_Share/ZkCtrlCli/Exp_WatchLat/
