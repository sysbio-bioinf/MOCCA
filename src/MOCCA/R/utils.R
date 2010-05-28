predict_baseline <- function(x, k, subset){
  train <- x[sample(subset, k),]
  as.integer(knn1(train, x, 1:k))
}

predict_kmeans <- function(x, k, subset, iter.max, nstart){
  train <- kmeans(x[subset, ], k, iter.max=iter.max, nstart=nstart)$centers
  as.integer(knn1(train, x, 1:k))
}

predict_cmeans <- function(x, k, subset, iter.max){
  train <- cmeans(x[subset, ], k, m = 1.112, iter.max=iter.max)$centers
  as.integer(knn1(train, x, 1:k))
}

predict_cclust <- function(x, k, subset, iter.max){
  train <- cclust(x[subset, ], k, method = "neuralgas", iter.max=iter.max)
  predict.cclust(train, x)$cluster
}

runcluster <- function(x, k, subset, method, iter.max, nstart){
  res <- switch(method,
                baseline  =  predict_baseline(x, k, subset),
                kmeans    =  predict_kmeans(x, k, subset, iter.max, nstart),
                fcmeans   =  predict_cmeans(x, k, subset, iter.max),
                neuralgas =  predict_cclust(x, k, subset, iter.max))
  res
}

clusval <- function(cluster1, cluster2, k){
  cluster1 <- as.integer(cluster1)
  cluster2 <- as.integer(cluster2)
  
  if(length(unique(cluster1))<k || length(unique(cluster2))<k)
    return(list(MCA=NA, Rand=NA, Jaccard=NA, FM=NA, RR=NA, DP=NA))
  
  ext <- std.ext(cluster1, cluster2)
  cfm <- confusion.matrix(cluster1, cluster2)
  
  list(MCA=similarity.index(cfm), Rand=clv.Rand(ext), Jaccard=clv.Jaccard(ext), FM=clv.Folkes.Mallows(ext), RR=clv.Russel.Rao(ext), DP=dot.product(cluster1, cluster2))
}

extract_index <- function(eres, algorithm, index){
  K <- which(!(sapply(eres[[algorithm]], is.null)))
  res <- vector("list", max(K))
  for(k in K){
    res[[k]] <- sapply(eres[[algorithm]][[k]], function(u) u[[index]])
  }
  res
}

create_objectives <- function(eres, algorithms, indices, methods){
  maxK <- length(eres[["baseline"]])
  maxB <- length(indices)*length(methods)
  bases <- matrix(nr=maxB,nc=maxK)
  counter <- 1
  for(idx in indices){
    ei <- extract_index(eres, c("baseline"), idx)
    for(met in methods){
      bases[counter,] <- sapply(ei, function(u) if(is.null(u)) NA else do.call(met, list(u)))
      counter <- counter+1
    }
  }

  res <- matrix(nr=length(algorithms)*maxB, nc=maxK)
  rownames(res) <- 1:nrow(res)
  colnames(res) <- 1:maxK
  counter <- 1
  for(alg in algorithms){
    for(idx in indices){
      ei <- extract_index(eres, alg, idx)
      for(met in methods){
        # correct for chance -> subtract value from baseline
        # minimization problem -> (1 - idx)
        res[counter,] <- 1 - (sapply(ei, function(u) if(is.null(u)) NA else do.call(met, list(u))) - bases[((counter-1)%%maxB)+1,])
        rownames(res)[counter] <- paste(alg, idx, met, sep=".")
        counter <- counter+1
      }
    }
  }
  res
}

dominate <- function(x, y){
  all(x<=y) & any(x<y)
}

nondominated <- function(x, sol){
  !any(apply(sol, 2, function(u) dominate(u,x)))
}

getParetoSet <- function(sol){
  # format input -> na.omit omits rows not columns
  tmpsol <- t(na.omit(t(sol)))

  # build pareto set
  ps <- which(sapply(1:ncol(tmpsol), function(u) nondominated(tmpsol[,u], tmpsol[,-u])))
  
  # format output
  omitted <- attributes(tmpsol)$na.action
  for(i in omitted){
    idx <- ps >= i
    ps[idx] <- ps[idx] + 1
  }
  
  ps
}

getParetoRanking <- function(sol, ps){
  # format input -> na.omit omits rows not columns
  tmpsol <- t(na.omit(t(sol)))
  
  # for each solution in ps: how many objective function values in this solution
  # are better than values in corresponing objectives in other solutions
  t(apply(sol[,ps], 2, function(u) apply(tmpsol, 2, function(v) sum(u<v))))
}
