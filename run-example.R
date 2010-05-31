library(MOCCA, lib.loc="Rlib")

data(toy9)
dat <- toy9

mb <- mocca.boot(dat, 100, nrow(dat)-trunc(sqrt(nrow(dat))))
cres <- mocca.clust(dat, mb, 2:10, iter.max=1000, nstart=10)
eres <- mocca.validate(cres)
obj <- mocca.objectives(eres)
res <- mocca.pareto(obj)

#mocca(dat, R=10, K=2:10, save.all=T)
