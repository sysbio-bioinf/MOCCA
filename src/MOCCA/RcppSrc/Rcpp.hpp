// Rcpp.hpp: Part of the R/C++ interface class library, Version 3.1
//
// Copyright (C) 2005-2006 Dominick Samperi
//
// This library is free software; you can redistribute it and/or modify it 
// under the terms of the GNU Lesser General Public License as published by 
// the Free Software Foundation; either version 2.1 of the License, or (at 
// your option) any later version.
//
// This library is distributed in the hope that it will be useful, but 
// WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public 
// License for more details.
//
// You should have received a copy of the GNU Lesser General Public License 
// along with this library; if not, write to the Free Software Foundation, 
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA 

#ifndef R_NO_REMAP
#define R_NO_REMAP
#endif

#ifndef Rcpp_hpp
#define Rcpp_hpp

#ifdef USING_QUANTLIB
#include <ql/quantlib.hpp>
using namespace QuantLib;
#else
#include <sstream>
#include <string>
#include <cstring>
#include <list>
#include <map>
#endif

#include <stdexcept>
#include <vector>

using namespace std;

#include <R.h>
#include <Rinternals.h>

#ifdef BUILDING_DLL
#define RcppExport extern "C" __declspec(dllexport)
#else
#define RcppExport extern "C"
#endif

char *copyMessageToR(const char* const mesg);

class RcppParams {
public:
    RcppParams(SEXP params);
    void   checkNames(char *inputNames[], int len);
    double getDoubleValue(string name);
    int    getIntValue(string name);
    string getStringValue(string name);
    bool   getBoolValue(string name);
#ifdef USING_QUANTLIB
    Date   getDateValue(string name);
#endif
private:
    map<string, int> pmap;
    SEXP _params;
};

class RcppNamedList {
public:
    RcppNamedList(SEXP theList) {
	if(!Rf_isNewList(theList))
	    throw std::range_error("RcppNamedList: non-list passed to constructor");
        len = Rf_length(theList);
        names = Rf_getAttrib(theList, R_NamesSymbol);
        namedList = theList;
    }
    string getName(int i) {
        if(i < 0 || i >= len) {
	    std::ostringstream oss;
	    oss << "RcppNamedList::getName: index out of bounds: " << i;
	    throw std::range_error(oss.str());
	}
        return string(CHAR(STRING_ELT(names,i)));
    }
    double getValue(int i) {
        if(i < 0 || i >= len) {
	    std::ostringstream oss;
	    oss << "RcppNamedList::getValue: index out of bounds: " << i;
	    throw std::range_error(oss.str());
	}
	SEXP elt = VECTOR_ELT(namedList, i);
	if(Rf_isReal(elt))
	    return REAL(elt)[0];
	else if(Rf_isInteger(elt))
	    return (double)INTEGER(elt)[0];
	else
	    throw std::range_error("RcppNamedList: contains non-numeric value");
	return 0; // never get here
    }
    int getLength() { return len; }
private:
    int len;
    SEXP namedList;
    SEXP names;
};

template <typename T>
class RcppVector {
public:
    RcppVector(SEXP vec);
    RcppVector(int len);
    int getLength() { return len; }
    inline T& operator()(int i) {
	if(i < 0 || i >= len) {
	    std::ostringstream oss;
	    oss << "RcppVector: subscript out of range: " << i;
	    throw std::range_error(oss.str());
	}
	return v[i];
    }
    T *cVector();
    vector<T> stlVector();
private:
    int len;
    T *v;
};

template <typename T>
class RcppMatrix {
public:
    RcppMatrix(SEXP mat);
    RcppMatrix(int nx, int ny);
    int getDim1() { return dim1; }
    int getDim2() { return dim2; }
    inline T& operator()(int i, int j) {
	if(i < 0 || i >= dim1 || j < 0 || j >= dim2) {
	    std::ostringstream oss;
	    oss << "RcppMatrix: subscripts out of range: " << i << ", " << j;
	    throw std::range_error(oss.str());
	}
	return a[i][j];
    }
    T **cMatrix();
    vector<vector<T> > stlMatrix();
private:
    int dim1, dim2;
    T **a;
};

class RcppResultSet {
public:
    RcppResultSet() { numProtected = 0; }
    void add(string, double);
    void add(string, int);
    void add(string, string);
    void add(string, double *, int);
    void add(string, int *, int);
    void add(string, double **, int, int);
    void add(string, int **, int, int);
    void add(string, vector<double>&);
    void add(string, vector<int>&);
    void add(string, vector<vector<double> >&);
    void add(string, vector<vector<int> >&);
    void add(string, RcppVector<int>&);
    void add(string, RcppVector<double>&);
    void add(string, RcppMatrix<int>&);
    void add(string, RcppMatrix<double>&);
    void add(string, SEXP, bool isProtected);
    SEXP getReturnList();
private:
    int numProtected;
    list<pair<string,SEXP> > values;
};

#endif
