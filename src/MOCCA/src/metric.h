/*
 * metric.h
 *
 * Metric/Norm-Definitions (lnorm, hamming-distance)
 *
 * written by Andre Mueller
 *
 * $Id: metric.h,v 1.1 2001/04/26 13:22:08 andre Exp andre $
 *
 * $Log: metric.h,v $
 * Revision 1.1  2001/04/26 13:22:08  andre
 * Initial revision
 *
 *
 */
 
/*
	CHANGES
	
	2003/03/31	A.Mueller  reformatings, optimization of LNorm
*/

/*!  \file metric.h
\brief Base classes for defining metric on spaces.

We define here metric and norm. A metric class must have the two operators
\code
T operator ()(const Vector &v) const;
\endcode
and
\code
T operator ()(const Vector &u,const Vector &v) const;
\endcode
*/

#ifndef METRIC_H
#define METRIC_H

#include <cmath>
#include <functional>
#include "error_handler.h"

/*! Abstract metric base class.

All metric classes should be inherited from this class.
*/
template <class Vector, class T = float>
class BaseMetric {

public:
	/* I tried it out with a multiple inheritance
		public unary_function<Vector,T>, public binary_function<Vector,Vector,T>
		but : g++ crashes too ...
	*/
	typedef Vector first_argument_type;
	typedef Vector second_argument_type;
	typedef Vector argument_type;
	typedef T result_type;

	BaseMetric() {}
	virtual ~BaseMetric() {}

	/* The function operator () calcuates the norm of the given vector */
	virtual T operator()(const Vector &v) const = 0;

	/* dist(u,v) returns the distance between the two vectors u and v */
	virtual T operator()(const Vector &u, const Vector &v) const = 0;
};


/*! The L-Norm.

\f[
	\|{\bf x}\|_\ell := \left( \sum_i |x_i|^\ell \right)^{1/\ell}	\\
	d_\ell({\bf x},{\bf y}) := \|{\bf x}-{\bf y}\|_\ell
\f]

*/
template <class Vector, class T = typename Vector::value_type >
class LNorm : public BaseMetric <Vector,T> {

public:
	LNorm() : BaseMetric<Vector,T>(), L_(2) { }
	LNorm(T L) : BaseMetric<Vector,T>(), L_(L) { }
	LNorm(const LNorm &norm) : BaseMetric<Vector,T>(), L_(norm.L_) { }

	virtual T operator ()(const Vector &v) const;
	virtual T operator()(const Vector &u, const Vector &v) const;

private:
	T	L_;
};

template <class Vector, class T>
T LNorm<Vector,T>::operator ()( const Vector &v ) const
{
	typename Vector::const_iterator i;
	T	sum;


	sum = 0;
	for( i = v.begin(); i != v.end(); ++i )
	{
		sum += std::pow(std::abs(*i),L_);
	}

	return (T)std::pow(sum,1.0/L_);
}

template <class Vector, class T>
T LNorm<Vector,T>::operator()( const Vector &u, const Vector &v ) const
{
	_AssertParam( u.size() == v.size() , "LNorm<Vector,T>::operator()( const Vector &, const Vector & )" );

	typename Vector::size_type i;
	T	sum;

	sum = 0.0;

	if( L_ == 1.0 )
	{
		for( i = 0; i < u.size(); ++i )
		{
			sum += std::abs( u[i] - v[i] );
		}
		return sum;
	}

	if( L_ == 2.0 )
	{
		T		d;

		for( i = 0; i < u.size(); ++i )
		{
			d	 = u[i] - v[i];
			sum += d * d;
		}
		return std::sqrt(sum);
	}


	for( i = 0; i < u.size(); ++i )
	{
		sum += (T)std::pow(std::abs(u[i]-v[i]),L_);
	}
	return std::pow(sum,1.0/L_);
}


/*! Hamming norm/metric.

\f[
	\|{\bf x}\|_H	:= \sum_i \chi [ x_i\ne 0 ]	\\
	d_H({\bf x},{\bf y}) := \|{\bf x}-{\bf y}\|_H
\f]
*/
template <class Vector,class T>
class Hamming : public BaseMetric <Vector,T> {

public:
	Hamming() : BaseMetric<Vector,T>() {};
	Hamming(const Hamming & ) : BaseMetric<Vector,T>() { }

	virtual T operator () (const Vector &v) const {
		T	sum;
		typename Vector::const_iterator i;

		sum = 0;
		for(i=v.begin(); i!=v.end(); ++i)
		{
			if((*i) != 0)
			{
				sum += 1;
			}
		}

		return sum;
	}

	virtual T operator () (const Vector &u, const Vector &v) const {
		T	sum;
		typename Vector::const_iterator i,j;

		sum = 0;
		for(i=u.begin(), j=v.begin(); (i!=u.end()) && (j!=v.end()); ++i,++j)
		{
			if((*i) != (*j))
			{
				sum += 1;
			}
		}

		return sum;
	}
};


/*! A mapping into a matrix.

The feature space has to be discrete in the range \f$ {\cal K}={0\ldots k-1}^n \f$.
\f[
	d_I({\bf x},{\bf y}) := \sum_i m_{x_i,y_i} \quad {\bf m}\in {\cal R}^d
\f]
*/
template <class Vector, class T, class Matrix>
class IndexedMetric : public BaseMetric<Vector,T> 
{

public:
	IndexedMetric(const Matrix &A) : BaseMetric<Vector,T>(), A_(A) {}

	IndexedMetric(const IndexedMetric &norm) : BaseMetric<Vector,T>(), A_(norm.A_) {}

	virtual T operator () (const Vector &v) const {
		T	sum;
		typename Vector::const_iterator i;

		sum = 0;
		for(i=v.begin(); i!=v.end(); ++i)
			sum += A_[0][static_cast<size_type>(*i)];

		return sum;
	}

	virtual T operator () (const Vector &u, const Vector &v) const {
		T	sum;
		typename Vector::const_iterator i,j;

		sum = 0;
		for(i=u.begin(), j=v.begin(); i!=u.end() && j!=v.end(); ++i, ++j)
			sum += A_[static_cast<size_type>(*i) ][static_cast<size_type>(*j)];

		return sum;
	}

private:
	Matrix		A_;
	typedef typename Matrix::size_type	size_type;

};

#endif

