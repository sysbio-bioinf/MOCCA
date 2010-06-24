\name{analyzePareto}
\alias{analyzePareto}
\title{
  Analyze the Pareto-optimal cluster sizes
}
\description{
  Computes the set of Pareto-optimal cluster sizes in \code{obj} according to the values of the cluster validation indices. A ranking of optimal cluster sizes and a table illustrating the ranking of solutions are returned.
}
\usage{
analyzePareto(obj)
}
\arguments{
  \item{obj}{
    A matrix returned by \code{\link{mocca}} in its \code{objectiveVals} component. This matrix contains the values of several cluster validation indices for different cluster algorithms and different cluster sizes.
  }
}
\value{
  A list with the following components
  \item{rank}{A vector containing the ranking of the Pareto-optimal cluster sizes.}
  \item{table}{{A table specifying the ranking of Pareto-optimal cluster sizes. Each row is associated with a particular Pareto-optimal cluster size. Its entries specify in how many objective functions it dominates clusterings of other cluster sizes. The Pareto-optimal cluster sizes are ranked by the minimum number of objectives in which they dominate other cluster sizes.}}
}
\examples{
data(toy5)
obj <- mocca(toy5, R=10, K=2:5)
print(analyzePareto(obj$objectiveVals))
}
