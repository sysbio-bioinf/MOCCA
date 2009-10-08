mca.baseline <- function(x, noc = 3, runs = 1000, method = "label", init = "random", pairwise = TRUE)
{
  if(missing(x))
    stop("data set 'x' must be specified")
  x <- as.matrix(x)
  if(method!="label")
  if(method!="cluster")
	if(method!="partition")
	if(method!="theoretical")
    stop("method must be one of 'label', 'cluster', 'partition', 'theoretical'")

  if(init!="random")
    if(init!="even")
      if(init!="maxmin")
        if(init!="fcmeans")
          stop("init must be one of 'random', 'even', 'maxmin', 'fcmeans'")

  # Build parameter list
  params = list(noc = as.integer(noc), runs = as.integer(runs), method = method, init = init, pairwise = pairwise)

  # Finally ready to make the call...
  val <- .Call("MOCCA_Mca_Baseline", params, x, PACKAGE="MOCCA")

  return(val)
}
