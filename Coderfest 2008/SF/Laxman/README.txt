INPUT:

1)No of Parts specification file
2)Job sequence for parts specification file
3)Machine details file
4)Job time specification file

OUTPUT:

1) output1.txt
	This file has the plan for given no of parts.

2) extra.txt 
	This file has the plan to produce extra parts by filling the gaps in the main plan generated already.


LOGIC:

For main plan:

We have schduled the jobs based on the priority.

The priority for jobs is
 1) The no of jobs of a part is waiting to be scduled 
 2) No of the jobs it is blocking.

We are taking the job with highest priority.The priority will change dynamically.