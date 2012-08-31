/*#include "StackGameClientCommunicator.h" */
#include "StackGameClientCommunicator.c"

int board[8][8][10];
int priority[160];
int count;
int t_count;
int all_moves[160][6];
int t_all_moves[160][6];
int top[8][8];
int row;
int col;
int sm_cnt;
int res=0;
int cap=0;
int opp_res=0;
int opp_cap=0;
int play=1;
int t_board[8][8][10];
int t_top[8][8];
int t_res;
int t_cap;
int t_opp_res;
int t_opp_cap,Iam,Op;
int sm_moves[20][6];

int sampleMethod();
int init_board();
int update_board(int [6], int);
int intelligence();
int all_possible_moves(int [8][8][10],int);
int prior(int [6]);
int single_move(int ,int ,int);
int check(int,int);

int main() {

   init_board(); /* Initialise the internal board */

   sampleMethod(); /* Function where game is played */
 
   return 0;
}

/* Function where game is played */

int sampleMethod() {

	int val,i,j,k;
	int * retVal;
	setPropertyFileName(".\\stackgameclient.properties");
 	printf("Waiting for handshake ... ...\n");
  	retVal = handShake();
  	printf("The response is: %d %d %d\n",retVal[0],retVal[1],retVal[2]);
	if (retVal[1] == RED_COINS) {
		Op = 4;
		Iam = 3;
	}
	else 
	{
		Iam = 4;
		Op = 3;
	}
  	if(retVal[2] == PLAY_SECOND)
	 {
		printf("I am playing Second\n");
		play =2;
	  	retVal = readMoves();
	 	printf("The response is: %d %d %d %d %d %d\n",retVal[0],retVal[1],retVal[2],retVal[3],retVal[4],retVal[5]);
                retVal[1]-=1;
                retVal[2]-=1;
                retVal[4]-=1;
                retVal[5]-=1;
	 	update_board(retVal,1);
		for(i=0;i<8;i++)
		   for(j=0;j<8;j++)
         		for(k=0;k<5;k++)
           			board[i][j][k] = t_board[i][j][k];
        	for(i=0;i<8;i++)
         		for(j=0;j<8;j++)
           			top[i][j] = t_top[i][j];

         	res = t_res;
         	cap = t_cap;
         	opp_res = t_opp_res;
         	opp_cap = t_opp_cap;
 
           } 
	   else
	    {
		play=1;
		printf("I am playing First\n");
  	    }
	while(1)
	 {
  	   val =  intelligence();
             for(i=0;i<8;i++)
               for(j=0;j<8;j++)
                 for(k=0;k<5;k++)
                  t_board[i][j][k] = board[i][j][k];
     	       for(i=0;i<8;i++)
                   for(j=0;j<8;j++)
                      t_top[i][j] = top[i][j];

         t_res = res;
         t_cap = cap;
         t_opp_res = opp_res;
         t_opp_cap = opp_cap;

 	update_board(all_moves[val],0);

  	for(i=0;i<8;i++)
   		for(j=0;j<8;j++)
         		for(k=0;k<5;k++)
           			board[i][j][k] = t_board[i][j][k];
        for(i=0;i<8;i++)
         	for(j=0;j<8;j++)
           		top[i][j] = t_top[i][j];

         res = t_res;
         cap = t_cap;
         opp_res = t_opp_res;
         opp_cap = t_opp_cap;

         writeMoves(all_moves[val][0],all_moves[val][1]+1,all_moves[val][2]+1,all_moves[val][3],all_moves[val][4]+1,all_moves[val][5]+1);



	   	printf("Waiting for response ... ...\n");
	  	retVal = readMoves();

	  	printf("The response is: %d %d %d %d %d %d\n",retVal[0],retVal[1],retVal[2],retVal[3],retVal[4],retVal[5]);
		retVal[1]-=1;
                retVal[2]-=1;
                retVal[4]-=1;
                retVal[5]-=1;
	  	 for(i=0;i<8;i++)
              		  for(j=0;j<8;j++)
                        	for(k=0;k<5;k++)
                                	t_board[i][j][k] = board[i][j][k];
        	for(i=0;i<8;i++)
                	for(j=0;j<8;j++)
                        	t_top[i][j] = top[i][j];

        	 t_res = res;
      		 t_cap = cap;
         	 t_opp_res = opp_res;
         	 t_opp_cap = opp_cap;
	
 		 update_board(retVal,1);

                for(i=0;i<8;i++)
                   for(j=0;j<8;j++)
                        for(k=0;k<5;k++)
                                board[i][j][k] = t_board[i][j][k];
                for(i=0;i<8;i++)
                        for(j=0;j<8;j++)
                                top[i][j] = t_top[i][j];

                res = t_res;
                cap = t_cap;
                opp_res = t_opp_res;
                opp_cap = t_opp_cap;

	  	if(retVal[0] !=0 && retVal[0] != 1) {

			if(retVal[0] == GAME_ENDED) {

				break;
			}
			else if(retVal[0] == GAME_RESET) {

				break;
			}
			else if(retVal[0] == INVALID_MOVE) {

				break;
			} 
		}

	}
}

/* FUNCTION : init_board() initialises the board to startin position 
 */            
int init_board()
{
int i,j;
for(i=0;i<8;i++)
for (j=0;j<8;j++)
 { board[i][j][0]=2; t_board[i][j][0]=2; }
	for(row=1;row<7;row++)
		for(col=1;col<7;col++)
		{
			if(row%2 == 0)
				{ board[row][col][top[row][col]] = 4; t_board[row][col][t_top[row][col]] = 4; }
			else
				{ board[row][col][top[row][col]] = 3; t_board[row][col][t_top[row][col]] = 3; }
			if(col==2) col=4;
		}
	for(row=1;row<7;row++)
		if(row%2 == 0 )
		{
			board[row][3][top[row][col]] = 3;
			board[row][4][top[row][col]] = 3;
			t_board[row][3][t_top[row][col]] = 3;
			t_board[row][4][t_top[row][col]] = 3;
		}
		else
		{
			board[row][3][top[row][col]] = 4;
			board[row][4][top[row][col]] = 4;
			t_board[row][3][t_top[row][col]] = 4;
			t_board[row][4][t_top[row][col]] = 4;
		}

	for(col=2;col<6;col++)
	{
		board[0][col][top[0][col]]=0;
		board[7][col][top[7][col]]=0;
		board[col][0][top[col][0]]=0;
		board[col][7][top[col][7]]=0;

		t_board[0][col][t_top[0][col]]=0;
		t_board[7][col][t_top[7][col]]=0;
		t_board[col][0][t_top[col][0]]=0;
		t_board[col][7][t_top[col][7]]=0;
	}
	 for (i=1;i<7;i++)
	 for (j=1;j<7;j++) {top[i][j]=1; t_top[i][j]=1; }
return 0;
}

/* FUNCTION : update board() updates the board with the given move.
 * ARGS     : 1. move vector 
 *          : 2. Player
 */
int update_board(int v[],int flag)
{
int i,tot;
/*flag =0 -- our move - write;
	1, opp.'s move - read;*/
if (flag == 0)
{
	if (play==1)
	{
	   if (v[0] ==0 )
	   {
	   t_res--;
	   tot=t_top[v[4]][v[5]]+1;
	  if (tot>5)
	  {
	  if (t_board[v[4]][v[5]][0]==Iam) t_res++;
	  else t_cap++;
	  t_board[v[4]][v[5]][tot-1]=Iam;
	  for (i=0;i<5;i++)
	  t_board[v[4]][v[5]][i]=t_board[v[4]][v[5]][i+tot-5];

	  t_top[v[4]][v[5]]=5;
	  }
	  else /* tot <5 */
	  {
	  t_board[v[4]][v[5]][t_top[v[4]][v[5]]]=Iam;
	  t_top[v[4]][v[5]]++;
	  }
	   }
	   else /* v[0]=1 */
	   {
	  tot=t_top[v[1]][v[2]]-v[3]+t_top[v[4]][v[5]]+1;
	  if (tot>5)
	  {
		 for (i=0;i<tot-5;i++)
		 {
		   if (t_board[v[4]][v[5]][i]==Iam )   t_res++;
		   else t_cap++;
		 }

		 for(i=v[3];i<=t_top[v[1]][v[2]];i++)
		 t_board[v[4]][v[5]][t_top[v[4]][v[5]]+i-v[3]]=t_board[v[1]][v[2]][i-1];
		 for (i=0;i<5;i++)
		 t_board[v[4]][v[5]][i] =t_board[v[4]][v[5]][tot-5+i];

		 t_top[v[4]][v[5]]=5;
		 t_top[v[1]][v[2]]=v[3]-1;
	  }
	  else /* tot<5 */
	  {
	  for(i=v[3];i<=t_top[v[1]][v[2]];i++)
	  t_board[v[4]][v[5]][t_top[v[4]][v[5]]+i-v[3]]=t_board[v[1]][v[2]][i-1];
	  t_top[v[4]][v[5]]=tot;
	  t_top[v[1]][v[2]]=v[3]-1;
	  }
	   }
	}
	else  /* play =2 */
	{
	   if (v[0] ==0 )
	   {
	   t_res--;
	   tot=t_top[v[4]][v[5]]+1;
		  if (tot>5)
		  {
			  if (t_board[v[4]][v[5]][0]==Iam) t_res++;
			  else t_cap++;
			  t_board[v[4]][v[5]][tot-1]=Iam;
			  for (i=0;i<5;i++)
			  t_board[v[4]][v[5]][i]=t_board[v[4]][v[5]][i+tot-5];
			  t_top[v[4]][v[5]] = 5;
		  }
		  else /* tot <5 */
		  {
		  t_board[v[4]][v[5]][t_top[v[4]][v[5]]]=Iam;
		  t_top[v[4]][v[5]]++;
		  }
		}
		else /* v[0]=1 */
		   {
		  tot=t_top[v[1]][v[2]]-v[3]+t_top[v[4]][v[5]]+1;
		  if (tot>5)
		  {
			 for (i=0;i<tot-5;i++)
			 {
			   if (t_board[v[4]][v[5]][i]==Iam )   t_res++;
			   else t_cap++;
			 }

			 for(i=v[3];i<=t_top[v[1]][v[2]];i++)
			 t_board[v[4]][v[5]][t_top[v[4]][v[5]]+i-v[3]]=t_board[v[1]][v[2]][i-1];

			 for (i=0;i<5;i++)
			 t_board[v[4]][v[5]][i] =t_board[v[4]][v[5]][tot-5+i];

			 t_top[v[4]][v[5]]=5;
			 t_top[v[1]][v[2]]=v[3]-1;
		  }
		   else /* tot<5 */
		  {
		  for(i=v[3];i<=t_top[v[1]][v[2]];i++)
		  t_board[v[4]][v[5]][t_top[v[4]][v[5]]+i-v[3]]=t_board[v[1]][v[2]][i-1];
		  t_top[v[4]][v[5]]=tot;
		  t_top[v[1]][v[2]]=v[3]-1;
		  }
	   }
	}
}

/* opp's move */
else
{
	if (play==1)
	{
	   if (v[0] ==0 )
	   {
	   t_opp_res--;
	   tot=t_top[v[4]][v[5]]+1;
	  if (tot>5)
	  {
	  if (t_board[v[4]][v[5]][0]==Op) t_opp_res++;
	  else t_opp_cap++;
	  t_board[v[4]][v[5]][tot-1]=Op;
	  for (i=0;i<5;i++)
	  t_board[v[4]][v[5]][i]=t_board[v[4]][v[5]][i+tot-5];
	  t_top[v[4]][v[5]] = 5;
	  }
	  else /* tot <5 */
	  {
	  t_board[v[4]][v[5]][t_top[v[4]][v[5]]]=Op;
	  t_top[v[4]][v[5]]++;
	  }
	   }
	   else /* v[0]=1 */
	   {
	  tot=t_top[v[1]][v[2]]-v[3]+t_top[v[4]][v[5]]+1;
	  if (tot>5)
	  {
		 for (i=0;i<tot-5;i++)
		 {
		   if (t_board[v[4]][v[5]][i]==Op )   t_opp_res++;
		   else t_opp_cap++;
		 }

			 for(i=v[3];i<=t_top[v[1]][v[2]];i++)
			 t_board[v[4]][v[5]][t_top[v[4]][v[5]]+i-v[3]]=t_board[v[1]][v[2]][i-1];

			 for (i=0;i<5;i++)
			 t_board[v[4]][v[5]][i] =t_board[v[4]][v[5]][tot-5+i];

		 t_top[v[4]][v[5]]=5;
		 t_top[v[1]][v[2]]=v[3]-1;
	  }
	  else /* tot<5 */
	  {
	  for(i=v[3];i<=t_top[v[1]][v[2]];i++)
	  t_board[v[4]][v[5]][t_top[v[4]][v[5]]+i-v[3]]=t_board[v[1]][v[2]][i-1];
	  t_top[v[4]][v[5]]=tot;
	  t_top[v[1]][v[2]]=v[3]-1;
	  }
	   }
	}
	else  /* play =2 */
	{
	   if (v[0] ==0 )
	   {
	   t_opp_res--;
	   tot=t_top[v[4]][v[5]]+1;
		  if (tot>5)
		  {
			  if (t_board[v[4]][v[5]][0]==Op) t_opp_res++;
			  else t_opp_cap++;
			  t_board[v[4]][v[5]][tot-1]=Op;
			  for (i=0;i<5;i++)
			  t_board[v[4]][v[5]][i]=t_board[v[4]][v[5]][i+tot-5];
			  t_top[v[4]][v[5]]= 5;
		  }
		  else /* tot <5 */
		  {
		  t_board[v[4]][v[5]][t_top[v[4]][v[5]]]=Op;
		  t_top[v[4]][v[5]]++;
		  }
		}
		else /* v[0]=1 */
		   {
		  tot=t_top[v[1]][v[2]]-v[3]+t_top[v[4]][v[5]]+1;
		  if (tot>5)
		  {
			 for (i=0;i<tot-5;i++)
			 {
			   if (t_board[v[4]][v[5]][i]==Op )   t_opp_res++;
			   else t_opp_cap++;
			 }

			 for(i=v[3];i<=t_top[v[1]][v[2]];i++)
			 t_board[v[4]][v[5]][t_top[v[4]][v[5]]+i-v[3]]=t_board[v[1]][v[2]][i-1];

			 for (i=0;i<5;i++)
			 t_board[v[4]][v[5]][i] =t_board[v[4]][v[5]][tot-5+i];

			 t_top[v[4]][v[5]]=5;
			 t_top[v[1]][v[2]]=v[3]-1;
		 }
		 else /* tot<5 */
		  {
		  for(i=v[3];i<=t_top[v[1]][v[2]];i++)
		  t_board[v[4]][v[5]][t_top[v[4]][v[5]]+i-v[3]]=t_board[v[1]][v[2]][i-1];
		  t_top[v[4]][v[5]]=tot;
		  t_top[v[1]][v[2]]=v[3]-1;
		  }
	   }
	}
}

return 0;
}

/* FUNCTION : intelligence() is used to ad inteligence to the system.
 *            It checks best move among all possible move.
 */ 
 
int intelligence()
{
int high=0,i,j;
all_possible_moves(board,Iam);
 for(i=0;i<t_count;i++)
   for(j=0;j<6;j++)
	 all_moves[i][j] = t_all_moves[i][j];
  count = t_count;
  for(i=0;i<count;i++)
	{
	priority[i] = prior(all_moves[i]);
	if ( i==0 ) high =0;
	else if (priority[i] > priority[high] ) high = i;
	}
	return high;
}

/* FUNCTION : It finds out all possible moves for given board position and for a given player.
 */
int all_possible_moves(int b[8][8][10],int fl)
{
int i,j,x,z;
t_count = 0 ;
for (i=0;i<8;i++)
{
   for (j=0;j<8;j++)
	{
	   if( b[i][j][top[i][j]-1]==fl && top[i][j]!=0)
	   {
		   for(x=1;x<=top[i][j];x++)
		   {
		   if(check(i-x,j))
		if((i-x)>=0 && b[i-x][j][top[i-x][j]-1]!=2)
		{
			 t_all_moves[t_count][0]=1;
			 t_all_moves[t_count][1]=i;
			 t_all_moves[t_count][2]=j;
			 t_all_moves[t_count][3]=top[i][j]-x+1;
			 t_all_moves[t_count][4]=i-x;
			 t_all_moves[t_count][5]=j;
			 t_count++;
		}
		if(check(i+x,j))
		if((i+x)<8 && b[i+x][j][top[i+x][j]-1]!=2)
		{
			 t_all_moves[t_count][0]=1;
			 t_all_moves[t_count][1]=i;
			 t_all_moves[t_count][2]=j;
			 t_all_moves[t_count][3]=top[i][j]-x+1;
			 t_all_moves[t_count][4]=i+x;
			 t_all_moves[t_count][5]=j;
			 t_count++;
		}
		if(check(i,j-x))
		if((j-x)>=0 && b[i][j-x][top[i][j-x]-1]!=2)
		{
			 t_all_moves[t_count][0]=1;
			 t_all_moves[t_count][1]=i;
			 t_all_moves[t_count][2]=j;
			 t_all_moves[t_count][3]=top[i][j]-x+1;
			 t_all_moves[t_count][4]=i;
			 t_all_moves[t_count][5]=j-x;
			 t_count++;
		}
		if(check(i,j+x))
		if((j+x)<8 && b[i][j+x][top[i][j+x]-1]!=2)
		{
			 t_all_moves[t_count][0]=1;
			 t_all_moves[t_count][1]=i;
			 t_all_moves[t_count][2]=j;
			 t_all_moves[t_count][3]=top[i][j]-x+1;
			 t_all_moves[t_count][4]=i;
			 t_all_moves[t_count][5]=j+x;
			 t_count++;
		}
	   }
	   }
   }
}

if((play==1 && fl == Iam) || (play ==2  && fl ==Iam))
{
if(res>0)
for(i=0;i<8;i++)
for(j=0;j<8;j++)
if(b[i][j][top[i][j]]!=2)
{
t_all_moves[t_count][0]=0;
t_all_moves[t_count][1]=0;
t_all_moves[t_count][2]=0;
t_all_moves[t_count][3]=0;
t_all_moves[t_count][4]=i;
t_all_moves[t_count][5]=j;
t_count++;
}
}
if((play == 1 && fl == Op) || (play == 2 && fl ==Op))
{
if(opp_res>0)
for(i=0;i<8;i++)
for(j=0;j<8;j++)
if(b[i][j][top[i][j]]!=2)
{
t_all_moves[t_count][0]=0;
t_all_moves[t_count][1]=0;
t_all_moves[t_count][2]=0;
t_all_moves[t_count][3]=0;
t_all_moves[t_count][4]=i;
t_all_moves[t_count][5]=j;
t_count++;
}
}

return 0;
}

/* FUNCTION : It checks for the  board boundary.
 */
int check(int i,int j)
{
if((i==0 && j==1) ||(i==0 && j==6) || (i==1 && j==0) || (i==1 && j==7)||(i==6 && j==0) || (i==6 && j==7) || (i==7 && j==1) || (i==7 && j==6))
return 0;
return 1;
}

/* FUNCTION : Calculates priority for a given moves.
 */
int prior(int move[6])
{
  int p=0,i,j,k,x,y,z;
  int t_all_moves_dest[160][6];
  int t_board_dest[8][8][10];
  int t_top_dest[8][8];
  int t_count_dest=0;
  int old_res=0, old_cap = 0;
  int old_t_opp_res;
  int old_t_opp_cap;
  for(i=0;i<8;i++)
   for(j=0;j<8;j++)
	 for(k=0;k<5;k++)
	   t_board[i][j][k] = board[i][j][k];
	for(i=0;i<8;i++)
	 for(j=0;j<8;j++)
	   t_top[i][j] = top[i][j];

	 t_res = res;
	 t_cap = cap;
	 t_opp_res = opp_res;
	 t_opp_cap = opp_cap;

	 if(play==1)
	  {
		/* own stack */
	   if(t_top[move[4]][move[5]] == 0 ) p+=6;
	   else if(t_board[move[4]][move[5]][t_top[move[4]][move[5]]-1] == Op ) p+= 8;
	   else p+=7;
	  }
	  else
	  {
		 /* own stack*/
		if(t_top[move[4]][move[5]] == 0 ) p+=6;
		else if(t_board[move[4]][move[5]][t_top[move[4]][move[5]]-1] == Op ) p+= 8;
		else p+=7;
	  }
	old_res=t_res;
	old_cap=t_cap;
	single_move(move[1],move[2],move[3]);
	for(i=0;i<sm_cnt;i++)
	  p-=t_top[sm_moves[i][4]][sm_moves[i][5]];

	update_board(move,0);

	single_move(move[4],move[5],1);
	for(i=0;i<sm_cnt;i++)
	  p+=t_top[sm_moves[i][4]][sm_moves[i][5]];

	/* anything captured*/ if(t_cap>0)  p+= (t_cap-old_cap) * 100;
	/* anything reserved*/ if(t_res>0)  p+= (t_res-old_res) * 90;

	 if(move[0]==0 && old_res == t_res ) p+=75;
	 /*proximity */

	  if (play ==1 )
	  {
	   all_possible_moves(t_board,Iam);
	   for(i=0;i<t_count;i++)
		 for(j=0;j<6;j++)
		  t_all_moves_dest[i][j] = t_all_moves[i][j];

	   t_count_dest = t_count;
		for(x=0;x<8;x++)
				  for(y=0;y<8;y++)
					 for(z=0;z<5;z++)
						 t_board_dest[x][y][z] = t_board[x][y][z];
				for(x=0;x<8;x++)
					for(y=0;y<8;y++)
					  t_top_dest[x][y] = t_top[x][y];
	   for(i=0;i<t_count_dest;i++)
		  {
		  if(t_all_moves_dest[i][4] == move[4] && t_all_moves_dest[i][5] == move[5] )
		  {
		   p+=(t_top[t_all_moves_dest[i][1]][t_all_moves_dest[i][2]]-t_all_moves_dest[i][3]+1) * 2;
		  }
		  }
		for(i=0;i<t_count_dest;i++)
		  for(j=0;j<6;j++)
		   t_all_moves[i][j] = t_all_moves_dest[i][j];

	   t_count= t_count_dest;
		for(x=0;x<8;x++)
				  for(y=0;y<8;y++)
					 for(z=0;z<5;z++)
						 t_board[x][y][z] = t_board_dest[x][y][z];
				for(x=0;x<8;x++)
					for(y=0;y<8;y++)
					  t_top[x][y] = t_top_dest[x][y];

	   all_possible_moves(t_board,Op);
		for(i=0;i<t_count;i++)
		 for(j=0;j<6;j++)
		  t_all_moves_dest[i][j] = t_all_moves[i][j];

	   t_count_dest = t_count;
		for(x=0;x<8;x++)
				  for(y=0;y<8;y++)
					 for(z=0;z<5;z++)
						 t_board_dest[x][y][z] = t_board[x][y][z];
				for(x=0;x<8;x++)
					for(y=0;y<8;y++)
					  t_top_dest[x][y] = t_top[x][y];
		for(i=0;i<t_count_dest;i++)
		  {
		  if(t_all_moves_dest[i][4] == move[4] && t_all_moves_dest[i][5] == move[5] )
			 p-=(t_top[t_all_moves_dest[i][1]][t_all_moves_dest[i][2]]-t_all_moves_dest[i][3]+1)*2;
		  }
		  for(i=0;i<t_count;i++)
			{
			 old_t_opp_res = t_opp_res;
			 old_t_opp_cap = t_opp_cap;
			 update_board(t_all_moves_dest[i],1);
			 t_opp_res = opp_res;
			 t_opp_cap = opp_cap;
			 p-= (t_opp_res-old_t_opp_res)* 90;
			 p-= (t_opp_cap-old_t_opp_cap)*100;
				for(x=0;x<8;x++)
				  for(y=0;y<8;y++)
					 for(z=0;z<5;z++)
						 t_board[x][y][z] = t_board_dest[x][y][z];
				for(x=0;x<8;x++)
					for(y=0;y<8;y++)
					  t_top[x][y] = t_top_dest[x][y];
			}
		}
	   if(play == 2 )
		{
		all_possible_moves(t_board,Iam);
		for(i=0;i<t_count;i++)
		 for(j=0;j<6;j++)
		  t_all_moves_dest[i][j] = t_all_moves[i][j];

	   t_count_dest = t_count;
		for(x=0;x<8;x++)
				  for(y=0;y<8;y++)
					 for(z=0;z<5;z++)
						 t_board_dest[x][y][z] = t_board[x][y][z];
				for(x=0;x<8;x++)
					for(y=0;y<8;y++)
					  t_top_dest[x][y] = t_top[x][y];
	   for(i=0;i<t_count_dest;i++)
		  {
		  if(t_all_moves_dest[i][4] == move[4] && t_all_moves_dest[i][5] == move[5] )
			p+=(t_top[t_all_moves_dest[i][1]][t_all_moves_dest[i][2]]-t_all_moves_dest[i][3]+1)*2;
		  }
		for(i=0;i<t_count_dest;i++)
		  for(j=0;j<6;j++)
		   t_all_moves[i][j] = t_all_moves_dest[i][j];

	   t_count= t_count_dest;
		for(x=0;x<8;x++)
				  for(y=0;y<8;y++)
					 for(z=0;z<5;z++)
						 t_board[x][y][z] = t_board_dest[x][y][z];
				for(x=0;x<8;x++)
					for(y=0;y<8;y++)
					  t_top[x][y] = t_top_dest[x][y];

	   all_possible_moves(t_board,Op);
		for(i=0;i<t_count;i++)
		 for(j=0;j<6;j++)
		  t_all_moves_dest[i][j] = t_all_moves[i][j];

	   t_count_dest = t_count;
		for(x=0;x<8;x++)
				  for(y=0;y<8;y++)
					 for(z=0;z<5;z++)
						 t_board_dest[x][y][z] = t_board[x][y][z];
				for(x=0;x<8;x++)
					for(y=0;y<8;y++)
					  t_top_dest[x][y] = t_top[x][y];
		for(i=0;i<t_count_dest;i++)
		  {
		  if(t_all_moves_dest[i][4] == move[4] && t_all_moves_dest[i][5] == move[5] )
			p-=(t_top[t_all_moves_dest[i][1]][t_all_moves_dest[i][2]]-t_all_moves_dest[i][3]+1)*2;
		  }
		   for(i=0;i<t_count;i++)
			{
			 old_t_opp_res = t_opp_res;
			 old_t_opp_cap = t_opp_cap;
			 update_board(t_all_moves_dest[i],1);
			 t_opp_res = opp_res;
			 t_opp_cap = opp_cap;
			 p-= (t_opp_res-old_t_opp_res)*90;
			 p-=(t_opp_cap- old_t_opp_cap)*100;
				for(x=0;x<8;x++)
				  for(y=0;y<8;y++)
					 for(z=0;z<5;z++)
						 t_board[x][y][z] = t_board_dest[x][y][z];
				for(x=0;x<8;x++)
					for(y=0;y<8;y++)
					  t_top[x][y] = t_top_dest[x][y];
			}
		}
	  return p;
 }

/* FUNCTION : For a given coin it returns possible destinations from the source given .
 */

int single_move(int p,int q,int st)
{
int x;
sm_cnt=0;

if(t_top[p][q]!=0)
{
	   for(x=st;x<=t_top[p][q];x++)
	   {
		   if(check(p-x,q))
			if((p-x)>=0 && t_board[p-x][q][t_top[p-x][q]-1]!=2)
			{
			 sm_moves[sm_cnt][0]=1;
			 sm_moves[sm_cnt][1]=p;
			 sm_moves[sm_cnt][2]=q;
			 sm_moves[sm_cnt][3]=t_top[p][q]-x+1;
			 sm_moves[sm_cnt][4]=p-x;
			 sm_moves[sm_cnt][5]=q;
			 sm_cnt++;
			}

		if(check(p+x,q))
		if((p+x)<8 && t_board[p+x][q][t_top[p+x][q]-1]!=2)
		{
			 sm_moves[sm_cnt][0]=1;
			 sm_moves[sm_cnt][1]=p;
			 sm_moves[sm_cnt][2]=q;
			 sm_moves[sm_cnt][3]=t_top[p][q]-x+1;
			 sm_moves[sm_cnt][4]=p+x;
			 sm_moves[sm_cnt][5]=q;
			 sm_cnt++;
		}
		if(check(p,q-x))
		if((q-x)>=0 && t_board[p][q-x][t_top[p][q-x]-1]!=2)
		{
			 sm_moves[sm_cnt][0]=1;
			 sm_moves[sm_cnt][1]=p;
			 sm_moves[sm_cnt][2]=q;
			 sm_moves[sm_cnt][3]=t_top[p][q]-x+1;
			 sm_moves[sm_cnt][4]=p;
			 sm_moves[sm_cnt][5]=q-x;
			 sm_cnt++;
		}
		if(check(p,q+x))
		if((q+x)<8 && t_board[p][q+x][t_top[p][q+x]-1]!=2)
		{
			 sm_moves[sm_cnt][0]=1;
			 sm_moves[sm_cnt][1]=p;
			 sm_moves[sm_cnt][2]=q;
			 sm_moves[sm_cnt][3]=t_top[p][q]-x+1;
			 sm_moves[sm_cnt][4]=p ;
			 sm_moves[sm_cnt][5]=q+x;
			 sm_cnt++;
		}
	}
}

return 0;
}

