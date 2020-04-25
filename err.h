// borrowed from University of Warsaw's Computer Networks course
// adapted by: Jan Klinkosz, id number: 394 342
#ifndef _ERR_
#define _ERR_

// prints info about termination of system function with an error
extern void syserr(const char *fmt, ...);

// prints info about an error during program execution
extern void fatal(const char *fmt, ...);

// prints info that system is unable to allocate memory
extern void memerr_alloc();

#endif
