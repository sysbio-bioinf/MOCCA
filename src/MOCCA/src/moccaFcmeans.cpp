#include "Rcpp.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iterator>
#include <algorithm>
#include <set>
#include <list>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include "def.h"
#include "random.h"
#include "misc.h"
#include "metric.h"
#include "matrixio.h"
#include "matrixutil.h"
#include "fcmeans.h"
#include "kmeans.h"
using namespace std;

typedef vector<Float> FloatVector;
typedef vector<FloatVector> FloatMatrix;
RcppExport SEXP MOCCA_Fcmeans(SEXP params, SEXP dataset, SEXP centers)
{
  SEXP  rl=0;
  char* exceptionMesg=NULL;

  FloatMatrix data,cod,u;
  Float m,eps;
  int max_steps;
  int changed;
  Clusters clusters;
  Hits h;
  
  try
  {
    // Get parameters in params.
    RcppParams rparam(params);
    max_steps = rparam.getIntValue("maxsteps");
    m = rparam.getDoubleValue("m");
    eps = rparam.getDoubleValue("eps");

    // format input
    RcppMatrix<Float> RcppDataset(dataset);
    RcppMatrix<Float> RcppCenters(centers);
//  vector<vector<Float>> datasetC = matD.stlMatrix();
    FloatMatrix data = RcppDataset.stlMatrix();
    FloatMatrix cod = RcppCenters.stlMatrix();

    // start
    LNorm<FloatVector,Float> norm(2.0);
    changed = cluster_fcmeans(data, cod, u, norm, m, max_steps, eps);
    create_hit_list(data.begin(), data.end(), cod.begin(), cod.end(), clusters, norm);      	  
	clusters_to_hits(clusters,h,data.size());
	FloatVector cluster(data.size());
	Hits::iterator idx;
	int i=0;
	for(idx=h.begin();idx!=h.end();++idx)
		cluster[i++]=(*idx)+1;

	FloatVector clustersizes(clusters.size());
	for(int i=0;i<clusters.size();++i)
		clustersizes[i]=clusters[i].size();
    // Build result set to be returned as a list to R.
    RcppResultSet rs;
    rs.add("centers", cod);
    rs.add("membership", u);
    rs.add("steps", changed);
	rs.add("cluster", cluster);
	rs.add("size", clustersizes);
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
