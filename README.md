# Parallel Load Balancer 

Load balancer developed in C as a project for the Parallel and Distributed Programming course, part of the Applied CS program at WUT.

## Usage

Compile:
```
make 
```

Clean:
```
make clean
```

Example run:
```
./loadBalancer -n 10 -alg ROUND_ROBIN
./loadBalancer -sync
```
The program allows running with three flags (if no or incorrect values are specified, the default configuration is used):
-n - specify the number of threads.
-alg - specify the load balancing algorithm (ROUND_ROBIN or IP_HASH).
-sync - run the load balancer fully synchronously.

## Measure Load Balancer Performance

To measure how quickly the load balancer processes requests:
```
cd scripts/
./executor.sh 10 
```
You can specify the number of processes that will 
make requests by changing the argument passed to executor.sh
