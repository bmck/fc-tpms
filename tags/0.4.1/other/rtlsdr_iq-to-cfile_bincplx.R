#! /usr/bin/Rscript
args<-commandArgs(TRUE)
infile <- args[1]; incon <- file(infile, "rb")
outfile <- args[2]; outcon <- file(outfile, "wb")
for (i in 1:file.info(infile)$size) {
  f <- readChar(incon, nchars = 1, useBytes = TRUE)
  f <- ((ifelse((f==''),0.0,as.numeric(charToRaw(f))) - 127.0) / 128.0)
  writeBin(f, outcon, size = 4);
}

close(incon); close(outcon)