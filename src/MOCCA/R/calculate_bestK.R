calculate_bestK <- function(x, CRES, ERES, BRES, baseline.compare = "cluster", summary.methods = c("mean", "median", "mode")){

  if(missing(x))
    stop("'x' must be a matrix")
  if(missing(CRES))
    stop("'CRES' must be a list of clusterings")
  if(missing(ERES))
    stop("'ERES' must be a list of evaluations")
  if(missing(BRES))
    stop("'BRES' must be a list of baseline evaluations")
  if(!is.element(baseline.compare, names(BRES)))
    stop("Baseline to compare must occur in BRES")

  summary.methods <- match.arg(summary.methods, several.ok = T)

  algorithms <- names(ERES)
  K <- which(!(sapply(ERES[[1]], is.null)))
  
  BESTK <- vector("list", length(summary.methods))
  names(BESTK) <- summary.methods

  tmp <- vector("list", length(algorithms))
  names(tmp) <- algorithms
  
  for(alg in algorithms){
    tmp[[alg]] <- vector("list", length = length(K))
    for(k in 1:length(K)){
      tmp[[alg]][[k]] <- vector("list", length = length(summary.methods))
      names(tmp[[alg]][[k]]) <- summary.methods
    }
  }

  idx <- 0
  for(k in K){
    idx <- idx + 1
    for(alg in algorithms){
      a <- sapply(ERES[[alg]][[k]], function(u) u$mca)
      b <- sapply(BRES[[baseline.compare]][[k]], function(u) u)
      for(summet in summary.methods){
         tmp[[alg]][[idx]][[summet]] <- switch(summet, mean   = mean(a) - mean(b),
                                                       median = median(a) - median(b),
                                                       mode   = as.numeric(names(sort(table(a), decreasing = T))[1]) - as.numeric(names(sort(table(b), decreasing = T))[1]))
      }
    }
  }
  
  for(summet in summary.methods){
    bestKalg <- c()
    bestMCAalg <- c()
    for(alg in algorithms){
      bestKalg <- c(bestKalg, K[which.max(sapply(tmp[[alg]], function(u) u[[summet]]))])
      bestMCAalg <- c(bestMCAalg, max(sapply(tmp[[alg]], function(u) u[[summet]])))
    }
    BESTK[[summet]]$alg <- algorithms[which.max(bestMCAalg)]
    BESTK[[summet]]$k <- bestKalg[which.max(bestMCAalg)]
    BESTK[[summet]]$cluster <- calculate_bestClustering(x, CRES[[BESTK[[summet]]$alg]][[BESTK[[summet]]$k]])
  }

  BESTK
}
