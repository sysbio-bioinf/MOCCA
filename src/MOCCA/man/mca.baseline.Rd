\name{mca.baseline}
\alias{mca.baseline}
\title{MOCCA Maximum Cluster Assignment baseline}
\description{
  \code{mca.baseline} is a interface to a C++ function calculating a Maximum Cluster Assignment baseline.
}
\usage{
  mca.baseline(x, noc = 3, runs = 1000, method = "label", init = "random", pairwise = TRUE)
}
\arguments{
  \item{x}{An optional numeric matrix of data, or an object that can be coerced to such a matrix (such as a numeric vector or a data frame with all numeric columns).}
  \item{noc}{An integer for the number of clusters. Default is 3.}
  \item{runs}{An integer for the number of runs. Default is 1000.}
  \item{method}{The two possible baseline hypotheses (see details). One of 'label' or 'cluster'. Default is label.}
  \item{init}{The initialization method for 'cluster' hypotheses (see details). One of 'even', 'random', 'maxmin', 'fcmeans'. Default is random.}
  \item{pairwise}{An optional boolean flag. If TRUE first <runs> partitions are created and then all pairwise mca's are calculated. If FALSE <runs> pairs of partitions are created and compared directly.}
}
\value{
  \code{mca.baseline} returns a list containing:
  \item{baseline}{A vector of floats containing the computed MCA indices.}
}
\details{
  Calculates a Maximum Cluster Assignment baseline (mail authors H. Kestler and A. Mueller for additional information). The baseline distribution can be computed with and without respect to the data. Therefore if method 'label' is used, a baseline for a complete random clustering without information about the data is computed. Using method 'cluster', the baseline is computed by performing a basic clustering. Either 'even', i.e. uniform choosing cluster centers from data, 'random', i.e. randomized choosing cluster centers from data or 'maxmin', i.e. a implementation of the maxmin algorithm (A. Juan and E. Vidal, Comparison of Four Initialization Techniques for the K-Medians Clustering Algorithm, "Advances in Pattern Recognition", F. J. Ferri and J. M. Inesta and A. Amin and P. Pudil (editors), LNCS 1876, pp. 842-852, Springer-Verlag, Heidelberg 2000). Additional 'fcmeans' could be used to initialize better cluster centers. The clusters then are computed using a 1-nearest-neighbour algorithm. Each of these partitionings are evaluated either pairwise for two partitionings per run or pairwise over all computed clusterings.
}
\author{Johann Kraus (R-Code), Andre Mueller (C++-Code)}
\examples{
  x <- rbind(matrix(rnorm(50, sd = 1), ncol = 2), matrix(rnorm(50, mean = 0.5, sd = 1), ncol = 2))
  mca.baseline(x,2,100)
}
\keyword{cluster}
