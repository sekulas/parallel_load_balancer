#!/bin/bash

parallel_execution_number=2

if [ $# -eq 1 ]; then
    parallel_execution_number=$1
fi

start_time=$(date +%s.%N)

for ((i=0; i<$parallel_execution_number; i++)); do
    python3 client.py &
done

wait

end_time=$(date +%s.%N)
execution_time=$(echo "$end_time - $start_time" | bc)

if (( $(echo "$execution_time < 1" | bc -l) )); then
    execution_time="0$execution_time"
fi

echo "Time measured: $execution_time seconds"
