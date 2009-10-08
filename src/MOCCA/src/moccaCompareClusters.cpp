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
RcppExport SEXP MOCCA_Compare_Clusters(SEXP dataset, SEXP centersA, SEXP centersB)
{
  SEXP  rl=0;
  char* exceptionMesg=NULL;

  FloatMatrix data; // data set
  vector< FloatMatrix > cod(2);  // codebook vectors
//  cod.resize(2);
  vector< Clusters > clusters;  // cluster hit lits
  Float mca_value, jaccard_value, rand_value, trans_value, sym_value, qerror_value;
  vector< Float > self_value(2), intra_value(2), inter_value(2);

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
    // start
    /* compare the clusterings */

    /*
      calculate MAXIMUM CLUSTER ASSIGNMENT
    */
      mca_value = compare_mca(clusters[0],clusters[1],data.size());

    /*
      calculate JACCARD INDEX
    */

      long a,b,c,d;
      contingency_table(clusters[0],clusters[1],data.size(),a,b,c,d);
      jaccard_value = (Float)(a)/(Float)(a+b+c);  // JACCARD INDEX

    /*
      calculate RAND INDEX
    */

      rand_value = (Float)(a+d)/(Float)(a+b+c+d); // RAND INDEX

    /*
      calculate Transinformation
    */

      trans_value = compare_trans( clusters[0], clusters[1], data.size() );

    /*
      calculate Selfinformation
    */

      self_value[0] = self_information( clusters[0], data.size() );
      self_value[1] = self_information( clusters[1], data.size() );

    /*
      calculate symmetric difference
    */

      sym_value = compare_symmdiff(clusters[0],clusters[1]);

    /*
      calculate the cross quantization error
    */

      Hits h1, h2;
      clusters_to_hits( clusters[0], h1, data.size() );
      clusters_to_hits( clusters[1], h2, data.size() );
      qerror_value = compare_crossqerr( h1, h2, cod[0], cod[1], LNorm<FloatVector,Float>(2.0) );

    /*
      intra cluster correlation
    */

      intra_value[0] = intra_cluster_corr( clusters[0], data );
      intra_value[1] = intra_cluster_corr( clusters[1], data );

		/*
      inter cluster correlation
    */

      inter_value[0] = inter_cluster_corr( clusters[0], data );
      inter_value[1] = inter_cluster_corr( clusters[1], data );

    // Build result set to be returned as a list to R.
    RcppResultSet rs;
//    outputstring = s.str();
      rs.add("mca", mca_value);
      rs.add("jaccard", jaccard_value);
      rs.add("rand", rand_value);
      rs.add("trans", trans_value);
      rs.add("self", self_value);
      rs.add("sym.diff", sym_value);
      rs.add("cross.quant.error", qerror_value);
      rs.add("intra.cluster.cor", intra_value);
      rs.add("inter.cluster.cor", inter_value);

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
