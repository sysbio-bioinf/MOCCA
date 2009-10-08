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
RcppExport SEXP MOCCA_Predict_Clusters(SEXP dataset, SEXP centers)
{
  SEXP  rl=0;
  char* exceptionMesg=NULL;

  FloatMatrix data; // data set
  vector< FloatMatrix > cod(1);  // codebook vectors
//  cod.resize(2);
  vector< Clusters > clusters;  // cluster hit lits

  try
  {

    // format input
    RcppMatrix<Float> RcppDataset(dataset);
    RcppMatrix<Float> RcppCenters(centers);
//  vector<vector<Float>> datasetC = matD.stlMatrix();
    data = RcppDataset.stlMatrix();
    cod[0] = RcppCenters.stlMatrix();

//     clusters.assign(cod.size(),Clusters());
//     for(int i=0; i<cod.size(); i++)
//     {
//       create_hit_list( data.begin(), data.end(), cod[i].begin(), cod[i].end(), clusters[i], LNorm<FloatVector,Float>(2.0) );
//     }



    clusters.assign(cod.size(),Clusters());
    for(int i=0; i<cod.size(); i++)
    {
      create_hit_list( data.begin(), data.end(), cod[i].begin(), cod[i].end(), clusters[i], LNorm<FloatVector,Float>(2.0) );
    }

		Hits hits;
		
		clusters_to_hits( clusters[0], hits, data.size() );

		vector<int> clustering(hits.size());
		
		for(int i=0; i<hits.size();i++){
			clustering[i] = hits[i]+1;
		}

		
    // Build result set to be returned as a list to R.
    RcppResultSet rs;
//    outputstring = s.str();
      rs.add("cluster", clustering);

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
