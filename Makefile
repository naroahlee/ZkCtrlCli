INCLUDE_DIR := ./inc
SRC_PATH := ./src
#CFLAG = -I${INCLUDE_DIR} -I/usr/local/include/zookeeper -Wall -O2
CFLAG = -I${INCLUDE_DIR} -Wall -O2 -DTHREADED

CC = /usr/bin/gcc

LDLIBS = -lrt -lzookeeper_mt

vpath %.c ${SRC_PATH}

ALL = main watch_tst

.PHONY: all clean

all: ${ALL}

clean:
	rm -rf ${ALL} *.o *.csv
	cscope -Rb
	clear

%.o: %.c
	${CC} ${CFLAG} -c $^ -o $@

main: main.o
	${CC} ${CFLAG} $^ ${LDLIBS} -o $@

watch_tst: watch_tst.o
	${CC} ${CFLAG} $^ ${LDLIBS} -o $@
