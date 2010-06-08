\name{mocca.boot}
\alias{mocca.boot}
\title{
Generating mocca.boot
}
\description{
  Generate \code{R} replicates of index subsets of size \code{size}. If
  replicate is \code{TRUE} sampling is done with replacement.
}
\usage{
mocca.boot(x, R, size = nrow(x), replace = T)
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
  \item{size}{
A non-negative integer giving the number of items to choose.
}
  \item{replace}{
Should sampling be with replacement?
}
}
\value{
An object of class mocca.boot, which is a matrix of size \code{size} x
\code{R}.
}
\seealso{
\code{\link{boot}}
}
\examples{
data(toy5)
mocca.boot(toy5, R=10)
}
