\name{fcmeans}
\alias{fcmeans}
\title{MOCCA fuzzy-cmeans algorithm}
\description{
  \code{fcmeans} is a interface to a C++ Fuzzy-Cmeans implementation.
}
\usage{
  fcmeans(x, centers = 2, iter.max = 20, m = 2, eps = 1e-15)
}
\arguments{
  \item{x}{A numeric matrix of data, or an object that can be coerced to such a matrix (such as a numeric vector or a data frame with all numeric columns).}
  \item{centers}{Either the number of clusters or a set of initial (distinct) cluster centres. If a number, a random set of (distinct) rows in 'x' is chosen as the initial centres. Default is 2.}
  \item{iter.max}{The maximum number of iterations performed. Default is 20.}
  \item{m}{A number strictly larger than 1 specifying the 'membership exponent' used in the fit criterion. Default is 2.}
  \item{eps}{A number for the default tolerance for convergence (relative convergence of the fit criterion). Default is 1e-15.}
  }
\value{
  \code{fcmeans} returns a list containing:
  \item{steps}{The number of computation steps.}
  \item{centers}{A matrix of cluster centres.}
  \item{cluster}{A vector of integers indicating the cluster to which each point is allocated.}
  \item{membership}{A matrix containing the memberships for each pair consisting of an observation and a cluster.}
  \item{size}{The number of points in each cluster.}
}
\details{
  Computes a fuzzy clustering of the data into 'k' clusters. In a fuzzy clustering, each observation is 'spread out' over the various clusters. The memberships are nonnegative, and for a fixed observation i they sum to 1.
}
\author{Johann Kraus (R-Code), Andre Mueller (C++-Code)}
\examples{
  x <- rbind(matrix(rnorm(100, sd = 0.3), ncol = 2), matrix(rnorm(100, mean = 1, sd = 0.3), ncol = 2))
  colnames(x) <- c("x", "y")
  cl <- fcmeans(x, 2)
  plot(x, col = cl$cluster, pch=cl$cluster)
  points(cl$centers, col = cl$cluster, pch = 8, cex=2)
}
\keyword{cluster}
