/*
 * misc.h
 *
 * $Id: misc.h,v 1.1 2001/04/09 14:01:29 andre Exp andre $
 *
 * $Log: misc.h,v $
 * Revision 1.1  2001/04/09 14:01:29  andre
 * Initial revision
 *
 *
 */
 
#ifndef MISC_H
#define MISC_H

// #include "def.h"

#define NO_ARGUMENT	0
#define ARGUMENT	1

char *get_parameter(int argc, char *argv[], int *start, const char *param, int option);
bool fexists(const char *fname);
void progress(long cur,long maxn);
char *extract_fname(char *str);
char *gettime();
void log(const char *str);
void log_error(const char *str);
const char *format_error(const char *file, long line, const char *func, const char *str);

#endif
