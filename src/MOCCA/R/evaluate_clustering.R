evaluate_clustering <- function(x, CRES){

  if(missing(x))
    stop("'x' must be a matrix")
  if(missing(CRES))
    stop("'CRES' must be a list of clusterings")
  
  algorithms <- names(CRES)
  K <- which(!(sapply(CRES[[1]], is.null)))
  R = length(CRES[[1]][[K[1]]])
  
  ERES <- vector("list", length = length(CRES))
  names(ERES) <- algorithms
  
  for(alg in algorithms){
    ERES[[alg]] <- vector("list", length = max(K))
  }
  
  for(k in K){
    for(alg in algorithms){
      ERES[[alg]][[k]] <- combn(R, 2, function(u) cluster.validation(x, CRES[[alg]][[k]][[u[1]]]$centers, CRES[[alg]][[k]][[u[2]]]$centers),simplify=F)
    }
  }
  
  ERES
}
