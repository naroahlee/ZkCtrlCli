# My Zookeeper Client in C

Usage:
  1. Download Zookeeper tar ball through Apache Mirror.
  2. Install Zookeeper library: zookeeper_st.so zookeeper_mt.so
	2.1 Install Tool-Link and dependencies:
        sudo apt-get install libcppunit-dev libtool autoreconf
	2.2 configure and install
		./configure
        make
		sudo make install
  3. Use -DTHREADED compiler option to enable sync API. Link the zookeeper_mt library. (Already in Makefile)

