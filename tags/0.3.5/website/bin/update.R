#! /usr/bin/Rscript
ip <- installed.packages()
pkgs.to.remove <- ip[!(ip[,"Priority"] %in% c("base", "recommended")), 1]
remove.packages(pkgs.to.remove)
local({
  r <- getOption("repos")
  r["CRAN"] <- "http://cran.revolutionanalytics.com"
  options(repos = r)
})
update.packages(ask=FALSE, repos="http://cran.revolutionanalytics.com")
install.packages(c("R.utils","png","RMySQL","jpeg", "devtools"),repo="http://cran.revolutionanalytics.com",dep=TRUE)
q(status=0)
