dominate <- function(x, y){
  all(x<=y) & any(x<y)
}

nondominated <- function(x,sol){
  !any(apply(sol, 1, function(u) dominate(u,x)))
}

buildParetoSet <- function(sol){
  which(sapply(1:nrow(sol), function(u) nondominated(sol[u,], sol[-u,])))
}

wrapper_buildParetoSet <- function(sol){
  # format input
  tmpsol <- na.omit(sol)

  # build pareto set
  PS <- buildParetoSet(tmpsol)
  
  # format output
  omitted <- attributes(tmpsol)$na.action
  for(i in omitted){
    idx <- PS >= i
    PS[idx] <- PS[idx] + 1
  }
  
  PS
}

plot_pareto_2dim <- function(x, y, wz = 0){
  z <- (1:length(x))/length(x)
  y <- (1-wz)*y + wz*z
  p <- wrapper_buildParetoSet(cbind(x, y))
  plot(x, y, col = rep(1,length(x))+is.element(1:length(x),p),pch=18,cex=3,xlab="cross-quantization error", ylab="dissimilarity",cex.lab=1.3)
  lines(x[p], y[p],lwd=2,col=2)
  text(x[p]+0.8*par("cxy")[1], y[p]+0.8*par("cxy")[2], labels = as.character(p), cex=1.3,col=2)
}

collect_validity_index <- function(ERES, BRES, algorithm = c("neuralgas","kmeans","fcmeans"), val.index = c("mca", "jaccard", "rand", "cross.quant.error", "trans", "self", "sym.diff", "intra.cluster.cor", "inter.cluster.cor"), baseline = c(NA, "cluster", "partition", "label", "theoretical"), summary.method = c("mean", "median"), stand = F){

  algorithm <- match.arg(algorithm)
  val.index <- match.arg(val.index)
  baseline <- match.arg(baseline)
  summet <- match.arg(summary.method)
  
  K <- which(!(sapply(ERES[[1]], is.null)))
  
  x <- switch(summet, mean   = sapply(ERES[[algorithm]][K], function(u) mean(sapply(u, function(v) v[[val.index]]))),
                      median = sapply(ERES[[algorithm]][K], function(u) median(sapply(u, function(v) v[[val.index]]))))

# switch(summet, mean = mean(sapply(ERES[[alg]][[k]], function(u) u$mca)),
#                median = median(sapply(ERES[[alg]][[k]], function(u) u$mca)),
#                mode = as.numeric(names(sort(table(sapply(ERES[[alg]][[k]], function(u) u$mca)), decreasing = T))[1])))
  
  if(!is.na(baseline)){
    b <- switch(summet, mean   = sapply(BRES[[baseline]][K], function(u) mean(sapply(u, function(v) v))),
                        median = sapply(BRES[[baseline]][K], function(u) median(sapply(u, function(v) v))))
    x <- x-b
  }

  if(stand){
    x <- x/max(x)
  }

  res <- rep(NA, max(K))
  i <- 1
  for(k in K){
    res[k] <- x[i]
    i <- i+1
  }
  
  res
}

buildSolutionSet <- function(ERES, BRES, val.index = c("mca", "jaccard", "rand", "cross.quant.error", "trans", "self", "sym.diff")){
  algorithm <- names(ERES)
  val.index <- match.arg(val.index, several.ok = T)
  K <- length(ERES[[1]])
  summary.method <- c("mean", "median")
  
  SOL <- matrix(nr=K,nc=0)
  
  # collect all validity information into set of solutions
  for(i in algorithm){
    for(j in val.index){
      for(k in summary.method){
        if(j =="mca")
          baseline <- "cluster"
        else
          baseline <- NULL
        if(is.element(j, c("cross.quant.error", "self")))
          stand <- T
        else
          stand <- F
        res <- collect_validity_index(ERES, BRES, i, j, baseline=baseline, summary.method=k, stand=stand)
        if(is.element(j, c("mca", "jaccard", "rand", "trans")))
          res <- 1-res
        SOL <- cbind(SOL, res)
        colnames(SOL)[ncol(SOL)] <- paste(i, j, k,sep="-")
      }
    }
  }
  
  SOL
}
