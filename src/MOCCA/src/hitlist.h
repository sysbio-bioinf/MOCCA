/*
 * hitlist.h
 *
 * written by Andre Mueller
 *
 * Implementation of a generic hit list
 *
 * $Id: hitlist.h,v 1.1 2001/05/18 14:36:14 andre Exp andre $
 *
 * $Log: hitlist.h,v $
 * Revision 1.1  2001/05/18 14:36:14  andre
 * Initial revision
 *
 *
 */

/*! \file hitlist.h
\brief Create nearest neighbour hitlists.


\sa metric.h, LNorm
*/

/*
	CHANGES:

	2003/03/18	A.M. removed "using namespace std"
					some reformatings

	2003/03/14	A.M. new functions
				create_sample_list
				clusters_to_hits
*/

#ifndef __HITLIST_H__
#define __HITLIST_H__

#include <iostream>
#include <iterator>
#include <algorithm>		/* set_difference */
#include <set>
#include <vector>
#include <queue>
#include <utility>

#include "metric.h"
#include "utils.h"
#include "set_util.h"


typedef unsigned int 			Index;		/*!< type of a cluster or sample index	*/
typedef std::vector<Index> 		HitList;	/*!< A hit list is a sequence (or set) of sample indices for a single cluster */
typedef std::vector<HitList> 	Clusters;	/*!< a complete clustering consists of a hit list for each cluster 	*/
typedef std::vector<Index>		Hits;		/*!< cluster number for each sample									*/

/*! Find the nearest codebook vector.
	Find in the Codebook [\p first,l\p ast) the vector which is nearest to \p vector respective to the given
	Metric \p metric. It returns the iterator value where the metric is minimal.
*/
template <class ForwardIterator, class Vector, class Metric>
ForwardIterator find_nearest(ForwardIterator first, ForwardIterator last, const Vector &vector, Metric metric,
		typename Metric::result_type *pdist = NULL)
{

	typename Vector::value_type opt_dist,
								dist;
	ForwardIterator 			opt;

	opt = first;
	opt_dist = 0.0;

	while(first!=last)
	{
		dist = metric(*first, vector);
		if(dist<opt_dist || opt==first)
		{
			opt = first;
			opt_dist = dist;
		}
		++first;
	}
	if(pdist)
	{
		(*pdist) = opt_dist;
	}

	return opt;
}

/*! Find the \p k nearest codebook vectors.
	Find in the codebook [\p first,\p last) the \p knn vectors which are nearest to \p vector respective to the given
	Metric \p metric. It returns the iterators in the container \p seq.
*/
template <class ForwardIterator, class Vector, class Metric, class Sequence>
void find_knearest( ForwardIterator first, ForwardIterator last,
					const Vector &vector, Metric metric,
					int knn, Sequence &seq )
{

	typedef typename Vector::value_type Float;
	typedef typename Sequence::value_type Tuple;

	// TODO : the priority_queue should have a maximum size of knn ...

	std::priority_queue< Tuple , std::vector<Tuple> , std::greater<Tuple> > pq;

	Float dist;
	int i = 0;

	while(first!=last) 
	{
		dist = metric(*first, vector);
		pq.push( Tuple(dist, i ) );

		++i;
		++first;
	}

	// fill idx array with results
	seq.clear();

	while( !pq.empty() && knn>0 ) 
	{
		seq.push_back( pq.top() );
		pq.pop();

		--knn;
	}
}


/*! Creates a hitlist of given samples a given metric and a given codebook.
	Builds the hit list \p clusters given the data points [\ data_first,\v data_last) and
	the codebook vectors [cod_first,cod_last).
	Each element in \p clusters corresponds to a vector in the codebook and contains
	a list of indices of the data points hitting the respective cluster center.

*/
template <class DataIterator, class CodIterator, class Metric>
void create_hit_list( DataIterator data_first, DataIterator data_last,
					  CodIterator cod_first, CodIterator cod_last,
					  Clusters &clusters, Metric metric )
{

	CodIterator hit;

	clusters.clear();
	std::fill_n( std::back_inserter(clusters) , cod_last-cod_first , HitList() );

	HitList::value_type idx = 0;

	while( data_first != data_last )
	{
		hit = find_nearest(cod_first, cod_last, *data_first, metric);
		//clusters[ (hit - cod_first) ].insert( idx );
		clusters[ (hit - cod_first) ].push_back( idx );
		++idx;
		++data_first;
	}
}


/*! Updates a hitlist and evaluates the number of changing members.
	Builds the hitlist \p clusters given the data points [\p data_first,\p data_last) and
	the codebook vectors [\p cod_first,\p cod_last).
	<p>
	Each element in \p clusters corresponds to a vector in the codebook and contains
	a list of indices of the data points hitting the respective cluster center.
	It returns the number of changes
*/
template <class DataIterator, class CodIterator, class Metric>
int update_hit_list( DataIterator data_first, DataIterator data_last,
		CodIterator cod_first, CodIterator cod_last,
		Clusters &clusters, Metric metric ) 
{

	int changed = 0;
	Clusters tmp;
	Clusters::size_type n;

	n = std::distance(cod_first,cod_last);

	if(clusters.size() != n) 
	{
		clusters.clear();
		std::fill_n( std::back_inserter(clusters) , n , HitList() );
	}

	/**/
	create_hit_list(data_first, data_last, cod_first, cod_last, tmp, metric);

	/* count changes */
	Clusters::iterator iter,tmpiter;
	for(iter=clusters.begin(), tmpiter=tmp.begin();
			iter!=clusters.end() && tmpiter!=tmp.end(); ++iter, ++tmpiter) 
	{

		/* build symmetric difference ( C[new] - C[old] ) u ( C[old] - C[new] ) */
		changed += count_symmetric_difference((*iter).begin(),(*iter).end(), (*tmpiter).begin(),(*tmpiter).end());

		/* swap contents of both hitlists (the tmp lists will be destroyed at the end of the subroutine) */
		(*iter).swap(*tmpiter);
	}

	return changed;
}

/*! Give out a hitlist on a stream.
*/
void print_clusters(const Clusters &clusters, std::ostream &out = std::cout);

/*! Gives the nearest cluster center for each sample

*/
template <class DataIterator, class CodIterator, class Metric>
void create_sample_list( DataIterator data_first, DataIterator data_last,
					  CodIterator cod_first, CodIterator cod_last,
					  Metric metric, Hits hits )
{

	Hits::size_type		idx;

	hits = Hits( data_last - data_first );

	while( data_first != data_last )
	{
		hits[idx] = find_nearest(cod_first, cod_last, *data_first, metric) - cod_first;
		++idx;
		++data_first;
	}
}

/*! Creates a sample list from a hit list
*/
void clusters_to_hits( const Clusters & c, Hits & h, unsigned int n );

#endif
