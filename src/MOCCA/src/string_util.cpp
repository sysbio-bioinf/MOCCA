#include "string_util.h"

#include <cctype>
using namespace std;

#include <regex.h>
#include <assert.h>

// [+-]?[1-9][0-9]+
//
bool is_dec(const string & str) {
	regex_t reg;
	regmatch_t pmatch[1];
	
	pmatch[0].rm_so = -1;
	pmatch[0].rm_eo = -1;
	
	assert( regcomp(&reg, "^[+-]?([1-9][0-9]+|[0-9])$",REG_EXTENDED) == 0 );

	bool match;
	
	match = ( regexec(&reg, str.c_str(), 1, pmatch , 0) == 0 );

	regfree(&reg);
	
	return match;
}

// 0[xX][:xdigit:]+
//
bool is_hex(const string & str) {

	regex_t reg;
	regmatch_t pmatch[1];
	
	pmatch[0].rm_so = -1;
	pmatch[0].rm_eo = -1;
	
//	assert( regcomp(&reg, "^0[xX][0-9a-fA-F]+$",REG_EXTENDED) == 0 );
	assert( regcomp(&reg, "^0[xX][:xdigit:]+$",REG_EXTENDED) == 0 );
		
	bool match;
	
	match = ( regexec(&reg, str.c_str(), 1, pmatch , 0) == 0 );

	regfree(&reg);
	
	return match;
}

// 0[0-9]+
//
bool is_oct(const string & str) {
	regex_t reg;
	regmatch_t pmatch[1];
	
	pmatch[0].rm_so = -1;
	pmatch[0].rm_eo = -1;
	
	assert( regcomp(&reg, "^0[0-9]+$",REG_EXTENDED) == 0 );

	bool match;
	
	match = ( regexec(&reg, str.c_str(), 1, pmatch , 0) == 0 );

	regfree(&reg);
	
	return match;
}

// [+-]?[0-9]+(.[0-9]*)?([eE][+-]?[0-9]{1,3})
//
bool is_float(const string & str) {
	
	regex_t reg;
	regmatch_t pmatch[1];
	
	pmatch[0].rm_so = -1;
	pmatch[0].rm_eo = -1;
	
	assert( regcomp(&reg, "^[+-]?[0-9]+((\\.[0-9]*)|([eE][+-]?[0-9]{1,3})|(\\.[0-9]*[eE][+-]?[0-9]{1,3}))$",REG_EXTENDED) == 0 );

	bool match;
	
	match = ( regexec(&reg, str.c_str(), 1, pmatch , 0) == 0 );

	regfree(&reg);
	return match;
}


