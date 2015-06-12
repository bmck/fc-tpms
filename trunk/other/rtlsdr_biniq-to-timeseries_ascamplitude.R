#! /usr/bin/Rscript
args<-commandArgs(TRUE)
infile <- args[1]; incon <- file(infile, "rb")
outfile <- args[2]; outcon <- file(outfile, "w")

for (i in 1:file.info(infile)$size/2) {
  f <- readBin(incon, what="numeric", size=4)
  write(f, outcon)
  f <- readBin(incon, what="numeric", size=4)
}

close(incon); close(outcon)