/*
 * mathext.h
 *
 * some basic mathematical extensions
 *
 */
 
#ifndef __MATHEXT_H__
#define __MATHEXT_H__

template <class T>
T abs(const T &x) {

	if(x < 0)
		return -x;
	else
		return x;
}

template <class T>
T sign(const T &x) {

	if( x > 0 )
		return +1;
	else if( x < 0 )
		return -1;
	
	return 0;
}

template <class T>
T sign(const T &a , const T &b) {

	if( b >= 0 )
		return abs(a);
	else
		return -abs(a);
}

template <class T>
T sqr(const T &x) {
	return x*x;
}

#endif

