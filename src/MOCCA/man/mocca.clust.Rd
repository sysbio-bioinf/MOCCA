\name{mocca.clust}
\alias{mocca.clust}
\title{
Generating mocca.clust
}
\description{
  This function performs the cluster analysis for each algorithm in
  \code{algorithms} and each k in \code{K}. The clustering is repeated
  for all subsets given by \code{mb}.
}
\usage{
mocca.clust(x, mb, K = 2:10, algorithms = c("kmeans", "neuralgas", "single"), iter.max = 10, nstart = 1)
}
\arguments{
  \item{x}{
A numeric matrix of data, or an object that can be coerced to
such a matrix (such as a numeric vector or a data frame with
all numeric columns).
}
  \item{mb}{
An object of class mocca.boot. See \code{\link{mocca.boot}}.
}
  \item{K}{
The range of K clusters, i.e. a vector of integers from 2 to maximum
number of clusters.
}
  \item{algorithms}{
The cluster algorithms to be run. One or more from kmeans, neuralgas,
and single.
}
  \item{iter.max}{
The maximum number of iterations allowed.
}
  \item{nstart}{
    For kmeans, how many random sets should be chosen?
}
}
\value{
  An object of class mocca.clust. This is a list containing slots for
  baseline and all chosen algorithms. Each slot takes a list of
  clusternumbers k in 1 to max(K). Each list contains ncol(mb) cluster
  results as raw assignment vectors, see e.g. value cluster in kmeans.
}
\seealso{
\code{\link{mocca.boot}}, \code{\link{mocca}}
}
\examples{
data(toy5)
mb <- mocca.boot(toy5, R=10)
cres <- mocca.clust(toy5, mb, 2:5)
}