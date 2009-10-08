\name{mca.index}
\alias{mca.index}
\title{Robclust cluster evaluation with Maximum Cluster Assignment}
\description{
  \code{mca.index} is a interface to a C++ cluster evaluation method.
}
\usage{
mca.index(x, centers.A, centers.B)
}
\arguments{
  \item{x}{A numeric matrix of data, or an object that can be coerced to such a matrix (such as a numeric vector or a data frame with all numeric columns).}
  \item{centers.A}{The set of cluster centres from the first clustering.}
  \item{centers.B}{The set of cluster centres from the second clustering.}
  }
\value{
  \code{mca.index} returns a list containing:
  \item{mca}{The Maximum Cluster Assignment index (see details).}
  \item{jac}{The jaccard index.}
  \item{rand}{The rand index.}
  \item{trans}{The transinformation.}
  \item{self}{The selfinformation.}
  \item{sym.diff}{The symmetric differences.}
  \item{cross.quant.error}{The cross quantization error.}
  \item{intra.cluster.cor}{The intra cluster correlation.}
  \item{inter.cluster.cor}{The inter cluster correlation.}
}
\details{
  Given two sets of cluster centers from two different clusterings, the mca index is computed. This is a new cluster index from H.A. Kestler and A. Mueller based on solving the linear assingment problem. Mail authors for additional information. Some more common indices are computed as well (see value). The RNNLib package provides a baseline computation for the mca index too (see function mca.baseline).
}
\author{Johann Kraus (R-Code), Andre Mueller (C++-Code)}
\examples{
  x <- rbind(matrix(rnorm(50, sd = 1), ncol = 2), matrix(rnorm(50, mean = 0.5, sd = 1), ncol = 2))
  c1 <- kmeans(x,2)$centers
  c2 <- kmeans(x,2)$centers
  cs <- mca.index(x, c1, c2)
}
\keyword{cluster}
