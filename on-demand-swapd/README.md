
This package is to support user-space On-Demand SWAP daemon.
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
and then, dynamically add swap files as needed.

This daemon allows free space in the filesystem to be used as swap when
needed by adding additional swap files as needed.

Swapd periodically monitors the amount of free swap available in
the system.  It tries to maintain the amount of swap using upper and
lower limits.  If the amount of free swap falls below the lower limit,
it will add an extra swapfile.  A swap file will be removed if it will
still leave the system with at least the upper limit available.

Since swapping from a file is less efficient than swapping from a
partition, this can be used to `top up' the swap when needed.

Note that all of the parameters can either be changed at compile time, or at
run time from the command line.

