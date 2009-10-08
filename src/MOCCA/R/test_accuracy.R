test_accuracy <- function(predicted.clustering, labels){
  if(max(predicted.clustering)!=max(labels))
    stop("K must be equal for both clusterings")
  mca.index(predicted.clustering, labels)$mca
}
