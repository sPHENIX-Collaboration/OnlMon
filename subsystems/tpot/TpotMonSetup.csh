#! /bin/csh
if (! $?TPOTCALIBREF ) then
  setenv TPOTCALIBREF /home/phnxrc/operations/TPOT/onlmon_ref
  echo "TPOTCALIBREF environment variable set to $TPOTCALIBREF"
  exit
endif

setenv TPOTCALIB $ONLMON_CALIB/tpot
