\name{mocca}
\alias{mocca}
\title{
Running MOCCA analysis
}
\description{
Multi-objective optimization for collecting cluster alternatives. R
repeated runs of all clusters in K are performed for kmeans, neuralgas,
and single linkage clustering. The results are compared with different
validation indices and a collection of objective functions is
build. Finally, a pareto set of optimal solutions is computed.
}
\usage{
mocca(x, R = 50, K = 2:10, iter.max = 1000, nstart = 10, save.all = F, save.dir = "./")
}
\arguments{
  \item{x}{
A numeric matrix of data, or an object that can be coerced to
such a matrix (such as a numeric vector or a data frame with
all numeric columns).
}
  \item{R}{
The number of replicates.
}
  \item{K}{
The range of K clusters, i.e. a vector of integers from 2 to maximum
number of clusters.
}
  \item{iter.max}{
The maximum number of iterations allowed.
}
  \item{nstart}{
    For kmeans, how many random sets should be chosen?
}
  \item{save.all}{
Should all intermediate results be saved to file?
}
  \item{save.dir}{
The directory where intermediate results are stored.
}
}
\value{
A list containing
\item{rank}{A ranking of best solutions.}
\item{table}{A table illustrating the ranking of solutions. Each row
  is a pareto optimal solution, cols contain any possible solutions. The
  number of objective function values i of solution s for which the
  corresponding entry of p is better is reported.}
}
\examples{
data(toy5)
mocca(toy5, R=10, K=2:5)
}
