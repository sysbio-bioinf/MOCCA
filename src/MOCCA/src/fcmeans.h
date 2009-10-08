/*
 * fcmeans.h
 *
 * written by Andre Mueller
 *
 * fuzzy c-means implementation
 *
 * $Id: fcmeans.h,v 1.4 2002/01/14 15:58:15 andre Exp andre $
 *
 * $Log: fcmeans.h,v $
 * Revision 1.4  2002/01/14 15:58:15  andre
 * Removed references to the Metric class. Using copies instead.
 *
 * Revision 1.3  2001/07/12 12:00:32  andre
 * Before discarding the MTL.
 *
 * Revision 1.2  2001/05/21 13:30:56  andre
 * Version without templates
 *
 * Revision 1.1  2001/04/30 12:37:12  andre
 * Initial revision
 *
 *
 */

/*! \file fcmeans.h
\brief Fuzzy c-means implementation.



*/

#ifndef __FCMEANS_H__
#define __FCMEANS_H__

#include <iterator>

using namespace std;

#include "error_handler.h"
#include "metric.h"
#include "matrixutil.h"

/*!	Calculates the value of the fuzzy c-means error target function

	�\f[
	 	E = \sum_{i=1}^n \sum_{j=1}^k u_{ij}^m d^2({\bf x}_i,{\bf c}_j)
	\f]

	\f$ d({\bf u},{\bf v}) \f$ is the (euklidian) distance function

	\param data
		\f$ \in {\cal R}^{n \times d} \f$
		data matrix (n row vectors)

	\param cod
		\f$ \in {\cal R}^{k \times d} \f$
		codebook vectors (k row vectors)

	\param u
		\f$ \in {\cal R}^{n \times k} \f$
		assignment matrix (n row vectors)

	\param norm Distance measure to be used.

	\param m > 1.0 the fuzzy exponent

	\return the error value E

	In most cases you would take

	\code
	LNorm <FloatMatrix::OneD> norm(2.0);
	\endcode

	as standard metric.

	\sa metric.h
*/
template <class DataSeq, class CodSeq, class Matrix, class Metric>
typename Metric::result_type
		fcmeans_error(const DataSeq &data, const CodSeq &cod, const Matrix &u,
				Metric norm, typename Metric::result_type m) {

	/* check parameters */
	_Assert( (*data.begin()).size() == (*cod.begin()).size() , "fcmeans_error()" );
	_Assert( data.size() == u.size(), "fcmeans_error()" );
	_Assert( cod.size() == (*u.begin()).size(), "fcmeans_error()" );

	typename Matrix::size_type i,j;
	typename Metric::result_type d,sum = 0.0;

	for(i=0; i<u.size(); i++) {
		for(j=0; j<u[0].size(); j++) {
			d = norm( data[i], cod[j] );
			sum += pow( u[i][j] , m ) * d * d;
		}
	}

	return sum;
}

/*!  Fuzzy c-means clustering

	\param data
		\f$ \in {\cal R}^{n \times d} \f$
		data matrix (n row vectors)

	\retval cod
		\f$ \in {\cal R}^{k \times d} \f$
		codebook vectors (k row vectors). This array has to be initialized with the initial cluster centers by
		the caller.

	\retval u
		\f$ \in {\cal R}^{n \times k} \f$
		assignment matrix (n row vectors)

	\param norm Distance measure to be used.

	\param m > 1.0 the fuzzy exponent

	\param maxsteps > 0 the maximum number of clustering steps

	\param eps > 0.0 stop criterion. When the relative error decrease comes below eps the clustering process will be stopped.
		\f[ \frac{E_{t-1}-E_t}{E_t} < \epsilon \f]

   	\param epsilon > 0.0 (default 1E-10) If the distance between a data point \f$ {\bf x}_i \f$ and a codebook vector
		\f$ {\bf c}_j \f$ is below epsilon the membership \f$ u_{ij} \f$ will be 1.

	\return the number of performed clustering steps

	\sa metric.h, init.h
*/

template <class DataSeq, class CodSeq, class Matrix, class Metric, class Distance>
Distance cluster_fcmeans(const DataSeq &data, CodSeq &cod, Matrix &u,
		Metric norm, typename Metric::result_type m, Distance maxsteps,
		typename Metric::result_type eps, typename Metric::result_type epsilon = 1E-15) {

	typedef typename CodSeq::value_type	Vector;
	typedef typename Matrix::value_type::value_type Float;

	Distance step;
	typename Matrix::size_type i,j,k;
	Vector dist( cod.size() );

	dim_matrix(u, data.size(), cod.size() );

	Float t = 2.0 / (m - 1.0);
	Float qerr = 0, last_qerr = 0;

	for(step=0; step<maxsteps; step++) {
		/*
			update membership matrix u

		*/
		for(i=0; i<data.size(); i++) {
			/*
				calculate distances

											2/(m-1)
				d[i][j] <- || x[i] - c[j] ||

			*/

			int found = 0;
			for(j=0; j<cod.size(); j++) {
				u[i][j] = 0.0;
				dist[j] = pow( norm( data[i] , cod[j] ) , t );

				if( dist[j] < epsilon ) {
					found++;
					u[i][j] = 1.0;
				}
			}

			/*
				calculate u

			*/
			if(found == 0) {
				for(j=0; j<cod.size(); j++) {
					Float sum = 0.0;
					for(k=0; k<cod.size(); k++)
						sum += dist[j]/dist[k];
					u[i][j] = 1.0 / sum;
				}
			}
		}

		/*
			update cluster centers cod[j]

		*/
		for(j=0; j<cod.size(); j++) {
			Float ud, sum;
			Vector v(cod[j].size());

			sum = 0.0;
			// mtl::set( v , 0.0 );
			std::fill(v.begin(),v.end(),0.0);

			for(i=0; i<data.size(); i++) {
				ud = pow( u[i][j] , m );
				sum += ud;
				add_scaled( data[i], ud, v );
			}
			scale_vector(v, 1.0/sum);
			std::copy( v.begin(), v.end() , cod[j].begin() );
		}

		/*
			evaluate error function and return if convergence criteria is fulfilled
		*/
		last_qerr = qerr;
		qerr = fcmeans_error(data, cod, u, norm, m);

		if(step>0) {
			if(fabs(last_qerr - qerr)/qerr < eps) {
				return step;
			}
		}
	}

	return step;
}

#endif

