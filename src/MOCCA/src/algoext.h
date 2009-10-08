/*
 * algoext.h
 *
 * some extensions of the <algorithm> of the STL
 *
 * $Id: algoext.h,v 1.1 2002/05/02 11:12:14 andre Exp andre $
 *
 * $Log: algoext.h,v $
 * Revision 1.1  2002/05/02 11:12:14  andre
 * Initial revision
 *
 *
 */

/*
	CHANGES

	2003/03/31	A.Mueller reformatings, assertions
*/

/*!	\file algoext.h
	\brief Extensions of the STL algorithm's

*/
#ifndef __ALGOEXT_H__
#define __ALGOEXT_H__

#include <algorithm>
#include <iterator>
#include <utility>
#include <vector>
#include <numeric>

#include <ext/numeric>		// iota

#ifdef __GNUC__
using namespace __gnu_cxx;
#endif

#include "error_handler.h"

/*!
	Shuffles the two arrays [first1,last1) and [first2,last2) parallel.
	The two arrays have to be of the same size.
	The same as random_shuffle of the STL.
*/
template < class RandomAccessIter1, class RandomAccessIter2, class RandomNumberGenerator >
void random_shuffle2( RandomAccessIter1 first1, RandomAccessIter1 last1,
					  RandomAccessIter2 first2, RandomAccessIter2 last2, RandomNumberGenerator &rand )
{

	_AssertParam( std::distance(first1,last1) == std::distance(first2,last2), "random_shuffle2" );

	RandomAccessIter1 i1 = first1;
	RandomAccessIter2 i2 = first2;

	while( i1 != last1 )
	{
		int j = rand( last1-i1 );

		swap(*i1, *(i1 + j) );
		swap(*i2, *(i2 + j) );

		++i1;
		++i2;
	}
}

/*!
	Implements the same functionality as random_sample_n of the STL only with pairs
	of arrays.
*/
template < class ForwardIterator1, class ForwardIterator2,
		   class OutputIterator1, class OutputIterator2, class Distance,
           class RandomNumberGenerator >
pair<OutputIterator1,OutputIterator2>
random_sample2_n(
		ForwardIterator1 first1, ForwardIterator1 last1,
		ForwardIterator2 first2, ForwardIterator2 last2,
        OutputIterator1 out1, OutputIterator2 out2,
		Distance n, RandomNumberGenerator& rand )
{

	Distance remaining;

	remaining = min( distance(first1, last1) , distance(first2, last2));
	Distance m = min(n, remaining);

	while (m > 0)
	{
		if ((Distance)rand(remaining) < m)
		{
			*out1 = *first1;
			*out2 = *first2;
			++out1;
			++out2;
			--m;
		}

		--remaining;
		++first1;
		++first2;
	}
	return make_pair(out1,out2);
}


/*!
	Shuffles the vector [first,last) with a permutation P so that each index
	i is mapped to i + P[i] and vice versa <B>in place</B>.

	<PRE>
	0      P[0]
	1      P[1]
	:      :
	n-1    P[n-1]
	</PRE>
*/
template < class RandomAccessIter, class Permutation >
void permutate( RandomAccessIter first, RandomAccessIter last, const Permutation & P) 
{
	typedef typename Permutation::size_type size_type;

	std::vector<int> val(P.size()), idx(P.size());

	iota(val.begin(),val.end(),0);
	iota(idx.begin(),idx.end(),0);

	/*	The array val should contain the permutation P after all swap steps
		The array idx remembers the indices of the values and fulfills the invariant

		val[ idx[j] ] = j		for all j = 0..n-1

		idx and val are inverse functions respective

	*/


	if( first==last )
		return;

	size_type 	size = std::distance(first,last);

	_AssertParam( size == P.size(), "permutate" );

	size_type	i, j;
	for( i = 0; i < size; i++ )
	{
		// i  <-> idx[ P[i] ]
		j = idx[ P[i] ];

		swap(val[i],val[j]);
		swap(idx[ val[i] ], idx[ val[j] ]);

		swap(*(first+i),*(first+j));
	}
}

/*!
	Partitions the elements E = { first , last } randomly into C[] = { ofirst , olast } categories so
	that the following invariants holds:

	<OL>

	  <LI> Every category C[i] has minimum nmin elements ( i = 0 .. K - 1 )</LI>
	  <LI>The unification of C[] is the original set E<BR>
	        C[0] + C[1] + ... + C[K-1] = E </LI>
	  <LI>The categories are disjoint<BR>
	        C[i] ~ C[j] = 0  for all i <> j</LI>
	</OL>

	The OutputIterators ofirst, olast must point to a container which supports the insert operation.
*/
template< class InputIterator , class OutputIterator , class RandomNumberGenerator >
void random_partition(  InputIterator first, InputIterator last,
						OutputIterator ofirst, OutputIterator olast, 
						int nmin, RandomNumberGenerator & rand ) 
{

	typedef typename iterator_traits< InputIterator >::difference_type Distance;

	Distance n = std::distance( first, last );			// number of elements
	Distance K = std::distance( ofirst, olast );		// number of categories

	if( n <= K * nmin || K <= 0 )
		return ;						// not enough elements for partitioning

	int					sum = 0, i, t;
	std::vector<int>	count(K,0);

	// fill count array with number of elements to be in class i
	for( i = 0; i < K-1; ++i ) 
	{
		t = n - sum - ( K - i ) * nmin;

		if( t > 0 )
			count[i] = rand( t ) + nmin;
		else
			count[i] = nmin;

		sum += count[i];
	}
	count[ K-1 ] = n - sum; // the rest goes to the last category

	while( first != last ) 
	{
		do 
		{
			i = rand( K );
		} 
		while( count[i] == 0 );

		( ofirst + i )->insert( *first );

		--count[i];

		++first;
	}

}

#endif

