## stress-ng

stress-ng will stress test a computer system in various selectable ways. It
was designed to exercise various physical subsystems of a computer as well as
the various operating system kernel interfaces. Stress-ng features:

  * Over 260 stress tests
  * 85+ CPU specific stress tests that exercise floating point, integer,
    bit manipulation and control flow
  * over 20 virtual memory stress tests

stress-ng was originally intended to make a machine work hard and trip hardware
issues such as thermal overruns as well as operating system bugs that only
occur when a system is being thrashed hard. Use stress-ng with caution as some
of the tests can make a system run hot on poorly designed hardware and also can
cause excessive system thrashing which may be difficult to stop.

stress-ng can also measure test throughput rates; this can be useful to observe
performance changes across different operating system releases or types of
hardware. However, it has never been intended to be used as a precise benchmark
test suite, so do NOT use it in this manner.


## Build (for Android)
Before starting explainging how to build stress-ng for Android platform, 
we assume that you use Ubuntu 18.04 LTS (64bit) distribution.

If you need to install the gcc-4.9-aarch64-linux-gnu on Ubuntu 18.04 for the build
compatibility, Please download the binaries at the below Linaro webpage.

### Install the ARM toolchain manually
  * https://releases.linaro.org/components/toolchain/binaries/4.9-2017.01/aarch64-linux-gnu/

### Install the ARM toolchain with apt command
```bash
sudo apt install gcc-4.9-aarch64-linux-gnu
git checkout -b V0.10.17
CC=aarch64-linux-gnu-gcc-4.9 STATIC=1 make
ls -al ./stress-ng
 ```


## Examples

Run 4 CPU, 2 virtual memory, 1 disk and 8 fork stressors for 2 minutes and print measurements:
```
stress-ng --cpu 4 --vm 2 --hdd 1 --fork 8 --timeout 2m --metrics
stress-ng: info:  [573366] setting to a 120 second (2 mins, 0.00 secs) run per stressor
stress-ng: info:  [573366] dispatching hogs: 4 cpu, 2 vm, 1 hdd, 8 fork
stress-ng: info:  [573366] successful run completed in 123.78s (2 mins, 3.78 secs)
stress-ng: info:  [573366] stressor       bogo ops real time  usr time  sys time   bogo ops/s     bogo ops/s CPU used per
stress-ng: info:  [573366]                           (secs)    (secs)    (secs)   (real time) (usr+sys time) instance (%)
stress-ng: info:  [573366] cpu              515396    120.00    453.02      0.18      4294.89        1137.24        94.42
stress-ng: info:  [573366] vm              2261023    120.01    223.80      1.80     18840.15       10022.27        93.99
stress-ng: info:  [573366] hdd              367558    123.78     10.63     11.67      2969.49       16482.42        18.02
stress-ng: info:  [573366] fork             598058    120.00     68.24     65.88      4983.80        4459.13        13.97
```

Run matrix stressor on all online CPUs for 60 seconds and measure temperature:
```
stress-ng --matrix -1 --tz -t 60
stress-ng: info:  [1171459] setting to a 60 second run per stressor
stress-ng: info:  [1171459] dispatching hogs: 8 matrix
stress-ng: info:  [1171459] successful run completed in 60.01s (1 min, 0.01 secs)
stress-ng: info:  [1171459] matrix:
stress-ng: info:  [1171459]               acpitz0   75.00 C (348.15 K)
stress-ng: info:  [1171459]               acpitz1   75.00 C (348.15 K)
stress-ng: info:  [1171459]          pch_skylake   60.17 C (333.32 K)
stress-ng: info:  [1171459]         x86_pkg_temp   62.72 C (335.87 K)
```

Generate and measure branch misses using perf metrics:
```
sudo stress-ng --branch 1 --perf -t 10 2>& 1 | grep Branch
stress-ng: info:  [1171714]                604,703,327 Branch Instructions            53.30 M/sec
stress-ng: info:  [1171714]                598,760,234 Branch Misses                  52.77 M/sec (99.02%)
```


## Contributing to stress-ng:

Send patches to colin.i.king@gmail.com or merge requests at
https://github.com/ColinIanKing/stress-ng

## Quick Start Reference Guide
The [Ubuntu stress-ng reference guide](https://wiki.ubuntu.com/Kernel/Reference/stress-ng)
contains a brief overview and worked examples.


