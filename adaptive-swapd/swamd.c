/*
 * swamd - dynamically add and remove swap-format files
 */ 

# include <stdio.h>
# include <string.h>
# include <syslog.h>
# include <unistd.h>
# include <stdlib.h>
# include <malloc.h>
# include <getopt.h>
# include <signal.h>
# include <errno.h>
# include <sys/stat.h>
# include <sys/file.h>
# include <sys/vfs.h>
# include <sys/resource.h>
# include <sys/swap.h>
# include <sys/user.h>
# include <sched.h>

/* A header fie */
# include "swamd.h"

/* Data structure */
int     priority  = PRIORITY;   /* -p */
int     chunksz   = CHUNKSZ;    /* -s */
int     numchunks = NUMCHUNKS;  /* -n */
int     lower     = LOWER;      /* -l */
int     upper     = UPPER;      /* -u */
int     interval  = INTERVAL;   /* -i */
char   *tmpdir    = TMPDIR;     /* -d */

int	debug	= 0;		/* -D */

char  **swamfile;		/* the names of swam files created */
int     chunks	= 0;		/* number of swam files currently alloc'ed */

int     addswap ( int );
int     delswap ( int );

/*
 * Check the parameters (either given or compiled in) for sense.
 */
void	ckconf ( char *argv0 )
{
    struct stat st;
    char *msg = NULL;

    if ( lstat ( tmpdir, &st ) < 0 || ! S_ISDIR ( st.st_mode ) )
	msg = "tmpdir is not a directory";
    else if ( chunksz < 2 * PAGE_SIZE )
	msg = "size too small for swamfile";
    else if ( chunksz > ( PAGE_SIZE - 10 ) * 8 * PAGE_SIZE )
	msg = "size too large for swamfile (strange, but true)";
    else if ( upper < lower )
	msg = "lower limit is smaller than upper limit";

    if ( msg ) {
	fprintf ( stderr, "%s: %s\n", argv0, msg );
	exit ( 1 );
    }
}

/*
 * Remove all swamfiles.
 */
void	cleanup ()
{
    while ( chunks ) {
        delswap ( --chunks );
    }
}

/*
 * On signals, tidy up, and vanish.
 */
void    sighandler ( int signal )
{
    syslog ( LOG_INFO, "shutting down on signal %d...", signal );
    cleanup ();
    syslog ( LOG_INFO, "...done" );
    exit ( 1 );
}

/*
 * Usage, with optional error message.
 */
void	usage ( char *argv0, char *str )
{
    if ( str && *str )
	(void) fprintf ( stderr, "%s: %s\n", argv0, str );
    (void) fprintf ( stderr, "usage: %s "
	    "[-p prio] [-d dir] [-i interval] [-n num] [-s size] [-l lower] [-u upper]\n",
	    argv0 );
    exit ( 1 );
}

/*
 * help message
 */
void	help ( char *argv0 )
{
    (void) fprintf ( stderr, "%s: dynamically maintain swap-style swam files\n"
	    "\t-p\tpriority to run at\n"
	    "\t-d\tdirectory to create swam files in\n"
	    "\t-i\tinterval to check system\n"
	    "\t-s\tsize of each swam file\n"
	    "\t-l\tlower limit for spare VM (trigger to add swam)\n"
	    "\t-u\tupper limit for spare VM (trigger to remove swam)\n"
	    "\t-n\tmaximum number of swam files to create\n", argv0 );
}

/*
 * Work out what to multiply a value by based on the suffix.
 */
int suffix ( char **str )
{
    int mult = 1;
    while ( **str ) {
	switch ( *(*str)++ ) {
	case 'b': case 'B':
	    mult *= 512;
	    break;
	case 'k': case 'K':
	    mult *= 1024;
	    break;
	case 'm': case 'M':
	    mult *= 1024 * 1024;
	    break;
	default:
	    return 0;
	}
    }
    return mult;
}

/*
 * The main program.
 * Parse arguments, prepare for battle stations.
 * Loop repeatedly, deciding whether to add or remove swam files based on swap
 * currently available, and the high and low water marks.
 */
int     main ( int argc, char *argv[] )
{
    long    swap, getswap();
    struct  sigaction act;
    int     i;

    while ( ( i = getopt ( argc, argv, "vhp:d:i:l:n:s:u:D" ) ) != -1 ) {
	switch ( i ) {
	case '?':
	    usage ( argv[0], "unknown flag, use -h for help" );
	    break;
	case 'v':
	    fprintf ( stderr, "swapd version %s\n", VERSION );
	    exit ( 0 );
	case 'h':
	    help ( argv[0] ); 
	    exit ( 0 );
	case 'p':
	    priority = strtol ( optarg, &optarg, 10 );
	    if ( priority < PRIO_MIN || priority > PRIO_MAX || *optarg != '\0' )
	        usage ( argv[0], "bad value for priority" );
	    break;
	case 'd':
	    tmpdir = optarg;
	    break;
	case 'i':
	    interval = strtol ( optarg, &optarg, 10 );
	    if ( interval < 0 || *optarg != '\0' )
	        usage ( argv[0], "bad value for interval" );
	    break;
	case 'l':
	    lower = strtol ( optarg, &optarg, 10 );
	    lower *= suffix ( &optarg );
	    if ( lower <= 0 || *optarg != '\0' )
		usage ( argv[0], "bad value for lower limit" );
	    break;
	case 'n':
	    numchunks = strtol ( optarg, &optarg, 10 );
	    numchunks *= suffix ( &optarg );
	    if ( numchunks <= 0 || *optarg != '\0' )
		usage ( argv[0], "bad value for maximum number of swam files" );
	    break;
	case 's':
	    chunksz = strtol ( optarg, &optarg, 10 );
	    chunksz *= suffix ( &optarg );
	    if ( chunksz <= 0 || *optarg != '\0' )
		usage ( argv[0], "bad value for size of swam file" );
	    break;
	case 'u':
	    upper = strtol ( optarg, &optarg, 10 );
	    upper *= suffix ( &optarg );
	    if ( upper <= 0 || *optarg != '\0' )
		usage ( argv[0], "bad value for upper limit" );
	    break;
	case 'D':
	    debug = 1;
	    break;
	default:
	    usage ( argv[0], "internal getopt foulup" );
	    break;
	}
    }

    if ( argc - optind != 0 ) {
	usage ( argv[0], "" );
    }

    ckconf ( argv[0] );

    if ( geteuid () ) {
        (void) fprintf ( stderr, "%s: must be run as root\n", argv[0] );
        exit ( 1 );
    }

    /* don't want swam files world readable */
    (void) umask ( 0077 );

    /* want to run at higher priority so get a chance to add swap space */
    if ( priority <= 0 ) {
	if ( setpriority ( PRIO_PROCESS, getpid (), priority ) < 0 ) {
	    (void) fprintf ( stderr, "%s: setpriority: %s\n", argv[0],
		    strerror ( errno ) );
	    exit ( 1 );
	}
    } else {
	struct sched_param s;
	s.sched_priority = priority;
	if ( sched_setscheduler( 0, SCHED_RR, &s ) < 0 )
	{
	    (void) fprintf( stderr, "%s: sched_setschedular: %s",
			argv[0], strerror( errno ) );
	    exit( 1 );
	}
    }

    if ( ! debug ) {
	switch ( fork() ) {
	case -1:		/* error */
	    (void) fprintf ( stderr, "%s: fork failed\n", argv[0] );
	    exit ( 1 );
	case 0:			/* child */
	    for ( i = getdtablesize()-1; i >= 0; i-- )
	        (void) close ( i );
	    (void) setsid ();
	    break;
	default:		/* parent */
	    exit ( 0 );
	}
    }

    openlog ( "swamd", LOG_DAEMON | LOG_CONS | (debug ? LOG_PERROR : 0), 0 );
    setlogmask ( debug ? LOG_UPTO ( LOG_DEBUG ) : LOG_UPTO ( LOG_WARNING ) );

    /* allocate memory needed at start, rather than leaving to later */
    swamfile = malloc ( sizeof(char*) * numchunks );
    if ( swamfile == NULL ) {
        syslog ( LOG_ERR, "malloc failed... bye" );
        exit ( 1 );
    }
    for ( i = 0; i < numchunks; i++ ) {
        swamfile[i] = malloc ( strlen ( tmpdir ) + sizeof ( SUFFIX ) );
	if ( swamfile[i] == NULL ) {
	    syslog ( LOG_ERR, "malloc failed... bye" );
	    exit ( 1 );
	}
    }

    act.sa_handler = sighandler;
    sigfillset ( &act.sa_mask );
    act.sa_flags = 0;
    (void) sigaction ( SIGHUP, &act, (struct sigaction*)NULL );
    (void) sigaction ( SIGINT, &act, (struct sigaction*)NULL );
    (void) sigaction ( SIGQUIT, &act, (struct sigaction*)NULL );
    (void) sigaction ( SIGTERM, &act, (struct sigaction*)NULL );

    for ( ; ; ) {
	swap = getswap();
	syslog ( LOG_DEBUG, "%ld available swap", swap / 1024 );
	if ( swap < lower && chunks < numchunks ) {
	    if ( addswap ( chunks ) )
		chunks++;
	}
	if ( swap > chunksz + upper && chunks > 0 ) {
	    delswap ( --chunks );
	}
	sleep ( interval );
    }
}

/*
 * Return the current amount of swap available.
 * This is the sum of free and shared.  It is got by parsing /proc/meminfo
 * Linux 1.2:
 *             total:   used:    free:   shared:  buffers:
 *     Mem:   7417856  5074944  2342912   880640  3563520
 *     Swap:  8441856        0  8441856
 *
 * Linux 1.3:
 *             total:    used:    free:  shared: buffers:  cached:
 *     Mem:   7069696  6782976   286720  3633152   753664  2564096
 *     Swap: 16801792   962560 15839232
 *     MemTotal:      6904 kB
 *     MemFree:        280 kB
 *     MemShared:     3548 kB
 *     Buffers:        736 kB
 *     Cached:        2504 kB
 *     SwapTotal:    16408 kB
 *     SwapFree:     15468 kB
 */
long getswap ()
{
    static  int     fd	= -1;		/* fd to read meminfo from */
    static  char    buffer [ 1024 ];	/* enough to slurp meminfo into */
    char   *cp;
    long    memfree, buffers, cached, swapfree;
    int     n;

    if ( fd < 0 ) {
        fd = open ( "/proc/meminfo", O_RDONLY );
        if ( fd < 0 ) {
	    syslog ( LOG_ERR, "can't open \"/proc/meminfo\": %m" );
            exit ( 1 );
	}
    }

    if ( lseek ( fd, 0, SEEK_SET ) == EOF ) {
        syslog ( LOG_ERR, "lseek failed on \"/proc/meminfo\": %m" );
        exit ( 1 );
    }
    if ( ( n = read ( fd, buffer, sizeof(buffer)-1 ) ) < 0 ) {
	syslog ( LOG_ERR, "read failed on \"/proc/meminfo\": %m" );
	exit ( 1 );
    }
    buffer[n] = '\0';				/* null terminate */

    memfree = -1;
    buffers = 0;
    cached  = 0;
    swapfree = 0;

    cp = buffer;
    while ( cp )
    {
	if ( strncmp( cp, "MemFree:", 8 ) == 0 )
	{
	    memfree = atol( cp+9 ) * 1024;
	    printf( "MemFree: %ld\n", memfree );
	}
	else if ( strncmp( cp, "Buffers:", 8 ) == 0 )
	{
	    buffers = atol( cp+9 ) * 1024;
	    printf( "Buffers: %ld\n", buffers );
	}
	else if ( strncmp( cp, "Cached:", 7 ) == 0 )
	{
	    cached = atol( cp+8 ) * 1024;
	    printf( "Cached: %ld\n", cached );
	}
	else if ( strncmp( cp, "SwapFree:", 9 ) == 0 )
	{
	    swapfree = atol( cp+10 ) * 1024;
	    printf( "SwapFree: %ld\n", swapfree );
	}
#if 0 	/* Linux 1.2 compatability */
	else if ( strncmp( cp, "Mem:", 4 ) == 0 )
	{
	    if ( 2 != sscanf ( cp+5, "%*d %*d %ld %*d %ld", &memfree, &buffers ) ) {
		syslog ( LOG_ERR, "sscanf failed on memory info" );
		exit ( 1 );
	    }
	    printf( "Mem: %ld %ld\n", memfree, buffers );
	}
	else if ( strncmp( cp, "Swap:", 5 ) == 0 )
	{
	    if ( 1 != sscanf ( cp+6, "%*d %*d %ld\n", &swapfree ) ) {
		syslog ( LOG_ERR, "sscanf failed on swap info" );
		exit ( 1 );
	    }
	    printf( "Swap: %ld\n", swapfree );
	}
#endif

	/* move onto next line */
	cp = strchr( cp, '\n' );
	if ( cp )
	{
	    cp++;
	}
    }

    /* If we haven't enabled 1.2 compatability on a 1.2 machine */
    if ( memfree < 0 )
    {
	syslog ( LOG_ERR, "error parsing old format \"/proc/meminfo\"" );
	exit ( 1 );
    }

    return ( memfree + swapfree + ( buffers + cached ) / 2 );
}

/*
 * Add swamfile number i.
 */
int addswap ( int i )
{
    static  char    page [ PAGE_SIZE ];	/* one page of memory used to make swap file */

    struct  statfs  fsstat;
    int     fd;
    int     pages   = chunksz / sizeof(page);
    int     clean   = 0;

    if ( statfs ( tmpdir, &fsstat ) < 0 ) {
        syslog ( LOG_ERR, "statfs failed on \"%s\": %m", tmpdir );
        cleanup ();
        exit ( 1 );
    }
    if ( fsstat.f_bsize * fsstat.f_bavail < chunksz ) {
        syslog ( LOG_WARNING, "no space for swamfile on \"%s\"", tmpdir );
	return 0;
    }

    strcpy ( swamfile[i], tmpdir );
    strcat ( swamfile[i], SUFFIX );
    if ( ( fd = mkstemp ( swamfile[i] ) ) < 0 ) {
	syslog ( LOG_ERR, "mkstemp failed: %m" );
	cleanup ();
	exit ( 1 );
    }

    /* add a swap signature */
    memcpy ( page + sizeof(page) - 10, "SWAP-SPACE", 10 );

    /* mark all pages as being valid */
    memset ( page, 0xFF, ( pages + 7 ) / 8 );
    page [ 0 ] &= 0xFE;
    if ( pages % 8 )
	page [ ( pages + 7 ) / 8 - 1 ] &= ( 0xFF >> ( 8 - ( pages % 8 ) ) );

    /* stick the pages for swapping onto disk */
    while ( pages-- > 0 ) {
        if ( write ( fd, page, sizeof(page) ) != sizeof(page) ) {
	    syslog ( LOG_WARNING, "write failed on \"%s\": %m", swamfile[i] );
	    (void) close ( fd );
	    (void) unlink ( swamfile[i] );
	    return 0;
	}
	if ( ! clean++ )
	    memset ( page, 0, sizeof(page) );
    }
    if ( fsync ( fd ) < 0 ) {
	syslog ( LOG_ERR, "fsync failed on \"%s\": %m", swamfile[i] );
	close ( fd );
	(void) unlink ( swamfile[i] );
	return 0;
    }
    if ( close ( fd ) < 0 ) {
	syslog ( LOG_ERR, "fsync failed on \"%s\": %m", swamfile[i] );
	(void) unlink ( swamfile[i] );
	return 0;
    }

    syslog ( LOG_INFO, "adding \"%s\" as swap", swamfile[i] );
    if ( swapon ( swamfile[i], 0 ) < 0 ) {
	syslog ( LOG_ERR, "swapon failed on \"%s\": %m", swamfile[i] );
	(void) unlink ( swamfile[i] );
	return 0;
    }

    return 1;
}

/*
 * Remove swamfile number i.
 */
int delswap ( int i )
{
    if ( swapoff ( swamfile[i] ) < 0 ) {
	syslog ( LOG_ERR, "swapoff failed on \"%s\": %m", swamfile[i] );
	return 0;
    }
    if ( unlink ( swamfile[i] ) < 0 ) {
        syslog ( LOG_ERR, "unlink of \"%s\" failed: %m", swamfile[i] );
    }

    syslog ( LOG_INFO, "removed \"%s\" as swap", swamfile[i] );

    return 1;
}
