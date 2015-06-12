#! /usr/bin/Rscript
infile <- infile_name; incon <- file(infile, "rb")
outfile <- outfile_name; outcon <- file(outfile, "w")

for (i in 1:file.info(infile)$size/2) {
  f <- readChar(incon, nchars = 1, useBytes = TRUE)
  r <- ((ifelse((f==''),0.0,as.numeric(charToRaw(f))) - 127.0) / 128.0)
  f <- readChar(incon, nchars = 1, useBytes = TRUE)
  i <- ((ifelse((f==''),0.0,as.numeric(charToRaw(f))) - 127.0) / 128.0)
  writeLines(paste(toString(r), toString(i)), outcon);
}

close(incon); close(outcon)