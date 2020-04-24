#ifndef _ERR_
#define _ERR_

// prints info about termination of system function with an error
extern void syserr(const char *fmt, ...);

// prints info about an error during program execution
extern void fatal(const char *fmt, ...);

#endif
