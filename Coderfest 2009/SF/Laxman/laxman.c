#include <stdio.h>
#include <stdlib.h>
#define TRUE 1
#define FALSE 0
#define MAXCELLS (16*16)
#define MAXBRDSZ (16)
#define NOTRAVEL '_'
#define NOOBJECT '0'

int brdsz; /* board size */
int expected_max, gflag;
float factor;
struct cord
{
	int r,c;
};
struct adjnode_t
{
	int cnt;
	struct cord adj[256];
};
struct cell_t
{
	char value[2];
	char travel[2];
	struct adjnode_t adj_c,adj_s;
};

struct chain_t
{
	char type;  		/* SHAPE CRAWL / COLOR CRAWL */
	char value;		/* VALUE OF THE SHAPE/COLOR */
	struct cord path[MAXCELLS];
	char pathmap[16][16];
	int pathcnt;
};

struct board_t
{
	struct cell_t cells[MAXBRDSZ][MAXBRDSZ];
	int sz;
};

static struct chain_t best_longpath;

void get_paths(struct board_t *brd,int r, int c,struct chain_t sp, int type, int itr)
{
	int i, j;
	if (gflag) return;
	for(i=0;i<itr;i++)
	{
		if ( gflag ) return;
		if(sp.pathmap[r][c] != '@')
		{
			sp.pathmap[r][c]='@';
			sp.path[sp.pathcnt].r = r;
			sp.path[sp.pathcnt++].c = c;
			if (type == 1)
			{
				get_paths(brd, brd->cells[r][c].adj_c.adj[i].r, brd->cells[r][c].adj_c.adj[i].c, sp, type, brd->cells[brd->cells[r][c].adj_c.adj[i].r][brd->cells[r][c].adj_c.adj[i].c].adj_c.cnt);
			}
			else
			{
				get_paths(brd, brd->cells[r][c].adj_s.adj[i].r, brd->cells[r][c].adj_s.adj[i].c, sp, type, brd->cells[brd->cells[r][c].adj_s.adj[i].r][brd->cells[r][c].adj_s.adj[i].c].adj_s.cnt);
			}
			sp.pathcnt--;
			sp.pathmap[r][c]=' ';
		}
	}
	/*for(j = 0; j < sp.pathcnt; j++)
	{
		printf("(%d, %d) -> ",sp.path[j].r, sp.path[j].c);
	}
	printf("\n");*/
	if(sp.pathcnt > best_longpath.pathcnt)
		best_longpath=sp;
	if (best_longpath.pathcnt >= expected_max)
		gflag = 1;

}

int read_board_file (struct board_t *brd, char *file)
{
	FILE *fptr = fopen(file, "r\0");
	int r,c;

	if (!fptr) return -2;

	brd->sz=brdsz * brdsz;

	for (r = 0; r < brdsz; r++)
	{
		for (c = 0; c < brdsz; c++)
		{
			if (fscanf (fptr, "%c,%c;", &brd->cells[r][c].value[0],&brd->cells[r][c].value[1]) != 2) return -3;
			brd->cells[r][c].travel[0]=brd->cells[r][c].travel[1]=NOTRAVEL;
		/*	if (brd->cells[r][c].value[0] == '0') brd->cells[r][c].value[0] = NOOBJECT, brd->cells[r][c].value[1] = NOOBJECT;*/
		}
		fscanf (fptr, "\n");
	}

	return 0;
}

/*void display_board (struct board_t* brd)
{
	int r, c;

	for (r = 0; r < brdsz; r++)
	{
		for (c = 0; c < brdsz; c++)
		{
			printf("| %c,%c,%c %c;", brd->cells[r][c].value[0],brd->cells[r][c].value[1],
						brd->cells[r][c].travel[0],brd->cells[r][c].travel[1]);
		}
		printf("\n");
	}

}*/



#define NEW_BORN	0
#define LIVE		1
#define DEAD		2

#define RIGHT 		0		
#define DOWN		1

#define SHAPE		0
#define COLOR		1

struct spider_t
{
	char type;  		/* SHAPE CRAWL / COLOR CRAWL */
	char value;		/* VALUE OF THE SHAPE/COLOR */
	int dir;   		/* RIGHT/DOWN */
	int dirchgcnt;
	struct cord path;
};

struct spiders_t
{
	struct spider_t spiders[100000];
	int cnt;
};


#define INLIMITS(cord) (cord.c < brdsz && cord.r < brdsz && cord.c >= 0 && cord.r >= 0)
#define ISMATCH(cord,spider) (spider.value ==brd->cells[cord.r][cord.c].value[spider.type])
#define ISSPACE(cord) (brd->cells[cord.r][cord.c].value[0] == NOOBJECT)
#define UPDATE_NEWBORN(spider,typ,row,col) {\
					spider.type=typ;\
					spider.value=brd->cells[row][col].value[typ];\
					spider.dir=-1;spider.dirchgcnt=0;\
					spider.path.r=row;spider.path.c=col;\
					}


struct index
{
	int link_index[256];
	int cn;
};

struct node
{
	struct cord name;
	struct cord links[256];
	struct index indexes[256];	
	int count;
	char type;
};

struct cord dir[4]={ {0,1}, {1,0},{0,-1},{-1,0} }; 

int isreverse(int d1, int d2)
{
	if((d1 == 0 && d2 == 2) || (d1 == 2 && d2 == 0) || (d1 == 1 && d2 == 3) || (d1 == 3 && d2 == 1))
		return 1;
	else return 0;
}
	
void get_chains(struct board_t *brd)
{
	struct spiders_t live,dead;
	struct spider_t spider;
	struct chain_t ch;
	struct node n[256];
	struct board_t b;
	int i, j, k, l, ii;
	int best_index;
	int color, shape, x, s, d, max = 0, mindex = 0;
	color = brdsz / 2 - 1;
	shape = brdsz / 2 + 1;
	b = *brd;
	memset(&best_longpath, '\0', sizeof(struct chain_t));
	memset(&ch, '\0', sizeof(struct chain_t));
	memset(&n[0], '\0', 256 * sizeof(struct node));
	for (i = 0; i < brdsz; i++)
	{
		for (j = 0; j < brdsz; j++)
		{
			if (b.cells[i][j].value[0] == NOOBJECT) continue;
			x = b.cells[i][j].value[0] - 'R';
			n[x].links[n[x].count].r = i;
			n[x].links[n[x].count].c = j;
			n[x].type = SHAPE;
			n[x].count++;
			x = b.cells[i][j].value[1] - 'E' + shape;
			n[x].links[n[x].count].r = i;
			n[x].links[n[x].count].c = j;
			n[x].type = COLOR;
			n[x].count++;
		}
	}
	for (i = 0; i < shape + color; i++)
	{
		for (j = 0; j < n[i].count; j++)
		{
			UPDATE_NEWBORN(spider,n[i].type ,n[i].links[j].r,n[i].links[j].c); live.spiders[0]=spider;
			n[i].indexes[j].cn = 0;
			live.cnt=1;
			while(live.cnt)
			{
				struct spiders_t tlive,tnew;
				tlive=live;

				memset(&tnew,'\0',sizeof(struct spiders_t));
				memset(&live,'\0',sizeof(struct spiders_t));

				for(s=0; s<tlive.cnt; s++) /* Move each spider one step */
				{
					int mvcnt=0;
					for(d=0; d<4; d++) /* both the directions RIGHT and DOWN */
					{
						struct cord nxtcord;
						spider=tlive.spiders[s];
						nxtcord = spider.path;
						if (isreverse(spider.dir, d))	continue;
						
						nxtcord.c+=dir[d].c; nxtcord.r+=dir[d].r;
						
						if( INLIMITS(nxtcord) && spider.dirchgcnt <= 2 )	
						{
							if (d != spider.dir && spider.dir != -1)
							{
								spider.dirchgcnt++;
								spider.dir = d;
							}
							if (spider.dir == -1)
								spider.dir = d;

							if (ISMATCH(nxtcord,spider) && spider.dirchgcnt <= 2)
							{
								for (l = 0; l < n[i].count; l++)
								{
									if (nxtcord.r == n[i].links[l].r && nxtcord.c == n[i].links[l].c)
									{
										int flag = 0;
										for (k = 0; k < n[i].indexes[j].cn; k++)
										{
											if (l == n[i].indexes[j].link_index[k]) {
											 flag = 1; break;}
										}
										if (!flag)
											n[i].indexes[j].link_index[n[i].indexes[j].cn++] = l;
										break;
									}	
								}
							}
							else if (ISSPACE(nxtcord))
							{
								spider.path = nxtcord;		
								live.spiders[live.cnt++]=spider;
							}
						} 
					}
				}
			}
		}
	}
        for (i = 0; i < brdsz; i++)
        {
                for (j = 0; j < brdsz; j++)
                {
                        b.cells[i][j].adj_c.cnt = 0;
                        b.cells[i][j].adj_s.cnt = 0;
		}
	}
	for (i = 0; i < shape + color; i++)
	{
		x = n[i].count;	
		for (j = 0; j < n[i].count; j++)
		{
			if (n[i].indexes[j].cn == 0)
				x--; 
			for (k = 0; k < n[i].indexes[j].cn; k++)
			{
				
				if (i < shape) {
				b.cells[n[i].links[j].r][n[i].links[j].c].adj_s.adj[b.cells[n[i].links[j].r][n[i].links[j].c].adj_s.cnt].r = n[i].links[n[i].indexes[j].link_index[k]].r;
				b.cells[n[i].links[j].r][n[i].links[j].c].adj_s.adj[b.cells[n[i].links[j].r][n[i].links[j].c].adj_s.cnt++].c = n[i].links[n[i].indexes[j].link_index[k]].c;
				}
				else {
				b.cells[n[i].links[j].r][n[i].links[j].c].adj_c.adj[b.cells[n[i].links[j].r][n[i].links[j].c].adj_c.cnt].r = n[i].links[n[i].indexes[j].link_index[k]].r;
				b.cells[n[i].links[j].r][n[i].links[j].c].adj_c.adj[b.cells[n[i].links[j].r][n[i].links[j].c].adj_c.cnt++].c = n[i].links[n[i].indexes[j].link_index[k]].c;
				}
			}
			if (x > max)
			{
				max = x;
				best_index = i;
			}
		}
	}
	
	for(i=0;i<n[best_index].count;i++)
		{
			if (n[best_index].indexes[i].cn == 0)
				continue;
			expected_max = (int)(max * factor);
			memset(&ch, '\0', sizeof(struct chain_t));
                        gflag = 0;
			get_paths(&b,n[best_index].links[i].r ,n[best_index].links[i].c , ch, best_index < shape ? 0 : 1, n[best_index].indexes[i].cn);
			if (best_longpath.pathcnt >= expected_max)
				return;
		}

		
	/*for (i = 0; i < brdsz; i++)
	{
		for (j = 0; j < brdsz; j++)
		{
			if (b.cells[i][j].value[0] == NOOBJECT)
				continue;
			memset(&ch, '\0', sizeof(struct chain_t));
			gflag = 0;
			expected_max = max;
			get_paths(&b, i, j, ch, 1, b.cells[i][j].adj_c.cnt);
			if (best_longpath.pathcnt == max)
				return;
		}
	}  
	for (i = 0; i < brdsz; i++)
	{
		for (j = 0; j < brdsz; j++)
		{
			if (b.cells[i][j].value[0] == NOOBJECT)
				continue;
			memset(&ch, '\0', sizeof(struct chain_t));
               		gflag = 0;
                        expected_max = max;
			get_paths(&b, i, j, ch, 0, b.cells[i][j].adj_s.cnt);
                        if (best_longpath.pathcnt == max)
                                return;
		}
	} */ 

/*        for (ii = 0; ii < shape + color; ii++)
        {
		totpath = 0;
		memset(&pt[0], '\0', 90024 * sizeof(struct path_t));
                for (j = 0; j < n[ii].count; j++)
                {
			pt[totpath].pat.path[pt[totpath].pathcnt++] = n[ii].links[j];
			pt[totpath].backref = j;
			pt[totpath].type = n[ii].type;
			pt[totpath].pathcnt = 1;
			pt[totpath].growing = TRUE;
			pt[totpath].isvalid = TRUE;
			totpath++;
			growing_cnt ++;

                }
	for (i = 0; i < totpath; i++)
	{
		if (pt[i].growing == FALSE || pt[i].isvalid == FALSE)
			continue;
		if (pt[i].type == SHAPE)
			s = b.cells[pt[i].pat.path[pt[i].pathcnt-1].r][pt[i].pat.path[pt[i].pathcnt-1].c].value[0] - 'R';
		else 
			s = b.cells[pt[i].pat.path[pt[i].pathcnt-1].r][pt[i].pat.path[pt[i].pathcnt-1].c].value[1] - 'E' + shape;
		if (n[s].indexes[pt[i].backref].cn == 0)
		{
			pt[i].growing = FALSE;
			growing_cnt --;
		}
		for (j = 0; j < n[s].indexes[pt[i].backref].cn; j++)
		{
			int flag = 0;
			for (l = 0; l < pt[i].pathcnt; l++)
			{
				if (pt[i].pat.path[l].r == n[s].links[n[s].indexes[pt[i].backref].link_index[j]].r &&
					pt[i].pat.path[l].c == n[s].links[n[s].indexes[pt[i].backref].link_index[j]].c)
				{
					flag = 1;
				}
			}
			if (flag) {
				if (pt[i].growing == TRUE)
				{
					pt[i].growing = FALSE;
					growing_cnt --;
				}
				continue;
			}
			if (pt[i].isvalid == TRUE) {
				pt[i].isvalid = FALSE; pt[i].growing = FALSE;
				growing_cnt --;
			}
			pt[totpath].pat = pt[i].pat;
			pt[totpath].pat.path[pt[i].pathcnt] = n[s].links[n[s].indexes[pt[i].backref].link_index[j]];
			pt[totpath].backref = n[s].indexes[pt[i].backref].link_index[j];
			pt[totpath].type = n[s].type;
			pt[totpath].pathcnt = pt[i].pathcnt + 1;
			pt[totpath].growing = TRUE;
			pt[totpath].isvalid = TRUE;
			totpath++;
			growing_cnt ++;
		}
	}
*/	
/*	for (i = 0; i < totpath; i++)
	{
		if (pt[i].isvalid == FALSE) continue;
		for (j = 0; j < pt[i].pathcnt; j++)
		{
			printf("(%d, %d)[%c] -> ", pt[i].pat.path[j].r, pt[i].pat.path[j].c, pt[i].type == SHAPE ? 's': 'c');
		}
		printf("\n");
	} 
	for (i = totpath - 1; i >= 0; i--)
	{
		int tmp[16][16];
		memset(&tmp[0], '\0', sizeof(int)*16*16);
		if (pt[i].isvalid == FALSE || pt[i].pathcnt == 1) continue;
		if (pt[i].pathcnt == brdsz)
		{
			paths.path[0] = pt[i].pat;
			paths.length[0] = pt[i].pathcnt;
			paths.totalpaths = 1;
		}
		paths.path[paths.totalpaths] = pt[i].pat;
		paths.length[paths.totalpaths] = pt[i].pathcnt;
		paths.totalpaths++;
		for (k = 0; k < pt[i].pathcnt; k++)
			tmp[pt[i].pat.path[k].r][pt[i].pat.path[k].c] = 1;	
		for (j = i - 1; j >= 0; j--)
		{
			if (pt[j].pathcnt == 1)
			{
				pt[j].isvalid = FALSE;
				continue;
			}
			if (pt[j].isvalid == FALSE)
				continue;
			for (k = 0; k < pt[j].pathcnt; k++)
				if (tmp[pt[j].pat.path[k].r][pt[j].pat.path[k].c] == 0)	break;	
			if (k == pt[j].pathcnt)
				pt[j].isvalid = FALSE;
				
		}
	}
	for (i = 0; i < totpath; i++)
	{
		if (pt[i].isvalid == FALSE || pt[i].pathcnt == 1) continue;
		for (j = 0; j < pt[i].pathcnt; j++)
		{
			printf("(%d, %d)[%c] -> ", pt[i].pat.path[j].r, pt[i].pat.path[j].c, pt[i].type == SHAPE ? 's': 'c');
		}
		printf("\n");
	}
	printf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n");
	}*/
}
struct board_t remove_chain(struct board_t brd)
{
        struct board_t board = brd;
        int i;
        for(i=0; i<best_longpath.pathcnt; i++)
        {
              printf("%d,%d;",best_longpath.path[i].r+1, best_longpath.path[i].c+1);
              board.cells[best_longpath.path[i].r][best_longpath.path[i].c].value[0] = NOOBJECT;
              board.cells[best_longpath.path[i].r][best_longpath.path[i].c].value[1] = NOOBJECT;
        }
	printf("\n");
        return board;
}

struct board_t core_logic(struct board_t brd)
{
        struct board_t board = brd;
        struct board_t t_board = brd;
        struct board_t temp;
        int i;
        temp = board;
        get_chains(&board);
        board = temp;
        if(best_longpath.pathcnt == 0) exit(0);
/*        initial_high_count=0;
        printf("\ntotal paths: %d,remove_index: %d length:%d\n",saved.totalpaths,remove_index,saved.length[remove_index]);
        for(i=0; i<saved.totalpaths; i++)
                printf("\nAll lengths:%d",saved.length[i]);
        for(chains=0;chains<saved.totalpaths;chains++)
        {
                t_board = remove_chain(board,saved,chains,'t');
                temp = t_board;
                t_saved = get_chains(&t_board);
                t_board = temp;
                if(t_saved.totalpaths==0) { board = remove_chain(board,saved,chains,'p'); return board; }
                t_high_chain_count = get_highest(t_saved);
                printf("\n chain count: %d highest: %d\n",chains,t_high_chain_count);
                if(t_high_chain_count+saved.length[chains] >= initial_high_count)
                {
                        initial_high_count=t_high_chain_count+saved.length[chains];
                        remove_index=chains;
                        printf("\n remove index: %d", remove_index);
                }
        }
        printf("\ntotal paths: %d,remove_index: %d length:%d\n",saved.totalpaths,remove_index,saved.length[remove_index]); */
        board = remove_chain(board);
        return board;
}

int empty(struct board_t brd)
{
        int r, c;

        for (r = 0; r < brdsz; r++)
        {
                for (c = 0; c < brdsz; c++)
                {
                        if(brd.cells[r][c].value[0]!= '0') return 0;
                }

        }
        return 1;
}



int main(int argc, char* argv[])
{
	struct board_t brd, board;
	memset(&brd,'\0',sizeof(struct board_t));

	if (argc != 3)
	{
		fprintf (stderr, "Usage : %s <board_size> <board_file>\n", argv[0]);
		return -1;
	}

	brdsz = atoi(argv[1]); 

	if (read_board_file(&brd, argv[2]))
	{
		fprintf (stderr, "Error reading the file %s\n", argv[2]);
		return -2;
	}

	board = brd;
	if (brdsz == 6)
		factor = 1.0;
	else if(brdsz == 8)
		factor = 0.6;
	else if(brdsz == 10)
		factor = 0.8;
	else if (brdsz == 12)
		factor = 0.7;
	else if (brdsz == 14)
		factor = 0.6;
	else if (brdsz == 16)
		factor = 0.5;
        while(!empty(board))
        {
                board = core_logic(board);
	    }
	exit(0);
}





