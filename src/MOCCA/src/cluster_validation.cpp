/*
 * cluster_validation.cpp
 *
 * written by Andre Mueller
 *
 * Implements some indices of cluster validity like Figure Of Merit, Maximum Cluster Assignment, ....
 *
 * $Id: cluster_validation.cpp,v 1.1 2001/08/29 18:40:30 andre Exp andre $
 *
 * $Log: cluster_validation.cpp,v $
 * Revision 1.1  2001/08/29 18:40:30  andre
 * Initial revision
 *
 *
 */

/*
	CHANGES:

	2003/03/12	Bug in compare_symmdiff fixed.
*/

#include <cmath>

#include "cluster_validation.h"
#include "matrixutil.h"


#include "matrixio.h"
#include <iostream>

//
// Figure of merit
//
Float compare_fom( const Clusters &c1, const Clusters &c2, unsigned int n )
{
	unsigned int i,j;
	vector< vector<int> > D1,D2;

	dim_matrix(D1,n,n);
	dim_matrix(D2,n,n);

	long sum;

	// calculate the connectivity matrices of each clustering
	connectivity_matrix(c1,D1);
	connectivity_matrix(c2,D2);

	// calculate the score between the clusterings
	sum = 0;

	for( i = 0; i < n; i++ )
	{
		for( j = 0; j < n; j++ )
		{
			if( D1[i][j] != D2[i][j] )
			{
				sum++;
			}
		}
	}

	return (Float)sum/(Float)(n*n);
}

void contingency_table(const Clusters &c1, const Clusters &c2, unsigned int n, long &a, long &b, long &c, long &d)
{
/*
	unsigned int i,j;
	vector< vector<int> > D1,D2;
	vector< vector<long> > D;

	dim_matrix(D1,n,n);
	dim_matrix(D2,n,n);
	dim_matrix(D,2,2);

	fill_matrix(D,0);


	connectivity_matrix(c1,D1);
	connectivity_matrix(c2,D2);

	for(i=0; i<n; i++)
		for(j=0; j<n; j++)
			++D[ D1[i][j] ][ D2[i][j] ];

	a = D[1][1];
	b = D[1][0];
	c = D[0][1];
	d = D[0][0];
*/
	// faster calculation of the code above
	unsigned int i,j,K;
	vector< vector<int> > T;

	// calculate score matrix
	compare_clusters(c1,c2,T);

	K = T.size();

	a = b = c = d = 0;

	// calc a
	for(i = 0; i < K; i++)
	{
		for(j = 0; j < K; j++)
		{
			a += (T[i][j]*(T[i][j]-1))/2;
		}
	}

	// calc b
	long sum;

	for(j = 0; j < K; j++)
	{
		sum = 0;
		for(i = 0; i < K; i++)
		{
			sum += T[i][j];
		}
		b += (sum*(sum-1))/2;
	}
	b -= a;

	// calc c
	for(i = 0; i < K; i++)
	{
		sum = 0;
		for(j = 0; j < K; j++)
		{
			sum += T[i][j];
		}
		c += (sum*(sum-1))/2;
	}
	c -= a;

	// calc d
	d = (n*(n-1))/2 - a - b - c;
}

/*
	maximum cluster assignment
	(Kestler, Mueller)
*/
Float compare_mca(const Clusters &c1, const Clusters &c2, unsigned int n)
{
	vector<int> rowsol,colsol,u,v;
	vector< vector<int> > a;

	// calculate score matrix
	compare_clusters(c1,c2,a);

	vector< vector<int> >::iterator i;
	vector<int>::iterator j;
	int mx, costs;

	// mirror matrix => minimum assignment <=> maximum assignment
	mx = get_max(a);
	for(i=a.begin(); i!=a.end(); ++i)
	{
		for(j=(*i).begin(); j!=(*i).end(); ++j)
		{
			*j = mx - *j;
		}
	}



//for( int i = 0; i < a.size(); ++i )
//{
//	for( int j = 0; j < a[i].size(); ++j )
//	{
//		std::cout<<a[i][j];
//		std::cout<<"\t";
//	}
//	std::cout<<"\n";
//}



	// optimize the assignment of the two clusterings (LAP)
	costs = mx*a.size() - lap(a,rowsol,colsol,u,v);

	return (Float)(costs)/(Float)(n);
}


/*
	maximum cluster assignment WITH solution
	(Kestler, Mueller)
*/
Float compare_mca(const Clusters &c1, const Clusters &c2, unsigned int n, vector<int> & rowsol, vector<int> & colsol)
{
	vector<int> u,v;
	vector< vector<int> > a;

	// calculate score matrix
	compare_clusters(c1,c2,a);

	vector< vector<int> >::iterator i;
	vector<int>::iterator j;
	int mx, costs;

	// mirror matrix => minimum assignment <=> maximum assignment
	mx = get_max(a);
	for(i=a.begin(); i!=a.end(); ++i)
	{
		for(j=(*i).begin(); j!=(*i).end(); ++j)
		{
			*j = mx - *j;
		}
	}

	// optimize the assignment of the two clusterings (LAP)
	costs = mx*a.size() - lap(a,rowsol,colsol,u,v);

	return (Float)(costs)/(Float)(n);
}

// Calculate transinformation of all cluster pairs
//
Float compare_trans(const Clusters &c1, const Clusters &c2, unsigned int n)
{
	const Float 			LN2 = 1.0/((Float)n*log(2.0));

	vector< vector<int> > 	a;
	unsigned int 			i,
							j;
	Float 					sum;


	// get intersection matrix
	compare_clusters(c1,c2,a);

	sum = 0.0;

	for( i = 0; i < c1.size(); ++i )
	{
		for( j = 0; j < c2.size(); ++j )
		{
			if( a[i][j] > 0 )
			{
				sum += (Float)a[i][j] * LN2 * std::log((Float)a[i][j]*(Float)n/((Float)c1[i].size()*(Float)c2[j].size()));
			}
		}
	}

	if( sum > 0 )
	{
		return sum / std::sqrt( self_information(c1,n) * self_information(c2,n) );
	}

	return 0;
}

Float self_information( const Clusters &c, unsigned int n )
{
	unsigned int i;
	Float p,sum = 0.0;
	Float LN2 = 1.0/log(2.0);

	for( i = 0; i < c.size(); ++i )
	{
		if( c[i].size() > 0 )
		{
			p    = (Float)c[i].size()/(Float)n;
			sum -= p * LN2 * std::log(p);
		}

	}
	return sum;
}


/*
	Compare with symmetric difference
*/
Float compare_symmdiff(const Clusters &c1, const Clusters &c2 )
{
	vector<int> 				rowsol,
								colsol;

	vector<Float>				u,
								v;

	vector< vector<Float> > 	a;

	Float						cost;

	// calculate symmetric difference matrix
	symmdiff( c1, c2, a );




//for( int i = 0; i < a.size(); ++i )
//{
//	for( int j = 0; j < a[i].size(); ++j )
//	{
//		std::cout<<a[i][j];
//		std::cout<<"\t";
//	}
//	std::cout<<"\n";
//}
//return 0;



	// optimize the assignment of the two clusterings (LAP)
	cost = lap( a, rowsol, colsol, u, v );

	return cost/static_cast<Float>( c1.size() );
}
