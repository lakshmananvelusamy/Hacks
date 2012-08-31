#include<stdio.h>
#include<math.h>
int total_laps,Nc=0,P,check,Nt=0,actual_pitstop;
int min_pitstop,pitstop_time=0,pitstop_count;
int lap_pitstop,excess_lap,lap_per_pitstop,tyre_ch,flag,tflag;
int Tf_valid(float);
float tf_1,tf_2,tf_3,Tf,Ts,Wo,To,Ws,Ws1,Ws2;
float total_fuel,Ws_min,race_time,Ws_init,Tf1,Tf2,Tf3;
int T_Choose();
int Ws_Chooselt134();
int Ws_Choosegt134();
int Tf_Choose();
int compare(float,float);
FILE *fp;
int main(void)
{
   fp = fopen("output.txt","w"); /* Store the output in the file */
   /* Loop will continue to get the total_laps until its valid */
   while(1)
   {
	   fflush(stdin);
	   printf("\nEnter the Number of laps ( >= 60 ) : " );
	   scanf("%d",&total_laps);
	   if (total_laps < 60 )
	   {
	   printf("\nInvalid Input\n");
	   continue;
	   }
	   else break;
   }
   printf("Tyre factor should be >= 0.85 and <=0.95 & in multiples of 0.005\n");
   printf("\n Enter the tf_1 :");
   scanf("%f",&tf_1);
   /* Loop will continue to get the Tf_1 until its valid */
   while(Tf_valid(tf_1))
   {
	   printf("\nInvalid value for Tf_1\n Enter the tf_1(>= .85 & <=.95) :");
	   scanf("%f",&tf_1);
   }
   printf("\n Enter the tf_2 :");
   scanf("%f",&tf_2);
   /* Loop will continue to get the Tf_2 until its valid */
   while(Tf_valid(tf_2))
   {
	   printf("\nInvalid value for Tf_2\n Enter the tf_2(>= .85 & <=.95) :");
	   scanf("%f",&tf_2);
   }
   printf("\n Enter the tf_3 :");   scanf("%f",&tf_3);
   /* Loop will continue to get the Tf_3 until its valid */
   while(Tf_valid(tf_3))
   {
	   printf("\nInvalid value for Tf_3\n Enter the tf_3(>= .85 & <=.95) :");
	   scanf("%f",&tf_3);
   }
   min_pitstop = ((total_laps/15)+1);
   printf("\nEnter the maximum number of pitstops allowed:");
   scanf("%d",&P);
   /* Loop will continue to get the P until its valid */
   while (P < min_pitstop)
   {
	   printf("\nInvalid input( P >= (total_laps/15)+1 ) \nEnter the maximum number of pitstops allowed:");
	   scanf("%d",&P);
   }
   printf("\nEnter the minimum time for completion of a lap(To) : ");
   scanf("%f",&To);
   /* Loop will continue to get the To until its valid */
   while (compare(To,30.00) < 0 )
   {
	   printf("\nInvalid Input (The Minimum permissible value for To is 30)\nEnter the minimum time for completion of a lap(To) : ");
	   scanf("%f",&To);
   }
   printf("\nEnter the weight of the fuel (Wo) : ");
   scanf("%f",&Wo);
   /* Loop will continue to get the Wo until its valid */
   while(compare(Wo,120.00) > 0|| compare(Wo,0.0) <=0 )
   {
	   printf("\nInvalid Input (The maximum permissible value for Wo is 120)\nEnter the weight of the fuel (Wo) : ");
	   scanf("%f",&Wo);
   }
   T_Choose(); /* All the calculations are done in T_choose */
   printf("\n RACE SUMMARY :");
   printf("\nTotal time taken to complete the race = %.5f \nTotal pitstops = %d " ,race_time,pitstop_count);
   printf("\nTotal pitstop Time = %d \nTotal race time = %.5f ",pitstop_time,race_time-pitstop_time);
   printf("\nTotal Fuel consumed = %.5f \nTotal Tyre Changes = %d",total_fuel-Ws,tyre_ch);
   fclose(fp);
   return;
}

/* Whenever a pitstop is taken, tyres are changed and the fuel      *
 * is filled.So the pitstop time is always 6 seconds. The fuel is   *
 * filled within 6 seconds.Within 6 seconds 72 units of fuel can be *
 * filled. But actually, in our logic, we will never fill 72 units  *
 * of fuel for any no. of laps. So we do not need to check the fuel *
 * filling time. */

pitstop()
{
   int cur_pitstop_t=6,t1;
   Nt=0;
   tyre_ch++;
   pitstop_count++;
   race_time+=cur_pitstop_t;
   pitstop_time+=cur_pitstop_t;
   if(pitstop_count == actual_pitstop && flag)
   {
	Ws_init=Wo;
	for(t1=0,Nt=0;t1 < total_laps-Nc; t1++,Nt++)
	{
	   Ws_init = Ws_init - 2.5 - (0.1* (Nt+1)* (Nt+1) * (1-Tf));
	}
	Ws_init=Wo-Ws_init+0.00001;
	Nt=0;
   }

   printf("\nPitstop_count = %d Fuel added = %.5f Tf = %.5f pitstop_time = %d Nc = %d",pitstop_count,Ws_init-Ws,Tf,cur_pitstop_t,Nc);
   Ws=Ws_init;
   return;
}

/* Tf_Choose selects the minimum value of tyre factor out of  *
 * the three given values.We use the minimum tf value through *
 * out the race.                                              */
Tf_Choose()
{
   Tf1 = ( tf_1 < tf_2 ? tf_1 < tf_3 ? tf_1 : tf_3 : tf_2 < tf_3 ? tf_2 : tf_3 );
   Tf3 = ( tf_1 > tf_2 ? tf_1 > tf_3 ? tf_1 : tf_3 : tf_2 > tf_3 ? tf_2 : tf_3 );
   Tf2 = ( tf_1 > tf_2 ? (tf_1 > tf_3 ? (tf_3 > tf_2 ? tf_3 : tf_2) : tf_1):tf_2>tf_3? tf_3 : tf_2);
   return;
}

T_Choose()
{
   float T;
   int i,t1,temp;
   Tf_Choose();
   Tf = Tf1;
   flag=0;tflag=0;
   if ( total_laps < 134 )
   {
	  Ws_Chooselt134();
	  if( tflag )
	  {
		tflag=0;
		Tf = Tf2;
		Ws_Chooselt134();
		if( tflag )
		{
		   tflag=0;
		   Tf = Tf3;
		   Ws_Chooselt134();
		   if( tflag )
		   {
			printf("\nInsufficint amount of fuel and pitstops.");
			exit(1);
		   }
		}
   }
   }
   else
   {
	  Ws_Choosegt134();
	  if( tflag )
	  {
		tflag=0;
		Tf = Tf2;
		Ws_Choosegt134();
		if( tflag )
		{
		   tflag=0;
		   Tf = Tf3;
		   Ws_Choosegt134();
		   if( tflag )
		   {
			printf("\nInsufficint amount of fuel and pitstops.");
			exit(1);
		   }
		}
   }
   }
   Ws_init=Ws;
   /* In our logic, we split the Ws calculation into two parts. *
	* If total_laps < 134 and another is total_laps >=134       */
   while (Nc < total_laps )
   {
	  Nt=0;
	  if (pitstop_count == (actual_pitstop + 1)  - excess_lap && !flag)
	  {
	  lap_per_pitstop--;
	  Ws = Ws_init = Ws1;
	  }
	  total_fuel += Ws;
	  for(i=0;i<lap_per_pitstop;i++)
	  {
	  Nc++;
	  T =  To + ( 0.015 * Ws ) - (9 * (1-Tf)) + (0.35*(1-Tf)*Nt*(1-Tf)*Nt);
	  Ws = Ws - 2.5 - (0.1* (Nt+1)* (Nt+1) * (1-Tf));
	  Ts = (1-Tf) * Nt ;
	  /* If Ts exceeds 2.85 then a pitstop is taken */
	  if ( compare(Ts,2.85) >= 0) pitstop();
	  Nt++;
	  race_time+=T;
	  printf("\nNc = %d  Ws = %.5f  Ts = %.5f  Nt = %d Tf = %.5f  T = %.5f",Nc,Ws,Ts,Nt,Tf,T);
	  fprintf(fp,"\n%d:%.5f:%.5f:%d:%.5f:%.5f",Nc,Ws,Ts,Nt,Tf,T);
	  if(Nc == total_laps) return;
	  }
	  if(pitstop_count < actual_pitstop)
	  {
		 pitstop();
	  }
  }
  return;
}
/* Ws_choose selects the value of Ws with respect to the no. *
 * of laps. In our logic, the Ws value is calculated only    *
 * once. Because, fuel taken at each pitstop is same.At each *
 * pitstop, the tyres are changed and fuel is filled.        */

Ws_Choosegt134()
{
   int i,j;
   double temp1;
   Ws=Wo;
   actual_pitstop=min_pitstop;
   lap_per_pitstop = total_laps/(actual_pitstop+1);

   if ( lap_per_pitstop * (actual_pitstop+1) < total_laps )
   {
	 temp1=(total_laps/lap_per_pitstop);
	 actual_pitstop = (temp1);
	 flag=1;
	 if ( actual_pitstop >  P )
	  {
		actual_pitstop = P;
		lap_per_pitstop++;
		excess_lap = lap_per_pitstop * (actual_pitstop+1) - total_laps;
	  }
   }
   Nt=0;
   for(i=0;i<lap_per_pitstop;i++,Nt++)
   {
	   Ws_min = Ws;
	   Ws=Ws-2.5-(0.1*(Nt+1)*(Nt+1)*(1-Tf));
	   if ( i == lap_per_pitstop-2) Ws1=Ws;
	   if (compare(Ws,0.0)==-1)
	   {
		if (i == 0)
		 {
		  printf("\nThe fuel given is insufficient to complete even a single lap\n");
		  exit(1);
		 }
		 Ws=Wo-Ws_min+0.00001;
		 Ws1=Wo;
		 lap_per_pitstop=i;
		 actual_pitstop=(total_laps/lap_per_pitstop);
		 flag=1;
		 if (actual_pitstop > P)
		  {
		  tflag=1;
		  return;
		  }
		 return;
		}
		}
	   Ws = Wo-Ws+0.00001;
	   Ws1 = Wo-Ws1+0.00001;
	   return;
}
Ws_Chooselt134()
{
   int i,j;
   double temp1;
   Ws=Wo;
   actual_pitstop=min_pitstop;
   lap_per_pitstop = (int) ceil ( (double) total_laps/ (double) (actual_pitstop+1) ) ;
   excess_lap = lap_per_pitstop * (actual_pitstop+1) - total_laps;
   flag=1;
   Nt=0;
   for(i=0;i<lap_per_pitstop;i++,Nt++)
   {
	   Ws_min = Ws;
	   Ws=Ws-2.5-(0.1*(Nt+1)*(Nt+1)*(1-Tf));
	   if ( i == lap_per_pitstop-2) Ws1=Ws;
	   if (compare(Ws,0.0)==-1)
	   {
		if (i == 0)
		 {
		  printf("\nThe fuel given is insufficient to complete even a single lap\n");
		  exit(1);
		 }
		 Ws=Wo-Ws_min+0.00001;
		 Ws1=Wo;
		 lap_per_pitstop=i;
		 temp1=(total_laps/(double)lap_per_pitstop)-1.00;
		 actual_pitstop=ceil(temp1);
		 flag=1;
		 if (actual_pitstop > P)
		  {
			tflag=1;
			return;
		  }
		 return;
		}
		}
	   Ws = Wo-Ws+0.00001;
	   Ws1 = Wo-Ws1+0.00001;
	   return;
}

/* compare function compares the two float values  *
 * returns 0 if both are valid                     *
 * returns -1 if f1 > f2                           *
 * returns  1 if f2 > f1                           */

int compare(float f1,float f2)
{
	long temp_1,temp_2;
	f1*=100000;
	f2*=100000;
	temp_1 = (long ) f1;
	temp_2 = (long ) f2;
	return(temp_1 == temp_2 ? 0 : temp_1 < temp_2 ? -1 : 1 );
}

/* Tf_valid is to validate the Tf values   *
 * returns 1 if Tf is valid                *
 * returns 0 otherwise                     */

int Tf_valid(float tf)
{
	float al = tf*1000;
	int temp=50;
	temp = (int)al;
	if (temp >=850 && temp <= 950 && temp%5==0)
	return 0;
	else return 1;
}

