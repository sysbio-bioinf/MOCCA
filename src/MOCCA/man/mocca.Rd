\name{mocca}
\alias{mocca}
\title{
Multi-objective optimization for collecting cluster alternatives
}
\description{
Performs a multi-objective optimization for collecting cluster alternatives.
The algorithm draws \code{R} bootstrap samples from \code{x}. It calculates clusterings for all specified cluster numbers \code{K} using k-means, neuralgas, and single-linkage clustering. It then applies several cluster validation indices to the clusterings.}
\usage{
mocca(x, R = 50, K = 2:10, iter.max = 1000, nstart = 10)
}
\arguments{
  \item{x}{
A numeric matrix of data, or an object that can be coerced to
such a matrix (such as a numeric vector or a data frame with numeric columns).
}
  \item{R}{
The number of bootstrap samples.
}
  \item{K}{
The range of cluster numbers, i.e. a vector of integers listing the maximum numbers of clusters to be used by each of the algorithms.
}
  \item{iter.max}{
The maximum number of iterations allowed in k-means.
}
  \item{nstart}{
    For k-means, how many random sets should be chosen?
}

}
\value{
A list with two entries:
\item{cluster}{
A list containing one sublist for each clustering algorithm and the baseline cluster solution. Each of these lists hold an entry for each cluster size \code{K}, which again consists of \code{R} vectors of cluster assignments. These vectors assign each data point in \code{x} to a cluster.}
\item{objectiveVals}{A matrix of objective function values. Each row corresponds to a certain cluster validation index applied to a certain clustering algorithm. The columns correspond to different cluster numbers. Consequently, an entry of the matrix specifies the median value of a certain cluster validation index for a certain clustering algorithm with a specific number of clusters over the \code{R} bootstrap samples.}

}
\examples{
data(toy5)
print(mocca(toy5, R=10, K=2:5))
}
