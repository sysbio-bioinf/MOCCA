mocca.boot <- function(x, R, n=nrow(x), replace=F){
  if(missing(x))
    stop("'x' must be a matrix")
  if(R <= 0)
    stop("'R' must be positiv")
  
  if(replace)
    mb <- replicate(R, sort(sample(1:nrow(x), n, replace = T)))
  else
    mb <- replicate(R, sort(sample(1:nrow(x), n)))

  class(mb) <- "mocca.boot"
  mb
}

mocca.clust <- function(x, mb, K = 2:20, algorithms = c("kmeans", "fcmeans", "neuralgas", "single"), iter.max=10, nstart=1){
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
        eres[[alg]][[k]] <- combn(R, 2, function(u) clusval(cres[[alg]][[k]][[u[1]]], cres[[alg]][[k]][[u[2]]], k), simplify=F)
    }
  }

  class(eres) <- "mocca.validate"
  eres
}

mocca.objectives <- function(eres){
  if(missing(eres) || !inherits(eres,"mocca.validate"))
    stop("'eres' must be of type 'mocca.validate'")

  algorithms <- names(eres)[-1]
  indices <- names(eres$baseline[[which(!(sapply(eres[[1]], is.null)))[1]]][[1]])
  #methods <- c("mean", "median")
  methods <- c("mean")
  
  obj <- create_objectives(eres, algorithms, indices, methods)
  class(obj) <- "mocca.objectives"
  obj
}

`[.mocca.objectives` <- function(x, ..., drop=T){
  cl <- oldClass(x)
  class(x) <- NULL
  val <- NextMethod("[")
  class(val) <- cl
  val
}

plot.mocca.objectives <- function(x, y=c(1,2), ...){
  class(x) <- NULL
  plot(x[y[1],], x[y[2],], pch=NA, xlab=rownames(x)[y[1]], ylab=rownames(x)[y[2]], ...)
  text(x[y[1],], x[y[2],], labels=colnames(x))
}

mocca.pareto <- function(obj){
  if(missing(obj) || !inherits(obj,"mocca.objectives"))
    stop("'obj' must be of type 'mocca.objectives'")

  ps <- getParetoSet(obj)
  pstable <- getParetoRanking(obj, ps)

  res <- list(rank=ps[order(apply(pstable, 1, function(u) min(u[-which(u==0)])), decreasing=T)], table=pstable)

  class(res) <- "mocca.pareto"
  res
}

mocca <- function(x, R = 50, K = 2:20, sampling.method = c("jackknife", "bootstrap", "bisect"), cluster.algorithms = c("kmeans", "fcmeans", "neuralgas", "single"), validation.index = c("MCA", "Rand", "Jaccard", "FM", "RR", "DP"), iter.max=10, nstart=1, save.dir = "./", save.all=F){

  if(missing(x))
    stop("'x' must be a matrix")
  if(R <= 0)
    stop("'R' must be positiv")
  if(min(K)<2)
    stop("min 'K' must be bigger than 1")
  
  sampling.method <- match.arg(sampling.method)
  cluster.algorithms <- match.arg(cluster.algorithms, several.ok = T)
  validation.index <- match.arg(validation.index, several.ok = T)

  print("generating subsets")
  size <- switch(sampling.method,
                 jackknife= nrow(x)-trunc(sqrt(nrow(x))),
                 bootstrap= nrow(x),
                 bisect= trunc(nrow(x)/2))
  mb <- mocca.boot(x, R, size, if(sampling.method=="bootstrap"){T}else{F})

  print("running cluster algorithms")
  cres <- mocca.clust(x, mb, K, cluster.algorithms, iter.max=iter.max, nstart=nstart)

  print("running evaluation")
  eres <- mocca.validate(cres)

  print("running multi-objective optimization")
  obj <- mocca.objectives(eres)
  res <- mocca.pareto(obj)
  
  print("saving details")

  if(save.all){
    if(save.dir=="./")
      save.dir <- paste(save.dir, as.character(Sys.time(), format = "%Y-%m-%d-%H-%M-%S"), sep="")
    dir.create(save.dir)
    
    save(cres, file = paste(save.dir, "/cres_C", max(K), "_R", R, ".RData", sep=""))
    save(eres, file = paste(save.dir, "/eres_C", max(K), "_R", R, ".RData", sep=""))
    save(res, file = paste(save.dir, "/pres_C", max(K), "_R", R, ".RData", sep=""))
    write(res$ps, file = paste(save.dir, "/result_C", max(K), "_R", R, ".txt", sep=""),ncolumns=length(res$ps))
    write(t(res$psrank), file = paste(save.dir, "/result_C", max(K), "_R", R, ".txt", sep=""), ncolumns=nrow(res$psrank),append=T)
  }

  res
}
