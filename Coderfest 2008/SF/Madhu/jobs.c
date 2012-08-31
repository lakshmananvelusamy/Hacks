#include<stdio.h>

#define NAME_SZ   8
#define MAX_JOBS_PER_PART  1024
#define MAX_JOBS_BY_MACHINE   7

#define PART_SLNO_START 1000

int total_machines;
int total_jobs;
int total_parts;

int remaining_parts;
int tsec;
int alloc_time = 1024;

struct job
{ 
   char jname[NAME_SZ];
   int  rtime; /* run times */
   int count; /* number of machines that does this job */
} *jobs;

struct part
{
   char pname[NAME_SZ];
   int jobs[MAX_JOBS_PER_PART]; /* number of jobs to done to complete this part */
   int totjobs; /* */
   int count; /* number of parts to be completed */
   int count_added; /* number of parts that can be additionally manufactored */
} *parts;

struct machine
{
   char mname[NAME_SZ];
   int jobs[MAX_JOBS_BY_MACHINE]; /* jobs that can be done by this machine */
   int totjobs;
   int partid; 
   int partsrno;
   int partidx;
   int jid;
   int rtime;
   int workedby,workingfor;
} *machines;

struct readyjob
{
   int partid;
   int partsrno;
   int partidx;
   int jid;
   int rtime;
   int starttime;
   double priority;
   int priority_incby;
} *runq;

struct schedule_chart
{
   int partid;
   int partsrno;
   int jid;
   int secondfill;
} **chart;

int find_job (char* jname)
{ /* find a job id given its name */
	int i=0;

	for (i=0; i < total_jobs; i++)
		if (!strcmp(jobs[i].jname, jname))
			return i;

	return -1;
}

int find_part (char* pname)
{ /* find a part id given its name */
	int i;

	for (i=0; i < total_parts; i++)
		if (!strcmp(parts[i].pname, pname) )
			return i;

	return -1;
}

int read_parts_file (char * filename)
{ /* read parts file and init parts structures */
   char name[NAME_SZ];
   int count;
   int read;
	int alloc = 0;

   FILE *fptr = fopen (filename, "r\0");

   if (fptr == NULL) return 1;

   while (!feof(fptr)) {
      memset(name, 0, sizeof(name)); count = 0;

      if (!(read = fscanf (fptr, "%s %d\n", name, &count))) break;
      if (read == 1) return 3;
      if (name[0] == '\0') break;
      if (name[0] != 'P' && name[0] != 'p') return 2;

		if (alloc == total_parts)
			parts = (struct part*) realloc(parts, (alloc+=1024) * sizeof(struct part));

		strcpy(parts[total_parts].pname, name);
		parts[total_parts].count = count;
      parts[total_parts].count_added = 0;

		total_parts++;
   }

   fclose (fptr);
   return 0;
}


int read_jobs_file (char * filename)
{ /* read jobs file and init internal structures */
   char name[NAME_SZ];
   int tim;
   int read;
	int alloc = 0;

   FILE *fptr = fopen (filename, "r\0");

   if (fptr == NULL) return 1;

   while (!feof(fptr)) {
      memset(name, 0, sizeof(name)); tim = 0;

      if (!(read = fscanf (fptr, "%s %d\n", name, &tim))) break;
      if (read == 1) return 3;
      if (name[0] == '\0') break;
      if (name[0] != 'J' && name[0] != 'j') return 2;
      if (tim < 2 || tim > 12) return 3;

      if (alloc == total_jobs)
         jobs = (struct job*) realloc(jobs, (alloc+=1024) * sizeof(struct job));

      strcpy(jobs[total_jobs].jname, name);
      jobs[total_jobs].rtime = tim;
      jobs[total_jobs].count = 0;
      total_jobs++;
   }

   fclose (fptr);
   return 0;
}


int read_part_jobs_file (char* filename)
{ /* reads jobs sequence file and inits parts structures */
   char str[1024];
   char name[NAME_SZ];
   char* pstr, *tok;
   int read, ct;

   int pid, jid, jidx;

   FILE *fptr = fopen (filename, "r\0");

   if (fptr == NULL) return 1;

   ct = 0;
   while (!feof(fptr)) {
      memset(name, 0, sizeof(name));
      memset(str, 0, sizeof(str));

      if (!(read = fscanf (fptr, "%s %[^\n]s\n", name, str))) break;
      if (read == 1) return 4;
      if (name[0] == '\0') break;
      if (name[0] != 'P' && name[0] != 'p') return 2;

		pid = find_part(name);
		if (pid == -1) return 1;
		jidx = 0;

      pstr = (char *) str; tok = (char *)strtok (pstr, " ");
      while (tok != NULL) {
         if (tok[0] != 'J' && tok[0] != 'j') return 3;
			jid = find_job(tok);
			if (jid == -1) return 2;

			parts[pid].jobs[jidx] = jid;
			jidx++;

         tok = (char *)strtok ((char*)NULL, " ");
      }
		parts[pid].jobs[jidx] == -1;
		parts[pid].totjobs = jidx;
      ct++;
   }
  
   if (ct != total_parts) return 4;

   fclose (fptr);
   return 0;
}


int read_machines_file (char* filename)
{ /* reads machine file and inits machines structure */
   char str[1024];
   char name[5];
   char* pstr, *tok;
   int read;

   long long llid;
   int id;
	int alloc = 0;

	int jid, jidx, mid;

   FILE *fptr = fopen (filename, "r\0");

   if (fptr == NULL) return 1;

   while (!feof(fptr)) {
      memset(name, 0, sizeof(name));
      memset(str, 0, sizeof(str));

      if (!(read = fscanf (fptr, "%s %[^\n]s\n", name, str))) break;
      if (read == 1) return 4;
      if (name[0] == '\0') break;
      if (name[0] != 'M' && name[0] != 'm') return 2;

      if (alloc == total_machines)
         machines = (struct machine*) realloc(machines, (alloc+=1024) * sizeof(struct machine));

		strcpy(machines[total_machines].mname, name);
		jidx = 0;

      pstr = (char*)str; tok = (char *)strtok (pstr, " ");
      while (tok != NULL) {
         if (tok[0] != 'J' && tok[0] != 'j') return 3;
			jid = find_job(tok);
			if (jid == -1) return 1;

			jobs[jid].count++;
			machines[total_machines].jobs[jidx] = jid;

         tok = (char *)strtok ((char*)NULL, " "); jidx++;
      }

      machines[total_machines].totjobs = jidx;
      machines[total_machines].partid = -1;
      machines[total_machines].partidx = -1;
      machines[total_machines].partsrno = -1;
      machines[total_machines].jid = -1;
      machines[total_machines].rtime = 0;
      machines[total_machines].workedby = -1;
      machines[total_machines].workingfor = -1;

      total_machines++;
   }

   fclose (fptr);
   return 0;
}

init(int argc, char *argv[])
{
	int i,j,p;

	if (argc != 5) {
		fprintf (stderr, "Insufficient number of arguments\n");
		fprintf (stderr, "Usage : %s  <parts_file> <job_sequence_file> <machine_file> <jobs_file>\n", argv[0]);

		exit(-1);
	}

/*BUG this should come as third what if part definitions are more but less parts are requested  */
	if (read_parts_file(argv[1])) {
		fprintf (stderr, "Error reading parts file %s\n", argv[1]);
		exit(-1);
	}

	if (read_jobs_file(argv[4])) {
		fprintf (stderr, "Error reading jobs file %s\n", argv[2]);
		exit(-1);
	}

	if (read_part_jobs_file(argv[2])) {
		fprintf (stderr, "Error reading part jobs file %s\n", argv[3]);
		exit(-1);
	}


	if (read_machines_file(argv[3])) {
		fprintf (stderr, "Error reading machines file %s\n", argv[4]);
		exit(-1);
	}

	for(i=0;i<total_parts;i++)
		remaining_parts += parts[i].count;

	runq=(struct readyjob *) malloc(remaining_parts * sizeof(struct readyjob));

	for(p = i= 0; i < total_parts; i++) {
		for(j = 0; j < parts[i].count; j++) {
			runq[p].partid    = i;
			runq[p].partsrno  = PART_SLNO_START + j;
			runq[p].partidx   = 0;
			runq[p].jid       = parts[i].jobs[0];
			runq[p].starttime = runq[p].priority=0;
			runq[p].rtime     = jobs[parts[i].jobs[0]].rtime;
			runq[p].priority_incby = -1;
			p++;
		}
	}

	chart = (struct schedule_chart **) malloc (total_machines * sizeof(struct schedule_chart *));
	for(i = 0; i < total_machines; i++)
		chart[i] = (struct schedule_chart *) malloc (alloc_time * sizeof(struct schedule_chart));

	return 0;
}

int machineCanDo (int m, int jid)
{
    int i;
    for (i=0; i < machines[m].totjobs; i++)
        if (machines[m].jobs[i] == jid)
            return 1;

   return 0;
}


/* check if we have enough free slots to run the job */
int checkSlotsFor (int m, int jid,int slotIndex)
{
    int rtime = jobs[jid].rtime;
    int tt,r;

    for (r=0,tt=slotIndex; (tt < tsec) && (chart[m][tt].partid == -1); tt++)
        if (++r == rtime) return 1;

    return 0;
}

void markSlots(int m,int partid,int partsrno,int jid,int slotIndex,int rtime)
{
    int r,tt;

    for (r=0,tt=slotIndex; r<rtime; tt++,r++) {
        chart[m][tt].partid = partid;
        chart[m][tt].partsrno = partsrno;
        chart[m][tt].jid = jid;
        chart[m][tt].secondfill = 1;
    }
}


int checkPart (int partid,int partidx,int partsrno,int slotIndex)
{
    int jid,m,tt;

    /* we have found slots for all jobs for this part */
   if (partidx >= parts[partid].totjobs) return 1;

   jid = parts[partid].jobs[partidx];

   for (tt=slotIndex;tt<tsec;tt++) {
      for (m=0;m<total_machines;m++) {
         if (!machineCanDo (m,jid)) continue;

         if (checkSlotsFor (m,jid,tt)) {
            markSlots(m,partid,partsrno,jid,tt,jobs[jid].rtime);

            if (checkPart (partid,partidx+1,partsrno,tt+jobs[jid].rtime)) return 1;
            else {
               markSlots(m,-1,-1,-1,tt,jobs[jid].rtime);
               return 0;
            }
         }
      } 
   }
   return 0;
}


void fillEmptySlots ()
{
	int partscreated = 1,p;
	int *partsrnos = (int *) malloc (total_parts*sizeof(int));

	for(p=0;p<total_parts;p++) partsrnos[p]=1000;

	while (partscreated) {
		partscreated=0;
		for (p = 0; p < total_parts; p++) {
			if(checkPart (p, 0, partsrnos[p]+1, 0)) {
				partsrnos[p]++;
				partscreated++;
            parts[p].count_added++;
			}
		}
	}
	free(partsrnos);
}


int countMachineForJobs(int jid)
{
	int m,cnt=0;
	for(m = 0; m < total_machines; m++) {
		if(machines[m].partid != -1) continue;
		if(machineCanDo(m,jid)) cnt++;
	}
	return cnt;
}

int countJobsForMachine (int m)
{
   int i;
   int cnt=0;

   for (i=0; i < remaining_parts; i++) {
      if(runq[i].starttime) continue;
         if (machineCanDo (m,runq[i].jid))
            cnt++;
   }
   return cnt;
}



int getBestMachine (int rqid)
{
    int machineid = -1;
    int min = remaining_parts + 1;
    int i;

    for (i = 0; i< total_machines;i++) {
        int cnt;

        if (machines[i].partid != -1) continue;

        if (!machineCanDo (i, runq[rqid].jid))
            continue;
        cnt = countJobsForMachine (i);
        if (cnt < min) {
            min = cnt;
            machineid = i;
        }
    }

    return machineid;
}


void increasePriorityFor(int jid,int starttime,int mid)
{
	int i,j,r;
	double dist=0,tpriority=0;
	
	for(r=0;r<remaining_parts;r++) {
		int partid;
		if(runq[r].starttime) continue; /* upcoming one */

		partid=runq[r].partid;
		dist=runq[r].rtime;

		for(j=runq[r].partidx+1;j<parts[partid].totjobs;j++) {
			if(parts[partid].jobs[j]==jid) {

				if(dist<starttime) dist=starttime;
				tpriority=jobs[jid].rtime/(dist+jobs[jid].rtime);
				if(tpriority>runq[r].priority) {
					runq[r].priority=tpriority;
					runq[r].priority_incby=mid;
				}
				break;
			}
			dist+=jobs[parts[partid].jobs[j]].rtime;
		}
	}
}

int getJobFor(int m)
{
	int i;

	for(i=0;i<remaining_parts;i++) {
		if(runq[i].starttime) continue;
		if(machineCanDo(m,runq[i].jid)) return runq[i].jid;
	}
	return -1;
}

int prioritizeJobs(void)
{
	int i,j,r,m,n,jid;

	for(r=0;r<remaining_parts;runq[r].priority=0,runq[r].priority_incby=-1,r++);
	
	for(m=0;m<total_machines;m++) {
		if(machines[m].workedby != -1) continue;

		n=countJobsForMachine(m);
		if(n<=1)
			for(j=0;j<machines[m].totjobs;j++)
				if(n)
				{
					jid=getJobFor(m);
					increasePriorityFor(machines[m].jobs[j],machines[m].rtime+jobs[jid].rtime,m);
				}
				else
					increasePriorityFor(machines[m].jobs[j],machines[m].rtime,m);
	}

	for(i=0;i<remaining_parts;i++)
		for(j=i;j<remaining_parts;j++) {
         if(runq[i].priority<runq[j].priority || 
            (runq[i].priority==runq[j].priority && jobs[runq[i].jid].count>jobs[runq[j].jid].count)) {
            struct readyjob tmp=runq[i];
            runq[i]=runq[j];
            runq[j]=tmp;
         }
      }
}


void printChart(void)
{
	int i, j, m;

   FILE *outptr;

   outptr = fopen("output.txt", "w");
   if (outptr == NULL)
      outptr = stdout;      

   fprintf (outptr, "\n0");
   for(m=0; m<total_machines; m++) { /* dummy run, to satisfy contest's output format */
      if(chart[m][0].partid!=-1)
         fprintf(outptr, " (%s,%s:%4d,%s)",machines[m].mname,parts[chart[m][0].partid].pname, chart[m][0].partsrno, jobs[chart[m][0].jid].jname);
      else
         fprintf(outptr, " (%s,0,0)",machines[m].mname);
   }


	for(i=0;i<tsec;i++) {
		fprintf(outptr, "\n%d",i+1);
		for(m=0;m<total_machines;m++) {
			if(chart[m][i].partid!=-1) {
				fprintf(outptr, " (%s,%s:%4d,%s)",machines[m].mname,parts[chart[m][i].partid].pname, chart[m][i].partsrno, jobs[chart[m][i].jid].jname);
         } else {
            if (i != (tsec-1) && chart[m][i+1].partid!=-1) { /* again for contest's output format */
               fprintf(outptr, " (%s,%s:%4d,%s)",machines[m].mname,parts[chart[m][i+1].partid].pname, chart[m][i+1].partsrno, jobs[chart[m][i+1].jid].jname);
            }
            else {
               fprintf(outptr, " (%s,0,0)",machines[m].mname);
            }
         }
		}
	}

   fflush(outptr);
   if (outptr != stdout)
      fclose(outptr);
}

int getTotalEmptySlots(void) {
    int cnt = 0;
    int i,j;

    for (i=0;i<total_machines;i++)
        for (j=0;j<tsec;j++)
            if (chart[i][j].partid == -1)
                cnt++;

    return cnt;
}

int main(int argc,char *argv[])
{

	int m,i;

	init(argc,argv);

	while(remaining_parts) {
		int r;
		prioritizeJobs();
	
		for(r=0;r<remaining_parts;r++) {
			if(runq[r].starttime) continue;
			if( (m=getBestMachine(r))== -1) continue;

			int partid=runq[r].partid;
			
			machines[m].partid=partid;
			machines[m].partsrno=runq[r].partsrno;
			machines[m].partidx=runq[r].partidx;			
			machines[m].jid=runq[r].jid;
			machines[m].rtime=runq[r].rtime;
			machines[m].workingfor=runq[r].priority_incby;
			if(runq[r].priority_incby!=-1)
				machines[runq[r].priority_incby].workedby=m;
			
			if(runq[r].partidx+1 < parts[partid].totjobs) {
				runq[r].partidx++;
				runq[r].starttime=runq[r].rtime;
				runq[r].jid=parts[partid].jobs[runq[r].partidx];
				runq[r].rtime=jobs[runq[r].jid].rtime;
			} else {
				for(r;r<remaining_parts-1;r++)
					runq[r]=runq[r+1];
				remaining_parts--;
			}

			
			prioritizeJobs();
			r=-1;
		}

		for(m=0;m<total_machines;m++) {
			if(machines[m].partid != -1) /* if machine is working */ {
				chart[m][tsec].partid=machines[m].partid;
				chart[m][tsec].partsrno=machines[m].partsrno;
				chart[m][tsec].jid=machines[m].jid;
				chart[m][tsec].secondfill=0;

				machines[m].rtime--;
				for(r=0;r<remaining_parts;r++) {
					if(runq[r].partid==machines[m].partid && 
						runq[r].partsrno==machines[m].partsrno) {
						runq[r].starttime--;
						break;
					}
				}

				if(machines[m].rtime==0) {
					for(r=0;r<remaining_parts;r++) {
						if(runq[r].partid==machines[m].partid && 
							runq[r].partsrno==machines[m].partsrno)
							if(runq[r].starttime) {
								runq[r].starttime = 0;
							}
					}
					machines[m].partid=machines[m].partsrno=machines[m].partidx=-1;
					machines[m].jid=-1;
					if(machines[m].workingfor!=-1)
						machines[machines[m].workingfor].workedby=-1;
					machines[m].workingfor=-1;
				}
			}
			else
				chart[m][tsec].partid=-1;

		}

		if(++tsec == alloc_time) {
			alloc_time*=2;
			for(m=0;m<total_machines;m++)
				chart[m]=(struct schedule_chart *) realloc(chart[m],sizeof(struct schedule_chart) * alloc_time);
		}
	}

	while(1) {
		i=0;
		for(m=0;m<total_machines;m++) {
			chart[m][tsec].partid=machines[m].partid;
			chart[m][tsec].partsrno=machines[m].partsrno;
			chart[m][tsec].jid=machines[m].jid;
			chart[m][tsec].secondfill=0;
			if(machines[m].partid != -1) {
				i=1;
				--machines[m].rtime;
				if(machines[m].rtime == 0)
					machines[m].partid=-1;
			}
		}
		if(!i)  break;
		if(++tsec == alloc_time) {
			alloc_time*=2;
			for(m=0;m<total_machines;m++)
				chart[m]=(struct schedule_chart *) realloc(chart[m],sizeof(struct schedule_chart) * alloc_time);
		}
	}

	printChart();
   printf ("Number of empty slots : %d\n", getTotalEmptySlots());
	fillEmptySlots();

   printf ("Predicted number of empty slots after manufacturing additional parts : %d\n", getTotalEmptySlots());

   printf ("Following parts can be manufactured additionally\n");
   printf ("Part\tCount\n");
   for (i = 0; i < total_parts; i++) {
      if (parts[i].count_added) {
         printf ("%s\t%d\n", parts[i].pname, parts[i].count_added);
      }  
   }  
}
