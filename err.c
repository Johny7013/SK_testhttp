// borrowed from University of Warsaw's Computer Networks course
// adapted by: Jan Klinkosz, id number: 394 342

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "err.h"

// prints info about termination of system function with an error
void syserr(const char *fmt, ...)
{
  va_list fmt_args;
  int errno1 = errno;

  fprintf(stderr, "ERROR: ");
  va_start(fmt_args, fmt);
  vfprintf(stderr, fmt, fmt_args);
  va_end(fmt_args);
  fprintf(stderr, " (%d; %s)\n", errno1, strerror(errno1));
}

// prints info about an error during program execution
void fatal(const char *fmt, ...)
{
  va_list fmt_args;

  fprintf(stderr, "ERROR: ");
  va_start(fmt_args, fmt);
  vfprintf(stderr, fmt, fmt_args);
  va_end(fmt_args);
  fprintf(stderr, "\n");
}

// prints info that system is unable to allocate memory
void memerr_alloc() {
    syserr("Unable to allocate memory");
}
