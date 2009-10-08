predict_clusters <- function(x, centers)
{
  if(missing(x))
    stop("data set 'x' must be specified")
  x <- as.matrix(x)
  if(missing(centers))
    stop("specify two sets of centers to compare")
  centers <- as.matrix(centers)
  
  # Finally ready to make the call...
  val <- .Call("MOCCA_Predict_Clusters", x, centers, PACKAGE="MOCCA")

  return(val)
}
