mocca.boot <- function(x, R, n=nrow(x), replace=F){
  if(missing(x))
    stop("'x' must be a matrix")
  if(R <= 0)
    stop("'R' must be positiv")
  
  if(replace)
    mb <- replicate(R, sample(1:n, replace = T))    
  else
    mb <- replicate(R, sample(1:n))

  class(mb) <- "mocca.boot"
  mb
}

mocca.clust <- function(x, mb, K = 2:20, algorithms = c("kmeans", "fcmeans", "neuralgas"), iter.max=10, nstart=1){
  if(missing(x))
    stop("'x' must be a matrix")
  if(missing(mb) || !inherits(mb, "mocca.boot"))
    stop("'mb' must be of type 'mocca.boot'")
  if(min(K)<2)
    stop("minimum of 'K' must be a number larger than 2")

  x <- as.matrix(x)

  algorithms <- c("baseline", match.arg(algorithms, several.ok = T))
  R = ncol(mb)
  
  cres <- vector("list", length = length(algorithms))
  names(cres) <- algorithms

  # create lists for results
  for(alg in algorithms){
    cres[[alg]] <- vector("list", length = max(K))
    for (k in K){
      cres[[alg]][[k]] <- vector("list", length = R)
    }
  }

  # fill lists with results
  for(run in 1:R){
    subset <- mb[, run]
    for(k in K){
      for(alg in algorithms){
        cres[[alg]][[k]][[run]] <- runcluster(x, k, subset, alg, iter.max, nstart)
      }
    }
  }

  class(cres) <- "mocca.clust"
  cres
}

mocca.validate <- function(cres){
  if(missing(cres) || !inherits(cres,"mocca.clust"))
    stop("'cres' must be of type 'mocca.clust'")
  
  algorithms <- names(cres)
  K <- which(!(sapply(cres[[1]], is.null)))
  R <- length(cres[[1]][[K[1]]])

  eres <- vector("list", length = length(algorithms))
  names(eres) <- algorithms
  
  for(alg in algorithms){
    eres[[alg]] <- vector("list", length = max(K))
  }
  
  for(k in K){
    for(alg in algorithms){
      eres[[alg]][[k]] <- combn(R, 2, function(u) clusval(cres[[alg]][[k]][[u[1]]], cres[[alg]][[k]][[u[2]]]), simplify=F)
    }
  }

  class(eres) <- "mocca.validate"
  eres
}

mocca.optimize <- function(){

}
