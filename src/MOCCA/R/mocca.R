mocca <- function(x, R = 50, K = 2:20, sampling.method = c("jackknife", "bootstrap", "bisect"), cluster.algorithms = c("kmeans", "fcmeans", "neuralgas"), baseline.methods = c("cluster", "partition", "label", "theoretical"), summary.methods = c("mean", "median", "mode"), validation.index = c("mca", "jaccard", "rand", "cross.quant.error"), save.dir = "./", all.save = T){

  if(missing(x))
    stop("'x' must be a matrix")
  if(R <= 0)
    stop("'R' must be positiv")
  
  sampling.method <- match.arg(sampling.method)
  cluster.algorithms <- match.arg(cluster.algorithms, several.ok = T)
  baseline.methods <- match.arg(baseline.methods, several.ok = T)
  summary.methods <- match.arg(summary.methods, several.ok = T)
  validation.index <- match.arg(validation.index, several.ok = T)


  print("generating subsets")
  
  M <- generate_subsets(x, R, sampling.method)

  print("running cluster algorithms")
  
  CRES <- run_clustering(x, M, K, cluster.algorithms)

  print("running evaluation")
  
  ERES <- evaluate_clustering(x, CRES)

  print("running baseline calculation")
  
  BRES <- evaluate_baseline(x, 100, K, baseline.methods)

  print("running search for best K")
  
  BESTK <- calculate_bestK(x, CRES, ERES, BRES, "cluster", summary.methods)

  print("plotting")
  
  if(all.save){

    print("saving details")

    if(save.dir=="./")
      save.dir <- paste(save.dir, as.character(Sys.time(), format = "%Y-%m-%d-%H-%M-%S"), sep="")
    
    dir.create(save.dir)
    
    save(CRES, file = paste(save.dir, "/CRES_C", max(K), "_R", R, ".RData", sep=""))
    
    save(ERES, file = paste(save.dir, "/ERES_C", max(K), "_R", R, ".RData", sep=""))

    save(BRES, file = paste(save.dir, "/BRES_C", max(K), "_R", R, ".RData", sep=""))

    save(BESTK, file = paste(save.dir, "/BESTK_C", max(K), "_R", R, ".RData", sep=""))
    
    print_summary(ERES, BRES, BESTK, paste(save.dir, "/Summary_C", max(K), "_R", R, ".txt", sep=""), validation.index)
    
    plot_summary(ERES, BRES, BESTK, paste(save.dir,  "/Plot_C", max(K), "_R", R, ".pdf", sep=""), save = T)
  }
  else
    plot_summary(ERES, BRES, BESTK)

  BESTK
}
