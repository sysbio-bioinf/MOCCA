/*
 * cluster_validation.h
 *
 * written by Andre Mueller
 *
 * Implements some indices of cluster validity like Figure Of Merit, Maximum Cluster Assignment, ....
 *
 * CHANGES:
 *   2003/02/12  documentation
 *
 *   2003/02/13  compare_trans() now normalized
 *
 * $Id: cluster_validation.h,v 1.1 2001/05/22 14:33:01 andre Exp andre $
 *
 * $Log: cluster_validation.h,v $
 * Revision 1.1  2001/05/22 14:33:01  andre
 * Initial revision
 *
 *
 */

/*!	\file cluster_validation.h
	\brief Implements functions for validating the quality of a clustering.


	In the current version there are only relative clustering validation criteria represented.

	\sa hitlist.h, create_hit_list()

	<H3>References</H3>

	Jain and Dubes
*/

/*
	CHANGES:

	2003/03/11	AM		Symmetric Difference implemented
*/

#ifndef __CLUSTER_VALIDATION_H__
#define __CLUSTER_VALIDATION_H__

#include <iterator>
#include <cmath>

#include "def.h"
#include "set_util.h"
#include "hitlist.h"
#include "lap.h"
#include "matrixutil.h"
#include "statistics.h"

/********************************************************************
 *																	*
 *	 					Utility Functions							*
 *																	*
 ********************************************************************/

/*! Figure Of Merit

	Compares two clusterings with <em>figure of merit</em> that is the number
	of pairs which are in the same cluster in both clusterings divided by the square of the
	number of datapoints <var>n</var>.
*/
Float compare_fom(const Clusters &c1, const Clusters &c2, unsigned int n);

/*!	Maximum Cluster Assignment.

	Compares two partitions with  maximum cluster assignment (MCA).
	The returned index ist a floating point value in the interval (0,1]
	A value of 1 means that the two clusterings c1 and c2 are identical
	(c2 is a permutation of c1).
*/
Float compare_mca(const Clusters &c1, const Clusters &c2, unsigned int n);

/*!	Maximum Cluster Assignment with the LAP row and column solutions.

	The same as
	Float compare_mca(const Clusters &c1, const Clusters &c2, unsigned int n);
	but now returns rowsol and colsol from the linear assignment (LAP).
*/
Float compare_mca(const Clusters &c1, const Clusters &c2, unsigned int n, vector<int> & rowsol, vector<int> & colsol);

/*! Transinformation between two clusterings.

	Let \f$ {\cal A} \f$ and \f$ {\cal B} \f$ be paritions of \f$ {\cal X} \f$.
	Observe the random variables \f$ X, Y \f$ uniformly independently distributed on the sample set \f$ {\cal X} \f$.
	\f[
		X : {\cal X} \mapsto \{ 1 \ldots |{\cal A}| \}
	\f]
	and
	\f[
		Y : {\cal X} \mapsto \{ 1 \ldots |{\cal B}| \} \quad .
	\f]
	The two random variables map each datapoint to its cluster index.

	The transinformation defined as
	\f[
		I(X;Y) =
		\sum_{i=1}^{|{\cal A}|} \sum_{j=1}^{|{\cal B}|}
		P(X=i, Y=j) \log \frac{P(X=i,Y=j)}{P(X=i) \cdot P(Y=j)}
	\f]
	computes with the given distribution to
	\f[
			I({\cal A};{\cal B}) =
			\sum_{i=1}^{|{\cal A}|} \sum_{j=1}^{|{\cal B}|}
			\frac{|A_i\cap B_j|}{|{\cal X}|}
			\log
			\left(
				|{\cal X}| \frac{|A_i\cap B_j|}{|A_i| \cdot |B_j|}
			\right) \quad .
	\f]
	Now we normalize according to the selfinformation of the two partitions:
	\f[
		NMI({\cal A};{\cal B}) = \frac{I({\cal A};{\cal B})}{\sqrt{H({\cal A}) H({\cal B})}}
	\f]
	If the two partitions are identically we get \f$ NMI({\cal A},{\cal A}) = 1 \f$.

*/
Float compare_trans(const Clusters &c1, const Clusters &c2, unsigned int n);

/*! Selfinformation of a clustering

	Let \f$ X \f$ be a random variable uniformly distributed over the datapoints \f$ {\cal X} \f$.
	We observe now the events that \f$ X \f$ is in \f$ A_i \f$, \f$ {\cal A} \f$ a partition of \f$ {\cal X} \f$.
	The entropy is now
	\f[
		H({\cal A}) = -\sum_i \frac{|A_i|}{|{\cal X}|} \log \frac{|A_i|}{|{\cal X}|}
	\f]

*/
Float self_information( const Clusters &c, unsigned int n );

/*!
	Compare with symmetric difference.
	<br>
	See:
	A polynomial time computable metric between point sets, J. Ramon and M. Bruynooghe, Acta Informatica 37, 765--780 (2001)
*/
Float compare_symmdiff( const Clusters &c1, const Clusters &c2 );


/*! Calculates the connectivity matrix D of a clustering.

  \f[

  	D(i,j) =
		\left\{
		\begin{array}{ll}
			1  & \exists k \mbox{ so that } \{i,j\} \in A_k \\
			0  & \mbox{otherwise}
		\end{array}
		\right.
  \f]

*/
template <class Matrix>
void connectivity_matrix( const Clusters &c, Matrix &D )
{
	Clusters::const_iterator i;
	HitList::const_iterator j1,j2;

	fill_matrix(D,false);

	for(i=c.begin(); i!=c.end(); ++i)
	{
		for(j1=(*i).begin(); j1!=(*i).end(); ++j1)
		{
			for(j2=(*i).begin(); j2!=(*i).end(); ++j2)
			{
				D[*j1][*j2] = true;
			}
		}
	}
}


/*!	Creates a 2x2 contingency table for the two clusterings c1 and c2.

<table>
	<th><td colspan="3">c2</td></th>
	<tr><td>&nbsp;</td><td>1</td><td>0</td></tr>
	<tr><td>1</td><td>a</td><td>b</td></tr>
	<tr><td>0</td><td>c</td><td>d</td></tr>
</table>

<ol>

	<li>Jaccard Index
		\f[
			J = \frac{a}{a + b + c}
		\f]
	</li>
	<li> Rand Index<BR>
		\f[
			R = \frac{a + d}{ {n \choose 2} }
		\f]
	</li>
</ol>

*/
void contingency_table(const Clusters &c1, const Clusters &c2, unsigned int n, long &a, long &b, long &c, long &d );


/********************************************************************
 *																	*
 *	 					Relative Criteria							*
 *																	*
 ********************************************************************/


/*!
	Compares the two clusterings c1 with c2 with the intersection set.
	The target values of all combinations c1[i] | c2[j] are stored to the matrix KxK matrix a

<table>
	<tr> <td>&nbsp;</td> <td>c2[0]</td>  <td>c2[1]</td>  <td>...</td>    <td>c2[n-1]</td> </tr>
	<tr> <td>c1[0]</td>  <td>&nbsp;</td> <td>&nbsp;</td> <td>&nbsp;</td> <td>&nbsp;</td>  </tr>
	<tr> <td>c1[1]</td>  <td>&nbsp;</td> <td>&nbsp;</td> <td>&nbsp;</td> <td>&nbsp;</td>  </tr>
	<tr> <td>:</td>		 <td>&nbsp;</td> <td>&nbsp;</td> <td>&nbsp;</td> <td>&nbsp;</td>  </tr>
	<tr> <td>c1[n-1]</td><td>&nbsp;</td> <td>&nbsp;</td> <td>&nbsp;</td> <td>&nbsp;</td>  </tr>
</table>

	a[i][j] is the number of members in the intersection set of c1[i] and c2[j]

*/
template <class Matrix>
void compare_clusters(const Clusters &c1,const Clusters &c2, Matrix &a)
{
	typename Matrix::size_type i ,j;

	a = Matrix( c1.size(), typename Matrix::value_type(c2.size(),0) );

	for( i = 0; i < c1.size(); ++i )
	{
		for( j = 0; j < c2.size(); ++j )
		{
			// count size of the intersection set of the two clusters
			a[i][j] = count_intersection( c1[i].begin(),c1[i].end(),c2[j].begin(),c2[j].end());
		}
	}
}

/*!
	Compares two clusterings c1, c2 with the symmetric difference:
	\f[
		\Delta(A,B) =
			\left\{
				\begin{array}{ll}
 					\frac{ | A \Delta B | }{ | A \cup B | } & |A \cup B| > 0 \\
					0 & \mbox{otherwise}
				\end{array}
			\right.
	\f]
	with
	\f[
		A \Delta B = ( A \cup B ) - ( A \cap B )
	\f]
*/
template < class Matrix >
void symmdiff(const Clusters &c1,const Clusters &c2, Matrix &a)
{
	typedef typename Matrix::value_type			Vector;
	typedef typename Vector::value_type			T;

	typename Matrix::size_type 					i ,j;
	long 										A, B;

	a = Matrix( c1.size(), Vector(c2.size(),0) );

	for( i = 0; i < c1.size(); ++i )
	{
		for( j = 0; j < c2.size(); ++j )
		{
			// count size of the intersection set of the two clusters
			A	= count_intersection( c1[i].begin(),c1[i].end(),c2[j].begin(),c2[j].end() );
			B	= count_union( c1[i].begin(),c1[i].end(),c2[j].begin(),c2[j].end() );
			if( B > 0 )
			{
				a[i][j] = static_cast<T>(B-A)/static_cast<T>(B);
			}
			else
			{
				a[i][j] = 0;
			}

		}
	}
}

/*
	Clustercenter measures
*/

/*! Reproducability Index (cross quantization error)
	Fritz Sommer, Axel Baune - V(i,j) Index

	\param c1	Hitlists for the first clustering
	\param c2   Hitlists for the second clustering

	Be \f$ \cal X \f$ the data set and \f$\omega_i \f$ the mapping from a data point to
	a cluster center and \f$ d \f$ a distance function.
	To compare now the two clusterings ${\cal C}_i$ and ${\cal C}_j$ we calculate the
	<em>cross-distortion</em> over all data points.
	\f[
		V_{ij} = \frac{1}{|{\cal X}|}
				\sum_{{\bf x}\in {\cal X}}
				d( \omega_i({\bf x}), \omega_j({\bf x}) )
	\f]

*/
template< class Matrix, class Metric >
Float compare_crossqerr( const Hits & c1,
						 const Hits & c2,
						 const Matrix & cod1,
						 const Matrix & cod2,
						 Metric metric )
{
	Hits::size_type 	i;
	Float 				sum;

	_AssertParam( c1.size() == c2.size(), "compare_crossqerr" );

	sum = 0.0;

	for( i = 0; i < c1.size(); ++i )
	{
		sum += metric( cod1[ c1[i] ], cod2[ c2[i] ] );
	}

	return sum/static_cast<Float>( c1.size() );
}





/********************************************************************
 *																	*
 *	 					Inner Criteria								*
 *																	*
 ********************************************************************/

/*! Intra cluster mean correlation.
	The mean over all correlations in a cluster.

	\param c 		cluster hit lists
	\param data 	sample vectors

	\return The correlation value

*/
template< class Matrix >
Float intra_cluster_corr( const Clusters & c, const Matrix & data )
{
	Clusters::size_type 	j, u, v, n;
	Float 					sum, subsum;

	sum = 0.0;

	// iterate over clusters
	for( j = 0; j < c.size(); ++j )
	{
		n = c[j].size();

		if( n > 1 )
		{
			// iterate over the samples
			subsum = 0.0;
			for( u = 0; u < n - 1; ++u )
			{
				for( v = u + 1; v < n; ++v )
				{
					subsum += std::abs( Stat::corr( data[ c[j][u] ].begin(), data[ c[j][u] ].end(),
													data[ c[j][v] ].begin(), data[ c[j][v] ].end() )
									);
				}
			}
			subsum	*= 2.0 / ((Float)n*(Float)(n-1));
		}
		else
		{
			subsum = 1.0;	// if there is only one datapoint in a cluster we define a correlation of 1.0
		}
		sum		+= subsum;
	}


	return sum/static_cast<Float>( c.size() );
}

/*! Inter cluster mean correlation.
	The mean over all correlations between all cluster pairings.

	\param c 		cluster hit lists
	\param data 	sample vectors

	\return The correlation value
*/
template< class Matrix >
Float inter_cluster_corr( const Clusters & c, const Matrix & data )
{
	Clusters::size_type 	i, j, u, v;
	Float 					sum, subsum;

	sum = 0.0;

	// iterate over clusters
	for( i = 0; i < c.size()-1; ++i )
	{
		for( j = i+1; j < c.size(); ++j )
		{
			// iterate over the samples
			subsum = 0.0;
			for( u = 0; u < c[i].size(); ++u )
			{
				for( v = 0; v < c[j].size(); ++v )
				{
					subsum += std::abs( Stat::corr( data[ c[i][u] ].begin(), data[ c[i][u] ].end(),
													data[ c[j][v] ].begin(), data[ c[j][v] ].end() ) );
				}
			}
			subsum /= (Float)(c[i].size())*(Float)(c[j].size());
			sum += subsum;
		}
	}

	return sum*2.0/( (Float)(c.size()) * (Float)(c.size()-1));
}

#endif
