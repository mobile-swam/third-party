# User-space On-Demand SWAP daemon

BINDIR=/usr/sbin

# You can add below custom flags to the CFLAGS line to override swam.h
#  -DTMPDIR=\"/var/tmp\"	Directory to create swam files in
#  -DPRIORITY=-4		Nice level for daemon to run at
#  -DINTERVAL=30		Interval to check free VM
#  -DNUMCHUNKS=8		Maximum number of swam files
#  -DCHUNKSZ=4*1024*1024       	Size of each swam file
#  -DLOWER=CHUNKSZ/2		Lower limit for VM
#  -DUPPER=CHUNKSZ		Upper limit for VM

CFLAGS  = -O2 -Wall
LDFLAGS = -s

swamd: swamd.o

swamd.o: swamd.c swamd.h

install: swapd
	install -s -m 0755 swapd $(BINDIR)

clean:
	rm -f *.o core swamd
