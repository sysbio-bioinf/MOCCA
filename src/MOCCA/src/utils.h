/*
 * utils.h
 *
 * writte by Andre Mueller
 *
 * $Id$
 *
 * $Log$
 *
 */
 
#ifndef __UTILS_H__
#define __UTILS_H__

#include <iterator>

namespace utils {

/*
	copy_ref(first,last,result)
	
	Copy references (pointers) from the elements [first,last) to result.
	
	The return value is 
		result + (last - first)
*/
template <class InputIterator, class OutputIterator>
OutputIterator copy_ref(InputIterator first, InputIterator last, OutputIterator result) {
	while(first!=last) {
		*result = & (*first);
		++first;
		++result;
	}
	return result;
}

/*
	add(first,last,result)
	
	adds the elements *(first+i) to *(result+i)
	returns result+(last-first)
	
*/
template <class InputIterator, class OutputIterator>
OutputIterator add(InputIterator first, InputIterator last, OutputIterator result) {
	while(first!=last) {
		*result += (*first);
		++first;
		++result;
	}
	return result;
}

};

#endif

