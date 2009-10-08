/*
 * random.cpp
 *
 * fibonacci random generator
 *
 * $Id: random.cpp,v 1.1 2001/04/09 14:03:18 andre Exp andre $
 *
 * $Log: random.cpp,v $
 * Revision 1.1  2001/04/09 14:03:18  andre
 * Initial revision
 *
 *
 */

#include <cstdlib>
#include <cstdio>

using namespace std;

#include "random.h"

// -------------------------------------------------------------------
// ------------- linear congruential random generator ----------------
// -------------------------------------------------------------------


// Park and Miller algorithm
#define LIN_A		16807
#define LIN_M		2147483647	// 0x7fffffff
#define LIN_Q		127773		//
#define LIN_R		2836
#define LIN_MASK	123459878
#define LIN_MAX		LIN_M

static rand_t lin_num = 0;

void lin_srand(rand_t seed){
	lin_num = seed % LIN_M;
}

rand_t lin_rand(){
	rand_t k;
	long num = lin_num;
	
	num ^= LIN_MASK;
	k = num / LIN_Q;
	num = LIN_A*(num-k*LIN_Q)-LIN_R*k;
	if(num<0) num += LIN_M;
	num ^= LIN_MASK;
	
	lin_num = (rand_t)num;
	return lin_num;
}

/*
rand_t lin_rand(){
	return (0xffff & lin_alg0()) | ((0xffff & lin_alg0())<<16);
}
*/



// -------------------------------------------------------------------
// --------------------- fibonacci random generator ------------------
// -------------------------------------------------------------------

// FIB_A must be greater then FIB_B
#define FIB_A			83
#define FIB_B			47
#define FIB_INIT		1009		// cycles to run for initializing
#define FIB_MAX			((1uL << (8*sizeof(rand_t)-1)) - 1)

// ringbuffer
static int fib_a = 0;				// pointer to the ringbuffer
static int fib_b = FIB_A-FIB_B;		// 
static rand_t fib_ring[FIB_A];		// ringbuffer to save the last FIB_A states

rand_t fib_rand() {
	rand_t result;
	
//	result = (rand_t)(fib_ring[fib_a] += fib_ring[fib_b]);
	result = (fib_ring[fib_a]+fib_ring[fib_b]) % FIB_MAX;
	fib_ring[fib_a] = result;
	
	if(!fib_a)
		fib_a = FIB_A;
	if(!fib_b)
		fib_b = FIB_B;
		
	fib_a--;
	fib_b--;
	
	return result;
}

void fib_srand(rand_t seed) {
	int i;
	
	lin_srand(seed);
	
	fib_a = 0;
	fib_b = FIB_A-FIB_B;
	
	// initialize ring buffer with algorithm 0
	for(i=0;i<FIB_A;i++){
		fib_ring[i] = lin_rand();
	}
	
	// run some cycles
	for(i=0;i<FIB_INIT;i++)
		fib_rand();
}

const long RANDOM_MAX[NUM_GENERATORS] =
		{LIN_MAX,FIB_MAX};

const RandomSeedFunc RANDOM_SEED[NUM_GENERATORS] = 
		{lin_srand,fib_srand};

const RandomFunc RANDOM_FUNC[NUM_GENERATORS] =
		{lin_rand,fib_rand};

int				CurGenerator = DEFAULT_GENERATOR;
rand_t			CurRandMax   = RANDOM_MAX[DEFAULT_GENERATOR];
RandomSeedFunc	CurRandomSeed = RANDOM_SEED[DEFAULT_GENERATOR];
RandomFunc		CurRandomFunc = RANDOM_FUNC[DEFAULT_GENERATOR];

void SetRandomGenerator(int num) {
	if((num<0)||(num>=NUM_GENERATORS)){
		fprintf(stderr,"\nSetRandomGenerator() : wrong generator\n\n");
		exit(1);
	}
	
	CurGenerator = num;
	CurRandMax = RANDOM_MAX[num];
	CurRandomSeed = RANDOM_SEED[num];
	CurRandomFunc = RANDOM_FUNC[num];
}

rand_t Random(rand_t range) {
	if( range > 0 )
		return (CurRandomFunc() % range); // OHHH very bad
	else
		return 0;
}

Float Random() {
//	return (Float)(Random(CurRandMax-1)%CurRandMax)/(Float)CurRandMax;
	return (Float)(CurRandomFunc() % CurRandMax)/(Float)CurRandMax;
}

void RandomSeed(rand_t seed){
	CurRandomSeed(seed);
}

Float RandomGauss() {
	int i;
	Float sum;
	const int n = 32;

	sum = 0.0;
	for(i=0; i<n; i++)
		sum += Random()-0.5;
	return sum/(Float)n;
}

