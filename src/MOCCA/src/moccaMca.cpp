#include "Rcpp.hpp"
#include "def.h"
#include "random.h"
#include "misc.h"
#include "matrixio.h"
#include "matrixutil.h"
#include "hitlist.h"
#include "lap.h"
#include "nn.h"
#include "set_util.h"
#include "cluster_validation.h"
#include "statistics.h"
/* standard template library */
#include <vector>
#include <iterator>
#include <algorithm>
#include <set>
#include <list>
/* c++ libraries */
#include <string>
#include <cstdlib>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <cstdio>
#include <stdexcept>
using namespace std;

typedef vector<Float> FloatVector;
typedef vector<FloatVector> FloatMatrix;
RcppExport SEXP MOCCA_Mca(SEXP clusterA, SEXP clusterB)
{
  SEXP  rl=0;
  char* exceptionMesg=NULL;

	vector<int> thisclusterA;
        vector<int> thisclusterB;
	vector<int> rowsol,colsol;

	int nclust;
	HitList::value_type idx;
	
  vector< Clusters > clusters(2);  // cluster hit lits
  Float mca_value;

  try
  {
		
    // format input
    RcppVector<int> RcppClusterA(clusterA);
    RcppVector<int> RcppClusterB(clusterB);

		thisclusterA = RcppClusterA.stlVector();
		thisclusterB = RcppClusterB.stlVector();

		int min=thisclusterA[0];
		int max=thisclusterA[0];
		for(int i=0;i<thisclusterA.size();i++){
			if(thisclusterA[i] > max)
				max = thisclusterA[i];
			if(thisclusterA[i] < min)
				min = thisclusterA[i];
		}
		nclust=max-min+1;

		std::fill_n( std::back_inserter(clusters[0]) , nclust , HitList());
		idx = 0;
		for(int i=0;i<thisclusterA.size();i++){
			clusters[0][ thisclusterA[i]-min ].push_back( idx );
			++idx;
		}

		std::fill_n( std::back_inserter(clusters[1]) , nclust , HitList());
		idx = 0;
		for(int i=0;i<thisclusterB.size();i++){
			clusters[1][ thisclusterB[i]-min ].push_back( idx );
			++idx;
		}

		
//     clusters.assign(cod.size(),Clusters());
//     for(int i=0; i<cod.size(); i++)
//     {
//       create_hit_list( data.begin(), data.end(), cod[i].begin(), cod[i].end(), clusters[i], LNorm<FloatVector,Float>(2.0) );
//     }


    // start
    /* compare the clusterings */

    /*
      calculate MAXIMUM CLUSTER ASSIGNMENT
    */
      mca_value = compare_mca(clusters[0],clusters[1],thisclusterA.size(),rowsol,colsol);


    // Build result set to be returned as a list to R.
    RcppResultSet rs;
//    outputstring = s.str();
      rs.add("mca", mca_value);
      rs.add("rowsol", rowsol);
      rs.add("colsol", colsol);

    // Get the list to be returned to R.
    rl = rs.getReturnList();
  } catch(std::exception& ex) {
      exceptionMesg = copyMessageToR(ex.what());
  } catch(...) {
      exceptionMesg = copyMessageToR("unknown reason");
  }

  if(exceptionMesg != NULL)
      Rf_error(exceptionMesg);

  return rl;
}
