calculate_bestClustering <- function(x, allclusters){
  
  best_refClustering <- function(x, allclusters, refcluster){
    hits <- lapply(allclusters, function(u) predict_clusters(x, u$centers)$cluster)
    lap.sols <- lapply(allclusters, function(u) lap(x, refcluster$centers, u$centers))
    mcas <- sapply(lap.sols, function(u) u$mca)
    colsols <- lapply(lap.sols, function(u) u$colsol)
    mappings <- mapply(function(u, v) map_cluster(u,v), hits, colsols, SIMPLIFY=T)
    clust <- apply(mappings, 1, function(u) as.numeric(names(which.max(table(u)))))
    list(clustering = clust, mean.mca = mean(mcas))
  }
  
  allbest <- lapply(allclusters, function(u) best_refClustering(x, allclusters , u))
  allbest[[which.max(sapply(allbest, function(u) u$mean.mca))]]
}
