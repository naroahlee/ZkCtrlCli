INCLUDE_DIR := ./inc
SRC_PATH := ./src
#CFLAG = -I${INCLUDE_DIR} -I/usr/local/include/zookeeper -Wall -O2
CFLAG = -I${INCLUDE_DIR} -Wall -O2 -DTHREADED

CC = /usr/bin/gcc

LDLIBS = -lrt -lzookeeper_mt

vpath %.c ${SRC_PATH}

ALL = main watch_tst trace_filter calib_tsc

.PHONY: all clean

all: ${ALL}
	cscope -Rb

EXP_CAL:
	cp calib_tsc trace_filter /home/NFS_Share/ZkCtrlCli/Exp_Cal/


clean:
	rm -rf ${ALL} *.o *.csv
	clear

%.o: %.c
	${CC} ${CFLAG} -c $^ -o $@

main: main.o
	${CC} ${CFLAG} $^ ${LDLIBS} -o $@

watch_tst: watch_tst.o
	${CC} ${CFLAG} $^ ${LDLIBS} -o $@

calib_tsc: calib_tsc.o tracing.o
	${CC} ${CFLAG} $^ ${LDLIBS} -o $@

trace_filter: trace_filter.o tracing.o
	${CC} ${CFLAG} $^           -o $@
