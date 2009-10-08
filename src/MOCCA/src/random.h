/*
 * random.h
 *
 * fibonacci random generator
 *
 * $Id: random.h,v 1.1 2001/04/09 14:03:37 andre Exp andre $
 *
 * $Log: random.h,v $
 * Revision 1.1  2001/04/09 14:03:37  andre
 * Initial revision
 *
 *
 */
 
#ifndef RANDOM_H
#define RANDOM_H

#include <stdlib.h>
#include "def.h"

#define RND_LINEAR		0
#define RND_FIBONACCI	1

#define DEFAULT_GENERATOR 1
#define NUM_GENERATORS	2

typedef unsigned int rand_t;

#ifndef RAND_MAX
#define RAND_MAX ((1uL << (8*sizeof(rand_t)-1)) - 1uL)
#endif

void SetRandomGenerator(int num);
Float Random();					// generates a random number in the interval [0,1)
rand_t Random(rand_t range);	// generates a integer random number from {0,...,range-1}
void RandomSeed(rand_t seed);	// sets the random seed value
Float RandomGauss();			// normal distributed random variable

typedef void (*RandomSeedFunc)(rand_t seed);
typedef rand_t (*RandomFunc)(void);

extern int				CurGenerator;
extern rand_t			CurRandMax;
extern RandomSeedFunc	CurRandomSeed;
extern RandomFunc		CurRandomFunc;


template <class T >
T RandomT(T n) {
	return static_cast<T>(Random( static_cast<rand_t>(n) ));
}

template <class T>
T Uniform01() {
	return static_cast<T>( CurRandomFunc() % CurRandMax ) / static_cast<T>( CurRandMax );
}

#endif
