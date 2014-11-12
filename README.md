SSD-based-storage-system
========================
This project gives an innovative replacement algorithm for SSD-based hybrid storage system. The project contains four parts: input generator, core algorithm, disk simulator and event driver. Input generator produces customized various kinds of workloads. Core algorithm contains traditional replacement algorithms like FIFO, LRU and Round Robin in addition to our innovative algorithm. Disk simulator mimics behaviors of HDDs and SSDs. Last, event driver triggers other three parts to coordinate with each other and make the system work well.

How to build and run?

Currently we provide only Mac version of code to quick start using Xcode. But still we provide all source code that you can use to build and run in Linux or Windows. The only cons is there is no configure or Makefile :(. Soon later I will provide such compile options.

Code format standards

The code follows Google C++ style. Please refer to http://google-styleguide.googlecode.com/svn/trunk/cppguide.html.

