generate_subsets <- function(x, R, method=c("jackknife", "bootstrap", "bisect")){
  if(missing(x))
    stop("'x' must be a matrix")
  if(R <= 0)
    stop("'R' must be positiv")
  
  method <- match.arg(method)
  n <- nrow(x)
  M = switch(method, jackknife = replicate(R, sample(1:n)[-c(1:trunc(sqrt(n)))]),
                     bootstrap = replicate(R, sample(1:n, replace = T)),
                     bisect    = replicate(R, sample(1:n)[1:trunc(n/2)]))
  M
}
