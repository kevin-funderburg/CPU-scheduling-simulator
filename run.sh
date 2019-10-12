#l!/bin/bash
rm sim.data
rm sim.csv
#echo 'sheduler is: ' $scheduler
for ((i = 1; i < 31; i++)); do
   ./sim 1 $i 0.06 0.2

    cp sim.data ./output/sim-1-$i-001.data
done
