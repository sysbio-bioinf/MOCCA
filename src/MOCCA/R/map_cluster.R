map_cluster <- function(cluster, mapping){
  res <- rep(NA,length(cluster))
  for(i in 1:length(cluster)){
    res[i] <- mapping[cluster[i]]+1
  }
  res
}
