mca.index <- function(cluster.A, cluster.B)
{
#  if(missing(x))
#    stop("data set 'x' must be specified")
#  x <- as.matrix(x)
  if(missing(cluster.A)||missing(cluster.B))
    stop("specify two sets of clusters to compare")
  if(max(cluster.A)!=max(cluster.B))
     stop("K must be equal for both clusterings")
  #centers.A <- as.matrix(centers.A)
  #centers.B <- as.matrix(centers.B)

  # Finally ready to make the call...
  val <- .Call("MOCCA_Mca", cluster.A, cluster.B, PACKAGE="MOCCA")

  return(val)
}
