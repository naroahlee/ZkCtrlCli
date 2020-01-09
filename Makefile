INCLUDE_DIR := ./inc
SRC_PATH := ./src
#CFLAG = -I${INCLUDE_DIR} -I/usr/local/include/zookeeper -Wall -O2
CFLAG = -I${INCLUDE_DIR} -Wall -O2 -DTHREADED

CC = /usr/bin/gcc

LDLIBS = -lrt -lzookeeper_mt

vpath %.c ${SRC_PATH}

ALL = cli_tobekilled watch_tst trace_filter calib_tsc clikill stat_lat

.PHONY: all clean

all: ${ALL}
	cscope -Rb

clean:
	rm -rf ${ALL} *.o *.csv
	clear

%.o: %.c
	${CC} ${CFLAG} -c $^ -o $@

cli_tobekilled: cli_tobekilled.o
	${CC} ${CFLAG} $^ ${LDLIBS} -o $@

stat_lat: stat_lat.o
	${CC} ${CFLAG} $^ ${LDLIBS} -o $@

clikill: clikill.o tracing.o
	${CC} ${CFLAG} $^ ${LDLIBS} -o $@

watch_tst: watch_tst.o tracing.o
	${CC} ${CFLAG} $^ ${LDLIBS} -o $@

calib_tsc: calib_tsc.o tracing.o
	${CC} ${CFLAG} $^ ${LDLIBS} -o $@

trace_filter: trace_filter.o tracing.o
	${CC} ${CFLAG} $^           -o $@

EXP_CAL: calib_tsc trace_filter
	cp $^ /home/NFS_Share/ZkCtrlCli/Exp_Cal/

EXP_WATCHLAT: cli_tobekilled watch_tst clikill trace_filter stat_lat
	cp $^ /home/NFS_Share/ZkCtrlCli/Exp_WatchLat/
