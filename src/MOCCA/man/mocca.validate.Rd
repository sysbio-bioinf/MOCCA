\name{mocca.validate}
\alias{mocca.validate}
\title{
Generating mocca.validate
}
\description{
Evaluate results from mocca.clust for different cluster indices.
}
\usage{
mocca.validate(x, cres)
}
\arguments{
  \item{x}{
A numeric matrix of data, or an object that can be coerced to
such a matrix (such as a numeric vector or a data frame with
all numeric columns).
}
  \item{cres}{
An object of class mocca.clust. See \code{\link{mocca.clust}}.
}
}
\value{
  An object of class mocca.validate. This is a list with slots for
  baseline and each algorithm. Each slot has list with validation
  indices for each number of k and each run r in R.
}
\seealso{
\code{\link{mocca}}, \code{\link{mocca.clust}}
}
\examples{
data(toy5)
mb <- mocca.boot(toy5, 10)
cres <- mocca.clust(toy5, mb, 2:5)
eres <- mocca.validate(toy5, cres)
}