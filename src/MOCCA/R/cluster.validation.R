cluster.validation <- function(x, centers.A, centers.B)
{
  if(missing(x))
    stop("data set 'x' must be specified")
  x <- as.matrix(x)
  if(missing(centers.A)||missing(centers.B))
    stop("specify two sets of centers to compare")
  centers.A <- as.matrix(centers.A)
  centers.B <- as.matrix(centers.B)

  # Finally ready to make the call...
  val <- .Call("MOCCA_Compare_Clusters", x, centers.A, centers.B, PACKAGE="MOCCA")

  return(val)
}
