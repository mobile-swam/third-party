# User-space On-Demand SWAP daemon
# It is to activate kernel-space on-demand swap.

BINDIR=/usr/sbin
MANDIR=/usr/man
MANEXT=8

# You can add below custom flags to the CFLAGS line to override swapd.h
#  -DTMPDIR=\"/var/tmp\"	Directory to create swap files in
#  -DPRIORITY=-4		Nice level for daemon to run at
#  -DINTERVAL=30		Interval to check free VM
#  -DNUMCHUNKS=8		Maximum number of swap files
#  -DCHUNKSZ=4*1024*1024       	Size of each swap file
#  -DLOWER=CHUNKSZ/2		Lower limit for VM
#  -DUPPER=CHUNKSZ		Upper limit for VM

CFLAGS  = -O2 -Wall
LDFLAGS = -s

swapd: swapd.o

swapd.o: swapd.c swapd.h

install: swapd swapd.man
	install -s -m 0755 swapd $(BINDIR)

clean:
	rm -f *.o core
