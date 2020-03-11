\name{MOCCA-package}
\alias{MOCCA-package}
\alias{MOCCA}
\docType{package}
\title{
Multi-objective optimization for collecting cluster alternatives
}
\description{
This package provides methods to analyze cluster alternatives based on multi-objective optimization of cluster validation indices. 
}
\details{
\tabular{ll}{
Package: \tab MOCCA\cr
Version: \tab 1.4\cr
Date: \tab 2020-03-05\cr
Depends: \tab R (>= 2.0.0), cclust, clue, cluster, class\cr
License: \tab Artistic License 2.0\cr
}

Estimating the optimal cluster number of a dataset is often a difficult problem. Cluster validation indices are designed to rate a clustering and can be used to rank different cluster sizes. Bootstrapping has been proposed to determine robust cluster numbers based on such indices. However, these estimations vary depending on the employed clustering algorithm and cluster validation index. The idea of MOCCA is to estimate robust cluster numbers by aggregating the best cluster numbers of several clustering algorithms and cluster validation indices in a multi-objective setting.

The main function of the package is \code{\link{mocca}}, which applies multiple cluster algorithms to a cluster dataset in a bootstrapping setting and calculates several cluster validation indices. These results can be compared by calculating the Pareto-optimal cluster sizes and ranking them according to their domination. This is implemented in \code{\link{analyzePareto}}.

}
\author{
Johann Kraus <johann.kraus@uni-ulm.de>
Maintainer: Hans Kestler <hans.kestler@uni-ulm.de>
}
\keyword{ package }
\examples{
data(toy5)
obj <- mocca(toy5, R=10, K=2:5)
print(analyzePareto(obj$objectiveVals))
}
