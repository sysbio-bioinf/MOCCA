#ifndef __CDFLIB_H__
#define __CDFLIB_H__

#include "def.h"

#ifdef __cplusplus
extern "C" {
#endif

Float algdiv(Float*,Float*);
Float alngam(Float*);
Float alnrel(Float*);
Float apser(Float*,Float*,Float*,Float*);
Float basym(Float*,Float*,Float*,Float*);
Float bcorr(Float*,Float*);
Float betaln(Float*,Float*);
Float bfrac(Float*,Float*,Float*,Float*,Float*,Float*);
void bgrat(Float*,Float*,Float*,Float*,Float*,Float*,int*i);
Float bpser(Float*,Float*,Float*,Float*);
void bratio(Float*,Float*,Float*,Float*,Float*,Float*,int*);
Float brcmp1(int*,Float*,Float*,Float*,Float*);
Float brcomp(Float*,Float*,Float*,Float*);
Float bup(Float*,Float*,Float*,Float*,int*,Float*);
void cdfbet(int*,Float*,Float*,Float*,Float*,Float*,Float*,
            int*,Float*);
void cdfbin(int*,Float*,Float*,Float*,Float*,Float*,Float*,
            int*,Float*);
void cdfchi(int*,Float*,Float*,Float*,Float*,int*,Float*);
void cdfchn(int*,Float*,Float*,Float*,Float*,Float*,int*,Float*);
void cdff(int*,Float*,Float*,Float*,Float*,Float*,int*,Float*);
void cdffnc(int*,Float*,Float*,Float*,Float*,Float*,Float*,
            int*s,Float*);
void cdfgam(int*,Float*,Float*,Float*,Float*,Float*,int*,Float*);
void cdfnbn(int*,Float*,Float*,Float*,Float*,Float*,Float*,
            int*,Float*);
void cdfnor(int*,Float*,Float*,Float*,Float*,Float*,int*,Float*);
void cdfpoi(int*,Float*,Float*,Float*,Float*,int*,Float*);
void cdft(int*,Float*,Float*,Float*,Float*,int*,Float*);
void cdftnc(int*,Float*,Float*,Float*,Float*,Float*,int*,Float*);
void cumbet(Float*,Float*,Float*,Float*,Float*,Float*);
void cumbin(Float*,Float*,Float*,Float*,Float*,Float*);
void cumchi(Float*,Float*,Float*,Float*);
void cumchn(Float*,Float*,Float*,Float*,Float*);
void cumf(Float*,Float*,Float*,Float*,Float*);
void cumfnc(Float*,Float*,Float*,Float*,Float*,Float*);
void cumgam(Float*,Float*,Float*,Float*);
void cumnbn(Float*,Float*,Float*,Float*,Float*,Float*);
void cumnor(Float*,Float*,Float*);
void cumpoi(Float*,Float*,Float*,Float*);
void cumt(Float*,Float*,Float*,Float*);
void cumtnc(Float*,Float*,Float*,Float*,Float*);
Float devlpl(Float [],int*,Float*);
Float dinvnr(Float *p,Float *q);
/*
void E0000(int,int*,Float*,Float*,unsigned long*,
                  unsigned long*,Float*,Float*,Float*,
                  Float*,Float*,Float*,Float*);
*/
void dinvr(int*,Float*,Float*,unsigned long*,unsigned long*);
void dstinv(Float*,Float*,Float*,Float*,Float*,Float*,
            Float*);
Float dt1(Float*,Float*,Float*);
/*
void E0001(int,int*,Float*,Float*,Float*,Float*,
                  unsigned long*,unsigned long*,Float*,Float*,
                  Float*,Float*);
*/
void dzror(int*,Float*,Float*,Float*,Float *,
           unsigned long*,unsigned long*);
void dstzr(Float *zxlo,Float *zxhi,Float *zabstl,Float *zreltl);
Float erf1(Float*);
Float erfc1(int*,Float*);
Float esum(int*,Float*);
Float exparg(int*);
Float fpser(Float*,Float*,Float*,Float*);
Float gam1(Float*);
void gaminv(Float*,Float*,Float*,Float*,Float*,int*);
Float gamln(Float*);
Float gamln1(Float*);
Float Xgamm(Float*);
void grat1(Float*,Float*,Float*,Float*,Float*,Float*);
void gratio(Float*,Float*,Float*,Float*,int*);
Float gsumln(Float*,Float*);
Float psi(Float*);
Float rcomp(Float*,Float*);
Float rexp(Float*);
Float rlog(Float*);
Float rlog1(Float*);
Float spmpar(int*);
Float stvaln(Float*);
Float fifdint(Float);
Float fifdmax1(Float,Float);
Float fifdmin1(Float,Float);
Float fifdsign(Float,Float);
long fifidint(Float);
long fifmod(long,long);
void ftnstop(char*);
extern int ipmpar(int*);

#ifdef __cplusplus
}
#endif

#endif

