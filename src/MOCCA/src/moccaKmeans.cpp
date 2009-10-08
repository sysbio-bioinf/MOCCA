#include <vector>
#include <iterator>
#include <algorithm>
#include <set>
#include <list>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include "Rcpp.hpp"
#include "kmeans.h"
#include "def.h"
#include "matrixio.h"
#include "metric.h"
#include "random.h"
#include "misc.h"
using namespace std;

typedef vector<Float> FloatVector;
typedef vector<FloatVector> FloatMatrix;
RcppExport SEXP MOCCA_Kmeans(SEXP params, SEXP dataset, SEXP centers)
{
  SEXP  rl=0;
  char* exceptionMesg=NULL;
  int changed;

  int lnorm;
  string filename;
  FloatMatrix data,
              cod;
  Clusters clusters;
  Hits h;

  try
  {
    // Get parameters in params.
    RcppParams rparam(params);
    int max_steps = rparam.getIntValue("maxsteps");
    string metric = rparam.getStringValue("metric");

    // format input
    RcppMatrix<Float> RcppDataset(dataset);
    RcppMatrix<Float> RcppCenters(centers);
    data = RcppDataset.stlMatrix();
    cod = RcppCenters.stlMatrix();

    // function call
    if(metric.compare("lnorm")==0){
      lnorm = rparam.getIntValue("lnorm");
      changed = cluster_kmeans(data, cod, max_steps, LNorm<FloatVector,Float>(lnorm));
      create_hit_list(data.begin(), data.end(), cod.begin(), cod.end(), clusters, LNorm<FloatVector,Float>(lnorm));      	  
    }
    if(metric.compare("hamming")==0){
      changed = cluster_kmeans(data, cod, max_steps, Hamming<FloatVector,Float>());
      create_hit_list(data.begin(), data.end(), cod.begin(), cod.end(), clusters, Hamming<FloatVector,Float>());
    }
    if(metric.compare("filename")==0)
    {
      filename = rparam.getStringValue("filename");
      /* READ METRIC FILE */
      FloatMatrix M;
      if(!read_matrix(M,filename.c_str()))
      {
      	string errorstring = "Cannot load metric from file ";
      	errorstring += filename;
      	exceptionMesg = copyMessageToR(errorstring.c_str());
		Rf_error(exceptionMesg);
		return rl;
      }
      changed = cluster_kmeans(data, cod, max_steps, IndexedMetric<FloatVector,Float,FloatMatrix>(M));
      create_hit_list(data.begin(), data.end(), cod.begin(), cod.end(), clusters, IndexedMetric<FloatVector,Float,FloatMatrix>(M));
    }
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
    rs.add("steps", changed);
    rs.add("centers", cod);
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
