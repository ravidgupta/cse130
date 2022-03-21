#!/bin/bash

#######################################################################
#
# Copyright (C) 2020-2021 David C. Harrison. All right reserved.
#
# You may not use, distribute, publish, or modify this code without 
# the express written permission of the copyright holder.
#
#######################################################################

pctot=0
passes=0

#####################################################

export LC_ALL=en_US.UTF-8

echo ""
timeout 5s ./fileman -b 2>&1 | grep -v 'cannot create' | tee fileman.out

pass=`grep PASS fileman.out | wc -l`
tests=10

pct=0;
if (( pass > 0 ))
then
  pct=`echo "scale=2; $pass / $tests * 30.0" | bc -l`
fi

printf "\n%20s:  %2d/%2d  %5.1f%% of 30%%\n" "Basic" $pass $tests $pct

pctot=`echo "scale=2; $pctot + $pct" | bc -l`
(( passes += pass ))
(( teststot += tests ))

#####################################################

echo ""
timeout 5s ./fileman -a 2>&1 | tee fileman.out

pass=`grep PASS fileman.out | wc -l`
tests=4

pct=0;
if (( pass > 0 ))
then
  pct=`echo "scale=2; $pass / $tests * 25.0" | bc -l`
fi

printf "\n%20s:  %2d/%2d  %5.1f%% of 25%%\n" "Advanced" $pass $tests $pct

pctot=`echo "scale=2; $pctot + $pct" | bc -l`
(( passes += pass ))
(( teststot += tests ))

#####################################################

echo ""
timeout 5s ./fileman -s 2>&1 | grep 'PASS\|FAIL' | tee fileman.out

pass=`grep PASS fileman.out | wc -l`
tests=1

pct=0;
if (( pass > 0 ))
then
  pct=`echo "scale=2; $pass / $tests * 20.0" | bc -l`
fi

printf "\n%20s:  %2d/%2d  %5.1f%% of 20%%\n" "Stretch" $pass $tests $pct

pctot=`echo "scale=2; $pctot + $pct" | bc -l`
(( passes += pass ))
(( teststot += tests ))

#####################################################

echo ""
timeout 5s ./fileman -e 2>&1 | grep 'PASS\|FAIL' | tee fileman.out

pass=`grep PASS fileman.out | wc -l`
tests=1

pct=0;
if (( pass > 0 ))
then
  pct=`echo "scale=2; $pass / $tests * 15.0" | bc -l`
fi

printf "\n%20s:  %2d/%2d  %5.1f%% of 15%%\n" "Extreme" $pass $tests $pct

pctot=`echo "scale=2; $pctot + $pct" | bc -l`
(( passes += pass ))
(( teststot += tests ))

#####################################################

printf "\n%20s:  %2d/%2d  %5.1f%% of 90%%\n" "Tests" $passes $teststot $pctot
