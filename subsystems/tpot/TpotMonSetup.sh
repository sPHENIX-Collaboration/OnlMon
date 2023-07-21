#! /bin/bash


if [[ -z "$TPOTCALIBREF" ]]
then
  export TPOTCALIBREF=/home/phnxrc/operations/TPOT/onlmon_ref
  echo "TPOTCALIBREF environment variable set to $TPOTCALIBREF"
  return
fi

export TPOTCALIB=$ONLMON_CALIB/tpot
