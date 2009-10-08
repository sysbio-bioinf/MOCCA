/*
 * matrixutil.h
 *
 * Numerical Utility functions
 *
 * $Id: matrixutil.h,v 1.1 2001/05/21 14:10:12 andre Exp andre $
 *
 * $Log: matrixutil.h,v $
 * Revision 1.1  2001/05/21 14:10:12  andre
 * Initial revision
 *
 *
 */

/*
	CHANGES

	2003/03/11	AM	random_matrix : value_type changed
					reformated source code
					exception specifications removed
					new assertions
*/

#ifndef __MATRIXUTIL_H__
#define __MATRIXUTIL_H__

#include <stdexcept>
#include <iosfwd>
#include <algorithm>
#include <iterator>
#include <limits>

// using namespace std;

#include "error_handler.h"
#include "random.h"

template <class Matrix>
void dim_matrix(Matrix &a, int rows, int cols)
{
	typedef typename Matrix::value_type RowType;

	if( (int)(a.size()) != rows || (int)(a[0].size()) != cols )
	{
		a = Matrix( rows, RowType(cols,0) );
	}
}

template <class MatrixA, class MatrixB, class MatrixC>
void row_concat_matrix(const MatrixA &a, const MatrixB &b, MatrixC &c) 
{

	_AssertParam(a.size() == b.size(),"row_concat_matrix");

	typename MatrixA::const_iterator	a_first	= a.begin(),
										a_last	= a.end();
	typename MatrixB::const_iterator	b_first	= b.begin();

	c.resize( a.size() );

	typename MatrixC::iterator			c_first = c.begin();


	while( a_first != a_last )
	{
		concat_vector(*a_first,*b_first,*c_first);

		++a_first;
		++b_first;
		++c_first;
	}
}

template <class InputIterator1, class InputIterator2, class OutputIterator>
void row_concat_matrix(InputIterator1 first1, InputIterator2 last1,
		InputIterator2 first2, InputIterator2 last2, OutputIterator out)
{

	_AssertParam(last1-first1 == last2-first2,"row_concat_matrix" );

	while( first1 != last1 ) 
	{
		concat_vector(*first1,*first2,*out);

		++first1;
		++first2;
		++out;
	}
}


template<class Matrix>
void show_matrix(const Matrix &m, ostream &out = cout)
{
	typename Matrix::const_iterator first = m.begin(),
									last = m.end();

	while(first!=last) 
	{
		show_vector(*first,out);
		out << endl;

		++first;
	}
}

template <class Matrix, class T>
void fill_matrix(Matrix &A, const T &value)
{

	typename Matrix::iterator	first = A.begin(),
								last = A.end();
	typename Matrix::value_type::iterator iter;

	while(first != last)
	{
		for(iter = (*first).begin(); iter != (*first).end(); ++iter)
		{
			*iter = value;
		}

		++first;
	}
}

template <class MatrixA, class MatrixB>
void add_matrix(const MatrixA &A, MatrixB &B)
{
	int i,j;

	_AssertParam(A.size() == B.size(), "add_matrix" );

	for(i=0; i<(int)A.size(); i++)
	{
		_AssertParam( A[i].size() == B[i].size(), "add_matrix" );

		for(j=0; j<(int)A[i].size(); j++)
		{
			B[i][j] += A[i][j];
		}
	}
}

template<class MatrixA, class MatrixB, class MatrixC>
void mult_matrix(const MatrixA &A, const MatrixB &B, MatrixC &C)
{

	typedef typename MatrixC::value_type::value_type Float;

	_AssertParam(A[0].size() == B.size(), "mult_matrix" );

	if( C.size() != A.size() || C[0].size() != B[0].size() )
		dim_matrix(C,A.size(),B[0].size());

	typename MatrixA::size_type i,j,k;
	Float sum;

	for(i=0; i<A.size(); i++)
	{
		for(j=0; j<B[0].size(); j++)
		{
			sum = 0;
			for(k=0; k<B.size(); k++)
				sum += A[i][k]*B[k][j];
			C[i][j] = sum;
		}
	}
}


/*
	get_min

	component-wise minimum over the rows of A
*/
template <class InputIterator, class Vector>
void get_min(InputIterator first, InputIterator last, Vector &v)
{
	typename std::iterator_traits< InputIterator >::value_type::const_iterator i;
	typename Vector::iterator j;
	bool firsttime = true;

	v = Vector((*first).size());

	while( first != last )
	{
		if(firsttime)
		{
			std::copy((*first).begin(),(*first).end(),v.begin());
			firsttime = false;
		}
		else
		{
			for(i=(*first).begin(), j=v.begin(); i!=(*first).end() && j!=v.end(); ++i, ++j)
			{
				(*j) = min(*j,*i);
			}
		}
		++first;
	}
}

/*
	get_max

	component-wise maximum over the rows of A
*/
template <class InputIterator, class Vector>
void get_max(InputIterator first, InputIterator last, Vector &v)
{
	typename std::iterator_traits< InputIterator >::value_type::const_iterator i;
	typename Vector::iterator j;
	bool firsttime = true;

	v = Vector((*first).size());

	while( first != last )
	{
		if(firsttime)
		{
			std::copy((*first).begin(),(*first).end(),v.begin());
			firsttime = false;
		}
		else
		{
			for( i=(*first).begin(), j=v.begin(); i!=(*first).end() && j!=v.end(); ++i, ++j )
			{
				(*j) = max(*j,*i);
			}
		}
		++first;
	}
}

/*
	get_max(A)

	Returns the maximum element of A
*/
template <class Matrix>
typename Matrix::value_type::value_type get_max(const Matrix &A)
{
	typedef typename Matrix::value_type::value_type T;
	typename Matrix::const_iterator i;
	typename Matrix::value_type::const_iterator j;
	T mx = 0;

	bool first = true;

	for(i=A.begin(); i!=A.end(); ++i)
	{
		for(j=(*i).begin(); j!=(*i).end(); ++j)
		{
			if(*j>mx || first)
			{
				mx = (*j);
				first = false;
			}
		}
	}
	return mx;
}

template <class Matrix>
typename Matrix::value_type::value_type get_min(const Matrix &A)
{
	typedef typename Matrix::value_type::value_type T;
	typename Matrix::const_iterator i;
	typename Matrix::value_type::const_iterator j;

	T mx;
	bool first = true;

	for(i=A.begin(); i!=A.end(); i++)
	{
		for(j=(*i).begin(); j!=(*i).end(); j++)
		{
			if(*j<mx || first)
			{
				mx = (*j);
				first = false;
			}
		}
	}
	return mx;
}


/*
	calc_mean_var

	calculates mean / variance of an vector
*/

template <class InputIterator, class T>
unsigned int calc_mean_var(InputIterator first, InputIterator last, T &mean, T &var, bool div_n = false)
{
	InputIterator old = first;
	T d;
	typedef typename std::iterator_traits<InputIterator>::value_type V;
	unsigned int num = 0;

	mean = 0;
	while(first!=last)
	{
		if( (*first) >= numeric_limits<V>::min() && (*first) <= numeric_limits<V>::max() )
		{
			mean += (*first);
			++num;
		}
		++first;
	}
	if(num == 0)
	{
		mean = numeric_limits<T>::infinity();
		var  = numeric_limits<T>::infinity();

		return 0;
	}
	mean /= static_cast<T>(num);

	first = old;
	var = 0;

	while(first != last)
	{
		if((*first) >= numeric_limits<V>::min() && (*first) <= numeric_limits<V>::max() )
		{
			d = (*first)-mean;
			var += d*d;
		}
		++first;
	}

	if(div_n)
	{
		var /= static_cast<T>(num);
	}
	else
	{
		if(num > 1)
		{
			var /= static_cast<T>(num-1);
		}
		else
		{
			var = numeric_limits<T>::infinity();
		}
	}

	return num;
}


/*
	gauss_vector(A,mu,sigma)

	Fills the matrix A with normal distribution

*/
template <class Vector>
void gauss_vector(Vector &v, typename Vector::value_type mu, typename Vector::value_type sigma)
{
	typename Vector::iterator first, last;

	first = v.begin();
	last = v.end();

	while( first != last )
	{
		*first = static_cast< typename Vector::value_type>(RandomGauss()*sigma) + mu;
		++first;
	}
}


/*
	random_vector(A,lower,upper)

	Fills the matrix A with random values in the interval [lower,upper].

*/
template <class Vector>
void random_vector(Vector &v, typename Vector::value_type lower, typename Vector::value_type upper)
{
	typename Vector::iterator i;

	for(i=v.begin(); i!=v.end(); i++)
	{
		(*i) = static_cast<typename Vector::value_type>(Random()*(Float)(upper-lower)) + lower;
	}
}

/*
	init_random(A,lower,upper)

	Fills the matrix A with random values in the interval [lower,upper].

*/
template <class T, class Matrix>
void random_matrix(Matrix &A, T lower, T upper)
{
	typename Matrix::iterator i;
	typename Matrix::value_type::iterator j;

	for(i=A.begin(); i!=A.end(); i++)
	{
		for(j=(*i).begin(); j!=(*i).end(); j++)
		{
			(*j) = (typename Matrix::value_type::value_type)((Random()*(upper-lower)) + lower);
		}
	}
}

template <class Matrix, class Vector, class RandomNumberGenerator>
void random_matrix_lu(Matrix &A, const Vector &lower, const Vector &upper, RandomNumberGenerator &rand)
{
	typename Matrix::iterator i;
	typename Matrix::value_type::iterator j;
	typename Vector::const_iterator l,u;
	typedef typename Matrix::value_type TVector;

	for(i=A.begin(); i!=A.end(); ++i)
	{
		(*i) = TVector(lower.size());

		l = lower.begin();
		u = upper.begin();

		for(j=(*i).begin(); j!=(*i).end(); ++j)
		{
			(*j) = rand()*((*u)-(*l)) + (*l);
			++l;
			++u;
		}
	}
}


/*
	permutate(first,last,rand)

	Shuffles the data in the range first -> last
*/
/*
template <class _RandomAccessIterator, class _RandomNumberGenerator>
void permutate(_RandomAccessIterator first, _RandomAccessIterator last,
				_RandomNumberGenerator& rand) {
	_RandomAccessIterator iter;
	iterator_traits<_RandomAccessIterator>::difference_type n;

	n = distance(first,last);
	for(iter=first; iter!=last; iter++) {
		swap(*iter, *(iter+rand(n--)) );
	}
}
*/

/*
   distance_matrix(X,D,metric)

   Calculates all distances between each row of X:

   D[i][j] = d(X[i],X[j])
*/
template <class Sequence, class Matrix, class Metric>
void distance_matrix(const Sequence &X, Matrix &D, Metric &metric)
{
	typename Sequence::const_iterator i,j;
	typename Matrix::iterator di;
	typename Matrix::value_type::iterator dj;
	typename Sequence::size_type n;

	n = std::distance(X.begin(), X.end());

//	D = Matrix(n,n);
	dim_matrix(D,n,n);

	for(i=X.begin(), di=D.begin(); i!=X.end(); i++, di++)
	{
		for(j=X.begin(), dj = (*di).begin(); j!=X.end(); j++, dj++)
		{
			(*dj) = metric((*i),(*j));
		}
	}
}

/*
   distance_matrix(X,Y,D,metric)

   Calculates all distances between each row X and Y

   D[i][j] = d(X[i],Y[j])
*/
template <class Sequence, class Matrix,class Metric>
void distance_matrix(const Sequence &X, const Sequence &Y, Matrix &D, Metric &metric) 
{
	typename Sequence::const_iterator i,j;
	typename Matrix::iterator di;
	typename Matrix::value_type::iterator dj;

	dim_matrix( D, X.size(), Y.size() );

	for(i=X.begin(), di=D.begin(); i!=X.end(); i++, di++)
	{
		for(j=Y.begin(), dj = (*di).begin(); j!=Y.end(); j++, dj++)
		{
			(*dj) = metric((*i),(*j));
		}
	}
}

/*
	algebraic operations
*/

template <class Vector>
void add_scalar(Vector & v, typename Vector::value_type a ) 
{
	typename Vector::iterator	first = v.begin(),
								last  = v.end();
	while(first!=last) 
	{
		(*first) += a;
		++first;
	}
}

template <class Vector>
void add_vector(const Vector &src, Vector &dst)
{
	typename Vector::const_iterator		src_first = src.begin(),
										src_last  = src.end();
	typename Vector::iterator 			dst_first = dst.begin();

	_AssertParam( src.size() == dst.size(), "add_vector" )

	while( src_first!=src_last )
	{
		(*dst_first) += (*src_first);
		++src_first;
		++dst_first;
	}
}


template <class Vector>
void sub_vector(const Vector &src, Vector &dst)
{
	typename Vector::const_iterator		src_first = src.begin(),
										src_last  = src.end();
	typename Vector::iterator 			dst_first = dst.begin(),
										dst_last  = dst.end();
	while( src_first!=src_last && dst_first!=dst_last )
	{
		(*dst_first) -= (*src_first);
		++src_first;
		++dst_first;
	}
}

template <class Vector>
void diff_vector(const Vector &a, const Vector &b, Vector &dst)
{
	typename Vector::const_iterator		a_first = a.begin(),
										a_last  = a.end(),
										b_first = b.begin(),
										b_last  = b.end();

	if(dst.size() < a.size())
		dst.resize( a.size() );

	typename Vector::iterator 			dst_iter = dst.begin();

	while( a_first!=a_last && b_first!=b_last )
	{
		(*dst_iter) = (*a_first) - (*b_first);
		++a_first;
		++b_first;
		++dst_iter;
	}
}


template <class Vector,class Float>
void add_scaled(const Vector &src, Float s, Vector &dst)
{
	typename Vector::const_iterator		src_first = src.begin(),
										src_last  = src.end();
	typename Vector::iterator 			dst_first = dst.begin(),
										dst_last  = dst.end();

	while( src_first!=src_last && dst_first!=dst_last ) 
	{
		(*dst_first) += (*src_first)*s;
		++src_first;
		++dst_first;
	}
}

template <class Vector,class Float>
void add_scaled(const Vector &a, const Vector &b, Float s, Vector &dst)
{
	typename Vector::const_iterator		a_first = a.begin(),
										a_last  = a.end(),
										b_first = b.begin();

	typename Vector::iterator 			dst_first = dst.begin();

	_AssertParam( a.size() == b.size(), "add_scaled" );

	dst.resize(a.size());

	while( a_first != a_last )
	{
		(*dst_first) = (*a_first) + s*(*b_first);

		++a_first;
		++b_first;
		++dst_first;
	}
}


template <class Vector>
typename Vector::value_type scalar_product(const Vector &a, const Vector &b)
{
	typename Vector::const_iterator		first1 = a.begin(),
										last1  = a.end(),
							 			first2 = b.begin(),
										last2  = b.end();
	typename Vector::value_type sum = 0;

	while( first1!=last1 && first2!=last2 )
	{
		sum += (*first1) * (*first2);
		++first1;
		++first2;
	}

	return sum;
}

template <class Vector>
void scale_vector(Vector &v, typename Vector::value_type s)
{
	typename Vector::iterator 			first = v.begin(),
										last  = v.end();

	while( first != last )
	{
		*first *= s;
		++first;
	}
}


template <class Vector>
void show_vector(const Vector &v, ostream &os = cout)
{
	typename Vector::const_iterator 	first = v.begin(),
										last = v.end();
	os<<"[ ";
	while(first != last)
	{
		os<<(*first)<<" ";
		++first;
	}
	os<<"]";
}

template <class VectorA, class VectorB, class VectorC>
void concat_vector(const VectorA &a, const VectorB &b, VectorC &c)
{
	c.resize( a.size() + b.size() );
	std::copy( a.begin(), a.end(), c.begin() );
	std::copy( b.begin(), b.end(), c.begin() + a.size() );
}


template <class Matrix, class Float>
void add_scaled_matrix(const Matrix &a, Float s, Matrix &dst)
{
	typename Matrix::const_iterator		a_first = a.begin(),
										a_last  = a.end();

	typename Matrix::iterator 			dst_first = dst.begin();

	_AssertParam( a.size() == dst.size() && a[0].size() == dst[0].size(), "add_scaled_matrix" );

	while( a_first != a_last )
	{
		add_scaled( *a_first, s, *dst_first );

		++a_first;
		++dst_first;
	}
}

template<class Matrix>
void assign_matrix( Matrix &dst, const Matrix &src )
{
	dim_matrix( dst, src.size(), src[0].size() );

	for( unsigned int i = 0; i < src.size(); ++i )
	{
		std::copy( src[i].begin(), src[i].end(), dst[i].begin() );
	}
}

#endif

