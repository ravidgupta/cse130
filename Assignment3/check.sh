#!/bin/bash

#######################################################################
#
# Copyright (C) 2020-2021 David C. Harrison. All right reserved.
#
# You may not use, distribute, publish, or modify this code without 
# the express written permission of the copyright holder.
#
#######################################################################

timeout --preserve-status 30s ./manpage $GRADINGFLAG > manpage.out

if [ -z ${GRADINGFLAG} ]
then
  diff manpage.out manpage.ck | tee manpage.diff
else
  diff manpage.out grading.ck | tee manpage.diff
fi

echo "" 
fail=`cat manpage.diff | wc -l`

rm manpage.diff manpage.out

(( pass = 1 ))
(( total = 1 ))

if (( fail > 0 ))
then
    (( pass = 0 ))
fi

(( passes += pass ))

(( pctot = 0 ))

pct=0;
if (( pass > 0 ))
then
  pct=`echo "scale=2; $pass / $total * 30.0" | bc -l`
fi
pctot=`echo "scale=2; $pctot + $pct" | bc -l`
printf "%20s:  %2d/%2d  %5.1f%% of 30%%\n" "Manpage" $pass $total $pct

printf "%20s:  " "Simple Cartman"
./cartman -s > cartman.out
if (( $? != 0 )) 
then
  echo "    Functional: FAIL   (program error)" >> cartman.out
fi
pass=`grep PASS cartman.out | wc -l`
(( passes += pass ))
pct=0;
if (( pass > 0 ))
then
  pct=`echo "scale=2; $pass / $total * 20.0" | bc -l`
fi
pctot=`echo "scale=2; $pctot + $pct" | bc -l`
printf "%2d/%2d  %5.1f%% of 20%%\n" $pass $total $pct

printf "%20s:  " "Deadlock Cartman"
./cartman -d > cartman.out
if (( $? != 0 )) 
then
  echo "    Functional: FAIL   (program error)" >> cartman.out
fi
pass=`grep PASS cartman.out | wc -l`
(( passes += pass ))
pct=0;
if (( pass > 0 ))
then
  pct=`echo "scale=2; $pass / $total * 20.0" | bc -l`
fi
pctot=`echo "scale=2; $pctot + $pct" | bc -l`
printf "%2d/%2d  %5.1f%% of 20%%\n" $pass $total $pct

printf "%20s:  " "Random Cartman"
./cartman -r > cartman.out
if (( $? != 0 )) 
then
  echo "    Functional: FAIL   (program error)" >> cartman.out
fi
pass=`grep PASS cartman.out | wc -l`
(( passes += pass ))
pct=0;
if (( pass > 0 ))
then
  pct=`echo "scale=2; $pass / $total * 20.0" | bc -l`
fi
pctot=`echo "scale=2; $pctot + $pct" | bc -l`
printf "%2d/%2d  %5.1f%% of 20%%\n" $pass $total $pct

(( tests = 4 ))

printf "\n%20s:  %2d/%2d  %5.1f%% of 90%%\n" "Tests" $passes $tests $pctot