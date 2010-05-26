library(MOCCA, lib.loc="Rlib")

data(iris)
dat <- as.matrix(iris[,-5])

M <- mocca.boot(dat, 10, nrow(dat)-trunc(sqrt(nrow(dat))))
cres <- mocca.clust(dat, M, 2:4)
eres <- mocca.validate(cres)
