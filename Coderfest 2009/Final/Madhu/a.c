/* TEAM : null */
/* WARNING : 
 *          Code is typed by different persons in different mood. 
 *          Due to lack of time, we are submitting the prototype.
 *                            SO
 *          Don't feel bad about you, if you are not able to understand the code logic
 */

#include<stdio.h>
#include"a.h"

int MAXLEVEL=4;
#define SECCHECKLEVEL 2

#define MAXPATTERN 2
#define TOTSTAGE 18

enum x{NOMIRROR,MIRROR};
enum s{STAGE1,STAGE2,STAGE3,STAGE4,STAGE5,STAGE6,STAGE7, STAGE8,STAGE9,STAGE10,STAGE11,STAGE12,STAGE13,STAGE14, STAGE15,STAGE16,STAGE17,STAGE18,STAGE19,STAGE20};

int debug=0;
int maxstage=TOTSTAGE;

cord_t solmvmts[3]={ {1,0},{0,1},{0,-1}};
path_t bpath;
board_t bbrd;

int oppfirst;

int initpos[7][7] = {
    { 9, 9, 1, 1, 1, 9, 9},
    { 9, 9, 1, 1, 1, 9, 9},
    { 1, 1, 1, 1, 1, 1, 1},
    { 0, 0, 0, 0, 0, 0, 0},
    { 0, 0, 0, 2, 0, 0, 0},
    { 9, 9, 0, 0, 0, 9, 9},
    { 9, 9, 0, 0, 0, 9, 9}
};

int debugpos[7][7] = {
    { 9, 9, 1, 0, 1, 9, 9},
    { 9, 9, 1, 1, 1, 9, 9},
    { 0, 0, 1, 1, 1, 1, 1},
    { 1, 0, 0, 0, 0, 0, 0},
    { 1, 1, 0, 0, 0, 0, 0},
    { 9, 9, 0, 0, 0, 9, 9},
    { 9, 9, 0, 2, 0, 9, 9}
};



typedef struct
{
	int pattern[MAXPATTERN][7][7];
	int isachieved[MAXPATTERN];
	soldiers_t soldiers[MAXPATTERN];
	soldiers_t nosoldiers[MAXPATTERN];
	int cnt,value,incr;
}stage_t;

stage_t stgs[TOTSTAGE];


int stage1_patterns[MAXPATTERN][7][7] = {
        {{ 9, 9, 0, 0, 0, 9, 9},
         { 9, 9, 0, 0, 0, 9, 9},
         { 0, 0, 0, 0, 0, 0, 0},
         { 1, 0, 0, 0, 0, 0, 0},
         { 1, 0, 0, 0, 0, 0, 0},
         { 9, 9, 0, 0, 0, 9, 9},
         { 9, 9, 0, 0, 0, 9, 9}},
};
int stage2_patterns[MAXPATTERN][7][7] = {
        {{ 9, 9, 0, 0, 0, 9, 9},
         { 9, 9, 0, 0, 0, 9, 9},
         { 0, 0, 0, 0, 0, 0, 0},
         { 0, 0, 0, 0, 0, 0, 0},
         { 1, 1, 0, 0, 0, 0, 0},
         { 9, 9, 0, 0, 0, 9, 9},
         { 9, 9, 0, 0, 0, 9, 9}}
};

int stage3_patterns[MAXPATTERN][7][7] = {
        {{ 9, 9, 0, 0, 0, 9, 9},
         { 9, 9, 0, 0, 0, 9, 9},
         { 0, 0, 0, 0, 0, 0, 0},
         { 1, 0, 0, 0, 0, 0, 0},
         { 1, 1, 0, 0, 0, 0, 0},
         { 9, 9, 0, 0, 0, 9, 9},
         { 9, 9, 0, 0, 0, 9, 9}}
};

int stage4_patterns[MAXPATTERN][7][7] = {
        {{ 9, 9, 0, 0, 0, 9, 9},
         { 9, 9, 0, -1, 0, 9, 9},
         { 0, 0, 0, -1, 0, 0, 0},
         { 1, 0, 0, 0, 0, 0, 0},
         { 1, 1, 0, 0, 0, 0, 0},
         { 9, 9, 0, 0, 0, 9, 9},
         { 9, 9, 0, 0, 0, 9, 9}},
};

int stage5_patterns[MAXPATTERN][7][7] = {
        {{ 9, 9, 0, 0, 0, 9, 9},
         { 9, 9, 0, 0, 0, 9, 9},
         { 1, 0, 0, 0, 0, 0, 0},
         { 1, 0, 0, 0, 0, 0, 0},
         { 1, 1, 0, 0, 0, 0, 0},
         { 9, 9, 0, 0, 0, 9, 9},
         { 9, 9, 0, 0, 0, 9, 9}},
};

int stage6_patterns[MAXPATTERN][7][7] = {
        {{ 9, 9, 0, 0, 0, 9, 9},
         { 9, 9, 0, 0, 0, 9, 9},
         { 1, 0, 0, 0, 0, 0, 0},
         { 1, 1, 0, 0, 0, 0, 0},
         { 1, 1, 0, 0, 0, 0, 0},
         { 9, 9, 0, 0, 0, 9, 9},
         { 9, 9, 0, 0, 0, 9, 9}},
};

int stage7_patterns[MAXPATTERN][7][7] = {
        {{ 9, 9, 0, 0, 0, 9, 9},
         { 9, 9, 0, 0, 0, 9, 9},
         { 1, 1, 0, 0, 0, 0, 0},
         { 1, 1, 0, 0, 0, 0, 0},
         { 1, 1, 0, 0, 0, 0, 0},
         { 9, 9, 0, 0, 0, 9, 9},
         { 9, 9, 0, 0, 0, 9, 9}},
};

int stage8_patterns[MAXPATTERN][7][7] = {
        {{ 9, 9, 0, 0, 0, 9, 9},
         { 9, 9, 0, 0, 0, 9, 9},
         { 0, 0, 0, 0, 0, 0, 0},
         { 0, 0, 0, 0, 0, 0, 0},
         { 0, 0, 0, 0, 0, 0, 0},
         { 9, 9, 0, 0, 0, 9, 9},
         { 9, 9, 1, 0, 0, 9, 9}},
};

int stage9_patterns[MAXPATTERN][7][7] = {
        {{ 9, 9, 0, 0, 0, 9, 9},
         { 9, 9, 0, 0, 0, 9, 9},
         { 0, 0, 0, 0, 0, 0, 0},
         { 0, 0, 0, 0, 0, 0, 0},
         { 0, 0, 0, 0, 0, 0, 0},
         { 9, 9, 0, 0, 0, 9, 9},
         { 9, 9, 1, 0, 1, 9, 9}},
};

int stage10_patterns[MAXPATTERN][7][7] = {
        {{ 9, 9, 0, 0, 0, 9, 9},
         { 9, 9, 0, 0, 0, 9, 9},
         { 0, 0, 0, 0, 0, 0, 0},
         { 0, 0, 0, 0, 0, 0, 0},
         { 0, 0, 0, 0, 0, 0, 0},
         { 9, 9, 1, 0, 0, 9, 9},
         { 9, 9, 1, 0, 0, 9, 9}},
};
int stage11_patterns[MAXPATTERN][7][7] = {
        {{ 9, 9, 0, 0, 0, 9, 9},
         { 9, 9, 0, 0, 0, 9, 9},
         { 0, 0, 0, 0, 0, 0, 0},
         { 0, 0, 0, 0, 0, 0, 0},
         { 0, 0, 0, 0, 0, 0, 0},
         { 9, 9, 1, 0, 1, 9, 9},
         { 9, 9, 1, 0, 1, 9, 9}},
};

int stage12_patterns[MAXPATTERN][7][7] = {
        {{ 9, 9, 0, 0, 0, 9, 9},
         { 9, 9, 0, 0, 0, 9, 9},
         { 0, 0, 0, 0, 0, 0, 0},
         { 1, 0, 0, 0, 0, 0, 0},
         { 1, 1, 0, 0, 0, 0, 0},
         { 9, 9, 1, 0, 1, 9, 9},
         { 9, 9, 1, 0, 1, 9, 9}},
};

int stage13_patterns[MAXPATTERN][7][7] = {
        {{ 9, 9, 0, 0, 0, 9, 9},
         { 9, 9, 0, 0, 0, 9, 9},
         { 0, 0, 0, 0, 0, 0, 0},
         { 0, 0, 0, 0, 0, 0, 0},
         { 0, 0, 0, 0, 0, 0, 0},
         { 9, 9, 1, 0, 0, 9, 9},
         { 9, 9, 1, 1, 1, 9, 9}},
};

int stage14_patterns[MAXPATTERN][7][7] = {
        {{ 9, 9, 0, 0, 0, 9, 9},
         { 9, 9, 0, 0, 0, 9, 9},
         { 0, 0, 0, 0, 0, 0, 0},
         { 0, 0, 0, 0, 0, 0, 0},
         { 0, 0, 0, 0, 0, 0, 0},
         { 9, 9, 1, 0, 1, 9, 9},
         { 9, 9, 1, 1, 1, 9, 9}},
};

int stage15_patterns[MAXPATTERN][7][7] = {
        {{ 9, 9, 0, 0, 0, 9, 9},
         { 9, 9, 0, 0, 0, 9, 9},
         { 0, 0, 0, 0, 0, 0, 0},
         { 0, 0, 0, 0, 0, 0, 0},
         { 0, 0, 0, 0, 0, 0, 0},
         { 9, 9, 1, 1, 0, 9, 9},
         { 9, 9, 1, 1, 1, 9, 9}},
};
int stage16_patterns[MAXPATTERN][7][7] = {
        {{ 9, 9, 0, 0, 0, 9, 9},
         { 9, 9, 0, 0, 0, 9, 9},
         { 0, 0, 0, 0, 0, 0, 0},
         { 0, 0, 0, 0, 0, 0, 0},
         { 0, 0, 0, 0, 0, 0, 0},
         { 9, 9, 1, 1, 1, 9, 9},
         { 9, 9, 1, 1, 1, 9, 9}},
 };

int stage17_patterns[MAXPATTERN][7][7] = {
        {{ 9, 9, 0, 0, 0, 9, 9},
         { 9, 9, 0, 0, 0, 9, 9},
         { 0, 0, 0, 0, 0, 0, 0},
         { 0, 0, 0, 0, 0, 0, 0},
         { 0, 0, 1, 1, 0, 0, 0},
         { 9, 9, 1, 1, 1, 9, 9},
         { 9, 9, 1, 1, 1, 9, 9}},
 };
int stage18_patterns[MAXPATTERN][7][7] = {
        {{ 9, 9, 0, 0, 0, 9, 9},
         { 9, 9, 0, 0, 0, 9, 9},
         { 0, 0, 0, 0, 0, 0, 0},
         { 0, 0, 0, 0, 0, 0, 0},
         { 0, 0, 1, 1, 1, 0, 0},
         { 9, 9, 1, 1, 1, 9, 9},
         { 9, 9, 1, 1, 1, 9, 9}},
 };

displayopenstages()
{
	int s,p;
	for(s=0;s<maxstage;s++)
		for(p=0;p<stgs[s].cnt;p++)
		{
			if(stgs[s].isachieved[p]) continue;
		/*	printf("STG=%d SUBSTG=%d\n",s,p); */
		}
}

mvmt_t actpath[512];  /*actual path, the real moves we have done till*/
int actpathlen = 0;          

getrandompath(board_t brd)
{
        int s,m,i;
        path_t tpath;

        for(s=0;s<brd.soldiers.cnt;s++)
        {
                for(m=0;m<3;m++)
                {
                        board_t tbrd=brd;
                        mvmt_t nxtmove;

                        nxtmove.src=tbrd.soldiers.sol[s];
                        nxtmove.des.r=nxtmove.src.r+solmvmts[m].r;
                        nxtmove.des.c=nxtmove.src.c+solmvmts[m].c;

                        if(!INLIMITS(tbrd,nxtmove.des.r,nxtmove.des.c)) continue;
                        if(tbrd.cells[nxtmove.des.r][nxtmove.des.c].coin != ' ') continue;
			if(isloop(nxtmove)) continue;

			bpath.movcnt=1;
			bpath.moves[0]=nxtmove;

                        if(!placesoldier(&tbrd,nxtmove.src,nxtmove.des,0)) return; 

                }
        }
}


int isloop(mvmt_t move)
{
	int i;
	int i1 = -1, i2 = -1,lsize;

	for (i = actpathlen - 1; i>=0; i--)
	{
		if (actpath[i].src.r == move.src.r && actpath[i].src.c == move.src.c && 
			actpath[i].des.r == move.des.r && actpath[i].des.c == move.des.c)
		{
			i1 = i;
			lsize=actpathlen - i1;
			i2=i1-lsize;
			break;
		}
	}
	if (i1 < 0 || i2 < 0 ) return 0; /* no loop */

	for (i=0;i<lsize;i++)
	{
		if ((actpath[i1].src.r != actpath[i2].src.r || actpath[i1].src.c != actpath[i2].src.c ||
			actpath[i1].des.r != actpath[i2].des.r || actpath[i1].des.c != actpath[i2].des.c))
			return 0;	
	}
	return 1; /* loop */
}

copypatterns(int stage,int count,int pattern[MAXPATTERN][7][7],int mirror)
{
	int r,c,p;
	stgs[stage].cnt=count;
	if(mirror)
		stgs[stage].cnt*=2;
	for(p=0;p<count;p++)
	{
		soldiers_t tsoldiers,tsoldiers1;
		soldiers_t tnosoldiers,tnosoldiers1;
		memset(&tsoldiers,'\0',sizeof(tsoldiers));
		memset(&tsoldiers1,'\0',sizeof(tsoldiers));
		memset(&tnosoldiers,'\0',sizeof(tsoldiers));
		memset(&tnosoldiers1,'\0',sizeof(tsoldiers));
		for(r=0;r<7;r++)
			for(c=0;c<7;c++)
			{
				stgs[stage].pattern[p][r][c]=pattern[p][r][c];
				if(stgs[stage].pattern[p][r][c]==1)
				{
					tsoldiers.sol[tsoldiers.cnt].r=r;
					tsoldiers.sol[tsoldiers.cnt++].c=c;
					stgs[stage].soldiers[p]=tsoldiers;
				}
				if(stgs[stage].pattern[p][r][c]==-1)
				{
					tnosoldiers.sol[tnosoldiers.cnt].r=r;
					tnosoldiers.sol[tnosoldiers.cnt++].c=c;
					stgs[stage].nosoldiers[p]=tnosoldiers;
				}

				if(mirror)
				{
					stgs[stage].pattern[p+1][r][c]=pattern[p][r][6-c];
					if(stgs[stage].pattern[p+1][r][c]==1)
					{
						tsoldiers1.sol[tsoldiers1.cnt].r=r;
						tsoldiers1.sol[tsoldiers1.cnt++].c=c;
						stgs[stage].soldiers[p+1]=tsoldiers1;
					}
					if(stgs[stage].pattern[p+1][r][c]==-1)
					{
						tnosoldiers1.sol[tnosoldiers1.cnt].r=r;
						tnosoldiers1.sol[tnosoldiers1.cnt++].c=c;
						stgs[stage].nosoldiers[p+1]=tnosoldiers1;
					}
				}
			}
		if(mirror) p++;
	}
}

displaystage(int stage)
{
	int r,c,p;
	printf("\n No of patterns on stage %d is %d\n",stage,stgs[stage].cnt); 

	for(p=0;p<stgs[stage].cnt;p++)
	{
		printf("\n#####################\n");
		for(r=0;r<7;r++,printf("\n"))
			for(c=0;c<7;c++)
				printf("%1d |",stgs[stage].pattern[p][r][c]);

		printf("\nno of soldiers on pattern %d is %d\n",p,stgs[stage].soldiers[p].cnt);
		for(r=0;r<stgs[stage].soldiers[p].cnt;r++)
			printf("(%d %d) ",stgs[stage].soldiers[p].sol[r].r,stgs[stage].soldiers[p].sol[r].c);
		printf("\nno of NO soldiers on pattern %d is %d\n",p,stgs[stage].nosoldiers[p].cnt);
		for(r=0;r<stgs[stage].nosoldiers[p].cnt;r++)
			printf("(%d %d) ",stgs[stage].nosoldiers[p].sol[r].r,stgs[stage].nosoldiers[p].sol[r].c);

		printf("\n#####################\n");
	}
		
}

initgoals()
{
	int r,c,i;

	/* STAGE 1 */
        copypatterns(STAGE1,1,stage1_patterns,MIRROR);
        copypatterns(STAGE2,1,stage2_patterns,MIRROR);
        copypatterns(STAGE3,1,stage3_patterns,MIRROR);
        copypatterns(STAGE4,1,stage4_patterns,NOMIRROR);
        copypatterns(STAGE5,1,stage5_patterns,MIRROR);
        copypatterns(STAGE6,1,stage6_patterns,MIRROR);
        copypatterns(STAGE7,1,stage7_patterns,MIRROR);
        copypatterns(STAGE8,1,stage8_patterns,MIRROR);
        copypatterns(STAGE9,1,stage9_patterns,NOMIRROR);
        copypatterns(STAGE10,1,stage10_patterns,MIRROR);
        copypatterns(STAGE11,1,stage11_patterns,NOMIRROR);
        copypatterns(STAGE12,1,stage12_patterns,MIRROR);
        copypatterns(STAGE13,1,stage13_patterns,MIRROR);
        copypatterns(STAGE14,1,stage14_patterns,NOMIRROR);
        copypatterns(STAGE15,1,stage15_patterns,MIRROR);
        copypatterns(STAGE16,1,stage16_patterns,NOMIRROR);
        copypatterns(STAGE17,1,stage17_patterns,MIRROR);
        copypatterns(STAGE18,1,stage18_patterns,NOMIRROR);

}



initboard(board_t *brd,int positions[7][7])
{
	int r,c,coin;
	for(r=0;r<7;r++)
		for(c=0;c<7;c++)
		{
			CELL(r,c).solno=-1;
			if(positions[r][c]==1) coin='S';
			else if(positions[r][c]==9) coin='X';
			else if(positions[r][c]==0) coin=' ';
			else if(positions[r][c]==2) {coin='K'; brd->kpos.cord[0].r=r;brd->kpos.cord[0].c=c;}
			CELL(r,c).coin=coin;
			if(coin=='S')
			{
				brd->soldiers.sol[brd->soldiers.cnt].r=r;
				brd->soldiers.sol[brd->soldiers.cnt].c=c;
				CELL(r,c).solno=brd->soldiers.cnt++;
			}
		}
	brd->kpos.cnt=1;
	brd->stage=TOTSTAGE;
	initstaticboard(*brd);
}

displayboard(board_t brd)
{
	int r,c,i;

	printf("\n-----------------\n  ");
	for(c=0;c<7;printf("%2d",c++));

	printf("\n");

	for(r=0;r<7;r++)
	{
		printf("-----------------\n%2d",r);
		for(c=0;c<7;c++)
		{
			if(brd.cells[r][c].coin == 'X')
			{
				if(c==5) printf("| ");
				else printf("  ");
			}
			else 
			{
					printf("|%c",brd.cells[r][c].coin);
				/*if(brd.cells[r][c].coin=='K')
					printf("|%c",brd.cells[r][c].coin);
				else
					printf("|%1d",brd.cells[r][c].life);*/
			}

		}
		if(r<2||r>4) printf("\n");
		else printf("|\n");
	}
	printf("-----------------\n");
/*
	printf("kposcnt=%d,solcnt=%d stage=%d substg=%d\n",brd.kpos.cnt,brd.soldiers.cnt,brd.stage,brd.substage);
	for(i=0;i<brd.kpos.cnt;i++)
		printf("(%d,%d)",brd.kpos.cord[i].r,brd.kpos.cord[i].c);
	printf("\n"); */
	for(i=0;i<brd.soldiers.cnt;i++)
	{
	/*	printf(" (%d %d),",brd.soldiers.sol[i].r,brd.soldiers.sol[i].c); */
		/*if(brd.cells[brd.soldiers.sol[i].r][brd.soldiers.sol[i].c].coin !='S') exit(0);*/
	} /*
	printf("\n");
	if(brd.safe) printf("SAFE BOARD\n");
	if(brd.kcaptured) printf("K captured \n"); */
	fflush(stdout);
}

clearknight(board_t *brd)
{
	int i,j;

	/* Clean up existing Knight pos */
	for(i=0;i<brd->kpos.cnt;i++)
	{
		cord_t pos=brd->kpos.cord[i];
		if(CELL(pos.r,pos.c).coin !='K')
		{
			printf("fatal 3c\n\n");
			exit(0);
		}
		CELL(pos.r,pos.c).coin=' ';
	}
	brd->kpos.cnt=0;
}

spreadknight(board_t *brd)
{
	int i,j;

	/* Clean up existing Knight pos */
	for(i=0;i<brd->kpos.cnt;i++)
	{
		cord_t pos=brd->kpos.cord[i];
		if(CELL(pos.r,pos.c).coin !='K')
		{
			printf("fatal 3\n\n");
			exit(0);
		}
		CELL(pos.r,pos.c).coin=' ';
	}

	/* Spread it */
	pos_t tkpos= brd->kpos;
	brd->kpos.cnt=0;
	brd->kcaptured=0;
	for(i=0;i<tkpos.cnt;i++)
	{
		cord_t pos=tkpos.cord[i];
		for(j=0;j<sbrd.cells[pos.r][pos.c].kmoves.cnt;j++)
		{
			cord_t pos1=sbrd.cells[pos.r][pos.c].kmoves.cord[j];
			if(CELL(pos1.r,pos1.c).coin==' ')
			{
				CELL(pos1.r,pos1.c).coin='K';
				brd->kpos.cord[brd->kpos.cnt].r=pos1.r;
				brd->kpos.cord[brd->kpos.cnt].c=pos1.c;
				brd->kpos.cnt++;
			}
		}
	}
	if(brd->kpos.cnt==0) brd->kcaptured=1;
}
placesoldier(board_t *brd, cord_t src, cord_t des,int level)
{
	int r,c,m,i,j,rc=0;

	if(level >0 && CELL(des.r,des.c).coin == 'K')
		CELL(des.r,des.c).coin=' ';

	if(CELL(src.r,src.c).coin != 'S' || 
		CELL(des.r,des.c).coin != ' ' ||
		!INLIMITS((*brd),src.r,src.c)	||
		!INLIMITS((*brd),des.r,des.c))
	{
		printf("Invalid movement call %d,%d->%d,%d\n",src.r,src.c,des.r,des.c);
		displayboard(*brd);
		fflush(stdout);
		exit(0);
		return;
	}

	CELL(src.r,src.c).coin=' ';
	CELL(des.r,des.c).coin='S';
	brd->soldiers.sol[CELL(src.r,src.c).solno]=des;
	CELL(des.r,des.c).solno=CELL(src.r,src.c).solno;
	CELL(src.r,src.c).solno=-1;


	brd->safe=1;
	brd->kpos.cnt=0;
	for(r=0;r<7;r++)
		for(c=0;c<7;c++)
		{
			if(CELL(r,c).coin =='K')
			{
				brd->kpos.cord[brd->kpos.cnt].r=r;
				brd->kpos.cord[brd->kpos.cnt++].c=c;
			}

			if(CELL(r,c).coin != 'S') continue;

			for(i=0;i<sbrd.cells[r][c].cutposcnt;i++)
			{
				mvmt_t mvmt=sbrd.cells[r][c].cutpos[i];
				if(CELL(mvmt.src.r,mvmt.src.c).coin==' ' &&
					CELL(mvmt.des.r,mvmt.des.c).coin==' ')
						brd->safe=0;
				if(CELL(mvmt.src.r,mvmt.src.c).coin==' ' &&
					CELL(mvmt.des.r,mvmt.des.c).coin=='K')
				{
					brd->safe=0;
					if(CELL(r,c).coin=='S') rc=1;
				}
				if(CELL(mvmt.src.r,mvmt.src.c).coin=='K' &&
					CELL(mvmt.des.r,mvmt.des.c).coin==' ')
				{
					brd->safe=0;
					if(CELL(r,c).coin=='S') rc=1;
				}
				if(CELL(mvmt.src.r,mvmt.src.c).coin=='K' &&
					CELL(mvmt.des.r,mvmt.des.c).coin=='K')
				{
					brd->safe=0;
					if(CELL(r,c).coin=='S') rc=1;
				}
			}
		}

	return rc;
}

updateboardstage(board_t *brd)
{
	int s,p,i;
	brd->stage=TOTSTAGE;
	for(s=maxstage-1;s>=0;s--)
	{
		for(p=0;p<stgs[s].cnt;p++)
		{
			if(stgs[s].isachieved[p]) continue;
			soldiers_t tsoldiers=stgs[s].soldiers[p];
			for(i=0;i<tsoldiers.cnt;i++)
				if(brd->cells[tsoldiers.sol[i].r][tsoldiers.sol[i].c].coin!='S') break;
			if(i==tsoldiers.cnt)
			{
				int j;
				soldiers_t tnosoldiers=stgs[s].nosoldiers[p];
				for(j=0;j<tnosoldiers.cnt;j++)
					if(brd->cells[tnosoldiers.sol[j].r][tnosoldiers.sol[j].c].coin=='S') break;
				if(j==tnosoldiers.cnt)
				{
					brd->stage=s;
					brd->substage=p;
					return 0;
				}
			}
		}
	}
	return 1;
}

path_t
getbestpath(board_t brd,path_t path,int level)
{
	int s,m,i;
	static int cnt,cnt1;

	if(path.movcnt && isloop(path.moves[0])) return bpath;

	updateboardstage(&brd);
	brd.level=level;
	if((brd.kcaptured && !bbrd.kcaptured)|| (brd.kcaptured && bbrd.kcaptured && (path.movcnt < bpath.movcnt))) 
	{
		bpath=path;
		bbrd=brd;
		return bpath;
	}

	if(!bbrd.kcaptured && path.movcnt)
	if((!bpath.movcnt ) || (!bbrd.safe && brd.safe) || 
		(brd.safe && brd.stage < bbrd.stage) || 
		(brd.safe && brd.stage==bbrd.stage && path.movcnt<bpath.movcnt ) ||
		(brd.safe && brd.stage==bbrd.stage && path.movcnt==bpath.movcnt && path.vmovcnt>bpath.vmovcnt))
	{
		bpath=path;
		bbrd=brd;
	}

	if(level==MAXLEVEL) return bpath;

	for(s=0;s<brd.soldiers.cnt;s++)
	{
		for(m=0;m<3;m++,cnt++)
		{
			board_t tbrd=brd;
			path_t  tpath=path;
			mvmt_t nxtmove;

			nxtmove.src=tbrd.soldiers.sol[s];
			nxtmove.des.r=nxtmove.src.r+solmvmts[m].r;
			nxtmove.des.c=nxtmove.src.c+solmvmts[m].c;

			if(!INLIMITS(tbrd,nxtmove.des.r,nxtmove.des.c)) continue;
			if(tbrd.cells[nxtmove.des.r][nxtmove.des.c].coin == 'S') continue;
			if(tbrd.cells[nxtmove.des.r][nxtmove.des.c].coin == 'X') continue;
			if(tbrd.cells[nxtmove.des.r][nxtmove.des.c].coin == 'K' && level < 2) continue;
			if(tbrd.soldiers.map[s][nxtmove.des.r][nxtmove.des.c]=='@') continue;
			if(placesoldier(&tbrd,nxtmove.src,nxtmove.des,level))  continue;
			if(level <SECCHECKLEVEL || tbrd.kpos.cnt == 1)
				spreadknight(&tbrd);
			else
				clearknight(&tbrd);
			tpath.moves[tpath.movcnt++]=nxtmove;
			tbrd.soldiers.map[s][nxtmove.src.r][nxtmove.src.c]='@';
			tbrd.soldiers.map[s][nxtmove.des.r][nxtmove.des.c]='@';
			if(m==0) tpath.vmovcnt++;

			for(i=0;i<tbrd.soldiers.cnt;i++)
				if(tbrd.cells[tbrd.soldiers.sol[i].r][tbrd.soldiers.sol[i].c].coin!='S')
				{
					displayboard(tbrd);
					printf("%d %d->%d %d",nxtmove.src.r,nxtmove.src.c,
								nxtmove.des.r,nxtmove.des.c);
					printf("i=%d\n",i);
					printf("fatal error1\n");
					exit(0);
				}
			if(debug)
			{
				printf("\n###################################################################\n");
				printf("\nMoved %d from(%d,%d)->(%d,%d)\n",s,nxtmove.src.r,nxtmove.src.c,
								nxtmove.des.r,nxtmove.des.c);
				updateboardstage(&tbrd);
				displayboard(tbrd);
				printf("\n###################################################################\n");
			}
			getbestpath(tbrd,tpath,level+1);

			if(debug)
			{
				printf("\n###################################################################\n");
				printf("\nRever %d from(%d,%d)->(%d,%d)\n",s,nxtmove.src.r,nxtmove.src.c,
								nxtmove.des.r,nxtmove.des.c);
				updateboardstage(&brd);
				displayboard(brd);
				printf("\n###################################################################\n");
			}

		}
	}
	return bpath;
}

getsbestpath(board_t brd)
{
	path_t path;

	MAXLEVEL=4;
	memset(&bpath,'\0',sizeof(bpath));
	memset(&bbrd,'\0',sizeof(bbrd));
	memset(&path,'\0',sizeof(path));
	bbrd.stage=TOTSTAGE;
	getbestpath(brd,path,0);
	if(bbrd.stage==TOTSTAGE || bpath.movcnt==0)
	{
		board_t tbrd;
		path_t tpath;
		tbrd=bbrd;
		tpath=bpath;
		memset(&bpath,'\0',sizeof(bpath));
		memset(&bbrd,'\0',sizeof(bbrd));
		memset(&path,'\0',sizeof(path));
		MAXLEVEL=5;
		getbestpath(brd,path,0);
		if(bpath.movcnt==0)
		{
			getrandompath(brd);
			return;
		}
		if(bbrd.stage!=TOTSTAGE) return;
		bbrd=tbrd;
		bpath=tpath;
	}
}

int main_s(void)
{
	board_t brd;
	int i,r,c,count=0;
	char str[512];
	memset(&brd,'\0',sizeof(brd));
	memset(&sbrd,'\0',sizeof(sbrd));
	initboard(&brd,initpos);
	initgoals();
	updateboardstage(&brd);
	maxstage=7;
	displayboard(brd); /*
	for(i=0;i<TOTSTAGE;i++)
		displaystage(i);
*/

   if (oppfirst) {
      printf("Enter your Knight move : ");
		fflush(stdin);
		fflush(stdout);
		scanf("%s",str);
		string_place_knight (&brd,str);
   }

	while(1) 
	{
		getsbestpath(brd);
/*
		printf("\n#######\nBest solution no of moves=%d stg=%d substg=%d cap=%d\n",
				bpath.movcnt,bbrd.stage,bbrd.substage,bbrd.kcaptured);
		for(i=0;i<bpath.movcnt;i++)
		{
			printf("(%d,%d)->(%d,%d) ",bpath.moves[i].src.r,bpath.moves[i].src.c,
						bpath.moves[i].des.r,bpath.moves[i].des.c);
		}
*/
		placesoldier(&brd,bpath.moves[0].src,bpath.moves[0].des,0);
      printf ("My move : %d,%d;%d,%d;\n", bpath.moves[0].src.r, bpath.moves[0].src.c, bpath.moves[0].des.r, bpath.moves[0].des.c);
		actpath[actpathlen++] = bpath.moves[0];
		updateboardstage(&brd);
		if(brd.stage<TOTSTAGE)
		{
			if(brd.stage >= 7 && stgs[brd.stage].cnt > 1)
			{
				stgs[brd.stage].isachieved[0]=1;
				stgs[brd.stage].isachieved[1]=1;
			}
			else
				stgs[brd.stage].isachieved[brd.substage]=1;
		}
		displayboard(brd);
	/*	printf("count=%d\n",actpathlen); */
		displayopenstages();

		if( (maxstage < TOTSTAGE && stgs[maxstage-1].isachieved[0]==1 && stgs[maxstage-1].isachieved[1]==1) || actpathlen>50)
		{
			maxstage=TOTSTAGE;
		}

   a: printf("Enter your Knight move : ");
		fflush(stdin);
		fflush(stdout);
		scanf("%s",str);
		string_place_knight (&brd,str);

		/*scanf("%d,%d",&r,&c);
		if(!INLIMITS(brd,r,c) || brd.cells[r][c].coin!=' ')
		{
			printf("Ouside limit or the place is occupied\n");
			goto a;
		}

		cord_t pos=brd.kpos.cord[0];
		for(i=0;i<sbrd.cells[pos.r][pos.c].kmoves.cnt;i++)
		{
			cord_t pos1=sbrd.cells[pos.r][pos.c].kmoves.cord[i];
			if(pos1.r==r && pos1.c==c) break;
		}
		if(i==sbrd.cells[pos.r][pos.c].kmoves.cnt)
		{
			printf("You cannot place there\n");
			goto a;
		}

		brd.cells[brd.kpos.cord[0].r][brd.kpos.cord[0].c].coin=' ';
		brd.cells[r][c].coin='K';
		brd.kpos.cord[0].r=r;
		brd.kpos.cord[0].c=c; */
	}
}



getkillmove(board_t brd,path_t *bkpath)
{
        int i,j;
        memset(bkpath,'\0',sizeof(path_t));

        cord_t pos=brd.kpos.cord[0];
        for(i=0;i<sbrd.cells[pos.r][pos.c].kcutmoves.movcnt;i++)
        {
                mvmt_t jmp1=sbrd.cells[pos.r][pos.c].kcutmoves.moves[i];
                if(brd.cells[jmp1.src.r][jmp1.src.c].coin=='S' &&
                        brd.cells[jmp1.des.r][jmp1.des.c].coin==' ')
                {
                        bkpath->moves[bkpath->movcnt]=jmp1;
                        bkpath->movcnt=1;
                        for(j=0;j<sbrd.cells[jmp1.des.r][jmp1.des.c].kcutmoves.movcnt;j++)
                        {
                                mvmt_t jmp2=sbrd.cells[jmp1.des.r][jmp1.des.c].kcutmoves.moves[j];
                                if(brd.cells[jmp2.src.r][jmp2.src.c].coin=='S' &&
                                        brd.cells[jmp2.des.r][jmp2.des.c].coin==' ')
                                {
                                        bkpath->moves[bkpath->movcnt++]=jmp2;
                                        return  2;
                                }
                        }
                }
        }
        return bkpath->movcnt;
}


placeknight(board_t *brd,cord_t src, cord_t des)
{
   int r,c,i;

	if(brd->cells[src.r][src.c].coin!='K' ||
		brd->cells[des.r][des.c].coin!=' ')
	{
		fprintf(stderr, "Cannot place Knight at %d,%d from %d,%d\n",des.r,des.c, src.r, src.c);
		displayboard(*brd);
		exit(0);
	}
	brd->cells[src.r][src.c].coin=' ';
	brd->cells[des.r][des.c].coin='K';
	brd->kpos.cord[0]=des; 

   for(i=0;i<sbrd.cells[src.r][src.c].kcutmoves.movcnt;i++)
   {
      if(sbrd.cells[src.r][src.c].kcutmoves.moves[i].des.r == des.r &&
         sbrd.cells[src.r][src.c].kcutmoves.moves[i].des.c == des.c) 
      {
         cord_t cut = sbrd.cells[src.r][src.c].kcutmoves.moves[i].src;
         brd->cells[cut.r][cut.c].coin = ' '; 
         brd->soldiers.cnt = 0;

         for(r=0;r<7;r++)
            for(c=0;c<7;c++) {
               if(brd->cells[r][c].coin == 'S') {
                  brd->soldiers.sol[brd->soldiers.cnt].r=r;
                  brd->soldiers.sol[brd->soldiers.cnt].c=c;
                  brd->cells[r][c].solno=brd->soldiers.cnt++;
               }
            }
      }
   }

	return;
}


getbestkmove(board_t brd,path_t *killpath)
{
        int i,j,l1cnt = 0,s,m=0,l1,min;
        board_t l1brd[8];
        int safemoves[8] = {0},totalmoves[8] = {0};
        double ratio[8], temp = 0.0;
        mvmt_t l1moves[8];

        if(getkillmove(brd,killpath)) return 1;
        memset(killpath,'\0',sizeof(path_t));


        mvmt_t mvmt;mvmt.src=brd.kpos.cord[0];
        for(i=0;i<sbrd.cells[mvmt.src.r][mvmt.src.c].kmoves.cnt;i++)
        {
                mvmt.des=sbrd.cells[mvmt.src.r][mvmt.src.c].kmoves.cord[i];
		if(isloop(mvmt)) continue;
                if(brd.cells[mvmt.des.r][mvmt.des.c].coin==' ')
                {
                        l1brd[l1cnt]=brd;
                        l1moves[l1cnt]=mvmt;
                        placeknight(&l1brd[l1cnt],mvmt.src,mvmt.des);
                        l1cnt++;

                }
        }
        for(l1=0;l1<l1cnt;l1++)
        {
                for(s=0;s<brd.soldiers.cnt;s++)
                {
                        for(m=0;m<3;m++)
                        {
                                mvmt_t nxtmove;
                                board_t tbrd=l1brd[l1];
                                nxtmove.src=tbrd.soldiers.sol[s];
                                nxtmove.des.r=nxtmove.src.r+solmvmts[m].r;
                                nxtmove.des.c=nxtmove.src.c+solmvmts[m].c;
                                if(!INLIMITS(tbrd,nxtmove.des.r,nxtmove.des.c)) continue;
                                if(tbrd.cells[nxtmove.des.r][nxtmove.des.c].coin != ' ') continue;
                                totalmoves[l1]++;
                                if(!placesoldier(&tbrd,nxtmove.src,nxtmove.des,0))
                                        safemoves[l1]++;
                        }
                }
                ratio[l1] = (double)safemoves[l1] / (double)totalmoves[l1];
        }
        min = 0;
        temp = ratio[0];
    /*    printf(" %d %d %lf\n", safemoves[0], totalmoves[0], ratio[0]); */
        for (i = 1; i < l1cnt; i++)
        {
               /* printf(" %d %d %lf\n", safemoves[i], totalmoves[i], ratio[i]); */
                if (temp > ratio[i])
                {
                        min = i;
                        temp = ratio[i];
                }
        }
	if(ratio[min] < 0.5)
	{
        	killpath->moves[0]=l1moves[min];
        	killpath->movcnt=1;
	}
	else killpath->movcnt=0;
        return 1;
}


void displayboard_k(board_t brd)
{
	int r,c,i;

	printf("\n-----------------\n  ");
	for(c=0;c<7;printf("%2d",c++));

	printf("\n");

	for(r=0;r<7;r++)
	{
		printf("-----------------\n%2d",r);
		for(c=0;c<7;c++)
		{
			if(brd.cells[r][c].coin == 'X')
			{
				if(c==5) printf("| ");
				else printf("  ");
			}
			else 
			{
					printf("|%c",brd.cells[r][c].coin);
			}

		}
		if(r<2||r>4) printf("\n");
		else printf("|\n");
	}
	printf("-----------------\n");
	fflush(stdout);
}

int get_all_knight_moves (board_t* brd, int krow, int kcol, mvmt_t moves[], int level)
{
   int i = 0;
   mvmt_t nmvs[8];

   if (krow > 0 && brd->cells[krow-1][kcol].coin == ' ') {
      if ((level &&  get_all_knight_moves(brd, krow-1, kcol, nmvs, level-1) > 1) || !level) {
         moves[i].src.r = krow; moves[i].src.c = kcol;
         moves[i].des.r = krow-1; moves[i].des.c = kcol;
         if (!isloop(moves[i]))
         i++;
      }
   } 

   if (krow < 6 && brd->cells[krow+1][kcol].coin == ' ') {
      if ((level &&  get_all_knight_moves(brd, krow+1, kcol, nmvs, level-1) > 1) || !level) {
         moves[i].src.r = krow; moves[i].src.c = kcol;
         moves[i].des.r = krow+1; moves[i].des.c = kcol;
         if (!isloop(moves[i]))
         i++;
      }
   } 

   if (kcol > 0 && brd->cells[krow][kcol-1].coin == ' ') {
      if ((level &&  get_all_knight_moves(brd, krow, kcol-1, nmvs, level-1) > 1) || !level) {
         moves[i].src.r = krow; moves[i].src.c = kcol;
         moves[i].des.r = krow; moves[i].des.c = kcol-1;
         if (!isloop(moves[i]))
         i++;
      }
   }

   if (kcol < 6 && brd->cells[krow][kcol+1].coin == ' ') {
      if ((level &&  get_all_knight_moves(brd, krow, kcol+1, nmvs, level-1) > 1) || !level) {
         moves[i].src.r = krow; moves[i].src.c = kcol;
         moves[i].des.r = krow; moves[i].des.c = kcol+1;
         if (!isloop(moves[i]))
         i++;
      }
   }

   if (cross_points[krow][kcol] && krow > 0 && kcol > 0 && cross_points[krow-1][kcol-1]) {
      if (brd->cells[krow-1][kcol-1].coin == ' ') {
         if ((level &&  get_all_knight_moves(brd, krow-1, kcol-1, nmvs, level-1) > 1) || !level) {
            moves[i].src.r = krow; moves[i].src.c = kcol;
            moves[i].des.r = krow - 1; moves[i].des.c = kcol - 1; 
         if (!isloop(moves[i]))
            i++;
         }
      }
   }

   if (cross_points[krow][kcol] && krow < 6 && kcol > 0 && cross_points[krow+1][kcol-1]) {
      if (brd->cells[krow+1][kcol-1].coin == ' ') {
         if ((level &&  get_all_knight_moves(brd, krow+1, kcol-1, nmvs, level-1) > 1) || !level) {
            moves[i].src.r = krow; moves[i].src.c = kcol;
            moves[i].des.r = krow + 1; moves[i].des.c = kcol - 1; 
         if (!isloop(moves[i]))
            i++;
         }
      }
   }

   if (cross_points[krow][kcol] && krow <  6 && kcol < 6 && cross_points[krow+1][kcol+1]) {
      if (brd->cells[krow+1][kcol+1].coin == ' ') {
         if ((level &&  get_all_knight_moves(brd, krow+1, kcol+1, nmvs, level-1) > 1) || !level)  {
            moves[i].src.r = krow; moves[i].src.c = kcol;
            moves[i].des.r = krow + 1; moves[i].des.c = kcol + 1; 
         if (!isloop(moves[i]))
            i++;
         }
      }
   }

   if (cross_points[krow][kcol] && krow > 0 && kcol < 6 && cross_points[krow-1][kcol+1]) {
      if (brd->cells[krow-1][kcol+1].coin == ' ') {
         if ((level &&  get_all_knight_moves(brd, krow-1, kcol+1, nmvs, level-1) > 1) || !level) {
            moves[i].src.r = krow; moves[i].src.c = kcol;
            moves[i].des.r = krow - 1; moves[i].des.c = kcol + 1; 
         if (!isloop(moves[i]))
            i++;
         }
      }
   }

   level--;
   return i;
}

string_place_knight (board_t* brd, char* str)
{
   int slen;
   int pos, i;
   cord_t des;

   slen = strlen(str)+1;
   pos = slen/4;

   str = str+4; pos--;
   for (i = 0; i < pos; i++) {
      sscanf(str, "%d,%d", &des.r, &des.c);
      placeknight(brd, brd->kpos.cord[0], des);
      str = str+4;
   }
}

int main_k(void)
{
   board_t brd;
   char c;
   cord_t src, dest;
   path_t kpath;
   mvmt_t nmvs[8];
   int ct, li = 0;

   /* init board */
   memset(&brd, 0, sizeof(brd));
   initboard(&brd, initpos);
   /*
   brd.kpos.cnt = 1;
   brd.kpos.cord[0].r = 4; brd.kpos.cord[0].r = 3; */


   if (oppfirst) {
      displayboard_k(brd);

      printf ("Enter your soldier move : "); 
      scanf ("%d,%d;%d,%d;", &src.r, &src.c, &dest.r, &dest.c); 

      placesoldier(&brd, src, dest, 0);
   }

   displayboard_k(brd); 

   while (1) {
      memset(&kpath, 0, sizeof(path_t));
      getbestkmove (brd, &kpath);
      printf ("My move : ");
      if (kpath.movcnt > 0) {
         int i;
         printf ("%d,%d;", brd.kpos.cord[0].r, brd.kpos.cord[0].c);
         for ( i = 0 ; i < kpath.movcnt; i++) {
            placeknight(&brd, brd.kpos.cord[0], kpath.moves[i].des);
            printf("%d,%d;", kpath.moves[i].des.r, kpath.moves[i].des.c);
         }
         printf ("\n");
         actpath[actpathlen++] = kpath.moves[0];
      } else {
         memset(nmvs, 0, sizeof(nmvs));
         printf ("%d,%d;", brd.kpos.cord[0].r, brd.kpos.cord[0].c);
         ct = get_all_knight_moves (&brd, brd.kpos.cord[0].r, brd.kpos.cord[0].c, nmvs, 1);
         if (ct < li) li = 0;
         placeknight(&brd, nmvs[li].src, nmvs[li].des);
         printf ("%d,%d;", nmvs[li].des.r, nmvs[li].des.c); li++;
         actpath[actpathlen++] = nmvs[0];
      }
      
      displayboard_k(brd); 

      printf ("Enter your soldier move : "); 
      scanf ("%d,%d;%d,%d;", &src.r, &src.c, &dest.r, &dest.c); 

      placesoldier(&brd, src, dest, 0);
   }

   return 1;
}


int main(void)
{
   char c;

   fflush(stdin);
   fflush(stdout);
   printf ("Do you want to play first (y/n) ?"); fflush(stdout);scanf("%c", &c); 

   if (c == 'y') oppfirst = 1;

   fflush(stdin);
   fflush(stdout);
   printf ("Do you want to play as soldier (y/n) ?"); fflush(stdout);scanf ("%c", &c);

   printf ("\n Base coordinate used in this program is 0,0 \n"); fflush(stdout);

   if (c == 'y') { printf ("Computer : Knight \nYou : Soldier \n"); main_k(); }
   else { printf ("Computer : Soldier \nYou : Knight\n"); main_s(); }

   return 0;
}
