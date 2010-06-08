\name{MOCCA-package}
\alias{MOCCA-package}
\alias{MOCCA}
\docType{package}
\title{
Multi-objective optimization for collecting cluster alternatives
}
\description{
This package provides methods for collecting cluster alternatives using
multi-objective optimization. See references for more information on the method.
}
\details{
\tabular{ll}{
Package: \tab MOCCA\cr
Version: \tab 1.0\cr
Date: \tab 2010-05-27\cr
Depends: \tab R (>= 2.0.0), cclust, clv, e1071\cr
SystemRequirements: \tab None\cr
License: \tab Artistic License 2.0\cr
Built: \tab R 2.11.0; ; 2010-06-07 09:33:11 UTC; unix\cr
}

Index:
\preformatted{
mocca                   MOCCA main function
toy5                    Toy data set for 5 clusters
}
}
\author{
Johann Kraus <johann.kraus@uni-ulm.de>
Maintainer: Hans Kestler <hans.kestler@uni-ulm.de>
}
\references{
  Kraus, J. M. and Mussel, C. and Kestler, H. A. (2010). Multi-objective
optimization for collecting cluster alternatives. _Comput Stat_ *XX*, XX-XX.
}
\keyword{ package }
\examples{
data(toy5)
mocca(toy5, R=10, K=2:5)
}
