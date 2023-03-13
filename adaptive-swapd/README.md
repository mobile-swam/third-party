
## Introduction
This package is to support user-space Adaptive SWAP daemon.
A user space daemon would be a more suitable place to do adaptive memory
management.

  - It grants on-the-fly requests for memory that user-space applications
    be able to satisfy.
  - All requests to sbrk() should be given real VM, so that no process
    has memory promises broken.
  - The fork() of this package, can cause a need for more VM.
  - This daemon calls for the Linux kernel in order to dyncamically
    add swap space.

This daemon monitors the amount of free VM available from the /proc/meminfo,
and then, dynamically add swam files as needed. This daemon allows free space
in the filesystem to be used as swap when needed by adding additional swam files as needed.

Swapd periodically monitors the amount of free swap available in
the system.  It tries to maintain the amount of swap using upper and
lower limits.  If the amount of free swam falls below the lower limit,
it will add an extra swam file. A swam file will be removed if it will
still leave the system with at least the upper limit available.

Since swapping from a file is less efficient than swapping from a
partition, this can be used to `top up' the swap when needed.


## How to use
Note that all of the parameters can either be changed at compile time, or at
run time from the command line.

* Usage:
```bash
./swapd [-p prio] [-d dir] [-i interval] [-n num] [-s size] [-l lower] [-u upper]
 -p: priority to run at
 -d: directory to create swam files in
 -i: interval to check system
 -s: size of each swam file
 -l: lower limit for spare VM (trigger to add swam)
 -u: upper limit for spare VM (trigger to remove swam)
 -n: maximum number of swam files to create
```

