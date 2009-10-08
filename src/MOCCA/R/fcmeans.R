fcmeans <- function(x, centers = 2, iter.max = 20, m = 2, eps = 1e-15)
{
  if(missing(x))
    stop("data set 'x' must be specified")
  x <- as.matrix(x)
  d <- nrow(x)
  if(missing(centers))
    stop("'centers' must be a number or a matrix")
  if(length(centers) == 1)
  {
    k <- centers
    centers <- x[sample(1:d, k), , drop = FALSE]
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
  if(m <= 1)
      stop("'m' must be greater than 1")
  if(eps <= 0)
      stop("'eps' must be positive")
  eps <- as.double(eps)
  m <- as.double(m)
  iter.max <- as.integer(iter.max)

  # Build parameter list
  params <- list(maxsteps = iter.max, m = m, eps = eps)

  # Finally ready to make the call...
  val <- .Call("MOCCA_Fcmeans", params, x, centers, PACKAGE="MOCCA")

  return(val)
}
