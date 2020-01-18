# My Zookeeper Client in C
Development Note:
	I Updated the program interface to support zookeeper cluster test 
	I changed the cli_watch_ep's timeout
	

===========================Manual==============================
Install Zookeeper Library
  1. Download Zookeeper tar ball through Apache Mirror.
  2. Install Zookeeper library: zookeeper_st.so zookeeper_mt.so
	2.1 Install Tool-Link and dependencies:
        sudo apt-get install libcppunit-dev libtool autoreconf
	2.2 configure and install
		./configure
        make
		sudo make install

Make the Project
  1. Use -DTHREADED compiler option to enable sync API. Link the zookeeper_mt library. (Already in Makefile)
		make all

Do TSC Calibration:
Reason: The tracing system within this project relies on RDTSCP of x64 ISA.
        However, even if a CPU frequency has been fixed to its nominal value in BIOS,
        the actually ticks in one second vary among different machines.
        A calibration will help the system get more accurate result.
  1. Make sure the PCPU has been set at a fixed frequency.
  2. Remove SCHED_FIFO sched_rt_runtime throttle
  3. Run sudo ./calib 1000 101 98
  4. Using the new X TSC/Sec value to replace corresponding KHZ marco in inc/tracing.h

========================= Exp Watch Latency ===================
The following objects are used for testing the client fail detection latency.

cli_tobekilled: The client which creates an ephemeral node in ZKSrv.
watch_tst: The client which watches the ephemeral node. Once notified, it will dump a tracing file for latency test.
clikill: Sleep some random time and send SIGKILL to cli_tobekilled. You need to execute this program within the same VM of cli_tobekilled, and also specify the PID.


Method:
1. In VM-ZkCli01, we bring up a ZKclient (cli_tobekilled) and create an ephemeral node in zookeeper "/xyz". Then the client starts a busy loop.

2. In VM-ZkCli02, we bring up another ZKClient (watch_tst). This client will set an exist-watch to znote "/xyz".

3. We will send an external "kill -9 (SIGSTOP)" signal by running "clikill" in VM-ZkCli01, to force the cli_tobekilled terminated immediately, without dealing any zk session close routine.

4. After somewhile, the watch_tst can receive the "death notification" from the zookeeper and end its session.

We will measure the latency between the sending of the stop signal and the receiving of "death notification".

Tracing and Stat Method:
In each run:
	The clikill will dump start.tracing file when finished. The file contains only one start event.
	The watch_tst will dump end.tracing file when finished. The file contains only one receiving event.
	The two events will be *appended* to all.tracing.

Finally, after several runs (default = 100), we will use trace_filter to sort the events (kind of unnecessay).
Then stat_lat will transfer the binary events to latency samples (res.csv).

