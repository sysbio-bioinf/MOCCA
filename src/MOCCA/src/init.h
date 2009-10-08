/*
 * $Id: init.h,v 1.2 2001/06/05 20:46:06 andre Exp andre $
 *
 * $Log: init.h,v $
 * Revision 1.2  2001/06/05 20:46:06  andre
 * Implemented and checked the maxmin algorithm.
 *
 * Revision 1.1  2001/05/21 13:53:54  andre
 * Initial revision
 *
 *
 */


/*!	\file init.h
 	\brief Cluster initialization methods.

	Models of the concept "cluster initialization".
*/

#ifndef __INIT_H
#define __INIT_H

/* STL */
#include <stdexcept>
#include <algorithm>
#include <iterator>
#include <vector>
#include <numeric>
#include <limits>

using namespace std;

#include <ext/algorithm>		// random_sample_n
#include <ext/numeric>			// iota

#ifdef __GNUC__
using namespace __gnu_cxx;
#endif


#include "error_handler.h"
#include "random.h"
#include "matrixutil.h"
#include "algoext.h"

/*! Distribute n vectors evenly in the space spanned by data.
*/
template <class DataSeq, class CodSeq, class RandomNumberGenerator >
void init_even(const DataSeq &data, CodSeq &cod, unsigned int k, RandomNumberGenerator &rand) {
		
	typename CodSeq::value_type lower,upper;
	
	/* find upper and lower bounds of the spanning space */
	get_min(data.begin(),data.end(),lower);
	get_max(data.begin(),data.end(),upper);
			
	cod.resize(k);
	random_matrix_lu(cod,lower,upper,rand);
}

/*! Takes randomly a subset of k vectors from the data to initialize the codebook set.
   
*/
template <class DataSeq, class CodSeq, class RandomNumberGenerator >
void init_random(const DataSeq &data, CodSeq &cod, unsigned int k, RandomNumberGenerator &rand) {
	
	/* take a subset of n vectors out of the data */
	cod.resize(k);
	random_sample_n(data.begin(),data.end(),cod.begin(),k,rand);
}

/*! Takes randomly a subset of k vectors from the data to initialize the codebook set.
   
*/
template <class DataSeq, class DataNom, class CodSeq, class CodNom, class RandomNumberGenerator >
void init_random_nominal(const DataSeq &data, const DataNom &datanominal, CodSeq &cod, CodNom &codnominal, 
		unsigned int k, RandomNumberGenerator &rand) {
	
	/* take a subset of n vectors out of the data */
	cod.resize(k);
	codnominal.resize(k);
	
	/* create a permutation */
	random_sample2_n(	data.begin(),data.end(),
						datanominal.begin(),datanominal.end(),
						cod.begin(),codnominal.begin(),
						k,rand );
}

/*! Max-min algorithm to initialize codebook vectors.

	See
	<PRE>
	A. Juan and E. Vidal,
	Comparison of Four Initialization Techniques for the K-Medians Clustering Algorithm,
	"Advances in Pattern Recognition", 
	F. J. Ferri and J. M. Inesta and A. Amin and P. Pudil (editors),
	LNCS 1876, pp. 842-852,	Springer-Verlag, Heidelberg 2000
	</PRE>
*/
template <class DataSeq, class CodSeq, class RandomNumberGenerator, class Metric>
void init_maxmin(const DataSeq &data, CodSeq &cod, int k, RandomNumberGenerator &rand, Metric dist) {

	int n = data.size();
	
	vector<bool> marked(n,false);			/* set to true if a vector is taken into the codebook */
	vector<typename Metric::result_type> 
		D(n, numeric_limits<typename Metric::result_type>::max() ); /* distance array D[p] is the minimum distance of p to
															the vectors of the current codebook */
	
	int i,q,p,q_new;

	typename Metric::result_type dpq, maxmin;
	
	cod.resize(k);
	
	/* choose the first vector arbitrary */
	q = rand(n);
	q_new = 0;
	
	for(i=0; i < k; i++) {
		marked[q]	= true;
		cod[i] 		= data[q];
		
		/* choose the vector which is farest to all vectors in the codebook */
		maxmin = 0;
		for(p=0; p < n; p++) {
			if(marked[p])
				continue;
				
			dpq = dist(data[q],data[p]);
			
			if(dpq < D[p])
				D[p] = dpq;
				
			if(D[p] > maxmin) {
				maxmin = D[p];
				q_new = p;
			}
		}
		q = q_new;
	}
}

template <class DataSeq, class DataNom, class CodSeq, class CodNom, class RandomNumberGenerator, class Metric>
void init_maxmin_nominal(const DataSeq &data, const DataNom &datanom, CodSeq &cod, CodNom &codnom, 
		int k, RandomNumberGenerator &rand, Metric dist) {

	int n = data.size();
	
	vector<bool> marked(n,false);			/* set to true if a vector is taken into the codebook */
	vector<typename Metric::result_type> 
		D(n, numeric_limits<typename Metric::result_type>::max() ); /* distance array D[p] is the minimum distance of p to
															the vectors of the current codebook */

	int i,q,p,q_new;
	typename Metric::result_type dpq, maxmin;
	
	cod.resize(k);
	codnom.resize(k);
	
	/* choose the first vector arbitrary */
	q = rand(n);
	q_new = 0;
	
	for(i=0; i < k; i++) {
	
		marked[q]	= true;
		cod[i] 		= data[q];
		codnom[i]	= datanom[q];
		
		/* choose the vector which is farest to all (other unused) vectors in the codebook */
		maxmin = 0;
		for(p=0; p < n; p++) {
			if(marked[p])
				continue;
				
			dpq = dist(data[q],data[p]);
			
			if(dpq < D[p])
				D[p] = dpq;
				
			if(D[p] > maxmin) {
				maxmin = D[p];
				q_new = p;
			}
		}
		q = q_new;
	}
}

/*!	Initialize with K nearest neighbours.
*/
template <class RandomAccessIterator1, class RandomAccessIterator2, class CodSeq, class CodNom, class RandomNumberGenerator, class Metric>
void init_knn( // const DataSeq &data, const DataNom &datanom,
			RandomAccessIterator1 data_first,
			RandomAccessIterator1 data_last,
			RandomAccessIterator2 nominal_first,
			RandomAccessIterator2 nominal_last,
		    CodSeq &cod, CodNom &codnom,
			unsigned int k, unsigned int knn,
			RandomNumberGenerator &rand, Metric dist ) throw(length_error) {

	typedef typename Metric::result_type Float;
	typedef typename iterator_traits< RandomAccessIterator2 >::value_type ClassType;

	unsigned int m = data_last - data_first;

	// data.size() != datanom.size()
	if(distance(data_first,data_last) != distance(nominal_first,nominal_last) )
		throw length_error("init_knn data size doesn't match nominal vector size");

	cod.clear();
	codnom.clear();

	if(k > m)
		k = m;

	unsigned int maxn, j, i = 0;
	ClassType omega;

	enum MARK { FREE , NOMATCH , USED };

	vector<unsigned int>	marker(m,FREE);

	maxn = *max_element(nominal_first, nominal_last) + 1;

	typedef vector< pair<Float, unsigned int> > SeqVector;

	SeqVector	seq;
	typename SeqVector::const_iterator iter;
	vector< pair<unsigned int, ClassType > > classes( maxn );
	vector< unsigned int > 		perm(m);

	iota(perm.begin(),perm.end(),0);
	random_shuffle(perm.begin(),perm.end(),rand);

	omega = 0;

	while(i<k && m>0) {
		// choose a datapoint
		j = 0;
		while( j<m && ( marker[perm[j]]!=FREE  || *(nominal_first+perm[j])!=omega) ) ++j;
		if(j >= m)
			break;

		j = perm[j];

		// check if this datapoint fulfills the knn
		find_knearest(data_first,data_last, *(data_first+j), dist, knn, seq );

		for(int idx=0; idx<(int)classes.size(); ++idx) {
			classes[idx].first = 0;
			classes[idx].second = idx;
		}

		for(iter = seq.begin(); iter != seq.end(); ++iter)
			++classes[*(nominal_first+(*iter).second)].first;

		sort(classes.begin(),classes.end(),greater< pair<unsigned int,ClassType> > ());

		// is the class of the winner the correct one
		if( classes[0].second == *(nominal_first+j) ) {
			// take this vector
			cod.push_back( *(data_first+j) );
			codnom.push_back( *(nominal_first+j) );
			marker[j] = USED;

			omega = (omega+1) % maxn;
			++i;
		} else {
			marker[j] = NOMATCH;
		}
		--m;
	}

	// fill up with datapoints NOT fulfilling the knn
	while(i<k) {
		// choose a datapoint
		j = 0;
		while( j<m && marker[perm[j]] == USED ) ++j;
		if(j >= m)
			break;
			
		j = perm[j];
		
		cod.push_back( *(data_first+j) );
		codnom.push_back( *(nominal_first+j) );
		marker[j] = USED;
		omega = (omega+1) % maxn;	
		++i;		

	}
}

/*!	Takes the same number of codebook vectors for each class.
*/
template < class RandomAccessIterator1, class RandomAccessIterator2, class CodSeq, class CodNom, class RandomNumberGenerator >
void init_even_nominal(
			RandomAccessIterator1 data_first, 
			RandomAccessIterator1 data_last,
			RandomAccessIterator2 nominal_first, 
			RandomAccessIterator2 nominal_last,
			CodSeq &cod, CodNom &codnom, 
			unsigned int k, RandomNumberGenerator &rand ) throw(length_error) {

	RandomAccessIterator1 dat_iter;
	RandomAccessIterator2 nom_iter;

	unsigned int nclasses = 0;
	unsigned int i, sum, n = data_last - data_first;

	if( k > n )
		k = n;

	cod.resize( k );
	codnom.resize( k );
	
	
	vector<unsigned int> remaining, m;
	
	nom_iter = nominal_first;
	while( nom_iter != nominal_last ) {
	
		if( *nom_iter >= nclasses ) {
			nclasses = (*nom_iter) + 1;
			remaining.resize(nclasses,0);
			m.resize(nclasses,0);
		}
		remaining[ *nom_iter ]++;
		++nom_iter;
	}
	
	// try even distribution for the classes
	sum = 0;
	for( i = 0; i < nclasses; ++i ) {
		m[i] = min( k/nclasses , remaining[i] );
		sum += m[i];
	}
	
	// fill up remaining vectors
	i = 0;
	while( sum < k ) {
		if( m[i] < remaining[i] ) {
			++m[i];
			++sum;
		}
		i = ( i + 1 ) % nclasses;
	}
		
		
	
	nom_iter = nominal_first;
	dat_iter = data_first;
	i = 0;
	while( k > 0 ) {
		if( remaining[ *nom_iter ] > 0 && rand( remaining[ *nom_iter ] ) < m[ *nom_iter ] ) {
			cod[i]  = (*dat_iter);
			codnom[i] = *nom_iter;
			++i;
			--k;
			--m[ *nom_iter ];
		}
		--remaining[ *nom_iter ];
		++nom_iter;
		++dat_iter;
	}
}


/*! Resampling
   
*/
template <class DataSeq, class CodSeq, class RandomNumberGenerator >
void init_resample(const DataSeq &data, CodSeq &cod, unsigned int k, RandomNumberGenerator &rand) {
	
	unsigned int 	i,
					n = data.size();
	
	cod.resize( k );
	
	for( i = 0; i < k; ++i )
		cod[i] = data[ rand(n) ];
}

/*! Resampling
   
*/
template <class DataSeq, class DataNom, class CodSeq, class CodNom, class RandomNumberGenerator >
void init_resample_nominal(const DataSeq &data, const DataNom &datanominal, CodSeq &cod, CodNom &codnominal, 
		unsigned int k, RandomNumberGenerator &rand) {

	unsigned int i, idx, n;
	
	_AssertParam( data.size() == datanominal.size(), "init_resample_nominal()" );
	
	/* take a subset of n vectors out of the data */
	cod.resize(k);
	codnominal.resize(k);
	
	n = data.size();
	
	for( i = 0; i < k; ++i ) {
	
		idx = rand(n);
		
		cod[i] 			= data[idx];
		codnominal[i] 	= datanominal[idx];
	}
}

/*! Resampling no double vectors
   
*/
template <class DataSeq, class CodSeq, class RandomNumberGenerator >
void init_resamplend(const DataSeq &data, CodSeq &cod, unsigned int k, RandomNumberGenerator &rand) {
	
	unsigned int i, idx, n, remaining;
	vector< bool > checked( data.size(), false );
				
	cod.clear();
	
	n 			= data.size();
	remaining	= n;
	
	for( i = 0; i < k; ++i ) {
	
		idx = rand(n);
		if( ! checked[idx] ) {
		
			checked[idx] = true;
			cod.push_back( data[idx] );
			
			--remaining;
			if( remaining == 0 )
				return;
		}
	}
}

/*! Resampling no double vectors
   
*/
template <class DataSeq, class DataNom, class CodSeq, class CodNom, class RandomNumberGenerator >
void init_resamplend_nominal(const DataSeq &data, const DataNom &datanominal, CodSeq &cod, CodNom &codnominal, 
		unsigned int k, RandomNumberGenerator &rand) {

	unsigned int i, idx, n, remaining;
	vector< bool > checked( data.size(), false );
	
	_AssertParam( data.size() == datanominal.size(), "init_resamplend_nominal()");
	
	cod.clear();
	codnominal.clear();
	
	n 			= data.size();
	remaining 	= data.size();
	
	for( i = 0; i < k; ++i ) {
	
		idx = rand(n);
		
		if( ! checked[idx] ) {
		
			checked[idx] = true;
			cod.push_back( data[idx] );
			codnominal.push_back( datanominal[idx] );
			
			--remaining;
			if( remaining == 0 )
				return;			
		}
	}
}


#endif

