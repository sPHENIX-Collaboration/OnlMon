#!/bin/bash
[[ -e htmlrunning ]] && exit 0
echo $$ > htmlrunning
source ./setup_all.sh >& /dev/null
Xvfb :0 -nolisten tcp -screen 0 1072x924x24 &
export DISPLAY=unix:0
perl makehtml.pl >& makehmtl.log
kill $!
rm htmlrunning
