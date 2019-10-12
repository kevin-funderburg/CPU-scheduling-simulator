#l!/bin/bash
rm sim.data
rm sim.csv
#echo 'sheduler is: ' $scheduler
for ((i = 1; i < 31; i++)); do
<<<<<<< HEAD:run
   ./sim 1 $i 0.06 0.01
=======
   ./sim 1 $i 0.06 0.2
>>>>>>> version2:run.sh

    cp sim.data ./output/sim-1-$i-001.data
done
