/*
 *	misc.cpp
 +
 *	$Id: misc.cpp,v 1.1 2001/04/09 13:53:37 andre Exp andre $
 *
 *	$Log: misc.cpp,v $
 *	Revision 1.1  2001/04/09 13:53:37  andre
 *	Initial revision
 *
 */

#include <cstdio>
#include <cstring>
#include <ctime>

using namespace std;

#include "def.h"
#include "misc.h"

/*
	get_parameter
	
	Analyzes command line arguments.
*/
char *get_parameter(int argc, char *argv[], int *start, const char *param, int option){
	int i;
	if(start){
		if(*start<1)
			*start = 1;
		if(*start >= argc)
			return (char*)NULL;
		i=*start;
	} else
		i = 1;
	
	
	while((i<argc) && (strcmp(argv[i],param) != 0)) i++;
	
	if(start)
		*start = i+1;
			
	if(i>=argc)
		return (char*)NULL;	// option not found


	if(option == NO_ARGUMENT)
		return argv[i];
		
	if(start)
		*start = i+2;		
	
	if(i+1<argc)
		return argv[i+1];
	
	return (char*)NULL;
}

// returns true if the file <fname> exists
bool fexists(const char *fname){
	FILE *f;
	f=fopen(fname,"r");
	if(!f)
		return false;
	fclose(f);
	return true;
}

void progress(long cur,long maxn) {
	static int lcur;
	static int lmaxn;
	static int step;
	if(maxn == lmaxn){
		if(cur == lcur)
			return;
	} else {
		lmaxn = maxn;
		lcur = 0;
		step = maxn/400;
		if(!step)
			step = 1;
	}
	
	if(cur-lcur >= step) {
		fprintf(stderr,"\r%3.1f%%",(float)cur/(float)(maxn-1)*100.0);
		fflush(stderr);	
		cur = lcur;
	}
}

char *extract_fname(char *str){
	int n = strlen(str);
	while(n>0){
		if(str[n-1]=='/')
			return &(str[n]);
		n--;
	}
	return str;
}

char *gettime(){

	static char tstr[64];
	time_t t;
	
	time(&t);
//	return asctime(localtime(&t));
//	sprintf(tstr,"%.4d/%.2d/%.2d"

	strftime(tstr, sizeof(tstr), "%Y/%m/%d %H:%M:%S %Z", localtime(&t));
	return tstr;
}


void log_error(const char *str) {
	fprintf(stderr,str);
	fflush(stderr);
}

void log(const char *str) {
	printf("%s\n",str);
	fflush(stdout);
}

const char *format_error(const char *file, long line, const char *func, const char *str) {
	static char buf[255];
	sprintf(buf,"%s:%ld [%s] %s",file,line,func,str);
	return buf;
}
