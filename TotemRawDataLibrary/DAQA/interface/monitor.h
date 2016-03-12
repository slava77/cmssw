#ifndef __monitor_h__
#define __monitor_h__

/*
 * DATE monitor V 3.xx definitions
 * ===============================
 */

/* The error codes */
#define ERR_BASE                 0x80000000
#define MON_ERR(v) ((int)((ERR_BASE)|(v)))

	/* Entry not yet implemented */
#define MON_ERR_NOT_IMPLEMENTED  MON_ERR(0x1)

	/* Monitor internal error */
#define MON_ERR_INTERNAL         MON_ERR(0x2)

	/* Too many clients */
#define MON_ERR_TOO_MANY_CLIENTS MON_ERR(0x3)

	/* setDataSource not yet called */
#define MON_ERR_NO_DATA_SOURCE   MON_ERR(0x4)

	/* Bad event format detected */
#define MON_ERR_BAD_EVENT        MON_ERR(0x8)

	/* Event to monitor too big */
#define MON_ERR_EVENT_TOO_BIG    MON_ERR(0x10)

	/* Operation interrupted */
#define MON_ERR_INTERRUPTED      MON_ERR(0x20)

	/* Cannot connect to host */
#define MON_ERR_NOCONNECT        MON_ERR(0x40)

	/* Cannot malloc for event or other structures */
#define MON_ERR_MALLOC           MON_ERR(0x80)

	/* Monitor table too big */
#define MON_ERR_TABLE_TOO_BIG    MON_ERR(0x100)

	/* Error parsing monitor policy table */
#define MON_ERR_PARSE_ERROR      MON_ERR(0x200)

	/* Error parsing data source string */
#define MON_ERR_BAD_DATA_SOURCE  MON_ERR(0x400)

	/* Key file not found */
#define MON_ERR_NO_KEY_FILE      MON_ERR(0x800)

	/* System-dependent error (see errno) */
#define MON_ERR_SYS_ERROR        MON_ERR(0x1000)

	/* The client has been signed out by force */
#define MON_ERR_SIGNED_OUT       MON_ERR(0x2000)

	/* Configuration line too complex */
#define MON_ERR_LINE_TOO_COMPLEX MON_ERR(0x4000)

	/* Parse error within configuration file */
#define MON_ERR_CONFIG_ERROR     MON_ERR(0x8000)

	/* Cleanup of monitor clients/server needed */
#define MON_ERR_CLEANUP_NEEDED   MON_ERR(0x10000)

        /* Monitor buffer locked: manual intervention needed */
#define MON_ERR_LOCKFILE         MON_ERR(0x20000)

        /* Bad input parameter(s) */
#define MON_ERR_BAD_INPUT        MON_ERR(0x40000)

        /* End of input file */
#define MON_ERR_EOF              MON_ERR(0x80000)

        /* Illegal operation for this host (offline host) */
#define MON_ERR_OFFLINE_HOST     MON_ERR(0x100000)

        /* Operation unsupported for this version of the monitor library */
#define MON_ERR_UNSUPPORTED      MON_ERR(0x200000)

        /* A get event call could not transfer all the event data */
#define MON_ERR_EVENT_TRUNCATED  MON_ERR(0x400000)

        /* Input data file not found */
#define MON_ERR_NO_SUCH_FILE     MON_ERR(0x800000)

        /* Monitoring not allowed for this host */
#define MONITOR_NOT_ALLOWED      MON_ERR(0x1000000)

        /* Failed to load one or more databases */
#define MONITOR_DB_ERR           MON_ERR(0x2000000)

        /* Active configuration changed */
#define MON_ERR_ACTIVE_CONFIG_CHANGED MON_ERR(0x4000000)

        /* Failed to get active LDCs for monitoring by detector */
#define MON_FAILED_GET_ACTIVE_LDCS MON_ERR(0x8000000)

        /* Failed to get active GDCs for monitoring by GDCs */
#define MON_FAILED_GET_ACTIVE_GDCS MON_ERR(0x10000000)

#ifndef __CINT__
#ifdef __cplusplus
extern "C" {
#endif
#endif

/* The clients' interface */
int   monitorSetDataSource( char* );            /* 0 => OK, else => error    */
int   monitorDeclareTable( char** );		/* 0 => OK, else => error    */
int   monitorDeclareTableExtended( char** );	/* 0 => OK, else => error    */
int   monitorDeclareTableWithAttributes(        /* 0 => OK, else => error    */
				      char** );
int   monitorDeclareMp( char * );               /* 0 => OK, else => error    */
char *monitorDecodeError( int error ); 		/* NULL => error, else => OK */
int   monitorGetEvent( void*, long32 );	        /* 0 => OK, else => error    */
int   monitorGetEventDynamic( void** );		/* 0 => OK, else => error    */
int   monitorSetWait();			        /* 0 => OK, else => error    */
int   monitorSetNowait();			/* 0 => OK, else => error    */
int   monitorControlWait( int );		/* 0 => OK, else => error    */
int   monitorSetNoWaitNetworkTimeout( int );	/* 0 => OK, else => error    */
int   monitorSetSwap( int, int );               /* 0 => OK, else => error */
int   monitorFlushEvents();			/* 0 => OK, else => error    */
int   monitorLogout();				/* 0 => OK, else => error    */

/* The "old" deprecated entries: */
int   SetDataSource( char* ); 			/* 0 => OK, else => error    */
void  SetGetEventInteractive();			/* 0 => OK, else => error    */
void  SetGetEventBatch();			/* 0 => OK, else => error    */
void  FlushGetEventBuffer();			/* 0 => OK, else => error    */
int   getevent( char*, long32 );	        /* >0   => OK (byte count)
						   else => error             */

#ifndef __CINT__
#ifdef __cplusplus
}
#endif
#endif

#endif
