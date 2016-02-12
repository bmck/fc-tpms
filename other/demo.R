#! /usr/bin/Rscript
print(Sys.time())
dt_tm <- strftime(Sys.time(), "%Y%m%d_%H%M%Sz", tz="GMT")
dr <- paste("rtlsdr_315m_2.048m_", dt_tm, "_demo", sep = '')
cmd <- paste("mkdir /Users/bmck/src/", dr, sep = '')
system(cmd); print(cmd)
cmd <- paste("cd /Users/bmck/src/", dr, sep = '')
system(cmd); print(cmd)
Sys.sleep(0.5)
cmd <- paste("rtl_sdr -f 315000000 -n 4000000 - | /Users/bmck/src/stdiniq-to-stdoutcplx /Users/bmck/src/", dr, "/", dr, ".cfile", sep = '')
system(cmd); print(cmd)
cmd <- paste("/Users/bmck/src/jboone-tpms/src/extract_bursts.py /Users/bmck/src/", dr, "/", dr, ".cfile", sep = '')
system(cmd); print(cmd)
cmd <- paste("mv *.dat /Users/bmck/src/", dr, sep = '')
system(cmd); print(cmd)
cmd <- paste("ls -F /Users/bmck/src/", dr, "/*.dat", sep = '')
files = system(cmd, intern=TRUE); print(cmd)

flag <- FALSE
for (infile in files) {
  print(infile)
  # infile <- args[1];
  incon <- file(infile, "rb")
  max_f <- file.info(infile)$size/8
  f = vector(length=max_f)
  for (j in 1:max_f) {
    f[j] <- complex(real = readBin(incon, what="numeric", size=4), imaginary = readBin(incon, what="numeric", size=4))
  }
  close(incon)

  x = Re(f); i = Im(f); t = vector(length=max_f); for (j in 1:length(t)) { t[j] = i/2.048e6 }
  # cat("\n\n\n----------\n----------\n----------\n\n\n\nlength(x) =\n"); print(length(x))
  # cat("\n\n\n----------\n----------\n----------\n\n\n\nlength(i) =\n"); print(length(i))
  # cat("\n\n\n----------\n----------\n----------\n\n\n\nlength(t) =\n"); print(length(t))
  # cat("\n\n\n----------\n----------\n----------\n\n\n\nf =\n"); print(f)
  # cat("\n\n\n----------\n----------\n----------\n\n\n\nx =\n"); print(x)
  # cat("\n\n\n----------\n----------\n----------\n\n\n\ni =\n"); print(i)
  # cat("\n\n\n----------\n----------\n----------\n\n\n\nt =\n"); print(t)

  # "pos_peak_samples" is the set of samples within the file at which the signal amplitude is approximately 1.0
  # "neg_samples" is the set of samples within the file at which the signal amplitude is less than 0.0
  pos_peak_samples <- numeric(max_f); neg_samples <- numeric(max_f); ai <- 1; bi <- 1
  for (j in 1:max_f) {
    if (x[j]>0.995) { pos_peak_samples[ai] <- j; ai <- ai+1 }
    if (x[j]<0.0) { neg_samples[bi] <- j; bi <- bi+1 }
  }
  length(pos_peak_samples) <- ai-1; length(neg_samples) <- bi-1
  # cat("\n\n\n----------\n----------\n----------\n\n\n\nlength(pos_peak_samples) =\n"); print(length(pos_peak_samples))
  # cat("\n\n\n----------\n----------\n----------\n\n\n\npos_peak_samples =\n"); print(pos_peak_samples)
  # cat("\n\n\n----------\n----------\n----------\n\n\n\nlength(neg_samples) =\n"); print(length(neg_samples))
  # cat("\n\n\n----------\n----------\n----------\n\n\n\nneg_samples =\n"); print(neg_samples)

  # pos_peak_immed_follow_neg_samples is an array of transition points betw high freq and low freq.
  # This array does not show whether it is a transition hi->lo or lo->hi, but that can be determined by
  # sampling sufficiently far away from the transition.
  # This transition point is not exactly accurate; the positive error will always be on the side with the high freq,
  # and the error with average to be 50% of the high freq period.
  pos_peak_immed_follow_neg_samples <- numeric(length(pos_peak_samples)); cci <- 1
  for (cindex in 2:length(pos_peak_samples)) {
    pos_sample <- pos_peak_samples[cindex]; prev_pos_sample <- pos_peak_samples[cindex-1]
    if ((length(intersect(neg_samples, c((prev_pos_sample+1):(pos_sample-1)) ))>0)) {
      pos_peak_immed_follow_neg_samples[cci] <- pos_sample; cci <- cci+1
    }
  }
  length(pos_peak_immed_follow_neg_samples) <- cci-1
  cc <- pos_peak_immed_follow_neg_samples
  cum_pd <- pos_peak_immed_follow_neg_samples
  # Per above ...
  # "cc", and "pos_peak_immed_follow_neg_samples" & "cum_pd", are the samples (starting at 1, at the beginning of the file)
  # at which a value in "pos_peak_samples" that is immediately preceded by a value in "neg_samples" (and not another value
  # from "pos_peak_samples") occurs.  These points are the _possible_ approximate transition points betw "high frequency"
  # and "low frequency" periods.
  # cat("\n\n\n----------\n----------\n----------\n\n\n\ncc = pos_peak_immed_follow_neg_samples =\n"); print(pos_peak_immed_follow_neg_samples)
  # cat("length(pos_peak_immed_follow_neg_samples) = "); print(length(pos_peak_immed_follow_neg_samples))

  # pd is number of samples betw "peaks", where a "peak" is a point found in "pos_peak_immed_follow_neg_samples" (i.e., "cc").
  pd <- numeric(length(pos_peak_immed_follow_neg_samples)-1)
  for (j in 2:length(pos_peak_immed_follow_neg_samples)) {
    pd[j-1] <- pos_peak_immed_follow_neg_samples[j] - pos_peak_immed_follow_neg_samples[j-1] + 1
  }
  # cat("\n\n\n----------\n----------\n----------\n\n\n\npd =\n"); print(pd)
  freq <- 2.048e6/pd

  library(plyr)
  ff <- arrange(count(pd), desc(freq))
  # cat("\n\n\n----------\n----------\n----------\n\n\n\nff =\n"); print(ff)
  # hfreqs is the list of the very likely set of "high frequency" periodicities expressed in terms of samples.
  hfreqs <- c((floor(ff$x[1] * 0.9)):(ceiling(ff$x[1] * 1.1)))
  # cat("\n\n\n----------\n----------\n----------\n\n\n\nhfreqs =\n"); print(hfreqs)

  # Let's look for the prelude ... start with looking for at least 1000 consecutive samples containing only high frequency RF

  is_hf <- function(f) { ifelse(f %in% hfreqs,1,0) }
  in_hf <- sapply(pd, is_hf)
  # in_hf_s is a string of 1/0 values with value i indicating if the RF is in high or low frequency at sample pd[i]
  in_hf_s <- paste(in_hf, collapse='')
  # cat("\n\n\n----------\n----------\n----------\n\n\n\nin_hf_s =\n"); print(in_hf_s)

  num_consec_hf <- ceiling(1000/max(hfreqs))
  target_hf <- paste(replicate(num_consec_hf, '1'), collapse='')

  # Find the beginning and end of the high freq prelude (i.e., invalid data), and the high freq portion of the header (valid data)
  cc_index_first_peak_of_hf_prelude <- regexpr(target_hf, in_hf_s, fixed=T)[1]
  # cat("\n\n\n----------\n----------\n----------\n\n\n\ncc_index_first_peak_of_hf_prelude = "); print(cc_index_first_peak_of_hf_prelude)
  # cat("substring(in_hf_s, cc_index_first_peak_of_hf_prelude) = "); print(substring(in_hf_s, cc_index_first_peak_of_hf_prelude))
  cc_index_first_peak_of_lf_prelude <- regexpr('0', substring(in_hf_s, cc_index_first_peak_of_hf_prelude), fixed=T)[1] + cc_index_first_peak_of_hf_prelude - 1
  # cat("cc_index_first_peak_of_lf_prelude = "); print(cc_index_first_peak_of_lf_prelude)
  # cat("substring(in_hf_s, cc_index_first_peak_of_lf_prelude) = "); print(substring(in_hf_s, cc_index_first_peak_of_lf_prelude))
  cc_index_first_peak_of_hf_header <- regexpr('1',substring(in_hf_s, cc_index_first_peak_of_lf_prelude), fixed=T)[1] + cc_index_first_peak_of_lf_prelude - 1
  # cat("cc_index_first_peak_of_hf_header = "); print(cc_index_first_peak_of_hf_header)
  # cat("substring(in_hf_s, cc_index_first_peak_of_hf_header) = "); print(substring(in_hf_s, cc_index_first_peak_of_hf_header))
  cc_index_last_peak_of_hf_header <- regexpr('0', substring(in_hf_s, cc_index_first_peak_of_hf_header), fixed=T)[1] + cc_index_first_peak_of_hf_header - 1
  # cat("cc_index_last_peak_of_hf_header = "); print(cc_index_last_peak_of_hf_header)

  # From the prelude and the header, determine a starting guess for the number of samples per bit and
  # the locations of the start and midpoint of subsequent bits
  # cat("\n\n\n----------\n----------\n----------\n\n\n\ncc_index_last_peak_of_hf_header = "); print(cc_index_last_peak_of_hf_header)
  # cat("cc[cc_index_last_peak_of_hf_header] = "); print(cc[cc_index_last_peak_of_hf_header])
  # cat("cc_index_first_peak_of_hf_header = "); print(cc_index_first_peak_of_hf_header)
  # cat("cc[cc_index_first_peak_of_hf_header] = "); print(cc[cc_index_first_peak_of_hf_header])
  apx_samples_per_bit <- (cc[cc_index_last_peak_of_hf_header] - cc[cc_index_first_peak_of_hf_header])
  # cat("apx_samples_per_bit = "); print(apx_samples_per_bit)
  # cat("\n\n\n----------\n----------\n----------\n\n\n\nff$x[1] =\n"); print(ff$x[1])
  # apx_bit_boundary <- (cc[cc_index_last_peak_of_hf_header] - cc[cc_index_first_peak_of_hf_header]) / 2 + cc[cc_index_first_peak_of_hf_header]
  # cat("apx_bit_boundary = "); print(apx_bit_boundary)
  apx_mid_bit_boundary <- cc[cc_index_first_peak_of_hf_header]
  # cat("apx_mid_bit_boundary (starting) = cc[cc_index_first_peak_of_hf_header] = "); print(cc[cc_index_first_peak_of_hf_header])
  apx_samples_per_qrtr_bit <- floor(apx_samples_per_bit / 4.0)
  # cat("apx_samples_per_qrtr_bit = "); print(apx_samples_per_qrtr_bit)

  # We know the size of the packet from previous experimentation
  bits_per_pkt <- 69
  first_half_sample <- numeric(bits_per_pkt); second_half_sample <- numeric(bits_per_pkt); pkt_starts <- numeric(bits_per_pkt)
  mid_bits <- numeric(bits_per_pkt); first_bit_half <- numeric(bits_per_pkt); second_bit_half <- numeric(bits_per_pkt)
  valid_bit <- numeric(bits_per_pkt); bit_val <- numeric(bits_per_pkt)
  hf_at_sample <- function(samp) { in_hf[sum(cc <= samp)] == "1" }

  # We will iterate on our estimate for samples per bit, increasing that estimate until
  # we identify a validly encoded differential Manchester bit stream
  orig_samples_per_bit <- apx_samples_per_bit
  success <- FALSE

  while ((!success) && (apx_samples_per_bit / orig_samples_per_bit < 1.2)) {
    success <- TRUE
    for (j in 1:length(first_half_sample)) {
      if (j == 1) {
        mid_bits[j] <- apx_mid_bit_boundary
      }
      else {
        mid_bits[j] <- mid_bits[j-1] + apx_samples_per_bit
        tmp_mid_bits <- cc[abs(cc - mid_bits[j])<5.0]
        mid_bits[j] <- ifelse(length(tmp_mid_bits)>0, tmp_mid_bits[order(abs(tmp_mid_bits - mid_bits[j]))][1], mid_bits[j])
      }
      pkt_starts[j] <- floor(ifelse(j==1, (mid_bits[1] - apx_samples_per_bit/2) , (mid_bits[j] + mid_bits[j-1])/2))
      first_half_sample[j] <- mid_bits[j] - apx_samples_per_qrtr_bit
      second_half_sample[j] <- mid_bits[j] + apx_samples_per_qrtr_bit
      first_bit_half[j] <- hf_at_sample(first_half_sample[j])
      second_bit_half[j] <- hf_at_sample(second_half_sample[j])
      valid_bit[j] <- xor(first_bit_half[j], second_bit_half[j])
      success <- success & valid_bit[j]
      bit_val[j] <- ifelse((valid_bit[j] == FALSE), 'X', ifelse((j==1), '1', ifelse((first_bit_half[j] == second_bit_half[j-1]), '1', '0')))
      if (!success) { break }
    }

    if (success) {
      # cat("\n\n\n----------\n----------\n----------\n\n\n\napx_samples_per_bit =\n"); print(apx_samples_per_bit)
      # cat("\n\n\n\npkt_starts [sample instances] =\n"); print(pkt_starts)
      # cat("mid_bits [sample instances] =\n"); print(mid_bits)
      # cat("\n\n\n\nfirst_half_sample [sample instances] =\n"); print(first_half_sample)
      # cat("second_half_sample [sample instances] =\n"); print(second_half_sample)
      # cat("\n\n\n\nfirst_bit_half [baseband value] =\n"); print(first_bit_half)
      # cat("second_bit_half [baseband value] =\n"); print(second_bit_half)
      # cat("valid_bit =\n"); print(valid_bit)
      # cat("bit_val [binary value] =\n"); print(bit_val)
      # cat("\n\n\n\nsuccess = "); print(success)

      library(BMS)
      cat("address = "); print(as.hexmode(strtoi(paste(bit_val[3:30],collapse=''), base=2)))
      ad <- strtoi(paste(bit_val[3:30],collapse=''), base=2)
      rc <- 8 # receiver id
      cat("temp(F) = "); print((strtoi(paste(bit_val[53:60], collapse=''), base=2)-40)*1.8+32)
      tc <- strtoi(paste(bit_val[53:60], collapse=''), base=2)-40
      cat("pressure(kPa) = "); print(strtoi(paste(bit_val[37:44], collapse=''), base=2)*2.5-100)
      cat("pressure(psi) = "); print((strtoi(paste(bit_val[37:44], collapse=''), base=2)*2.5-100)*0.145037738)
      pr <- ((strtoi(paste(bit_val[37:44], collapse=''), base=2)*2.5-100)*0.145037738)
      dt_tm <- strftime(Sys.time(), "%Y-%m-%d%%20%H:%M:%S", tz="GMT")
      u = paste("http://www.fleetcents.com/tire_samples/create?tire_sample[sensor_id]=", ad, "&tire_sample[receiver_id]=",
        rc, "&tire_sample[psi]=", pr, "&tire_sample[tempc]=", tc, "&tire_sample[sample_time]=", dt_tm, sep = '')
      library(httr)
      print(u)
      uu <- GET(url=u)
      print(uu)
      flag <- TRUE
    }
    else {
      apx_samples_per_bit <- apx_samples_per_bit + 1
    }
  }
  if (flag) {
    print(Sys.time())
    break
  }
}

