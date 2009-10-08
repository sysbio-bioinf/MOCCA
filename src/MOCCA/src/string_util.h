#ifndef __STRING_UTIL_H__
#define __STRING_UTIL_H__

#include <string>
#include <sstream>

using namespace std;

bool is_dec(const string & str);
bool is_oct(const string & str);
bool is_hex(const string & str);
bool is_float(const string & str);

template< class T >
string to_string( const T & t ) {
	ostringstream 		os;
	os << t;
	return os.str();
}

template< class T >
bool from_string( const string & str, T & t ) {
	std::istringstream is( str );
	is >> t;
	return is.good();
}

#endif
