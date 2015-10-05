#! /usr/bin/Rscript
args<-commandArgs(TRUE)
infile <- args[1]
incon <- file(infile, "rb")

max_f <- file.info(infile)$size/8
f = vector(length=max_f); t = vector(length=max_f)

for (j in 1:max_f) {
  f[j] <- complex(real = readBin(incon, what="numeric", size=4), imaginary = readBin(incon, what="numeric", size=4))
  t[j] <- j/2.048e6
}
close(incon)

i <- Re(f); q <- Im(f)
a <- sqrt(i*i + q*q)
phi <- atan(q/i)

for (j in 0:39) {
  x <-j*100000
  plot(t[x:(x+100000)],i[x:(x+100000)], type="l")
  Sys.sleep(2)
}