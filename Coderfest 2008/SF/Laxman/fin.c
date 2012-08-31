#include <stdio.h>
#include <string.h>
struct mac
{
        int start[100],end[100],gap,flag,prev,check;
}m[500],tm1[500][500];
struct fill
{
        int start[100],end[100],gap,flag,prev,check;
        int job[100],part[100],ID[500];
}f[500][500];

int fin[100],max;
struct part
{
        char name[4];
        int count, T;
}pr[500];
struct jobs
{
        char name[4], jobs[30][4];
        int P[30], T[30], Q[30],done[30],IN[30];
        int j_cnt, Qlen;
}js[500];

struct machine
{
        char name[5],jobs[8][4];
        int p, j, ID, status;
        int m_cnt;
}mc[500];

struct jobtime
{
        char name[4];
        char mc[30][5];
        int P,N,mno[30];
        int t;
}jid[500];

FILE *f1, *f2, *f3, *f4,*fp;
FILE *op,*fp1;
char tmp[6];
int Qi,tmp_T,fre;
int tot_jobs,tot_mc,tot_part,Qempty;
long t;
struct Qu
{
        int p, j,P,C;
}Q[500];

void Machine_pri_calculator();
void Job_pri_calculator();
void Sort_Queue();
int done();
void Update_Queue();
void extra_plan();
void fun();


int main()
{
int i,j,c,k,flag;
f1 = fopen("file1.txt","r");
f2 = fopen("file2.txt","r");
f3 = fopen("file3.txt","r");
f4 = fopen("file4.txt","r");
op = fopen("output1.txt","w");
fp = fopen("extra.txt","w");
tot_part= 0;
while (!feof(f1))
{
fscanf(f1,"%s %d\n",pr[tot_part].name,&pr[tot_part].count);
tot_part++;
}
fclose(f1);

i=0;
while (!feof(f4))
{
fscanf(f4,"%s %d\n",jid[i].name,&jid[i].t);
i++;
}
tot_jobs =i;
fclose(f4);

fscanf(f2,"%s",tmp);
for(i=0;i<tot_part;i++)
{
strcpy(js[i].name,tmp);
j=0;
tmp_T = 0;
strcpy(tmp,"");
	while(tmp[0] != 'P' && !feof(f2))
	{
		fscanf(f2,"%s",tmp);
		if ( tmp[0] != 'P' && tmp[0]!='\0')
		{
		for(k = 0; k < tot_jobs ; k++)
		if(!strcmp(tmp,jid[k].name))
		{
			tmp_T += jid[k].t;
			js[i].T[j] = jid[k].t;
			break;
		}
		strcpy(js[i].jobs[j],tmp);
		strcpy(tmp,"");
		j++;
		}
	}
pr[i].T = tmp_T;
js[i].j_cnt=j;
}
fclose(f2);
strcpy(tmp,"");
fscanf(f3,"%s",tmp);
i=0;
while( !feof(f3))
{
strcpy(mc[i].name,tmp);
j=0;
strcpy(tmp,"");
	while(tmp[0] != 'M' && !feof(f3))
	{
		fscanf(f3,"%s",tmp);
		if ( tmp[0] != 'M' && tmp[0]!='\0')
		{
		strcpy(mc[i].jobs[j],tmp);
		strcpy(tmp,"");
		j++;
		}
	}
mc[i].m_cnt=j;
i++;
}
fclose(f3);
tot_mc=i;

for(k = 0; k < tot_jobs; k++)
{
c=0;
	for(i = 0;i < tot_mc; i++)
	{
		for(j = 0; j < mc[i].m_cnt; j++)
		{
			if(!strcmp(mc[i].jobs[j],jid[k].name))
			{
				strcpy(jid[k].mc[c], mc[i].name);
				jid[k].mno[c] = i;
				c++;
				break;
			}
		}
	}
	jid[k].N = c;
}

for(i=0; i<tot_part; i++)
{
	js[i].Q[0] = pr[i].count;
	js[i].Qlen = 1;
}
printf("\n");
for(t = 0; done() ; t++)
{
	fprintf(op,"%d ",t);
		Job_pri_calculator();
		Sort_Queue();
		for(i = 0; i < tot_mc; i++)
		{

				if(mc[i].status)
				{
					mc[i].status--;
					if(mc[i].status == 0)
					{
						js[mc[i].p].done[mc[i].j]++;
						if(js[mc[i].p].Qlen < js[mc[i].p].j_cnt && js[mc[i].p].IN[mc[i].j+1] == 0)
								js[mc[i].p].Qlen++;
						js[mc[i].p].Q[mc[i].j+1] ++;
						Job_pri_calculator();
						Sort_Queue();
					}

				}
				if(!mc[i].status)
				{
					for(flag = 1,k=0;flag && k<Qi;k++)
					{

						for(j=0;j<mc[i].m_cnt;j++)
						{
							if(!strcmp(mc[i].jobs[j],js[Q[k].p].jobs[Q[k].j]) && Q[k].C)
							{
								mc[i].status = js[Q[k].p].T[Q[k].j];
								mc[i].p = Q[k].p;
								mc[i].j = Q[k].j;
								js[Q[k].p].IN[Q[k].j]++;
								mc[i].ID = 1000+js[Q[k].p].IN[Q[k].j];
								Q[k].C--;
								js[Q[k].p].Q[Q[k].j]--;
								flag = 0;
								break;
							}
						}
					}
				}
				if(!mc[i].status)
					fprintf(op,"(%s,0,0) ",mc[i].name);
				else
					fprintf(op,"(%s,%s:%d,%s) ",mc[i].name,js[mc[i].p].name,mc[i].ID,js[mc[i].p].jobs[mc[i].j]);					//printf("(%s,%s:%d,%s) ",mc[i].name,js[mc[i].p].name,mc[i].ID,js[mc[i].p].jobs[mc[i].j]);
			}
		fprintf(op,"\n");
	}
	fclose(op);
	extra_plan();
fclose(fp);
}
int done()
{
int i;
		for(i = 0;i < tot_part;i++)
			if(js[i].IN[js[i].j_cnt - 1] != pr[i].count)
				 return 1;
		for(i = 0;i< tot_mc;i++)
			if(mc[i].status > 1)
				   return 1;
		return 0;
}



void Job_pri_calculator()
{
int i,j,k,tmp_T;
Qi = 0;
for(i = 0; i < tot_part; i++)
{
	tmp_T = 0;
	for(j = 0; j<js[i].Qlen; j++)
	{
	if(js[i].Q[j])
		{
		for(k = 0;k <=j;k++)
			tmp_T += js[i].T[k];
		js[i].P[j] = js[i].Q[j] * (js[i].T[j] + pr[i].T - tmp_T);
		Q[Qi].p = i;
		Q[Qi].j = j;
		Q[Qi].C = js[i].Q[j];
		Q[Qi].P = js[i].P[j];
		Qi++;
		}
	}
}
}

void Sort_Queue()
{
int i,j,tmp_p,tmp_j,tmp_P,tmp_C,max,change;
for(i =0;i < Qi-1;i++)
{
	tmp_P = Q[i].P;
	max = i;
	change = 0;
	for(j = i+1; j<Qi ; j++)
	{
		if(tmp_P < Q[j].P)
		{
			max =j;
			tmp_P = Q[j].P;
			change = 1;
		}
	}
	if(change)
	{
		tmp_p = Q[i].p;
		tmp_j = Q[i].j;
		tmp_P = Q[i].P;
		tmp_C = Q[i].C;
		Q[i].p = Q[max].p;
		Q[i].j = Q[max].j;
		Q[i].P = Q[max].P;
		Q[i].C = Q[max].C;
		Q[max].p = tmp_p;
		Q[max].j = tmp_j;
		Q[max].P = tmp_P;
		Q[max].C = tmp_C;
	}
}
}

void extra_plan()
{
  int time,i,j;
  char string[50];
  fp1 = fopen("output1.txt","r");
  while(!feof(f1))
  {
	fscanf(f1,"%d",&time);
	if(feof(f1)) break;
	for(i=0;i<tot_mc;i++)
	   {
		fscanf(f1,"%s",string);
		if (string[(strlen(string))-2] == '0')
		 {
		   if(!m[i].flag)
			{
				m[i].start[m[i].gap] =  time ;
				m[i].end[m[i].gap] = time;
				m[i].gap++;
				m[i].check = 1;
				m[i].flag = 1;
				m[i].prev = 1;
			}
		  }
		 else if ( m[i].prev == 1 )
			{
			m[i].end[m[i].gap-1] = time-1;
			m[i].flag = 0;
			m[i].prev = 0;
			}
	   }
  }
	 for(i=0;i<tot_mc;i++)
		  if ( m[i].prev == 1 )	m[i].end[m[i].gap-1] = time;
		 for(i=0;i<tot_part;i++)
			{
			fun();
			}
fclose(fp1);

}

void fun()
{
int i,i1,j, k,a,b,l,pend,p[50],nj,m1,max_part,max_val,b1,x,y,z,z1,z2,z3;
max = 1;
for( i=0;i<tot_part;i++)
{
	for(i1=0;i1<tot_mc;i1++)
		tm1[i][i1]=m[i1];

	 if(fin[i]) continue;
p[i]=1;

while(1)
{
  for(nj = 0,pend = -1,j=0;j<js[i].j_cnt;j++)
	 {
	 nj = 0;

		for(a=0;nj == 0 && a<tot_jobs;a++)
		{
			if(!strcmp(jid[a].name,js[i].jobs[j]))
			{
				for(b=0;nj==0 && b<jid[a].N; b++)
				{
				b1 = jid[a].mno[b];
					for(l=0;nj == 0 && l<tm1[i][b1].gap;l++)
					{
					   if(pend < tm1[i][b1].start[l] && tm1[i][b1].start[l]+js[i].T[j]-1 <=tm1[i][b1].end[l])
					   {
							f[i][b1].start[f[i][b1].gap] = tm1[i][b1].start[l];
							f[i][b1].end[f[i][b1].gap] = tm1[i][b1].start[l] + js[i].T[j];
							f[i][b1].check = 1;
							f[i][b1].job[f[i][b1].gap] = j;
							f[i][b1].part[f[i][b1].gap] = i;
							f[i][b1].ID[f[i][b1].gap] = 1000 + pr[i].count + p[i];
							f[i][b1].gap ++;
							
							if( tm1[i][b1].end[l]-tm1[i][b1].start[l] + 1 == js[i].T[j])
							 {
								for(x=l;x<tm1[i][b1].gap;x++)
								  {
								  tm1[i][b1].start[x] = tm1[i][b1].start[x+1];
								  tm1[i][b1].end[x] = tm1[i][b1].end[x+1];
								  }
								tm1[i][b1].gap--;
							 }
							 if ( tm1[i][b1].end[l] > tm1[i][b1].start[l]+js[i].T[j] -1)
							 {
								tm1[i][b1].start[l] = tm1[i][b1].start[l]+js[i].T[j];
							 }
							pend = f[i][b1].end[f[i][b1].gap-1] - 1;
							nj = 1;
							if(j == js[i].j_cnt-1)
								p[i]++;

							break;
						}
						else if(pend < tm1[i][b1].end[l] && pend + js[i].T[j] <= tm1[i][b1].end[l] && pend!=-1)
						{
							f[i][b1].start[f[i][b1].gap] = pend+1;
							f[i][b1].end[f[i][b1].gap] = pend + js[i].T[j];
							f[i][b1].check = 1;
							f[i][b1].job[f[i][b1].gap] = j;
							f[i][b1].part[f[i][b1].gap] = i;
							f[i][b1].ID[f[i][b1].gap] = 1000 + pr[i].count + p[i];
							f[i][b1].gap ++;
							if( pend + 1+ js[i].T[j] == tm1[i][b1].end[l] )
							{
							 tm1[i][b1].end[l] = tm1[i][b1].end[l] - js[i].T[j]-1;
							}
							else if ( pend + js[i].T[j] < tm1[i][b1].end[l])
							 {
								tm1[i][b1].gap++;
								for(x=tm1[i][b1].gap-2;x>=l;x--)
								{
								 tm1[i][b1].start[x+1] = tm1[i][b1].start[x] ;
								 tm1[i][b1].end[x+1] = tm1[i][b1].end[x];
								}
							   tm1[i][b1].start[l+1] = pend + js[i].T[j] + 1;
							   tm1[i][b1].end[l+1] = tm1[i][b1].end[l];
							   tm1[i][b1].end[l] = pend;
							 }
							pend = f[i][b1].end[f[i][b1].gap-1] - 1;
							nj = 1;
							if(j == js[i].j_cnt-1)
								p[i]++;
							break;
						   }
					}
				} 
			}
		}
		if(nj == 0)
			 break;
	 }
	 if(nj == 0)
		break;
}
}

 max_val = 0;max_part = -1;
 for( i=0;i<tot_part;i++)
 {
 if(fin[i])
	continue;
 if((p[i]-1)*pr[i].T > max_val)
	max_part = i;
 }

 if(max_part!=-1)
 {
 for(x=0;x<tot_mc;x++)
  {
   for(y=0;y<f[max_part][x].gap;y++)
   {
	if (f[max_part][x].ID[y] >= pr[max_part].count + p[max_part] + 1000 )
	  {
		f[max_part][x].ID[y] = 0;
		f[max_part][x].job[y] = 0;
		f[max_part][x].part[y] = 0;
		z1=1;
		for(z=0;z<tm1[max_part][x].gap;z++)
		  {
		  if(f[max_part][x].start[y] == tm1[max_part][x].end[z] && f[max_part][x].end[y] == tm1[max_part][x].start[z+1])
		   {
			tm1[max_part][x].end[z] = tm1[max_part][x].end[z+1];
			for(z1=z+1;z1<tm1[max_part][x].gap;z1++)
			  {
			   tm1[max_part][x].start[z1] = tm1[max_part][x].start[z1+1];
			   tm1[max_part][x].end[z1] = tm1[max_part][x].end[z1+1];
			  }
			  tm1[max_part][x].gap--;
			z1=0;
		   }
		   else if(f[max_part][x].end[y] == tm1[max_part][x].start[z])
			{
			 tm1[max_part][x].start[z] = f[max_part][x].start[y];
			 z1=0;
			}
		   else if (f[max_part][x].start[y] == tm1[max_part][x].end[z])
		   {
			 tm1[max_part][x].end[z] = f[max_part][x].end[z];
			 z1=0;
		   }
		  }
		 if(z1 == 1)
		  {
			if ( tm1[max_part][x].start[0] > f[max_part][x].start[y] )  z=-1;
			else if ( tm1[max_part][x].end[tm1[max_part][x].gap-1] < f[max_part][x].start[y] ) z = tm1[max_part][x].gap;
			for(z=0;z<tm1[max_part][x].gap;z++)
			 {
			   if(tm1[max_part][x].end[z] < f[max_part][x].start[y] && f[max_part][x].end[y] > tm1[max_part][x].start[z+1])
				{
				  break;
				}
			 }
			  for(z2=tm1[max_part][x].gap;z2>z;z2--)
			  {
				   tm1[max_part][x].start[z2+1] = tm1[max_part][x].start[z2];
				   tm1[max_part][x].end[z2+1] = tm1[max_part][x].end[z2];
			   }
			   tm1[max_part][x].start[z+1] = f[max_part][x].start[y];
			   tm1[max_part][x].end[z+1] = f[max_part][x].end[y]-1;
			   tm1[max_part][x].gap++;
		  }

			for(z3=y;z3<f[max_part][x].gap;z3++)
			  {
				f[max_part][x].start[z3] = f[max_part][x].start[z3+1];
				f[max_part][x].end[z3] = f[max_part][x].end[z3+1];
			  }
			 f[max_part][x].gap--;
		}
   }
  }
  for(i=0;i<tot_mc;i++)
   m[i]=tm1[max_part][i];
  for(i=0;i<tot_mc;i++)
    {	for(j=0;j<f[max_part][i].gap;j++)
	  {
             for(k=f[max_part][i].start[j];k<f[max_part][i].end[j];k++)
		{
                 fprintf(fp,"\n %d (%s,%s:%d,%s) ",k,mc[i].name,pr[f[max_part][i].part[j]].name,f[max_part][i].ID[j],jid[f[max_part][i].job[j]].name);
                }
           }
      }
   for(i=0;i<tot_part;i++)
         printf("%s %d \n",pr[i].name,p[i]-1);
    fin[max_part]=1;
 }
}

