plot_summary <- function(ERES, BRES, BESTK, file = "plot.pdf", save = F){
  
  if(missing(ERES))
    stop("'ERES' must be a list of evaluations")
  if(missing(BRES))
    stop("'BERES' must be a list of baseline evaluations")
  if(missing(BESTK))
    stop("'BESTK' must be a list of predicted cluster numbers")
  
  summary.methods <- names(BESTK)
  baseline.methods <- names(BRES)
  algorithms <- names(ERES)
  K <- which(!(sapply(ERES[[1]], is.null)))

  R = ceiling(sqrt(length(ERES[[1]][[K[1]]])*2))
  x.coords <- K
    
  if(save)
    pdf(file)

  par(mfrow = c(2,2))

  # mca plots
  for(summet in summary.methods){
  
    plot(0:max(x.coords), seq(0, 1, by=(1/max(x.coords))), type="n", xlab="number of clusters", ylab=paste(summet, "of MCA index from", R, "runs"))
  
    col <- 1
    for(alg in algorithms){
      y.coords <- c()
      for(k in K){
        y.coords <- c(y.coords, switch(summet, mean = mean(sapply(ERES[[alg]][[k]], function(u) u$mca)),
                                               median = median(sapply(ERES[[alg]][[k]], function(u) u$mca)),
                                               mode = as.numeric(names(sort(table(sapply(ERES[[alg]][[k]], function(u) u$mca)), decreasing = T))[1])))
      }
      
      lines(x.coords, y.coords, col = col)
      col <- col + 1
    }
    
    for(bas in baseline.methods){
      y.coords <- c()
      for(k in K){
        y.coords <- c(y.coords, switch(summet, mean = mean(sapply(BRES[[bas]][[k]], function(u) u)),
                                               median = median(sapply(BRES[[bas]][[k]], function(u) u)),
                                               mode = as.numeric(names(sort(table(sapply(BRES[[bas]][[k]], function(u) u)), decreasing = T))[1])))
      }
      lines(x.coords, y.coords, col = col)
      col <- col + 1
    }

    abline(v=BESTK[[summet]]$k)
    
    legend("bottomleft", c(algorithms, baseline.methods), text.col = 1:col, cex=0.8, y.intersp=0.8, bty="n")
    
  }

  # cross quant error plot
  plot(0:max(x.coords), seq(0, 1, by=(1/max(x.coords))), type="n", xlab="number of clusters", ylab=paste("mean of cross quantization error from", R, "runs"))

  col <- 1
  for(alg in algorithms){
    y.coords <- c()
    for(k in K){
      y.coords <- c(y.coords, mean(sapply(ERES[[alg]][[k]], function(u) u[["cross.quant.error"]])))
    }
    y.coords <- y.coords/max(y.coords)
    lines(x.coords, y.coords, col = col)
    col <- col + 1
  }
  legend("topright", algorithms, text.col = 1:col, cex=0.8, y.intersp=0.8, bty="n")

  if(save)
    dev.off()
}
