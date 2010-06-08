\name{mocca.pareto}
\alias{mocca.pareto}
\title{
  Generating mocca.pareto
}
\description{
  Compute the pareto set of optimal solutions in \code{obj}. A ranking of
  optimal solutions and a table illustrating the ranking of solutions are
  returned.
}
\usage{
mocca.pareto(obj)
}
\arguments{
  \item{obj}{
    An object of class mocca.objectives. This is a matrix with objectives in
    rows and values for different k in cols.
  }
}
\value{
  A list of
  \item{rank}{A ranking of best solutions.}
  \item{table}{A table illustrating the ranking of solutions. Each row
    is a pareto optimal solution, cols contain any possible solutions. The
    number of objective function values i of solution s for which the
    corresponding entry of p is better is reported.}
}
\examples{
data(toy5)
mb <- mocca.boot(toy5, 10)
cres <- mocca.clust(toy5, mb, 2:5)
eres <- mocca.validate(toy5, cres)
obj <- mocca.objectives(eres)
res <- mocca.pareto(obj)
}