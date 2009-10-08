evaluate_baseline <- function(x, R = 100, K = 2:20, methods=c("cluster", "partition", "label", "theoretical")){

  if(missing(x))
    stop("'x' must be a matrix")
  if(R <= 0)
    stop("'R' must be positiv")
  
  methods <- match.arg(methods, several.ok = T)
  
  BRES <- vector("list", length = length(methods))
  names(BRES) <- methods

  for(met in methods){
    BRES[[met]] <- vector("list", length = max(K))
  }

  for(k in K){
    for(met in methods){
      BRES[[met]][[k]] <- as.list(unlist(mca.baseline(x, k, R, met)))
    }
  }
  
  BRES
}
