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
RcppExport SEXP MOCCA_Lap(SEXP dataset, SEXP centersA, SEXP centersB)
{
  SEXP  rl=0;
  char* exceptionMesg=NULL;

  FloatMatrix data; // data set
  vector< FloatMatrix > cod(2);  // codebook vectors

  vector<int> rowsol,colsol;
  //vector<int> u,v;
  //vector< vector<int> > a;

  vector< Clusters > clusters;  // cluster hit lits
  Float mca;

  try
  {

    // format input
    RcppMatrix<Float> RcppDataset(dataset);
    RcppMatrix<Float> RcppCentersA(centersA);
    RcppMatrix<Float> RcppCentersB(centersB);
//  vector<vector<Float>> datasetC = matD.stlMatrix();
    data = RcppDataset.stlMatrix();
    cod[0] = RcppCentersA.stlMatrix();
    cod[1] = RcppCentersB.stlMatrix();

    clusters.assign(cod.size(),Clusters());
    for(int i=0; i<cod.size(); i++)
    {
      create_hit_list( data.begin(), data.end(), cod[i].begin(), cod[i].end(), clusters[i], LNorm<FloatVector,Float>(2.0) );
    }

    // calculate score matrix
    //compare_clusters(clusters[0],clusters[1],a);
    // calculate lap
    //lap(a,rowsol,colsol,u,v);
    mca = compare_mca(clusters[0],clusters[1],data.size(),rowsol,colsol);


    // Build result set to be returned as a list to R.
    RcppResultSet rs;
//    outputstring = s.str();
      rs.add("mca", mca);
      rs.add("rowsol", rowsol);
      rs.add("colsol", colsol);
      //rs.add("rn", u);
      //rs.add("cn", v);

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
