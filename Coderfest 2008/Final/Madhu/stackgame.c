#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "StackGameClientCommunicator.h"

#define MAX_STACK 5
#define ROW       8
#define COL       8

char  _MYCOIN =  'M';
char  _OPCOIN =  'X';

char  MYCOIN =  'M';
char  OPCOIN =  'X';

#define MAX_MOVES 512

#define STACK_TOP(brd,i,j) (brd.cells[i][j].stack[brd.cells[i][j].stlen-1])
#define STACK_TOP1(brd,i,j) (brd->cells[i][j].stack[brd->cells[i][j].stlen-1])

#define DIFF(x,y)       ((((x)-(y)) > 0)?((x)-(y)):((y)-(x)))

#define gc(r,c) brd->cells[r][c]

struct cell
{
   int stlen;
   char stack[MAX_STACK * 3 + 1];
   int strength; /* No of our coins reachable for this cell - No of Op coins reachable for this cell */
   int mycutpow,opcutpow;
   int maxopcutpow;
};



struct move
{
   int strength;           /* strength in new place */
   int maxcutpow;
   int cap,res,totcoins,savcnt;

   int frow,fcol;
   int trow,tcol;
   int maxcutrow,maxcutcol;
   int cmovcnt;
   int count;
   struct move *cmovs;
};

struct bmove
{
   struct move movs[3];
   int movcnt;
   int maxcutpow;
   int cap,res,totcoins;
   int row,col;
};


struct board
{
   struct cell cells[ROW][COL]; /* place holder for the cells */

   int capture; /* count of coins captured */
   int reserve; /* count of reserve coins */

   int opp_capture; /* count of coins captured by opponent */
   int opp_reserve; /* count of coins kept in reserve by opponent */
   int maxcutpow;
   int maxcutrow,maxcutcol;
};

int getSupportFrom (struct board *brd, int r, int c, int dist) 
{
   int support = 0;
   int factor;
   struct cell *pcell = & (brd->cells[r][c]);
   int stlen = pcell->stlen;
   int newstlen;

   if (stlen < dist)
      return support;
   if (pcell->stack[stlen -1] == MYCOIN)
      factor = 1;
   else
      factor = -1;

   newstlen = stlen - dist;

   if (newstlen == 0)
      support = factor;
   else {
      if (pcell->stack[newstlen - 1] == pcell->stack[stlen -1])
         support = factor;
   }
   return support;
}


int get_strength (struct board *brd, int row, int col) {
   int strength = 0;
   int i;
   int trow, tcol;

   if (brd->cells[row][col].stlen < 0)
       return 0;

   for (i = 1; i <= MAX_STACK; i++) { /* have to consider the 5 coin stack too. NEED TO CONFIRM */
       trow = row -i;
       if (trow >= 0)
           strength += getSupportFrom (brd,  trow, col, i);

       trow = row + i;
       if (trow < ROW)
           strength += getSupportFrom (brd, trow, col, i);

       tcol = col - i;
       if (tcol >= 0)
           strength += getSupportFrom (brd, row, tcol , i);

       tcol = col + i;
       if (tcol < COL)
           strength += getSupportFrom (brd, row, tcol, i);
   }

   return strength;
}


int getCutPowFrom (struct board *brd, int r, int c, int dist) {
   int pow = 0;
   struct cell *pcell = & (brd->cells[r][c]);
   int stlen = pcell->stlen;
   int newstlen;

   if (stlen < dist)
       return pow;
   if (pcell->stack[stlen -1] == MYCOIN){
       newstlen = stlen - dist;
       if ((newstlen >= dist) && (pcell->stack[newstlen - 1] != MYCOIN))
           pow = 0;
       else
           pow = dist;
   }
   else
       pow = -dist;

   return pow;
}


void update_cutpow (struct board *brd, int row, int col) {
   int pow;
   int i;
   int trow, tcol;
   struct cell *pcell = &(brd->cells[row][col]);
   int *myPower = &(pcell->mycutpow);
   int *oppPower = &(pcell->opcutpow);
   int *maxOpPower = &(pcell->maxopcutpow);

   *myPower = 0;
   *oppPower = 0;
   *maxOpPower = 0;

   if (brd->cells[row][col].stlen < 0)
      return;

   for (i =1; i < MAX_STACK; i++) {
       trow = row -i;
       if (trow >= 0) {
           pow = getCutPowFrom (brd,  trow, col, i);
           if (pow < 0) {
               pow = -pow;
               *oppPower += pow;
               if (*maxOpPower < pow)
                   *maxOpPower = pow;
           }
           else
               *myPower += pow;
       }

       trow = row + i;
       if (trow < ROW) {
           pow = getCutPowFrom (brd, trow, col, i);
           if (pow < 0) {
               pow = -pow;
               *oppPower += pow;
               if (*maxOpPower < pow)
                   *maxOpPower = pow;
           }
           else
               *myPower += pow;
       }

       tcol = col - i;
       if (tcol >= 0) {
           pow = getCutPowFrom (brd, row, tcol , i);
           if (pow < 0) {
               pow = -pow;
               *oppPower += pow;
               if (*maxOpPower < pow)
                   *maxOpPower = pow;
           }
           else
               *myPower += pow;
       }

       tcol = col + i;
       if (tcol < COL) {
           pow = getCutPowFrom (brd, row, tcol, i);
           if (pow < 0) {
               pow = -pow;
               *oppPower += pow;
               if (*maxOpPower < pow)
                   *maxOpPower = pow;
           }
           else
               *myPower += pow;
       }

   }

}


void update_board_metrics(struct board *brd)
{
   int i,j;
   for(i=0;i<ROW;i++)
      for(j=0;j<COL;j++)
      {
         if(gc(i,j).stlen != -1)
         {
            gc(i,j).strength=get_strength(brd,i,j);
            update_cutpow(brd,i,j);
            if( (gc(i,j).mycutpow + gc(i,j).stlen) > brd->maxcutpow )
            {
               brd->maxcutpow=gc(i,j).mycutpow + gc(i,j).stlen;
               brd->maxcutrow=i;
               brd->maxcutcol=j;
            }

         }
      }
}


void init_board (struct board* brd)
{
       int i,j;
       brd->maxcutpow=0;
       for(i=0;i<ROW;i++)
               for(j=0;j<COL;j++)
               {
                       memset(brd->cells[i][j].stack, 0, ((MAX_STACK+1) *  sizeof(char)));
                       brd->cells[i][j].strength=0;
                       gc(i,j).mycutpow=gc(i,j).opcutpow=0;
                       if(i == 0 || j == 0 || i == ROW-1 || j == COL-1)
                               brd->cells[i][j].stlen=0;
                       else
                               brd->cells[i][j].stlen=1;

               }

       brd->cells[0][1].stlen = brd->cells[0][COL-2].stlen = brd->cells[0][COL-1].stlen = -1;
       brd->cells[1][0].stlen = brd->cells[1][COL-1].stlen = brd->cells[0][0].stlen     = -1;
       brd->cells[ROW-2][0].stlen = brd->cells[ROW-2][COL-1].stlen = brd->cells[ROW-1][COL-1].stlen = -1;
       brd->cells[ROW-1][0].stlen = brd->cells[ROW-1][1].stlen     = brd->cells[ROW-1][COL-2].stlen = -1;

       for(i=0;i<ROW;i++)
               for(j=0;j<COL;j++)
                       if(brd->cells[i][j].stlen == -1) strcpy(brd->cells[i][j].stack,"#####");

       brd->capture = brd->reserve = 0;
       brd->opp_capture = brd->opp_reserve = 0;

       for (i=1; i<(ROW-1); i+=2) {
               brd->cells[i][1].stack[0] = _OPCOIN; brd->cells[i][2].stack[0] = _OPCOIN;
               brd->cells[i][3].stack[0] = _MYCOIN; brd->cells[i][4].stack[0] = _MYCOIN;
               brd->cells[i][5].stack[0] = _OPCOIN; brd->cells[i][6].stack[0] = _OPCOIN;
       }

       for (i=2; i<(ROW-1); i+=2) {
               brd->cells[i][1].stack[0] = _MYCOIN; brd->cells[i][2].stack[0] = _MYCOIN;
               brd->cells[i][3].stack[0] = _OPCOIN; brd->cells[i][4].stack[0] = _OPCOIN;
               brd->cells[i][5].stack[0] = _MYCOIN; brd->cells[i][6].stack[0] = _MYCOIN;
       }

       update_board_metrics(brd);
}

void print_board (struct board* brd)
{
   int i, j;


   printf ("\n   ");
   /* print column number */
   for (j=0; j<COL; j++) printf ("    %1d      ", j);
      printf ("\n\n");

   for (i=0; i<ROW; i++) {
      printf (" %1d ", i); /* print row number */
      for (j=0; j<COL; j++) {
         if (j == 0) printf ("  %-5s %1d %1d", brd->cells[i][j].stack, gc(i,j).mycutpow, gc(i,j).opcutpow);
         else printf ("| %-5s %1d %1d", brd->cells[i][j].stack, gc(i,j).mycutpow, gc(i,j).opcutpow );
      }
      printf (" \n");
      if (i != (ROW-1)) printf ("    -----------------------------------------------------------------------------------------\n");
   }
   printf ("    -----------------------------------------------------------------------------------------\n");
   printf ("    My Reserves : %d   My Captures : %d                 Opp Reserves : %d   Opp Captures : %d\n", 
            brd->reserve, brd->capture, brd->opp_reserve, brd->opp_capture);

   printf ("\n\n");
   fflush(stdout);
}


int validate_move (struct board* brd, struct move mov,int coin, int flag)
{
	if (mov.frow == 0 && mov.fcol == 0 && coin == OPCOIN) {
		if (!brd->opp_reserve) {
			if (flag) printf ("Error : You don't have any reserve.\n");
			return 6;
		} else return 0;
	} else if (mov.frow == (ROW-1) && mov.fcol == (COL-1) && coin == OPCOIN) {
		if (flag) printf ("Error : That is not your reserve.\n");
		return 7;
	}
	else if (mov.frow != mov.trow && mov.fcol != mov.tcol) { /* diagonal move */
               if (flag) printf ("Error : Can't move diagonally\n");
               return 1;
       } else if ( brd->cells[mov.frow][mov.fcol].stlen < mov.count) { /* count of coins to be moved */
               if (flag) printf ("Error : Don't have sufficient coins in %d,%d\n", mov.frow, mov.fcol);
               return 2;
       } else if (DIFF(mov.frow,mov.trow) != mov.count && DIFF(mov.fcol, mov.tcol) != mov.count) { /* mismatch in coin count and cell movement */
               if (flag) printf ("Error : Count doesn't match the number cells moved\n");
               return 3;
  } else if (gc(mov.trow,mov.tcol).stlen == -1 ) {
     if (flag) printf ("Error : Thats a reserve placed and you don't have coins to move from.\n");
     return 8;
       } else if (gc(mov.frow,mov.fcol).stack[gc(mov.frow,mov.fcol).stlen-1] != coin) {
               if (flag) printf ("Error : You(%c) don't own the stack\n",coin);
               return 4;
       } else if (gc(mov.trow,mov.tcol).stlen == -1 ) {
     if (flag) printf ("Error : Can't move to reserved place.\n");
     return 5;
  }

  fflush(stdout);
       return 0;
}



int make_move(struct board *brd, struct move mov)
{
       int i,j;
       char coin=STACK_TOP1(brd,mov.frow,mov.fcol);

	if (mov.frow == 0 && mov.fcol == 0) {
		gc(mov.trow,mov.tcol).stack[gc(mov.trow,mov.tcol).stlen++] = OPCOIN; 
      if (gc(mov.trow,mov.tcol).stlen > MAX_STACK) {
         if (gc(mov.trow,mov.tcol).stack[0] == OPCOIN) brd->opp_reserve++;
         else brd->opp_capture++;

         memmove (&gc(mov.trow,mov.tcol).stack[0], &gc(mov.trow,mov.tcol).stack[1], MAX_STACK);
         gc(mov.trow,mov.tcol).stlen--;
      }
		gc(mov.trow,mov.tcol).stack[gc(mov.trow,mov.tcol).stlen]='\0';
		update_board_metrics(brd);
		brd->opp_reserve--;
		return;
	} else if (mov.frow == (ROW-1)  && mov.fcol == (COL-1)) {
		gc(mov.trow,mov.tcol).stack[gc(mov.trow,mov.tcol).stlen++] = MYCOIN; 

      if (gc(mov.trow,mov.tcol).stlen > MAX_STACK) {
         if (gc(mov.trow,mov.tcol).stack[0] == MYCOIN) brd->reserve++;
         else brd->capture++;

         memmove (&gc(mov.trow,mov.tcol).stack[0], &gc(mov.trow,mov.tcol).stack[1], MAX_STACK);
         gc(mov.trow,mov.tcol).stlen--;
      }

		gc(mov.trow,mov.tcol).stack[gc(mov.trow,mov.tcol).stlen]='\0';
		update_board_metrics(brd);
		brd->reserve--;
		return;
	}

       for(i=gc(mov.frow,mov.fcol).stlen - mov.count; i < gc(mov.frow,mov.fcol).stlen ; i++) {
               if (gc(mov.trow,mov.tcol).stlen == MAX_STACK) {
                       if (coin == MYCOIN) {
                               if (gc(mov.trow, mov.tcol).stack[0] == MYCOIN) {
              brd->reserve++;
              memset(gc(ROW-1, COL-1).stack, MYCOIN, brd->reserve);
              gc(ROW-1, COL-1).stack[brd->reserve] = '\0';
           }
           else brd->capture++;
        }
                       else
                       {
           if (gc(mov.trow, mov.tcol).stack[0] == OPCOIN) {
              brd->opp_reserve++;
              memset(gc(ROW-1, COL-1).stack, OPCOIN, brd->opp_reserve);
              gc(ROW-1, COL-1).stack[brd->opp_reserve] = '\0';
           }
           else brd->opp_capture++;
        }
                       memmove(&gc(mov.trow, mov.tcol).stack[0], &gc(mov.trow, mov.tcol).stack[1], (MAX_STACK-1));
        gc(mov.trow, mov.tcol).stack[(MAX_STACK-1)] = '\0';
        gc(mov.trow, mov.tcol).stack[MAX_STACK] = '\0';
        gc(mov.trow, mov.tcol).stlen = MAX_STACK-1;
     }
               gc(mov.trow,mov.tcol).stack[gc(mov.trow,mov.tcol).stlen++] = gc(mov.frow,mov.fcol).stack[i];
		gc(mov.trow,mov.tcol).stack[gc(mov.trow,mov.tcol).stlen]='\0';
               gc(mov.frow,mov.fcol).stack[i]='\0';
       }

  gc(mov.frow,mov.fcol).stlen-=mov.count;
       update_board_metrics(brd);
}


int
update_move_metrics(struct board inbrd, struct move *mov,int minst)
{
       int i,j;
       struct board tbrd;

       tbrd=inbrd;
       mov->maxcutpow=mov->maxcutrow=mov->maxcutcol=0;
       mov->cap=mov->res=mov->totcoins=mov->savcnt=0;

       make_move(&tbrd,*mov);

       mov->strength=tbrd.cells[mov->trow][mov->tcol].strength;

       if (tbrd.maxcutpow > inbrd.maxcutpow)
       {
               mov->maxcutpow=tbrd.maxcutpow;
               mov->maxcutrow=tbrd.maxcutrow;
               mov->maxcutcol=tbrd.maxcutrow;
       }
       if(minst == -100) return 0;
       if(mov->strength < minst) return 1;

       if (inbrd.capture < tbrd.capture) mov->cap = tbrd.capture - inbrd.capture;
       if (inbrd.reserve < tbrd.reserve) mov->res = tbrd.reserve - inbrd.reserve;
       mov->totcoins=mov->cap + mov->res;

       /* CHECK Don't build stack to favor enemy */
       if( (mov->totcoins + tbrd.cells[mov->trow][mov->tcol].mycutpow) <
                       tbrd.cells[mov->trow][mov->tcol].opcutpow )
       if(mov->totcoins < mov->count - mov->totcoins)
               return 1;

       /* CHECK runaway from enemy territory */
       if(inbrd.cells[mov->frow][mov->fcol].stlen >= 3)
               if(inbrd.cells[mov->frow][mov->fcol].mycutpow < inbrd.cells[mov->frow][mov->fcol].opcutpow)
               if(inbrd.cells[mov->frow][mov->fcol].stlen + inbrd.cells[mov->frow][mov->fcol].opcutpow > 5)
               {
                       mov->savcnt = inbrd.cells[mov->frow][mov->fcol].stlen + 
                                               inbrd.cells[mov->frow][mov->fcol].opcutpow - 5 ;
                       mov->savcnt +=mov->totcoins;
		       mov->savcnt -=inbrd.cells[mov->frow][mov->fcol].mycutpow;

                       printf("Runnning away code for (%d,%d)->(%d %d) savcnt=%d\n",mov->frow,mov->fcol,
                                               mov->trow,mov->tcol,mov->savcnt);

               }

       if(tbrd.cells[mov->trow][mov->tcol].stlen == 5)
       if(mov->totcoins != mov->count)
       if(tbrd.cells[mov->trow][mov->tcol].opcutpow >= (tbrd.cells[mov->trow][mov->tcol].mycutpow + mov->totcoins))
             return 1;



       return 0;
}


int get_all_moves (struct board brd,struct move * movs,int coin,int minst)
{
       int m = 0;
       int i, j, k;

       for (i=0; i<ROW; i++)
       {
               for (j=0; j<COL; j++)
               {
                       if (brd.cells[i][j].stlen<=0 || brd.cells[i][j].stack[brd.cells[i][j].stlen-1] != coin) continue;

                       for (k=1; k <= brd.cells[i][j].stlen; k++)
                       {
                               if ( i - k >= 0 && brd.cells[i-k][j].stlen != -1)
                               {
                                       movs[m].frow = i; movs[m].fcol = j;
                                       movs[m].trow = i-k; movs[m].tcol = j; movs[m].count = k;
                                       if(update_move_metrics(brd,&movs[m],minst) == 0) m++;
                               }

                               if ( i + k < ROW && brd.cells[i+k][j].stlen != -1)
                               {
                                       movs[m].frow = i; movs[m].fcol = j;
                                       movs[m].trow = i+k; movs[m].tcol = j; movs[m].count = k;
                                       if(update_move_metrics(brd,&movs[m],minst) == 0) m++;
                               }
                               if ( j - k >=0 && brd.cells[i][j-k].stlen != -1)
                               {
                                       movs[m].frow = i; movs[m].fcol = j;
                                       movs[m].trow = i; movs[m].tcol = j-k; movs[m].count = k;
                                       if(update_move_metrics(brd,&movs[m],minst) == 0) m++;
                               }
                               if ( j + k < COL && brd.cells[i][j+k].stlen != -1)
                               {
                                       movs[m].frow = i; movs[m].fcol = j;
                                       movs[m].trow = i; movs[m].tcol = j+k; movs[m].count = k;
                                       if(update_move_metrics(brd,&movs[m],minst) == 0) m++;
                               }
                       }
               }
       }


       if(m >=512)
               printf("\nERROR>>>>>>>>>>>The value of m=%d\n",m);

       return m;
}

struct move
get_best_move(struct board inbrd)
{
       int m,m1,m2,t, tsavecnt = 0, tdcpow = 0;
       struct board tbrd,tbrd1,tbrd2;
       static struct move * root,*tmov,*tmov1,*tmov2,mov;
       struct move movbuf1[512],movbuf2[512],movbuf3[512];
       struct bmove bmov_cutpow,bmov_coins;

       bmov_cutpow.maxcutpow=0;
       bmov_coins.res=bmov_coins.cap=bmov_coins.totcoins=bmov_coins.maxcutpow=0;

       mov.frow=mov.fcol=mov.trow=mov.tcol=-1;
       root=&mov;
       root->cmovs=movbuf1;

       root->cmovcnt=get_all_moves(inbrd,root->cmovs,MYCOIN,-18);

       /* CHECK Run away from enemy territory */
       for(m=0;m<root->cmovcnt;m++)
       {
               if (tsavecnt < root->cmovs[m].savcnt)
               {
                       tsavecnt = root->cmovs[m].savcnt;
                       tdcpow = inbrd.cells[root->cmovs[m].trow][root->cmovs[m].tcol].mycutpow - inbrd.cells[root->cmovs[m].trow][root->cmovs[m].tcol].opcutpow;
                       bmov_coins.maxcutpow=root->cmovs[m].maxcutpow;
                       bmov_coins.totcoins=root->cmovs[m].savcnt;
                       bmov_coins.movs[0]=root->cmovs[m];
               } else if (tsavecnt == root->cmovs[m].savcnt) {
                       if ((inbrd.cells[root->cmovs[m].trow][root->cmovs[m].tcol].mycutpow - inbrd.cells[root->cmovs[m].trow][root->cmovs[m].tcol].opcutpow) > tdcpow) {
                          tdcpow = inbrd.cells[root->cmovs[m].trow][root->cmovs[m].tcol].mycutpow - inbrd.cells[root->cmovs[m].trow][root->cmovs[m].tcol].opcutpow;
                          bmov_coins.maxcutpow=root->cmovs[m].maxcutpow;
                          bmov_coins.totcoins=root->cmovs[m].savcnt;
                          bmov_coins.movs[0]=root->cmovs[m];
                       }
               }
       }

       if(bmov_coins.totcoins)
       {
               printf("Running away to save %d coins from opponent  \n",bmov_coins.movs[0].totcoins);
               return bmov_coins.movs[0];
       }


       /* If you can gain coin by placing on op stack/on a weak CHECK cell go ahead and do that,
       if you try to make better move, that stack might be taken to different place by
       opponent */
       for(m=0;m<root->cmovcnt;m++)
       {
               if( (STACK_TOP(inbrd,root->cmovs[m].trow,root->cmovs[m].tcol) == OPCOIN) ||
                   (inbrd.cells[root->cmovs[m].trow][root->cmovs[m].tcol].strength < 0) )
                       if(root->cmovs[m].totcoins > bmov_coins.totcoins)
                       {
                               bmov_coins.totcoins=root->cmovs[m].totcoins;
                               bmov_coins.movs[0]=root->cmovs[m];
                       }
       }
       if(bmov_coins.totcoins)
       {
               printf("Trying to get %d coins from op stack \n",bmov_coins.movs[0].totcoins);
               return bmov_coins.movs[0];
       }

       root->cmovcnt=get_all_moves(inbrd,root->cmovs,MYCOIN,0);
       t=root->cmovcnt;


       for(m=0;m<root->cmovcnt;m++)
       {
               tbrd=inbrd;
               tmov=&root->cmovs[m];

               make_move(&tbrd,*tmov);
               tmov->cmovs=movbuf2;
               tmov->cmovcnt = get_all_moves(tbrd,tmov->cmovs,MYCOIN,0);

               t+=tmov->cmovcnt;

               if(tmov->maxcutpow > bmov_cutpow.maxcutpow || 
		 (tmov->maxcutpow == bmov_cutpow.maxcutpow && bmov_cutpow.movcnt > 1))
               {
                       bmov_cutpow.maxcutpow=tmov->maxcutpow;
                       bmov_cutpow.movs[0]=*tmov;
                       bmov_cutpow.row=tmov->maxcutrow;
                       bmov_cutpow.col=tmov->maxcutcol;
                       bmov_cutpow.movcnt=1;
               }

               if(tmov->totcoins > bmov_coins.totcoins ||
		 (tmov->totcoins == bmov_coins.totcoins && bmov_coins.movcnt > 1))
               {
                       bmov_coins.totcoins= tmov->totcoins;
                       bmov_coins.movs[0]=*tmov;
                       bmov_coins.movcnt=1;
               }


               for(m1=0;m1<tmov->cmovcnt;m1++)
               {
                       tbrd1=tbrd;
                       tmov1=&tmov->cmovs[m1];

                       make_move(&tbrd1,*tmov1);
                       tmov1->cmovs=movbuf3;
                       tmov1->cmovcnt = get_all_moves(tbrd1,tmov1->cmovs,MYCOIN,0);
                       t+=tmov1->cmovcnt;

                       if(tmov1->maxcutpow > bmov_cutpow.maxcutpow ||
		         (tmov->maxcutpow == bmov_cutpow.maxcutpow && bmov_cutpow.movcnt > 2))
                       {
                               bmov_cutpow.maxcutpow=tmov1->maxcutpow;
                               bmov_cutpow.row=tmov1->maxcutrow;
                               bmov_cutpow.col=tmov1->maxcutcol;
                               bmov_cutpow.movs[0]=*tmov;
                               bmov_cutpow.movs[1]=*tmov1;
                               bmov_cutpow.movcnt=2;
                       }

                       if( (tmov->totcoins + tmov1->totcoins > bmov_coins.totcoins) ||
                         ( (tmov->totcoins + tmov1->totcoins == bmov_coins.totcoins) && (bmov_coins.movcnt > 2) ) || 
                         ( (tmov->totcoins + tmov1->totcoins == bmov_coins.totcoins) && (bmov_coins.movcnt == 2) &&
                         	(tmov->totcoins > bmov_coins.movs[0].totcoins) ) )

                       {
                               bmov_coins.totcoins= tmov->totcoins + tmov1->totcoins;
                               bmov_coins.movs[0]=*tmov;
                               bmov_coins.movs[1]=*tmov1;
                               bmov_coins.movcnt=2;
                       }

                       for(m2=0;m2<tmov1->cmovcnt;m2++)
                       {
                               tbrd2=tbrd1;
                               tmov2=&tmov1->cmovs[m2];
                               if(tmov2->maxcutpow > bmov_cutpow.maxcutpow)
                               {
                                       bmov_cutpow.maxcutpow=tmov2->maxcutpow;
                                       bmov_cutpow.row=tmov2->maxcutrow;
                                       bmov_cutpow.col=tmov2->maxcutcol;
                                       bmov_cutpow.movs[0]=*tmov;
                                       bmov_cutpow.movs[1]=*tmov1;
                                       bmov_cutpow.movs[2]=*tmov2;
                                       bmov_cutpow.movcnt=3;
                               }

                               if( (tmov->totcoins + tmov1->totcoins + tmov2->totcoins > bmov_coins.totcoins) ||
                                   ((tmov->totcoins + tmov1->totcoins + tmov2->totcoins == bmov_coins.totcoins) && 
					(bmov_coins.movcnt == 3) && (tmov->totcoins > bmov_coins.movs[0].totcoins)) )
                               {
                                       bmov_coins.totcoins= tmov->totcoins + tmov1->totcoins + tmov2->totcoins;
                                       bmov_coins.movs[0]=*tmov;
                                       bmov_coins.movs[1]=*tmov1;
                                       bmov_coins.movs[2]=*tmov2;
                                       bmov_coins.movcnt=3;
                               }

                       }
               }

       }

       printf("\n**T=%d\n",t);
       if(bmov_coins.totcoins)
       {
               printf("\n To get %d coins planning to make %d movs [(%d,%d)->(%d,%d)],[(%d,%d)->(%d,%d)],[(%d,%d)->(%d,%d)]\n",
                       bmov_coins.totcoins,bmov_coins.movcnt,
                       bmov_coins.movs[0].frow,bmov_coins.movs[0].fcol, bmov_coins.movs[0].trow,bmov_coins.movs[0].tcol,
                       bmov_coins.movs[1].frow,bmov_coins.movs[1].fcol, bmov_coins.movs[1].trow,bmov_coins.movs[1].tcol,
                       bmov_coins.movs[2].frow,bmov_coins.movs[2].fcol, bmov_coins.movs[2].trow,bmov_coins.movs[2].tcol);
               return bmov_coins.movs[0];
       }

       if(inbrd.reserve)
       {
               int i,j;
               for(i=0;i<ROW;i++)
                       for(j=0;j<COL;j++)
                       {
                               if(inbrd.cells[i][j].stlen == 5 && inbrd.cells[i][j].mycutpow == 0)
                               {
                                       printf("Use reserve on top of (%d,%d)\n",i,j);
                                       bmov_coins.movs[0].frow=(ROW-1);
                                       bmov_coins.movs[0].fcol=(COL-1);
                                       bmov_coins.movs[0].trow=i;
                                       bmov_coins.movs[0].tcol=j;
                                       bmov_coins.movs[0].count = 1;   
                                       return bmov_coins.movs[0];
                               }
                       }
       }

	if(bmov_cutpow.maxcutpow >0)
       		return bmov_cutpow.movs[0];
        
       root->cmovcnt=get_all_moves(inbrd,root->cmovs,MYCOIN,-100);
       return root->cmovs[0]; 
}

int play_manual(void)
{
       struct board brd;
       struct move mov;
       int play = 1, color = 1;

       printf ("You want to play first (1) or second (2) : "); fflush(stdout);
       scanf("%d", &play); fflush(stdin);

       printf ("You want to play red(1) or green (0) : "); fflush(stdout);
       scanf("%d", &color); fflush(stdin);

       if (color == 0) {
         MYCOIN = _OPCOIN;
         OPCOIN = _MYCOIN;      

         printf ("My coin is X and Opp coin is M\n");
       }

       init_board (&brd);
       print_board (&brd);

       while(1)
       {
               if (play == 2) {
                  mov=get_best_move(brd);
                  printf("\n\nMy mov is:(%d,%d)->(%d,%d),%d\n",mov.frow,mov.fcol,
                                                  mov.trow,mov.tcol,mov.count);

                  make_move(&brd,mov);
                  print_board (&brd);
               }

               do
               {
                       printf("Enter your move row,col->row,col,count\n");fflush(stdout);
                       scanf("%d,%d->%d,%d,%d",&mov.frow,&mov.fcol,&mov.trow,&mov.tcol,&mov.count);
                       fflush(stdin);
               }
               while(validate_move(&brd,mov, OPCOIN, 1));

               make_move(&brd,mov);
               print_board (&brd);

               if (play == 1) {
                  mov=get_best_move(brd);
                  printf("\n\nMy mov is:(%d,%d)->(%d,%d),%d\n",mov.frow,mov.fcol,
                                                  mov.trow,mov.tcol,mov.count);

                  make_move(&brd,mov);
                  print_board (&brd);
               }
       }

       return 0;
}


int play_program(void)
{
   int *retVal;
   struct board brd;
   struct move mov;
   int play = 1;

  setPropertyFileName(".\\stackgameclient.properties");

   printf("Waiting for handshake ... ...\n");
   retVal = handShake();
   printf("The response is: %d %d %d\n",retVal[0],retVal[1],retVal[2]); 

   if ( retVal[1] == RED_COINS) {
      MYCOIN = _OPCOIN;
      OPCOIN = _MYCOIN;

      printf ("My coin is X and Opp coin is M\n");
   }

   if ( retVal[2] == PLAY_SECOND) {
      play = 2;
      printf ("I am playing second.\n");
   }
   else {
      play = 1;
      printf ("I am playing first.\n");
   }

 init_board (&brd);
 print_board (&brd);

   while (1) {
      if (play == 1) {
         mov=get_best_move(brd);
         printf("\n\nMy move :(%d,%d)->(%d,%d),%d\n",mov.frow,mov.fcol, mov.trow,mov.tcol,mov.count);


         if (mov.frow == (ROW-1) && mov.fcol == (COL-1)) {
            writeMoves(MODE_RESERVE, 0, 0, 0, mov.trow + 1, mov.tcol + 1);
         } else {
            int pos = (brd.cells[mov.frow][mov.fcol].stlen - mov.count) + 1;
            writeMoves(MODE_NORMAL, mov.frow + 1, mov.fcol + 1, pos, mov.trow + 1, mov.tcol + 1);
         }

         make_move(&brd,mov);
         print_board (&brd);
      }

      memset(&mov, 0, sizeof(mov));
		printf("Waiting for response ... ...\n");
		retVal = readMoves();

		printf("The response is: %d %d %d %d %d %d\n",retVal[0],retVal[1],retVal[2],retVal[3],retVal[4],retVal[5]);

      if(retVal[0] !=0 && retVal[0] != 1) {

			if(retVal[0] == GAME_ENDED)
            printf ("Game ended. Exiting......\n");
			else if(retVal[0] == GAME_RESET)
            printf ("Game reset. Exiting......\n");
			else if(retVal[0] == INVALID_MOVE)
            printf ("Invalid move. Exiting......\n");

         exit(1);
		}

      if (retVal[0] == MODE_RESERVE) {
         mov.frow = 0; mov.fcol = 0; mov.count = 1;
      }
      else if (retVal[0] == MODE_NORMAL) {
         mov.frow = retVal[1] - 1; mov.fcol = retVal[2] - 1;
         mov.count = (brd.cells[mov.frow][mov.fcol].stlen - retVal[3]) + 1;
      }
      mov.trow = retVal[4] - 1; mov.tcol = retVal[5] - 1;
      printf("\n\nOp move :(%d,%d)->(%d,%d),%d\n",mov.frow,mov.fcol, mov.trow,mov.tcol,mov.count);

      make_move(&brd,mov);
      print_board (&brd);
      memset(&mov, 0, sizeof(mov));


      if (play == 2) {
         mov=get_best_move(brd);
         printf("\n\nMy move :(%d,%d)->(%d,%d),%d\n",mov.frow,mov.fcol, mov.trow,mov.tcol,mov.count);

         if (mov.frow == (ROW-1) && mov.fcol == (COL-1)) {
            writeMoves(MODE_RESERVE, 0, 0, 0, mov.trow + 1, mov.tcol + 1);
         } else {
            int pos = (brd.cells[mov.frow][mov.fcol].stlen - mov.count) + 1;
            writeMoves(MODE_NORMAL, mov.frow + 1, mov.fcol + 1, pos, mov.trow + 1, mov.tcol + 1);
         }

         make_move(&brd,mov);
         print_board (&brd);
      }
   }
}


int main(int argc, char *argv[])
{
   if (argc > 1) {
      play_manual();
   }
   else {
      play_program();
   }
}
