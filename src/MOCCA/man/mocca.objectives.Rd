\name{mocca.objectives}
\alias{mocca.objectives}
\alias{plot.mocca.objectives}
\alias{[.mocca.objectives}
\title{
Generating mocca.objectives
}
\description{
Extracts objective functions for the multi-objective optimization. An
objective function is the vector of values for a all k in 2:K from
evaluating validation index on cluster algorithm. The result is a matrix
where all objectives functions are collected.
}
\usage{
mocca.objectives(eres)
plot.mocca.objectives(x, y = c(1, 2), ...)
`[.mocca.objectives`(x, ..., drop = T)
}
\arguments{
  \item{eres}{
An object of class mocca.validate. See \code{\link{mocca.validate}}.
}
  \item{x}{
Object from which to extract element(s) or in which to replace
  element(s).
}
  \item{y}{
Row numbers to select for plotting. This is a two-dimensional plot,
  i.e. only a vector of length two is allowed.
}
  \item{\dots}{
Indices specifying elements to extract or replace. See \code{[}
}
  \item{drop}{
If \code{TRUE} the result is coerced to the lowest possible dimension
}
}
\value{
An object of class mocca.objectives. This is a matrix with nrow
objectives for 2:K values. There is a method for extracting or replacing
parts of an mocca.objectives object. A plot function is also provided.
}
\seealso{
\code{\link{mocca}}, \code{\link{mocca.validate}}
}
\examples{
data(toy5)
mb <- mocca.boot(toy5, R=10)
cres <- mocca.clust(toy5, mb, K=2:5)
eres <- mocca.validate(toy5, cres)
obj <- mocca.objectives(eres)
plot(obj)
}