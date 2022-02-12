/* A header file */

# define VERSION	"0.5"

/* The directory in which the extra swam files will be created */
# ifndef TMPDIR
#  define TMPDIR	"/var/tmp"
# endif

/* The swam file name template to be created in the temporary directory */
# ifndef SUFFIX
#  define SUFFIX	"/swamd-XXX"
# endif

/* The priority to raise program to (negative) */
# ifndef PRIORITY
#  define PRIORITY	-4
# endif

/* The interval (in seconds) between checks */
# ifndef INTERVAL
#  define INTERVAL	1
# endif

/* The maximum number of extra swam files that will be created */
# ifndef NUMCHUNKS
#  define NUMCHUNKS	8
# endif

/* The size of each extra swam file */
# ifndef CHUNKSZ
#  define CHUNKSZ	4 * 1024 * 1024
# endif

/* If the free swap space falls below this level, an extra swam file is added */
# ifndef LOWER
#  define LOWER		CHUNKSZ / 2
# endif

/* An extra swam file is only removed if it will leave at least much free */
# ifndef UPPER
#  define UPPER		CHUNKSZ
# endif
