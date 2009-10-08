/*
 * kmeans.h
 *
 * written by Andre Mueller
 * 
 * $Id: kmeans.h,v 1.3 2001/05/18 14:30:25 andre Exp andre $
 *
 * $Log: kmeans.h,v $
 * Revision 1.3  2001/05/18 14:30:25  andre
 * Initial version of k-means (without templates).
 *
 *
 */
 
#ifndef __KMEANS_H__
#define __KMEANS_H__

// #include <mtl/mtl.h>
#include <algorithm>
#include "hitlist.h"
#include "matrixutil.h"

/*
  batch_kmeans_step
  
  data    each row must be a vector of the input data
  cod     preinitialized codebook vectors

  returns the number of changes in the cluster assignment  
*/
template <class DataSeq, class CodSeq, class Metric, class Float>
int batch_kmeans_step(const DataSeq &data, CodSeq &cod, Metric metric, Float *qerr = NULL) {
	Clusters			clusters;
	typedef typename CodSeq::value_type	Vector;
	Vector				v;
	Clusters::iterator	cluster;
	HitList::iterator	i;
	typename CodSeq::iterator	codvec;
	int changed;
	
	changed = 0;
	
	create_hit_list(data.begin(),data.end(),cod.begin(),cod.end(),clusters,metric);
	
	/* sum up every cluster */
	if(qerr)
		*qerr = 0.0;

	v = Vector((*cod.begin()).size());
	for(cluster=clusters.begin(), codvec=cod.begin(); cluster!=clusters.end() && codvec!=cod.end() ; 
			++cluster, ++codvec) {

		// mtl::set(v,0.0);
		fill(v.begin(),v.end(),0);
		
		/* iterate over each data point in the hitlist of this cluster */
		for(i=(*cluster).begin(); i!=(*cluster).end(); ++i) {
			// mtl::add(data[*i],v);
			add_vector(data[*i],v);
			
			if(qerr)
				*qerr += metric(data[*i],*codvec);
		}
		if((*cluster).size() > 0) {
			// mtl::scale(v,1.0/(Float)(*cluster).size());
			scale_vector(v,1.0/(Float)(*cluster).size());
		}
//		if(l_dist((*ii),v,2.0) > 1E-20)
//			changed++;
		// mtl::copy(v,*codvec);
		(*codvec).resize(v.size());
		copy(v.begin(),v.end(),(*codvec).begin());
	}
	changed=1;
	if(qerr)
		*qerr /= (Float)cod.size()*(Float)data.size();
		
	return changed;
}


/*
  cluster_kmeans
  
  data    each row must be a vector of the input data
  cod     preinitialized codebook vectors

  returns the number of changes in the cluster assignment  
*/
template <class DataSeq, class CodSeq, class Distance, class Metric>
Distance cluster_kmeans(const DataSeq &data, CodSeq &cod, Distance maxsteps, Metric metric) {

	typedef typename CodSeq::value_type	Vector;
	typedef typename CodSeq::value_type::value_type Float;
	
	Clusters clusters;
	Vector v;

	Clusters::iterator cluster;
	HitList::iterator i;
	typename CodSeq::iterator codvec;
	Distance steps;

	v = Vector((*cod.begin()).size());
	
	for(steps=0; steps<maxsteps; steps++) {
		if(update_hit_list(data.begin(),data.end(),cod.begin(),cod.end(),clusters,metric) == 0)
			break;
			
		/* update the codevectors */
		for(cluster=clusters.begin(), codvec=cod.begin(); cluster!=clusters.end() && codvec!=cod.end() ; 
				++cluster, ++codvec) {

			// mtl::set(v,0.0);
			fill(v.begin(),v.end(),0);
		
			/* iterate over each data point in the hitlist of this cluster and sum up the vectors */
			for(i=(*cluster).begin(); i!=(*cluster).end(); ++i)
				add_vector(data[*i],v);
			
			if((*cluster).size() > 0) {
				scale_vector(v,1.0/(Float)(*cluster).size());
			}
			(*codvec).resize(v.size());
			copy(v.begin(),v.end(),(*codvec).begin());
		}
	}

	return steps;
}


#endif

