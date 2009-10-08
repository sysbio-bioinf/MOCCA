/*
 * set_util.h
 *
 * written by Andre Mueller
 *
 * Utility functions for sets
 *
 * $Id: set_util.h,v 1.1 2001/05/22 13:44:39 andre Exp andre $
 * 
 * $Log: set_util.h,v $
 * Revision 1.1  2001/05/22 13:44:39  andre
 * Initial revision
 *
 *
 */
 
#ifndef __SET_UTIL_H__
#define __SET_UTIL_H__

#include <iterator>		/* distance(...)		*/

/*
	count_union(first1,last1,first2,last2)
	
	Returns the number of elements in the union set of [first1,last1) and [first2,last2)
	
	| A u B |
*/
template <class InputIterator1, class InputIterator2>
unsigned int count_union( InputIterator1 first1, 
   							InputIterator1 last1,
							InputIterator2 first2,
							InputIterator2 last2 ) {
	unsigned int result = 0;

	while (first1 != last1 && first2 != last2) {
		if (*first1 < *first2)
			++first1;
		else if (*first2 < *first1)
			++first2;
		else {
			++first1;
			++first2;
		}
		++result;
	}
			
	return result  + std::distance(first1,last1) + std::distance(first2,last2);
}

/*
	count_intersection(first1,last1,first2,last2)
	
	Returns the number of elements in the intersection set of [first1,last1) and [first2,last2)
	
	| A n B |
*/
template <class InputIterator1, class InputIterator2>
unsigned int count_intersection( InputIterator1 first1, 
    								InputIterator1 last1,
									InputIterator2 first2,
									InputIterator2 last2 ) {
	unsigned int result = 0;

	while (first1 != last1 && first2 != last2) {
		if (*first1 < *first2)
			++first1;
		else if (*first2 < *first1)
			++first2;
		else {
			++first1;
			++first2;
			++result;
		}
	}
			
	return result;
}

/*
	count_difference(first1,last1,first2,last2)
	
	Returns the number of elements in the difference set of [first1,last1) and [first2,last2)
	
	| A - B |
*/
template <class InputIterator1, class InputIterator2>
unsigned int count_difference( InputIterator1 first1, 
    							InputIterator1 last1,
								InputIterator2 first2,
								InputIterator2 last2 ) {
	unsigned int result = 0;
	
	while (first1 != last1 && first2 != last2) {
		if (*first1 < *first2) {
			++first1;
			++result;
		}
		else if (*first2 < *first1)
			++first2;
		else {
			++first1;
			++first2;
		}
	}
	
	/* now count the rest */	
	return result + std::distance(first1,last1);
}

/*
	count_symmetric_difference(first1,last1,first2,last2)
	
	Returns the number of elements in the union set of [first1,last1) and [first2,last2)
	
	| A - B | + | B - A |
*/
template <class InputIterator1, class InputIterator2>
unsigned int count_symmetric_difference( InputIterator1 first1, 
    							InputIterator1 last1,
								InputIterator2 first2,
								InputIterator2 last2 ) {
	unsigned int result = 0;
	
	while (first1 != last1 && first2 != last2) {
		if (*first1 < *first2) {
			++first1;
			++result;
		}
		else if (*first2 < *first1) {
			++first2;
			++result;
		}
		else {
			++first1;
			++first2;
		}
	}
	
	return result + std::distance(first1,last1) + std::distance(first2,last2);
}

#endif

