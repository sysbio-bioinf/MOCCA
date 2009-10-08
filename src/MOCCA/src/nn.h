/*
 * nn.h
 *
 * Neural Networks Bibliothek
 *
 * $Id: nn.h,v 1.2 2001/05/18 14:37:12 andre Exp andre $
 *
 * $Log: nn.h,v $
 * Revision 1.2  2001/05/18 14:37:12  andre
 * *** empty log message ***
 *
 * Revision 1.1  2001/04/09 14:02:32  andre
 * Initial revision
 *
 *
 */

#ifndef __NN_H__
#define __NN_H__


#include <iostream>
#include <fstream>
#include <cctype>
#include <functional>

using namespace std;

#include "def.h"
#include "random.h"
#include "metric.h"

/* ************************************************************************** */
/*                         MATHEMATICAL HELP FUNCTIONS                        */
/* ************************************************************************** */

const Float EPSILON = 1E-20;

/* sign(x) calculates the sign of a number */
template <class T> 
T sign(T x) {
	return (x<0) ? -1 : +1;
}

/* cond_pow(x,y)  calculates a "secure" power x^y */
template <class Float>
Float cond_pow(Float x, Float y) {
	if(fabs(x) < EPSILON)
		return pow(EPSILON,y);
	
	return pow(x,y);
}

/* cond_div(a,b) calculates a "secure" division a/b */
template <class Float>
Float cond_div(Float a,Float b) {
	if(fabs(b)<EPSILON)
		return a/EPSILON*sign(b);
	return a/b;
}


template <class Matrix>
void write_matlab_matrix(const Matrix &A,ostream &f) {
	typename Matrix::size_type i,j;
	
	f<<"[ ";
	for(i=0;i<A.nrows();i++) {
		for(j=0;j<A.ncols();j++) {
			f << A(i,j);
				f << " ";
		}
		f << "; ";
	}
	f<<"]";
}

/*
	crossvalidate()
	
	Splits a dataset in training set and test set
*/
template <class MatrixA, class MatrixB>
void crossvalidate(MatrixA &data, typename MatrixA::size_type npieces, typename MatrixA::size_type piece,
	MatrixB &train, MatrixB &test) {
	typename MatrixA::size_type i,first,last,s,r;
	typename MatrixA::iterator iter;
	typename MatrixB::iterator dest;

	if(npieces > data.nrows())
		npieces = data.nrows();
	
	if(piece >= npieces)
		return;

	s = data.nrows() / npieces;	// datapoints per piece
	r = data.nrows() % npieces;	// rest 

	#define PIECE_SIZE(s, r, i) ( i<r ? s+1 : s )
	
	train = MatrixB(data.nrows()-PIECE_SIZE(s,r,piece),data.ncols());
	test = MatrixB(PIECE_SIZE(s,r,piece),data.ncols());
	
	/* find the borders of the leave out piece */
	first = 0;
	for(i=0;i<piece;i++)
		first+=PIECE_SIZE(s,r,i);
	last = first+PIECE_SIZE(s,r,piece);
	
	/* */
	dest = train.begin();
	for(iter=data.begin(); iter!=(data.begin()+first); iter++)
		std::copy((*iter).begin(),(*iter).end(),(*(dest++)).begin());
	for(iter=(data.begin()+last); iter!=data.end(); iter++)
		std::copy((*iter).begin(),(*iter).end(),(*(dest++)).begin());
	
	dest = test.begin();
	for(iter=(data.begin()+first); iter!=(data.begin()+last); iter++)
		std::copy((*iter).begin(),(*iter).end(),(*(dest++)).begin());
}

#endif

