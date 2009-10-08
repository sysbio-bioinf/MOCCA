print_summary <- function(ERES, BRES, BESTK, file = "summary.txt", validation.index = c("jaccard", "rand", "cross.quant.error")){

  if(missing(ERES))
    stop("'ERES' must be a list of evaluations")
  if(missing(BRES))
    stop("'BERES' must be a list of baseline evaluations")
  if(missing(BESTK))
    stop("'BESTK' must be a list of predicted cluster numbers")
  
  validation.index <- match.arg(validation.index, several.ok = T)

  summary.methods <- names(BESTK)
  baseline.methods <- names(BRES)
  algorithms <- names(ERES)
  K <- which(!(sapply(ERES[[1]], is.null)))
  
  sink(file)

    for(summet in summary.methods){
      print(paste("Method:", summet))
      print(paste("Best cluster algorithm:", BESTK[[summet]]$alg))
      print(paste("Predicted number of clusters:", BESTK[[summet]]$k))
      print(paste("Best clustering:", BESTK[[summet]]$cluster))
      cat("\n")
    }

    print("--- Details for evaluation with mca index ---")
    for(k in K){
      print(paste("Number of clusters:", k))
      for(alg in algorithms){
        print(paste("Cluster algorithm:", alg))
        tmp <- sapply(ERES[[alg]][[k]], function(u) u$mca)
        for(summet in summary.methods){
          print(paste("Method:", summet))
          print(switch(summet, mean   = mean(tmp),
                               median = median(tmp),
                               mode   = as.numeric(names(sort(table(tmp), decreasing = T))[1])))
        }
        cat("\n")
      }
      for(bas in baseline.methods){
        print(paste("Baseline method:", bas))
        tmp <- sapply(BRES[[bas]][[k]], function(u) u)
        for(summet in summary.methods){
          print(paste("Method:", summet))
          print(switch(summet, mean   = mean(tmp),
                               median = median(tmp),
                               mode   = as.numeric(names(sort(table(tmp), decreasing = T))[1])))
        }
        cat("\n")
      }
      cat("\n")
    }

    print("--- Additional validation indices ---")
    for(k in K){
      print(paste("Number of clusters:", k))
      for(alg in algorithms){
        print(paste("Cluster algorithm:", alg))
        for(val in validation.index){
          print(paste("Validation index:", val))
          tmp <- sapply(ERES[[alg]][[k]], function(u) u[[val]])
          for(summet in summary.methods){
            print(paste("Method:", summet))
            print(switch(summet, mean   = mean(tmp),
                                 median = median(tmp),
                                 mode   = as.numeric(names(sort(table(tmp), decreasing = T))[1])))
          }
          cat("\n")
        }
        cat("\n")
      }
      cat("\n")
    }

  sink()
}
