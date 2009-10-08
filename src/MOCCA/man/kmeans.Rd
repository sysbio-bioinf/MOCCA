\name{kmeans}
\alias{kmeans}
\title{MOCCA kmeans algorithm}
\description{
  \code{kmeans} is a interface to a C++ kmeans implementation.
}
\usage{
  kmeans(x, centers = 2, iter.max = 20, metric = "lnorm", lnorm = 2, filename = "")
}
\arguments{
  \item{x}{A numeric matrix of data, or an object that can be coerced to such a matrix (such as a numeric vector or a data frame with all numeric columns).}
  \item{centers}{Either the number of clusters or a set of initial (distinct) cluster centres. If a number, a random set of (distinct) rows in 'x' is chosen as the initial centres. Default is 2.}
  \item{iter.max}{The maximum number of iterations performed. Default is 20.}
  \item{metric}{A string for the distance metric to use. One of 'hamming', 'lnorm' or 'extern'. In case of 'lnorm' the parameter lnorm should be set properly. In case of 'extern' the filename of the extern metric file must be set too. Default is lnorm.}
  \item{lnorm}{An integer for the norm value to use. Default is 2.}
  \item{filename}{The filename of the extern metric.}
  }
\value{
  \code{kmeans} returns a list containing:
  \item{steps}{The number of computation steps.}
  \item{centers}{A matrix of cluster centres.}
  \item{cluster}{A vector of integers indicating the cluster to which each point is allocated.}
  \item{size}{The number of points in each cluster.}
  \item{withinss}{The within-cluster sum of squares for each cluster.}
}
\details{
  The data given by 'x' is clustered by the k-means method, which aims to partition the points into k groups such that the sum of squares from points to the assigned cluster centres is minimized. At the minimum, all cluster centres are at the mean of their Voronoi sets (the set of data points which are nearest to the cluster centre).
}
\author{Johann Kraus (R-Code), Andre Mueller (C++-Code)}
\examples{
  # a 2-dimensional example
  x <- rbind(matrix(rnorm(100, sd = 0.3), ncol = 2), matrix(rnorm(100, mean = 1, sd = 0.3), ncol = 2))
  colnames(x) <- c("x", "y")
  cl <- kmeans(x, 2)
  plot(x, col = cl$cluster, pch=cl$cluster)
  points(cl$centers, col = cl$cluster, pch = 8, cex=2)
}
\keyword{cluster}
