run_clustering <- function(x, M, K = 2:20, algorithms = c("kmeans", "fcmeans", "neuralgas")){

  if(missing(x))
    stop("'x' must be a matrix")
  if(missing(M))
    stop("'M' must be a matrix")

  algorithms <- match.arg(algorithms, several.ok = T)
  R = ncol(M)
  
  CRES <- vector("list", length = length(algorithms))
  names(CRES) <- algorithms

  for(alg in algorithms){
    CRES[[alg]] <- vector("list", length = max(K))
    for (k in K){
      CRES[[alg]][[k]] <- vector("list", length = R)
    }
  }

  for(run in 1:R){
    subset <- M[, run]
    for(k in K){
      for(alg in algorithms){
        CRES[[alg]][[k]][[run]] <- switch(alg, kmeans    =  kmeans(x[subset,], k, iter.max = 1000),
                                               fcmeans   =  fcmeans(x[subset,], k, m = 1.112, eps=1e-15, iter.max=1000),
                                               neuralgas =  cclust(x[subset,], k, method = "neuralgas", iter.max=1000))
      }
    }
  }

  CRES
}
