library(MOCCA, lib.loc="Rlib")

data(iris)
dat <- as.matrix(iris[,-5])

M <- mocca.boot(dat, 10, nrow(dat)-trunc(sqrt(nrow(dat))))
cres <- mocca.clust(dat, M, 2:10, iter.max=1000, nstart=10)
eres <- mocca.validate(cres)
obj <- mocca.objectives(eres)
res <- mocca.pareto(obj)
