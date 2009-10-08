kmeans <- function(x, centers = 2, iter.max = 20, metric = "lnorm", lnorm = 2, filename = "")
{
  if(missing(x))
    stop("data set 'x' must be specified")
  x <- as.matrix(x)
  m <- nrow(x)
  if(missing(centers))
    stop("'centers' must be a number or a matrix")
  if(length(centers) == 1)
  {
    k <- centers
    centers <- x[sample(1:m, k), , drop = FALSE]
  }
  else
  {
  centers <- as.matrix(centers)
  if(any(duplicated(centers)))
    stop("initial centers are not distinct")
  cn <- NULL
  k <- nrow(centers)
  if(m < k)
    stop("more cluster centers than data points")
  }
  if(iter.max < 1)
      stop("'iter.max' must be positive")
  if(ncol(x) != ncol(centers))
      stop("must have same number of columns in 'x' and 'centers'")

  # Build parameter list
  if(metric=="hamming")
    params = list(maxsteps = iter.max, metric = "hamming", lnorm = NA, filename = NA)
  else if(metric=="lnorm")
    params = list(maxsteps = iter.max, metric = "lnorm", lnorm = lnorm, filename = NA)
  else if(metric=="extern")
    params = list(maxsteps = iter.max, metric = "filename", lnorm = NA, filename = filename)
  else
  {
  	stop("Metric must be one of lnorm, hamming or extern.")
  }
  # Finally ready to make the call...
  val <- .Call("MOCCA_Kmeans", params, x, centers, PACKAGE="MOCCA")

  # Add 'Within cluster sum of squares by cluster' to val
  innersumofsquares<-c()
  for(i in 1:length(val$size))
  {
    innersumofsquares<-c(innersumofsquares, sum((as.matrix(dist(rbind(val$centers[i,], x[which(val$cluster==i),])))[1,])^2))
  }

  val <- c(val,list(withinss = innersumofsquares))

  return(val)
}
