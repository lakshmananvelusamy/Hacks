typedef struct
{
	int r,c;
}cord_t;

typedef struct
{
	cord_t cord[33];
	int cnt;
}pos_t;

typedef struct
{
	cord_t src,des;
}mvmt_t;

typedef struct
{
	mvmt_t moves[10];
	int movcnt,vmovcnt,killcnt;
}path_t;

typedef struct
{
	path_t path[10];
	int numpaths;
}pathmap_t;

typedef struct
{
	char coin;
	mvmt_t cutpos[4];
	int cutposcnt;
	pos_t kmoves; /* mvmt_t.des will be updated */
	path_t kcutmoves; /* mvmt_t.src will be soldier postion mvmt_t.des will be emtpy cell */
}scell_t;

typedef struct
{
	scell_t cells[7][7];
}sboard_t;

typedef struct 
{
	char coin;
	int solno;
}cell_t;

typedef struct
{
	cord_t sol[13];
	int map[13][7][7];
	int cnt;
}soldiers_t;

typedef struct 
{
	cell_t cells[7][7];
	soldiers_t soldiers;
	pos_t kpos;
	int safe,value,level,stage,substage,kcaptured;
}board_t;

sboard_t sbrd;

#define INLIMITS(brd,r,c) (r>-1 && r<7 && c>-1 && c<7 && brd.cells[r][c].coin!='X')
#define CELL(r,c)     brd->cells[r][c]

int cross_points[7][7] = {
    { 0, 0, 1, 0, 1, 0, 0},
    { 0, 0, 0, 1, 0, 0, 0},
    { 1, 0, 1, 0, 1, 0, 1},
    { 0, 1, 0, 1, 0, 1, 0},
    { 1, 0, 1, 0, 1, 0, 1},
    { 0, 0, 0, 1, 0, 0, 0},
    { 0, 0, 1, 0, 1, 0, 0}
};

displaystaticboard()
{
	int r,c,i;

	for(r=0;r<7;r++)
		for(c=0;c<7;c++)
		{
			if(sbrd.cells[r][c].cutposcnt)
			{
				printf("\n\n Cutpos for [%d][%d] ",r,c);

				for(i=0;i<sbrd.cells[r][c].cutposcnt;i++)
				{
					mvmt_t t=sbrd.cells[r][c].cutpos[i];
					printf("(%d %d, %d %d)  ",t.src.r,t.src.c,t.des.r,t.des.c);
				}

			}
			if(sbrd.cells[r][c].kcutmoves.movcnt)
			{
				printf("\n\n kcutmovecnt for [%d][%d] ",r,c);

				for(i=0;i<sbrd.cells[r][c].kcutmoves.movcnt;i++)
				{
					mvmt_t t=sbrd.cells[r][c].kcutmoves.moves[i];
					printf("(%d %d, %d %d)  ",t.src.r,t.src.c,t.des.r,t.des.c);
				}

			}
			if(sbrd.cells[r][c].kmoves.cnt)
			{
				printf("\n\n kmoves for [%d][%d] ",r,c);

				for(i=0;i<sbrd.cells[r][c].kmoves.cnt;i++)
				{
					cord_t t=sbrd.cells[r][c].kmoves.cord[i];
					printf("(%d %d)  ",t.r,t.c);
				}
			}
		}
			

	printf("\n");
}

initstaticboard(board_t brd)
{
	int r,c,coin,p;
	cord_t movpos[]={ {0,-1},{0,1},{-1,0},{1,0},{-1,-1},{1,1},{1,-1},{-1,1} };

	for(r=0;r<7;r++)
		for(c=0;c<7;c++)
			sbrd.cells[r][c].coin=brd.cells[r][c].coin;


	for(r=0;r<7;r++)
		for(c=0;c<7;c++)
		{
			if(sbrd.cells[r][c].coin=='X') continue;
			for(p=0;p<8;p++)
			{
				cord_t nxtmove={r,c};
				mvmt_t nxtcutmove={{r,c},{r,c}};
				if(p==4 && !cross_points[r][c]) break;
				nxtmove.r+=movpos[p].r; nxtmove.c+=movpos[p].c;
				if(INLIMITS(brd,nxtmove.r,nxtmove.c))
				{
					sbrd.cells[r][c].kmoves.cord[sbrd.cells[r][c].kmoves.cnt++]=nxtmove;
					nxtcutmove.src=nxtmove;
					nxtcutmove.des.r+=movpos[p].r*2; nxtcutmove.des.c+=movpos[p].c*2;
					if(INLIMITS(brd,nxtcutmove.des.r,nxtcutmove.des.c))
						sbrd.cells[r][c].kcutmoves.moves[sbrd.cells[r][c].kcutmoves.movcnt++]=\
							nxtcutmove;
				}
			}
			for(p=0;p<8;p+=2)
			{
				mvmt_t mvmt;
				if(p==4 && !cross_points[r][c]) break;
				mvmt.src.r=r+movpos[p].r;
				mvmt.src.c=c+movpos[p].c;
				mvmt.des.r=r+movpos[p+1].r;
				mvmt.des.c=c+movpos[p+1].c;
				if(INLIMITS(brd,mvmt.src.r,mvmt.src.c) && INLIMITS(brd,mvmt.des.r,mvmt.des.c))
					sbrd.cells[r][c].cutpos[sbrd.cells[r][c].cutposcnt++]=mvmt;
			}
		}
}
