# Workflow Scheduling
scheduler.cpp
To compile scheduler.cpp:

g++ scheduler.cpp -o SCHEDULER

To run SCHEDULER

./SCHEDULER

SCHEDULER programs asks for number of input DAGs, number of processors, number of nodes and edges in each DAG, number their computation and communication costs and deadlines.

input/ folder contains sample DAGs which we used to generate results.

daggen.cpp
To compile daggen.cpp:

g++ daggen.cpp -o DAGGEN

To run DAGGEN

./DAGGEN > output.txt

DAGGEN generates multiple random dags which can be stored in a file and can be used as input. Deadline for all DAGs must be specified at the end of file before using them as inputs to the scheduler.
