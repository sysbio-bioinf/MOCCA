#include <iostream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <iterator>
#include "fcmeans.h"
#include "Rcpp.hpp"
#include "def.h"
#include "misc.h"
#include "random.h"
#include "cluster_validation.h"
#include "hitlist.h"
#include "matrixio.h"
#include "statistics.h"
#include "set_util.h"
#include "metric.h"
#include "init.h"

using namespace std;

typedef vector<Float> FloatVector;
typedef vector<FloatVector> FloatMatrix;

// Johann Kraus 30.05.08
void random_stirling_partition( Clusters & A, long nsamples, long nclusters ) {



// randomclusters<-function(n,k){
// cluster.lengths<-c();for(i in 1:(k-1)){cluster.lengths<-c(cluster.lengths,sample(1:(n-k+i-sum(cluster.lengths)))[1])};cluster.lengths<-c(cluster.lengths,n-sum(cluster.lengths));sample(cluster.lengths)}
// 
// randomlabel<-function(p){n<-sum(p);z<-replicate(length(p),list(0));for(i in 1:length(p)){z[[i]]<-sample((which(!is.element((1:n),unlist(z)))))[1:p[i]]};z}


	A.clear();
	A.resize(nclusters);
	vector < int > samples;
	int idx;
	int maxsize;

	for(int i=0;i<nsamples;i++){
		samples.push_back(i);
	}

	//cout << "sample contains:";
	//for (int i=0; i<samples.size(); i++)
	//	cout << " " << samples[i];
	//cout << endl;

	for(int i = 0; i < nclusters-1; i++){
		maxsize = Random(samples.size()-nclusters+i+1)+1;
		for(int k = 0; k < maxsize; k++){
			idx = Random(samples.size());
			A[ i ].push_back(samples[idx]);
			samples.erase(samples.begin()+idx);
		}
	}

	for(int i=0; i<samples.size(); i++){
		A[nclusters-1].push_back(samples[i]);
	}

	for(int i=0; i<nclusters;i++){
		A[i].swap(A[Random(nclusters)]);
	}

	for(int i=0; i<nclusters;i++){
		sort(A[i].begin(),A[i].end());
	}
	
	//for(int k=0;k<nclusters;k++){
	//	cout << "myvector contains:";
	//	for (int i=0; i<A[k].size(); i++)
	//		cout << " " << A[k][i];
	//	cout << endl;
	//}
}

// Johann Kraus 30.05.08
double theoretical_mca(long nsamples, long nclusters){
	if((double)nclusters<ceil((double)nsamples/2.0))
		return ceil((double) nsamples / (double) nclusters) / (double) nsamples;
	else
		return ceil((double) nsamples / ((double) nsamples - (double) nclusters + 1.0 )) / (double) nsamples;
}

// Johann Kraus 30.05.08
void random_unique_partition( Clusters & A, long nsamples, long nclusters ) {
  A.clear();
  A.resize(nclusters);
  for(int i = 0; i < nsamples; ++i)
    A[ Random(nclusters) ].push_back(i);
  // check if there are empty clusters
  for(int i = 0; i < nclusters; i++) {
    if(A[i].empty()) {
      // find a nonempty cluster with more than 2 samples
      unsigned int j;
      do {
        j = Random( nclusters );
        if( A[j].size() > 1 ) {
          A[i].push_back( *A[j].begin() );
          A[j].erase( A[j].begin() );
          break;
        }
      } while(true);
    }
  }


	//for(int k=0;k<nclusters;k++){
	//	cout << "myvector contains:";
	//	for (int i=0; i<A[k].size(); i++)
	//		cout << " " << A[k][i];
	//	cout << endl;
	//}

}

template< class Matrix>// , class Norm >
void random_partition( const Matrix &data, Clusters &A, long nclusters , string init ) {
  Matrix cod;
  if(init.compare("even")==0)
    init_even(data, cod, nclusters, Uniform01<Float>);
  else if(init.compare("random")==0)
	init_random(data, cod, nclusters, RandomT<int>);
  else if(init.compare("maxmin")==0)
    init_maxmin(data, cod, nclusters, RandomT<unsigned int>, LNorm<vector< Float >,Float>(2));
  else if(init.compare("fcmeans")==0)
  {
    /* cluster data */
    FloatMatrix u;
    cluster_fcmeans( data, cod, u, LNorm<FloatVector>(2.0), 1.4, 20, 1E-5 );
  }
  A.clear();
  A.resize(nclusters);
  create_hit_list( data.begin(), data.end(), cod.begin(), cod.end(), A, LNorm< vector< Float > >(2.0) );
}

RcppExport SEXP MOCCA_Mca_Baseline(SEXP params, SEXP dataset)
{
  SEXP  rl=0;
  char* exceptionMesg=NULL;

  SetRandomGenerator(RND_FIBONACCI);
  unsigned long seed = time(0L);
  FloatMatrix X;
  vector<Float> mca_list;
  int num;

// 	vector < Float > cluster1;
// 	vector < Float > cluster2;
// 	vector < Float > cluster3;
	
  try
  {
    RcppMatrix<Float> RcppDataset(dataset);
    X = RcppDataset.stlMatrix();
    num = X.size();

    // Get parameters in params.
    RcppParams rparam(params);
    RandomSeed( seed );
    int runs = rparam.getIntValue("runs");
	int nclusters = rparam.getIntValue("noc");
    bool pairs = rparam.getBoolValue("pairwise"); // pairwise or paired
	string type = rparam.getStringValue("method"); // random label or random cluster hypothesis
    string init_method = rparam.getStringValue("init"); // init method for databased cluster hypothesis

    // start
      if(pairs) {
        /* create RUN partitions and produce pairwise comparations */
// Johann Kraus 30.05.08
	    if(type.compare("label")==0) {
			/* label hypothesis */
            // generate randomized partitions
	        Clusters A[runs];
	        for(int k = 0; k < runs; ++k) {
	          random_unique_partition( A[k], num, nclusters );
	        }
	        // now produce pairwise mca's
	        for(int i = 0; i < runs-1; ++i) {
	          for(int j = i+1; j < runs; ++j) {
	            Float mca = compare_mca(A[i],A[j],num);
	            mca_list.push_back(mca);
	          }
	        }
      	}
			if(type.compare("cluster")==0){
			/* cluster hypothesis */
            // generate randomized partitions
	        Clusters A[runs];
	        for(int k = 0; k < runs; ++k) {
	          random_partition( X, A[k], nclusters, init_method );
	        }
	        // now produce pairwise mca's
	        for(int i = 0; i < runs-1; ++i) {
	          for(int j = i+1; j < runs; ++j) {
	            Float mca = compare_mca(A[i],A[j],num);
	            mca_list.push_back(mca);
	          }
	        }
	    	}
// Johann Kraus 30.05.08
			if(type.compare("partition")==0){
			/* label hypothesis */
            // generate randomized partitions
	        Clusters A[runs];
	        for(int k = 0; k < runs; ++k) {
	          random_stirling_partition( A[k], num, nclusters );
	        }
	        // now produce pairwise mca's
	        for(int i = 0; i < runs-1; ++i) {
	          for(int j = i+1; j < runs; ++j) {
	            Float mca = compare_mca(A[i],A[j],num);
	            mca_list.push_back(mca);
	          }
	        }

// 					for(int i=0;i<A[0][0].size();i++){
// 						cluster1.push_back(A[0][0][i]);
// 					}

// 					for(int i=0;i<A[0][1].size();i++){
// 						cluster2.push_back(A[0][1][i]);
// 					}

// 					for(int i=0;i<A[0][2].size();i++){
// 						cluster3.push_back(A[0][2][i]);
// 					}

					
	    	}
// Johann Kraus 30.05.08
			if(type.compare("theoretical")==0){
	        // now produce pairwise mca's
	        for(int i = 0; i < runs-1; ++i) {
	          for(int j = i+1; j < runs; ++j) {
	            Float mca = theoretical_mca(num, nclusters);
	            mca_list.push_back(mca);
	          }
	        }
	    	}
			}
      else {
        /* for earch run create two partitions and compare */
        if(type.compare("label")==0) {
			/* label hypothesis */
	        Clusters A,B;
	        for(int k = 0; k < runs; ++k) {
	          // generate two randomized partitions
              random_unique_partition( A, num, nclusters );
	          random_unique_partition( B, num, nclusters );
	          // calculate MCA measure
	          mca_list.push_back( compare_mca( A, B, num ) );
	        }
        }
	    else {
			/* cluster hypothesis */
	        Clusters A,B;
	        for(int k = 0; k < runs; ++k) {
	          // generate two databased partitions
	          random_partition( X, A, nclusters, init_method );
	          random_partition( X, B, nclusters, init_method );
	          // calculate MCA measure
	          mca_list.push_back( compare_mca( A, B, num ) );
	        }
	    }
      }

    // Build result set to be returned as a list to R.
    RcppResultSet rs;
    rs.add("baseline", mca_list);
// 		rs.add("cluster1", cluster1);
// 		rs.add("cluster2", cluster2);
// 		rs.add("cluster3", cluster3);
		
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
