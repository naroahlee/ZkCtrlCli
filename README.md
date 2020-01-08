# My Zookeeper Client in C
Development Note:
	I need to design an Experiment for testing the watch latency.

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



