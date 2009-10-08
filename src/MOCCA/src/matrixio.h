/*
 * matrixio.h
 *
 * I/O routines for matrix handling
 *
 * $Id: matrixio.h,v 1.3 2001/06/21 06:41:04 andre Exp andre $
 *
 * $Log: matrixio.h,v $
 * Revision 1.3  2001/06/21 06:41:04  andre
 * Implementation of some methods for writing pairs of vectors and
 * nominal values.
 * TODO: The read_matrix... routines should check if the values in
 * the input stream are really floating points / nominal values.
 *
 * Revision 1.2  2001/05/21 09:43:17  andre
 * *** empty log message ***
 *
 *
 */

/*
	There are various data formats to be read by read_matrix, read_matrix2 and read_matrix2_nominal

*/
#ifndef __IOMATRIX_H__
#define __IOMATRIX_H__

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <list>
#include <map>
#include <iterator>
#include <algorithm>
#include <utility>
#include <string>
#include <vector>
#include <cstdlib>

using namespace std;

#include "error_handler.h"
#include "def.h"
#include "string_util.h"


enum MatrixFileFormat {
	format_csv = 0,
	format_binary = 1
};


// help functions
void skip_line(istream &f);
void skip_empty_lines(istream &f);
int count_words(istream &f);
int get_words(istream &f, list<string> &words);


/*
	write_matrix(first,last,out)

	Writes the vectors in the range [first,last) row by row to the output stream out
*/
template <class InputIterator>
void write_matrix(InputIterator first, InputIterator last, ostream &out, bool write_header = true, MatrixFileFormat format = format_csv ) {

	typedef typename iterator_traits< InputIterator >::value_type 	Vector;
	typedef typename Vector::value_type						value_type;

	switch( format ) {

		case format_csv:

			if(write_header)
				out << std::distance( (*first).begin() , (*first).end() ) << endl;

			out.setf( ios::fmtflags(0), ios::floatfield );
			out.width( 0 );
			out.precision( 10 );

			while( first != last ) {
				std::copy( (*first).begin(),(*first).end(),
					ostream_iterator< Float >(out," ") );
				++first;
				out<<endl;
			}

		break;

		case format_binary:
		{
			// write header

			unsigned long	sz;

			sz = distance( first, last );
			out.write( reinterpret_cast< const char *>( & sz ), sizeof( sz ) );

			// write binary data

			typename Vector::const_iterator	efirst, elast;

			while( first != last ) {

				efirst = (*first).begin();
				elast  = (*first).end();

				sz	   = distance( efirst, elast );

				out.write( reinterpret_cast< const char *>( & sz ), sizeof( sz ) );

				while( efirst != elast ) {

					out.write( reinterpret_cast< const char *>( &(*first) ), sizeof(value_type) );

					++efirst;
				}					
				++first;
			}
			
		}
		break;
		
		default:
		
			_AssertParam(false,"write_matrix");

	}
}

template <class Sequence>
void write_matrix(const Sequence &A, ostream &out, bool write_header = true) {
	write_matrix(A.begin(),A.end(),out,write_header);
}


template <class Sequence>
void write_matrix(const Sequence &A, const char *fname, bool write_header = true) {
	ofstream out(fname);
	if(!out)
		throw "write_matrix(const Sequence &A, const char *fname, bool write_header = true) : cannot open file";
	write_matrix(A.begin(),A.end(),out,write_header);
	out.close();
}


/*
	read_matrix(A,file)
	
	Reads a matrix from a text file
*/
template <class Sequence>
bool read_matrix(Sequence &A, istream &in) {
	int ncols,nrows;
	typedef typename Sequence::value_type Vector;
	
	if(!in)
		return false;
	
	ncols = nrows = 0;

	long pos;
	
	skip_empty_lines(in);
	pos = in.tellg();
	
	// get words in the first data line
	// 
/*
	int nw = count_words(in);

	if(nw == 1) {
		// TODO : check if we get an int or a float value
		in >> ncols;
	} else {
		ncols = nw;
		in.seekg(pos);			
	}
	
*/

	list<string> words;
	get_words(in,words);
	
	list<string>::const_iterator first,last;
	first = words.begin();
	last = words.end();
	
	
	// consists the line out of numbers??
	bool only_numbers = true;
	while(first != last) {
	
		if(!is_dec(*first) && !is_float(*first)) {
			only_numbers = false;
			break;
		}

		++first;
	}

/*	
	cout<<"HEADER\n";
	first = words.begin();
	while(first!=last){
		cout<<"<"<<(*first)<<">\n";
		++first;
	}
*/
	
	first = words.begin();

	bool has_header = false;		
	if( only_numbers ) {
	
		if(words.size() == 1) {
		
			if(is_float(*first)) {
				ncols = 1;
			} else {
				ncols = atoi( (*first).c_str() );
				has_header = true;
			}
				
		} else {
			ncols = words.size();
		}	
	} else {
		// it IS a header
		ncols = atoi( (*first).c_str() );
		has_header = true;
	}
	
	
	if(!has_header)
		in.seekg(pos);

/*	
	cout<<"NCOLS = "<<ncols<<endl;
	cout<<"has_header = " << has_header<<endl;
*/

	/*
	f >> nrows;
	if(f.bad() || (nrows<0))
		nrows = 0;
	*/
//	skip_line(in);
	
	in.clear();
	if(nrows <= 0) {
	
		/* count rows in the file */
		skip_empty_lines(in);
		pos = in.tellg();
		
		nrows = 0;
		while(!in.eof()) {
			skip_empty_lines(in);
			if(!in.eof())
				nrows++;
			else
				break;
			skip_line(in);
		}
		in.clear();
		in.seekg(pos);
	}
	
	/* set matrix dimensions */
	A.resize(nrows);
	
	typename Sequence::iterator iter;
	typename Sequence::value_type::iterator viter;

	for(iter=A.begin(); iter!=A.end(); ++iter) {
		// (*iter).resize(ncols);
		(*iter) = Vector(ncols);
		skip_empty_lines(in);
		for(viter=(*iter).begin(); viter!=(*iter).end(); ++viter)
			in >> (*viter);
			
		skip_line(in);
	}
		
	return true;
}

template <class Sequence>
bool read_matrix(Sequence &A, const char *fname) {
	ifstream in(fname);
	if(!in)
		return false;

	return read_matrix(A,in);
}


/*
	write_matrix(first1,last1,first2,last2,out)
	
	Writes the vectors in the range [first,last) row by row to the output stream out
*/
template <class InputIterator1,class InputIterator2>
void write_matrix2(InputIterator1 first1, InputIterator1 last1,
	InputIterator2 first2, InputIterator2 last2,
	ostream &out, bool write_header = true) {
	
	if( (last1-first1) != (last2-first2) ) {
		throw length_error("length of first sequence doesn't match length of second sequence");
	}
	
	if(write_header)
		out << std::distance( (*first1).begin() , (*first1).end() ) << endl;
		//	<< " " << std::distance(first1,last1) << endl;

	out.setf( ios::fmtflags(0), ios::floatfield );
	out.width( 0 );
	out.precision( 10 );

	while(first1!=last1) {
		std::copy( (*first1).begin(), (*first1).end(),
			ostream_iterator< Float >(out," ") );
			
		std::copy( (*first2).begin(), (*first2).end(),
			ostream_iterator< Float >(out," ") );
			
		++first1;
		++first2;
		out<<endl;
	}
}

template <class Sequence1, class Sequence2>
void write_matrix2(const Sequence1 &A, const Sequence2 &B, ostream &out, bool write_header = true) {
	write_matrix2(A.begin(), A.end(), B.begin(), B.end(), out,write_header);
}


template <class Sequence1, class Sequence2>
void write_matrix2(const Sequence1 &A, const Sequence2 &B, const char *fname, bool write_header = true) {
	ofstream out(fname);
	if(!out)
		throw "write_matrix(const Sequence &A, const char *fname, bool write_header = true) : cannot open file";
	write_matrix2(A.begin(),A.end(),B.begin(),B.end(),out,write_header);
	out.close();
}


/*
	read_matrix(A,B,file)
	
	Reads a matrix from a text file
*/
template <class Sequence1,class Sequence2>
bool read_matrix2(Sequence1 &A, Sequence2 &B, istream &in) {
	int ncolsA,ncolsB,nrows,n;
	typedef typename Sequence1::value_type Vector1;
	typedef typename Sequence2::value_type Vector2;	
	
	if(!in)
		return false;
	
	ncolsA = ncolsB = nrows = 0;

	skip_empty_lines(in);
	in >> ncolsA;

	/*
	f >> nrows;
	if(f.bad() || (nrows<0))
		nrows = 0;
	*/
	skip_line(in);
	
	in.clear();
	if(nrows <= 0) {
	
		/* count rows in the file */
		skip_empty_lines(in);
		long pos = in.tellg();
		
		nrows = 0;
		while(!in.eof()) {
			skip_empty_lines(in);
			if(!in.eof())
				nrows++;
			else
				break;
				
			// count words in this line (and eats the line)
			n = count_words(in);
			if(ncolsB == 0) {
				ncolsB = n - ncolsA;
				if(ncolsB < 0) {
					throw length_error("matrixio.h : read_matrix2(...) target dimension is 0 !!!");
				}
			} else {
				if(n - ncolsA != ncolsB)
					throw length_error("matrixio.h : read_matrix2(...) target dimension doesn't match original target dimension at line ");
			}
		}
		in.clear();
		in.seekg(pos);
	}

//	A = Matrix(nrows,ncols);
	/* set matrix dimensions */
	A.resize(nrows);
	B.resize(nrows);	
	
	typename Sequence1::iterator first1,last1;
	typename Sequence1::value_type::iterator viter1;
	
	typename Sequence2::iterator first2,last2;
	typename Sequence2::value_type::iterator viter2;

	first1 = A.begin();
	last1 = A.end();
	first2 = B.begin();
	last2 = B.end();

	while( first1 != last1 ) {
	
		*first1 = Vector1(ncolsA);
		*first2 = Vector2(ncolsB);

		skip_empty_lines(in);

		for(viter1=(*first1).begin(); viter1!=(*first1).end(); ++viter1)
			in >> (*viter1);
		for(viter2=(*first2).begin(); viter2!=(*first2).end(); ++viter2)
			in >> (*viter2);

		skip_line(in);

		++first1;
		++first2;
	}
		
	return true;
}

template <class Sequence1, class Sequence2>
bool read_matrix2(Sequence1 &A, Sequence2 &B, const char *fname) {
	ifstream in(fname);
	return read_matrix2(A,B,in);
}

/*
	
*/
template <class InputIterator1,class InputIterator2>
void write_matrix2_nominal(
	InputIterator1 first1, InputIterator1 last1,
	InputIterator2 first2, InputIterator2 last2,
	ostream &out, bool write_header = true) {
	
	if( std::distance(first1,last1) != std::distance(first2,last2) ) {
		throw length_error("length of first sequence doesn't match length of second sequence");
	}
	
	if(write_header)
		out << std::distance( (*first1).begin() , (*first1).end() ) << endl;
		//	<< " " << std::distance(first1,last1) << endl;
		
	out.setf( ios::fmtflags(0), ios::floatfield );
	out.width( 0 );
	out.precision( 10 );
	
	while(first1!=last1) {
		std::copy( (*first1).begin(), (*first1).end(),
			ostream_iterator< Float >(out," ") );
			
		out << (*first2);
			
		++first1;
		++first2;
		out<<endl;
	}
}

template <class Sequence1, class Sequence2>
void write_matrix2_nominal(const Sequence1 &A, const Sequence2 &B, ostream &out, bool write_header = true) {
	write_matrix2_nominal(A.begin(), A.end(), B.begin(), B.end(), out, write_header);
}


template <class Sequence1, class Sequence2>
void write_matrix2_nominal(const Sequence1 &A, const Sequence2 &B, const char *fname, bool write_header = true) {
	ofstream out(fname);
	if(!out)
		throw runtime_error("write_matrix(const Sequence &A, const char *fname, bool write_header = true) : cannot open file");
	write_matrix2_nominal(A.begin(), A.end(), B.begin(), B.end(), out, write_header);
	out.close();
}

/*
	read_matrix(A,B,file)
	
	Reads a matrix from a text file
*/
template <class Sequence1,class Sequence2>
bool read_matrix2_nominal(Sequence1 &A, Sequence2 &B, istream &in) {
	int ncolsA,ncolsB,nrows,n;
	typedef typename Sequence1::value_type Vector1;
	typedef typename Sequence2::value_type Vector2;	
	
	if(!in)
		return false;
	
	ncolsA = ncolsB = nrows = 0;

	skip_empty_lines(in);
	in >> ncolsA;

	/*
	f >> nrows;
	if(f.bad() || (nrows<0))
		nrows = 0;
	*/
	skip_line(in);
	
	in.clear();
	if(nrows <= 0) {
	
		/* count rows in the file */
		skip_empty_lines(in);
		long pos = in.tellg();
		
		nrows = 0;
		while(!in.eof()) {
			skip_empty_lines(in);
			if(!in.eof())
				nrows++;
			else
				break;
				
			// count words in this line (and eats the line)
			n = count_words(in);
			if(ncolsB == 0) {
				ncolsB = n - ncolsA;
				if(ncolsB != 1) {
					throw length_error("target dimension is != 1 (has to be fulfilled when using nominal values) !!!");
				}
			} else {
				if(n - ncolsA != ncolsB)
					throw length_error("target dimension doesn't match original target dimension at line ");
			}
		}
		in.clear();
		in.seekg(pos);
	}
	
//	A = Matrix(nrows,ncols);
	/* set matrix dimensions */
	A.resize(nrows);
	B.resize(nrows);	
	
	typename Sequence1::iterator first1,last1;
	typename Sequence1::value_type::iterator viter1;
	
	typename Sequence2::iterator first2,last2;

	typedef typename Sequence2::value_type ClassIndex;

	std::string str;
	std::map< std::string , ClassIndex > db;
	typename std::map< std::string , ClassIndex >::iterator iter;
	
	first1 = A.begin();
	last1 = A.end();
	first2 = B.begin();
	last2 = B.end();
	
	ClassIndex classindex = 0;

	while( first1 != last1 ) {
		*first1 = Vector1(ncolsA);
		*first2 = Vector2(ncolsB);

		skip_empty_lines(in);

		for(viter1=(*first1).begin(); viter1!=(*first1).end(); ++viter1)
			in >> (*viter1);
			
			
		// get nominal value
		in >> str;
			
		// TODO : Check if str is a float value. Give out a warning in this case like
		// "Input file do"

		/*		
		if( is_dec(str) ) {
			// class id is a decimal number
			*first2 = atoi(str.c_str());
			
		} else {
		*/
			// check database
			iter = db.find(str);
			if( iter == db.end() ) {
				// new db entry
				db[ str ] = classindex;
				++classindex;					
			}
			
			*first2 = db[ str ];
		// }

		skip_line(in);

		++first1;
		++first2;
	}
		
	return true;
}

template <class Sequence1, class Sequence2>
bool read_matrix2_nominal(Sequence1 &A, Sequence2 &B, const char *fname) {
	ifstream in(fname);
	if(!in)
		return false;
	
	return read_matrix2_nominal(A,B,in);
}

template<class Vector>
void write_vector(const Vector &v, ostream &out) {
	typename Vector::const_iterator first = v.begin(),	last = v.end();
	
	while(first!=last){
	//	out.form("%e ",(Float)(*first));
		out<<(*first)<<" ";
		++first;
	}
	
//	std::copy(v.begin(),v.end(),ostream_iterator<typename Vector::value_type>(out," "));
	
}


template<class Vector>
void write_vector(const Vector &v, const char *str) {
	ofstream f(str);
	if(f)
		write_vector(v,f);
}

template<class Vector>
bool read_vector(Vector &vec, istream &in) {

	vec.clear();
	
	Float v;
	while(in) {
		in>>v;
		if(in)
			vec.push_back(v);
	}
	
	return true;
}

template<class Vector>
bool read_vector(Vector &vec, const char *fname) {
	ifstream in(fname);
	
	return read_vector(vec,in);
}

template<class OutputIterator>
bool read_vector_iter(OutputIterator &out, istream &in) {
//	std::iterator_traits< OutputIterator >::value_type v;
	Float v;
	while(in) {
		in>>v;
		if(in){
			*out = v;
			++out;
		}
	}
	return true;
}

template<class OutputIterator>
bool read_vector_iter(OutputIterator &out, const char *fname) {
	ifstream in(fname);
	
	return read_vector(out,in);
}


#endif
