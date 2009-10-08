/*
 * statistics.h
 *
 * Implements some statistical methods based on ranks
 *
 * $Id: statistics.h,v 1.3 2002/04/24 09:30:21 andre Exp andre $
 *
 * $Log: statistics.h,v $
 * Revision 1.3  2002/04/24 09:30:21  andre
 * *** empty log message ***
 *
 * Revision 1.2  2002/04/22 11:53:41  andre
 * New functions implemented (mean, variance)
 *
 * Revision 1.1  2002/01/23 10:51:41  andre
 * Initial revision
 *
 *
 */
 
/*
	CHANGES
	
	2003/3/17	A.M. exception handling now with error_handler
				Comments
*/

#ifndef __STATISTICS_H__
#define __STATISTICS_H__

#include <numeric>
#include <iterator>
#include <vector>
#include <algorithm>
#include <functional>
#include <stdexcept>
#include <cmath>

// using namespace std;

#include "def.h"
#include "mathext.h"
#include "error_handler.h"

// ftp://odin.mdacc.tmc.edu/pub/source/dcdflib.c-1.1.tar.gz
#include "cdflib.h"

namespace Stat {

/******************************************************************************

	Probability density functions

*******************************************************************************/


/*
	The normal distribution
*/

// Returns the p value of the normal distribution
Float cdf_normal(Float x, Float mu, Float sigma);

// Returns the x value
Float cdf_normal_x(Float p, Float mu, Float sigma);

/*
	The cdf of the chi square distribution
*/

// Returns the p value of the chi square distribution with c degrees of freedom
Float cdf_chisquare(Float x, int c);

// Returns the x value of the chi square distribution with c degrees of freedom
Float cdf_chisquare_x(Float p, int c);

/*
	The t distribution
*/
// Returns the p value of the t distribution function wtih c degrees of freedom
Float cdf_t(Float x, int c);

// Returns the T value of the t distribution given a probabilty p and c degrees of freedom
Float cdf_t_x(Float p, int c);

/******************************************************************************

	Help functions

*******************************************************************************/


template< class RandomAccessIterator , class T>
typename iterator_traits<RandomAccessIterator>::value_type
		quantile( RandomAccessIterator first, RandomAccessIterator last, T p) 
{

	int n = last - first;
	T fr = p * (T)(n+1) - 1.0;
	int idx1 = (int)floor( fr );
	int idx2 = (int)ceil( fr );

	if(idx1 < 0) idx1 = 0;
	if(idx2 < 0) idx2 = 0;
	if(idx1 >= n) idx1 = n-1;
	if(idx2 >= n) idx2 = n-1;

	if( idx1 != idx2 ) 
	{
		// take middle between idx1 and idx2
		return ( *(first + idx1 ) + *(first + idx2) )/2;
	} 
	else
	{
		// return idx
		return *( first + idx1 );
	}
}

/*! Computes the median of the given data [first, last)
*/

template< class InputIterator >
typename iterator_traits<InputIterator>::value_type
median(InputIterator first, InputIterator last)
{
	typedef typename iterator_traits<InputIterator>::value_type T;
	vector< T > tmp(first,last);

	_AssertParam(last - first >= 2, "median");

	sort( tmp.begin(), tmp.end() );

	return quantile( tmp.begin(), tmp.end(), 0.5 );
}

template< class InputIterator >
void quartiles(InputIterator first, InputIterator last,
		typename iterator_traits<InputIterator>::value_type &quartile25,
		typename iterator_traits<InputIterator>::value_type &quartile50,
		typename iterator_traits<InputIterator>::value_type &quartile75 )
{
	_AssertParam( last - first >= 1, "quartiles" );

	vector< typename iterator_traits<InputIterator>::value_type > tmp(first,last);

	sort( tmp.begin(), tmp.end() );

	quartile25 = quantile( tmp.begin(), tmp.end(), 0.25 );
	quartile50 = quantile( tmp.begin(), tmp.end(), 0.5 );
	quartile75 = quantile( tmp.begin(), tmp.end(), 0.75 );
}

template< class InputIterator >
void quartiles(InputIterator first, InputIterator last,
		typename iterator_traits<InputIterator>::value_type &min,
		typename iterator_traits<InputIterator>::value_type &quartile25,
		typename iterator_traits<InputIterator>::value_type &quartile50,
		typename iterator_traits<InputIterator>::value_type &quartile75,
		typename iterator_traits<InputIterator>::value_type &max )
{

	_AssertParam( last - first >= 1, "quartiles" );

	vector< typename iterator_traits<InputIterator>::value_type > tmp(first,last);

	sort( tmp.begin(), tmp.end() );

	min = *tmp.begin();
	quartile25 = quantile( tmp.begin(), tmp.end(), 0.25 );
	quartile50 = quantile( tmp.begin(), tmp.end(), 0.5 );
	quartile75 = quantile( tmp.begin(), tmp.end(), 0.75 );
	max = *(tmp.end()-1);
}


template< class InputIterator >
typename iterator_traits<InputIterator>::value_type 
mean(InputIterator first, InputIterator last)
{
	typedef typename iterator_traits<InputIterator>::value_type T;
	T sum = 0;
	int n = last-first;

	_AssertParam( n > 0, "mean" );

	while(first != last)
	{
		sum += (*first);
		++first;
	}
	return sum/(T)n;
}

/*! Computes the variance (n-1) of the given data.
*/
template< class InputIterator >
typename iterator_traits<InputIterator>::value_type
variance(InputIterator first, InputIterator last)
{
	typedef typename iterator_traits<InputIterator>::value_type T;

	int n = last-first;

	_AssertParam( n >= 2, "variance" );

	T	sum		= 0,
		m		= mean(first,last),
		d;

	while(first != last)
	{
		d = (*first)-m;
		sum += d*d;
		++first;
	}

	return sum/(T)(n-1);
}

/*! Standard Deviation
	The square root of the variance.
*/
template< class InputIterator >
typename iterator_traits<InputIterator>::value_type
stddev(InputIterator first, InputIterator last)
{

	return sqrt( variance(first,last) );
}

/*! Calculates mean and variance (with a variance calculation with n-1 in the divisor)
*/
template< class InputIterator , class T >
void meanvar(InputIterator first, InputIterator last, T & mean, T & var)
{
	long n = last-first;

	mean = 0;
	var = -1;

	if(n<1)
		return;

	T	sum		= 0,
		qsum	= 0;

	while( first != last )
	{
		sum += (*first);
		qsum += (*first)*(*first);
		++first;
	}


	mean = sum/(T)(n);
	if( n < 2 )
		return ;

	var = qsum / (T)(n-1) - sum*sum/(T)(n*(n-1));
}

/*! Calculates mean and standard deviation.
*/
template< class InputIterator , class T >
void meanstddev(InputIterator first, InputIterator last, T & mean, T & stddev)
{
	long n = last-first;

	if( n < 1 )
	{
		mean = 0;
		stddev = -1;
		return;
	}

	T	sum		= 0,
		qsum	= 0;


	while(first != last)
	{
		sum += (*first);
		qsum += (*first)*(*first);
		++first;
	}


	mean = sum/(T)(n);

	if( n > 1 )
		stddev = sqrt( qsum / (T)(n-1) - sum*sum/(T)(n*(n-1)) );
	else
		stddev = -1;
}

/*! Calculates mean and variance (with a variance calculation with n in the divisor)
*/
template< class InputIterator , class T >
void meanvar_n( InputIterator first, InputIterator last, T & mean, T & var )
{
	long n = std::distance(first,last);

	_AssertParam( n >= 1, "meanvar_n" );

	T	sum		= 0,
		qsum	= 0,
		d;

	while( first != last )
	{
		d 	 = (*first);
		sum  += d;
		qsum += d*d;

		++first;
	}


	mean = sum / (T)(n);
	var  = qsum / (T)(n) - sum*sum/(T)(n*n);
}


/*!

	The median test statistic.

	(see W.J.Conover "Practical Nonparametric Statistics - Third Edition" 1999)

	<pre>

	H0 : All c populations have the same median
	H1 : At least two of the populations have different medians

	If T exceeds chi_square_x( 1-alpha, data.size() - 1 ) then reject H0

	The p - value is

		P = 1 - chi_square( median_t(data), data.size() - 1 )
	</pre>

*/

template< class Sequence>
typename Sequence::value_type median_t( const vector<Sequence> & data )
{

	typedef typename Sequence::value_type T;

	vector< int > count( data.size(), 0 );	// saves number of entries which are greater than the grand median (variable m)

	// build the grand median

	// slow but easy
	vector< T > tmp;
	typename vector<Sequence>::const_iterator first,last;
	int N;

	// count number of data records
	first	= data.begin();
	last 	= data.end();
	N		= 0;

	while(first != last)
	{
		N += (*first).size();
		++first;
	}

	tmp.reserve(N);
	back_insert_iterator< vector<T> > bi(tmp);

	first = data.begin();
	while(first != last)
	{
		std::copy((*first).begin(),(*first).end(),bi);
		++first;
	}

	// build the grand median
	sort( tmp.begin(), tmp.end() );
	T m = quantile( tmp.begin(), tmp.end(), 0.5 );

	// now build contingency table

	first = data.begin();
	unsigned int a,b,i = 0;

	a = b = 0;

	while(first != last)
	{
		count[i] = std::count_if( (*first).begin(), (*first).end(), bind2nd(greater<T>(), m) );
		a += count[i];
		b += data[i].size() - count[i];

		++i;
		++first;
	}

	// DEBUG
	/*
	cout<<"grand median = "<<m<<endl;
	cout<<"a = "<<a<<endl;
	cout<<"b = "<<b<<endl;
	cout<<"N = "<<N<<endl;

	for(i=0;i<count.size(); i++) {
		cout << (count[i]) << "/"<<(data[i].size() - count[i])<<"  ";
	}
	cout<<endl;
	*/
	// \DEBUG

	T t = 0;

	for(i=0; i<count.size(); i++) 
	{
		t += sqr( (T)count[i] - (T)data[i].size()*(T)a/(T)N ) / (T)data[i].size();
	}
	t *= (T)N*(T)N/((T)a*(T)b);

	return t;
}

/******************************************************************************

	Mann-Whitney test statistic

	(see W.J.Conover "Practical Nonparametric Statistics - Third Edition" 1999)


*******************************************************************************/

template< class Rank >
typename Rank::value_type mann_whitney_t( const vector<Rank> & R )
{
	_AssertParam( R.size() == 2, "mann_whitney_t" );

	return std::accumulate(R[0].begin(),R[0].end(), (typename Rank::value_type)0);
}

/*
	The Mann-Whitney T1 test statistic. Use if there are many ties in data.
	Compare T1 to the cdf of the standard normal distribution.

*/
template< class Rank >
typename Rank::value_type mann_whitney_t1( const vector<Rank> & R )
{
	_AssertParam( R.size() == 2, "mann_whitney_t1" );

	typename Rank::value_type T,rsum;

	T = std::accumulate( R[0].begin(), R[0].end(), (typename Rank::value_type) 0);

	// calculate square sum R
	typename vector<Rank>::const_iterator first_seq,last_seq;
	typename Rank::const_iterator first,last;

	rsum = 0;

	first_seq = R.begin();
	last_seq = R.end();

	while( first_seq != last_seq )
	{
		first = (*first_seq).begin();
		last = (*first_seq).end();
		while(first != last) {
			rsum += (*first)*(*first);
			++first;
		}
		++first_seq;
	}

	typename Rank::value_type 	n = R[0].size(),
								m = R[1].size(),
								N = n+m;

	return (T - n*(N+1)*0.5) / sqrt(n*m/(N*(N-1))*rsum-n*m*(N+1)*(N+1)/(4*(N-1)));
}

/*! Pearsons correlation coefficient.

*/
template< class InputIterator >
typename InputIterator::value_type
corr( InputIterator first1, InputIterator last1,
      InputIterator first2, InputIterator last2 )
{
	typedef typename InputIterator::value_type	T;

	int		n = std::distance( first1, last1 );

	_AssertParam( std::distance(first1,last1) == std::distance(first2,last2), "corr" );
	_AssertParam( n >= 1, "corr" );

	T		mean1, var1,
			mean2, var2,
			sum;

	// evaluate mean and variance of the two sequences
	meanvar_n( first1, last1, mean1, var1 );
	meanvar_n( first2, last2, mean2, var2 );

	// compute the crossterms
	sum = 0.0;
	while( first1 != last1 )
	{
		sum += (*first1) * (*first2);

		++first1;
		++first2;
	}

	return ( sum/(T)(n) - mean1*mean2 ) / std::sqrt( var1 * var2 );
}


}; // namespace Stat

#endif

