#include<string.h>
#include<stdio.h>
#define DICT_SIZE 101198
FILE *ip, *op, *ip1;
char words[DICT_SIZE][32];
int score[DICT_SIZE];
char out[8], out1[8];
char board[15][15]={0}, set1[8] = {0}, set2[8]={0}, set3[8]={0};
char board_1[15][15]={0},board_2[15][15]={0},board_3[15][15]={0};
int weight=0, prev_weight=0;
char word_set[3][15]={0},prev_word_set[3][15]={0};
int pos[3][4]={0},fin_pos[3][4]={0};
long indexes[26];
char clist[14000][8];
int c_cnt,weight1,weight2,weight3;
char sear[]="cat wordlist.txt | grep -q -e ";

struct SC
{
        char s[13699][8];
        int count;
}sc1, sc2, sc3;
        char clist[14000][8];
        int c_cnt, flag = 1;

char seen_word[1000][16];
int seen_cnt;

int grep1(char *s)
{
	int i;
	for (i = 0; i < seen_cnt; i++)
		if (strcmp(s, seen_word[i]) == 0)
			return 0;
	for ( i = 0; i < DICT_SIZE; i++)
		if (strstr(words[i], s))
		{
			strcpy(seen_word[seen_cnt], s);
			seen_cnt++;
			return 0;
		}
	return 1;
}
        int check(char *s)
        {
                int i, len1;
                len1 = strlen(s);
                for (i = 0; i < c_cnt; i++)
                {
                        if( len1 == strlen(clist[i]) )
                                if (strcmp(s, clist[i]) == 0)
                                        return 1;
                }
                return 0;
        }
int grep(char *s)
{
int i, j = 0;
int len, k;
len = strlen(s);
	for ( i = 0; i < DICT_SIZE; i++)
	{
		int len1;
		k = 0;
		len1 = strlen(words[i]);
		for (j = 0; j < len1; j++)
		{
			if (words[i][j] == s[k])
				k++;
		}
		if (k == len)
		{
			return 0;
		}
	}
	return 1;
}	

        void permute(char *str, const int strt, const int len, struct SC *sc) {
        int i = strt;
        int j;
        char temp;

        for ( i = strt; i < len-1; ++i ) {
        for ( j = i+1; j < len; ++j ) {
        temp = str[i], str[i] = str[j], str[j] = temp;
        permute(str, i+1, len, sc);
        temp = str[i], str[i] = str[j], str[j] = temp;
        }
        }
                strcpy(clist[c_cnt], str);
                if (check(str) == 0 && grep(str) == 0)
                {
                        c_cnt++;
                        strcpy(sc->s[sc->count++], str);
                }


        }

        void recurr_search_next_letter(char w[8], int size, int teller,
        int lengt, char s[8], struct SC*);
        void Combinations(char string[],int stack[],int combLength,int leftIndex, struct SC * sc);

        void ComputeCombinations(char string[], struct SC * sc)
        {
            int stringLength = 0;
            int *stack;
            int index = 0;
            int combLength = 0;
            int combIndex = 0;
            stringLength = (int)strlen(string);

            stack = (int *)calloc(stringLength,sizeof(int));
            if(!stack)
            {
                printf("\nmemory allocation for stack failed exiting ..\n\n");
                exit(1);
            }

            for(combIndex = 1;combIndex <= stringLength;combIndex++)
            {
                /*printf("\n %d letter combinations ...\n\n",combIndex);*/
                for(index = 0; index < stringLength;index++)
                {
                    stack[0] = index;
                    Combinations(string,stack,combIndex,index, sc);
                }
            }

            return;
        }
        void Combinations(char string[],int stack[],int combLength,int leftIndex, struct SC * sc)
        {
                char current[8], w[8];
            static int stringLength, riteIndex;
            static int level = 0;
            if (flag)
            {
               stringLength = riteIndex = (int)strlen(string);
               flag = 0;
		level = 0;
            }
            /*Initialiaze the left index*/
            if(level == 0)
            {
                leftIndex = stack[0]+1;
            }

            /*if the current depth of tree = combinations length then print & return*/
            if(level == combLength - 1)
            {
                        int i;
                        for(i = 0; i < combLength;i++)
                        {
                                w[i] = string[stack[i]];
                        }
                        w[i] = '\0';
                        strcpy(clist[c_cnt], w);
                        if (check(w) == 0 && grep(w) == 0)
                        {
                                c_cnt++;
                    		strcpy(current, w);
	                        strcpy(sc->s[sc->count++], w);
                   		permute(current, 0, strlen(current), sc);
			}
                        return;
            }
            level++;

            /*recurse from current index to end of string*/
            for(;leftIndex < riteIndex;)
            {
                stack[level] = leftIndex;
                leftIndex++;
                Combinations(string,stack,combLength,leftIndex, sc);
            }
            level--;
        }

        void DisplayStack(int stack[],int stringLength,char string[])
        {
            int index = 0;
            for(index = 0; index < stringLength;index++)
            {
                printf(" %c ",string[stack[index]]);
            }
            printf("\n");
        }

void assign( struct SC *, char *, int);
void sort(char *);

void read_board(char *file)
{
        int i, j, k, tmp, flag = 1, rc;
	char dir, word[15];
        do
        {
                rc = fscanf(ip, "%d", &tmp);
                if(rc > 0)
                {
                        i = tmp;
                        fscanf(ip, ",%d,%c,%s",&j, &dir, word);
                        for(k = 0; k < strlen(word); k++)
                                word[k] = tolower(word[k]);
                        dir = tolower(dir);
                        i--; j--;
                        if (dir == 'h')
                        {
                                for (k = 0; k < strlen(word); k++)
                                        board[i][j + k] = word[k];
                        }
                        else
                                for (k = 0; k < strlen(word); k++)
                                        board[i + k][j] = word[k];
                }
                else if (!file)
                {
                                flag = 0;
                                fscanf(ip, "%c %c %c %c %c %c %c\n", &set1[0], &set1[1], &set1[2], &set1[3], &set1[4], &set1[5], &set1[6]);
                                fscanf(ip, "%c %c %c %c %c %c %c\n", &set2[0], &set2[1], &set2[2], &set2[3], &set2[4], &set2[5], &set2[6]);
                                fscanf(ip, "%c %c %c %c %c %c %c", &set3[0], &set3[1], &set3[2], &set3[3], &set3[4], &set3[5], &set3[6]);
                                for (i = 0; i < 7; i++)
                                {
                                        set1[i] = tolower(set1[i]);
                                        set2[i] = tolower(set2[i]);
                                        set3[i] = tolower(set3[i]);
                                }
                                return;
                }
        }while(!feof(ip));
        if (file)
        {
                fclose(ip);
                ip = fopen(file, "r");
                fscanf(ip, "%c %c %c %c %c %c %c\n", &set1[0], &set1[1], &set1[2], &set1[3], &set1[4], &set1[5], &set1[6]);
                fscanf(ip, "%c %c %c %c %c %c %c\n", &set2[0], &set2[1], &set2[2], &set2[3], &set2[4], &set2[5], &set2[6]);
                fscanf(ip, "%c %c %c %c %c %c %c", &set3[0], &set3[1], &set3[2], &set3[3], &set3[4], &set3[5], &set3[6]);
                for (i = 0; i < 7; i++)
                {
                        set1[i] = tolower(set1[i]);
                        set2[i] = tolower(set2[i]);
                        set3[i] = tolower(set3[i]);
                }
        }
}

void print_board()
{
        int i, j;
        for (i = 0; i < 15; i++)
        {
                for (j = 0; j < 15; j++)
                {
                        if (board[i][j] ==  '\0')
                                printf("- ");
                        else
                                printf("%c ", board[i][j]);
                }
                printf("\n");
        }
}
int score_calculator(char *s)
{
        int value[] ={1, 3, 3, 2, 1, 4, 2, 4, 1, 8, 5, 1, 3, 1, 1, 3, 10, 1, 1, 1, 1, 4, 4, 8, 4, 10};
        if (*s == '\0')
                return 0;
        return(score_calculator(&s[1]) + value[s[0] - 'a']);
}

int search(char *s, long begin, long end)
{
        long mid = begin + (end - begin) / 2, rc;
        if( begin > end)
                return -1;
        rc = strcmp(s, words[mid]);
        if (rc > 0)
                search (s, mid + 1, end);
        else if (rc < 0)
                search(s, begin, mid - 1);
        else
                return score[mid];
}

void read_list()
{
long i = 0, k;
char c = '-';
do
{
fscanf(ip1, "%s", words[i]);
for(k = 0; k < strlen(words[i]); k++)
words[i][k] = tolower(words[i][k]);

if ( words[i][0] != c)
{
c = words[i][0];
indexes[c - 'a'] = i;
}
score[i] = score_calculator(words[i]);
i++;
}while(i < DICT_SIZE);
}



void level_2(char word[],int start,int end,int dir,int rc)
{
  int i,j;
  for(i=0;i<15;i++)
        for(j=0;j<15;j++)
                board_1[i][j] = board[i][j];

  if(dir ==1)
        for(i=start;i<=end;i++)
                board_1[i][rc]=word[i-start];
  else
        for(i=start;i<=end;i++)
                board_1[rc][i]=word[i-start];

      hori_core_engine_2();
        verti_core_engine_2();
}

void level_3(char word[],int start,int end,int dir,int rc)
{
  int i,j;
  for(i=0;i<15;i++)
        for(j=0;j<15;j++)
                board_2[i][j] = board_1[i][j];

  if(dir ==1)
        for(i=start;i<=end;i++)
                board_2[i][rc]=word[i-start];
  else
        for(i=start;i<=end;i++)
                board_2[rc][i]=word[i-start];

	hori_core_engine_3();
        verti_core_engine_3();

}

int verti_core_engine()
{
        int i, j, k, l, m, n,o,start,end,p;
        char word[15];
        char formed_word[15] = {0}, temp[15]={0},final_word[15]={0};
        for (j = 0; j < 15; j++)
        {
                for (i = 0; i < 15; i++)
                {
                        if (board[i][j] != '\0')
                        {
                                k=i;
                                word[0] = '\0';
                              while( k<15 && board[k][j] != '\0') k++;
                              for(l=0;l<k-i;l++)
                                word[l]=board[i+l][j];
                              i=k;
                              word[l]='\0';
                         if(grep1(word) == 0)   
			  {
			    for(o=0;o<sc1.count;o++)
                              for(m = 0; m < strlen(sc1.s[o]); m++)
                                {
                                        formed_word [0] = '\0';
                                        final_word[0] = '\0';
                                        temp[0] = '\0';
                                        start=end=n=0;
                                        if( i-strlen(word)-(m+1) >0 && board[i-strlen(word)-(m+1)][j] == '\0')
                                        {
                                                for(n = 0; n < m+1 ; n++)
                                                        formed_word[n]=sc1.s[o][n];
                                        }
                                        start=i-strlen(word)-(m+1);
                                        formed_word[n]='\0';
                                        /*strcat(formed_word,word); */
                                        for(n = m+1; n < 15 && board[i+n-(m+1)][j]=='\0' && n < strlen(sc1.s[o]); n++)
                                                temp[n-(m+1)]=sc1.s[o][n];
                                        temp[n-(m+1)]='\0';
                                        end=i+n-(m+1)-1;
                                        strcat(formed_word,temp);
                                        while( start >0 && board[start-1][j] != '\0' ) start--;
                                        while( end <15 && board[end+1][j] != '\0') end++;
                                        if(start >=0 && end <15)
                                        {
                                          for(n=start,p=0;n<=end;n++)
                                              if(board[n][j]!='\0') final_word[n-start]=board[n][j];
                                              else
                                              {
                                              final_word[n-start]=formed_word[p];
                                              p++;
                                              }
                                            final_word[n-start]='\0';
                                            if((end-start+1)!=strlen(word) && (end-start+1)==strlen(final_word))
                                            {
                                                /*printf("\nfinal :%s %d %d", final_word,start,end); */
                                                weight=0;
                                                weight = search(final_word,indexes[final_word[0] - 'a'], indexes[final_word[0] - 'a' +1] - 1);
                                                /*weight=10;*/
                                                if(weight>weight1)
                                                        {
                                                                strcpy(prev_word_set[0],final_word);
                                                                pos[0][0]=start;
                                                                pos[0][1]=j;
                                                                pos[0][2]=end;
                                                                pos[0][3]=j;
                                                                /*level_2(final_word,start,end,1,j);*/
                                                                /*prev_word_set[0][0]='\0';*/
								weight1=weight;	
                                                        }
                                            }
                                         }
                                }
			    	
                                formed_word [0] = '\0';
                                final_word [0] = '\0';
                                start=end=n=0;
                                /*strcat(formed_word,word);*/
                                end = strlen(sc1.s[o])+i-15;
                                if(end <=0 )
                                {
                                        strcat(formed_word,sc1.s[o]);
                                        start=i-strlen(word);
                                        end=strlen(sc1.s[o])+i-1;
                                }
                                else
                                {
                                        for(n=0;n<strlen(sc1.s[o])-end;n++)
                                                formed_word[n]=sc1.s[o][n];
                                        formed_word[n]='\0';
                                        start=i-strlen(word);
                                        end=14;
                                }
                                while( start >0 && board[start-1][j] != '\0' ) start--;
                                while( end <15 && board[end+1][j] != '\0') end++;
                                        if(start >=0 && end <15)
                                        {
                                          for(n=start,p=0;n<=end;n++)
                                              if(board[n][j]!='\0') final_word[n-start]=board[n][j];
                                              else
                                              {
                                              final_word[n-start]=formed_word[p];
                                              p++;
                                              }
                                            final_word[n-start]='\0';
                                            if((end-start+1)!=strlen(word) && (end-start+1)==strlen(final_word))
                                                {
                                                /*printf("\nfinal :%s %d %d", final_word,start,end);*/
                                                weight=0;
                                                 weight = search(final_word,indexes[final_word[0] - 'a'], indexes[final_word[0] - 'a' +1] - 1);
                                                /*weight=10;*/
                                                if(weight>weight1)
                                                        {
                                                                pos[0][0]=start;
                                                                pos[0][1]=j;
                                                                pos[0][2]=end;
                                                                pos[0][3]=j;
                                                                strcpy(prev_word_set[0],final_word);
                                                                /*level_2(final_word,start,end,1,j);
                                                                prev_word_set[0][0]='\0';*/
								weight1=weight;
                                                        }
                                            }
                                         }
                               }
                        }
                }
        }
}

int hori_core_engine()
{
        int i, j, k, l, m, n,o,start,end,p;
        char word[15];
        char formed_word[15] = {0}, temp[15]={0},final_word[15]={0};
        for (i = 0; i < 15; i++)
        {
                for (j = 0; j < 15; j++)
                {
                        if (board[i][j] != '\0')
                        {
                                k=j;
                                word[0] = '\0';
                              while( k<15 && board[i][k] != '\0') k++;
                              for(l=0;l<k-j;l++)
                                word[l]=board[i][j+l];
                              j=k;
                              word[l]='\0';
			  if(grep1(word) == 0)
                           {
                             for(o=0;o<sc1.count;o++)
                              for(m = 0; m < strlen(sc1.s[o]); m++)
                                {
                                        formed_word [0] = '\0';
                                        final_word[0] = '\0';
                                        temp[0] = '\0';
                                        start=end=n=0;
                                        if( j-strlen(word)-(m+1) >0 && board[i][j-strlen(word)-(m+1)] == '\0')
                                        {
                                                for(n = 0; n < m+1 ; n++)
                                                        formed_word[n]=sc1.s[o][n];
                                        }
                                        start=j-strlen(word)-(m+1);
                                        formed_word[n]='\0';
                                        /*strcat(formed_word,word);*/
                                        for(n = m+1; n < 15 && board[i][j+n-(m+1)]=='\0' && n < strlen(sc1.s[o]); n++)
                                                temp[n-(m+1)]=sc1.s[o][n];
                                        temp[n-(m+1)]='\0';
                                        end=j+n-(m+1)-1;
                                        strcat(formed_word,temp);
                                        while( start >0 && board[i][start-1] != '\0' ) start--;
                                        while( end <15 && board[i][end+1] != '\0') end++;
                                        if(start >=0 && end <15)
                                        {
                                          for(n=start,p=0;n<=end;n++)
                                              if(board[i][n]!='\0') final_word[n-start]=board[i][n];
                                              else
                                              {
                                              final_word[n-start]=formed_word[p];
                                              p++;
                                              }
                                            final_word[n-start]='\0';
                                            if((end-start+1)!=strlen(word) && (end-start+1)==strlen(final_word))
                                                {
                                                /*printf("\nfinal :%s %d %d", final_word,start,end);*/
                                                weight=0;
                                                 weight = search(final_word,indexes[final_word[0] - 'a'], indexes[final_word[0] - 'a' +1] - 1);
                                                /*weight=10;*/
                                                if(weight>weight1)
                                                        {
                                                                strcpy(prev_word_set[0],final_word);
                                                                pos[0][0]=i;
                                                                pos[0][1]=start;
                                                                pos[0][2]=i;
                                                                pos[0][3]=end;
                                                                /*level_2(final_word,start,end,0,i);*/
								weight1=weight;
                                                        }
                                            }

                                         }
                                }
                                formed_word [0] = '\0';
                                final_word [0] = '\0';
                                start=end=n=0;
                                /*strcat(formed_word,word);*/
                                end = strlen(sc1.s[o])+j-15;
                                if(end <=0 )
                                {
                                        strcat(formed_word,sc1.s[o]);
                                        start=j-strlen(word);
                                        end=strlen(sc1.s[o])+j-1;
                                }
                                else
                                {
                                        for(n=0;n<strlen(sc1.s[o])-end;n++)
                                                formed_word[n]=sc1.s[o][n];
                                        formed_word[n]='\0';
                                        start=j-strlen(word);
                                        end=14;
                                }
                                while( start >0 && board[i][start-1] != '\0' ) start--;
                                while( end <15 && board[i][end+1] != '\0') end++;
                                        if(start >=0 && end <15)
                                        {
                                          for(n=start,p=0;n<=end;n++)
                                              if(board[i][n]!='\0') final_word[n-start]=board[i][n];
                                              else
                                              {
                                              final_word[n-start]=formed_word[p];
                                              p++;
                                              }
                                            final_word[n-start]='\0';
                                            if((end-start+1)!=strlen(word) && (end-start+1)==strlen(final_word))
                                                {
                                                weight=0;
                                                 weight = search(final_word,indexes[final_word[0] - 'a'], indexes[final_word[0] - 'a' +1] - 1);
                                                /*weight=10;*/

                                                if(weight>weight1)
                                                        {
                                                                strcpy(prev_word_set[0],final_word);
                                                                pos[0][0]=i;
                                                                pos[0][1]=start;
                                                                pos[0][2]=i;
                                                                pos[0][3]=end;
                                                                /*level_2(final_word,start,end,0,i);*/
								weight1=weight;	
                                                        }
                                            }

                                         }
				}
                        }
                }
        }
}


int hori_core_engine_2()
{
        int i, j, k, l, m, n,o,start,end,p,temp_int=0;
        char word[15];
        char formed_word[15] = {0}, temp[15]={0},final_word[15]={0};
        for (i = 0; i < 15; i++)
        {
                for (j = 0; j < 15; j++)
                {
                        if (board_1[i][j] != '\0')
                        {
                                k=j;
                                word[0] = '\0';
                              while( k<15 && board_1[i][k] != '\0') k++;
                              for(l=0;l<k-j;l++)
                                word[l]=board_1[i][j+l];
                              j=k;
                              word[l]='\0';
			  if(grep1(word) == 0)
                          {	
                             for(o=0;o<sc2.count;o++)
                              for(m = 0; m < strlen(sc2.s[o]); m++)
                                {
                                        formed_word [0] = '\0';
                                        final_word[0] = '\0';
                                        temp[0] = '\0';
                                        start=end=n=0;
                                        if( j-strlen(word)-(m+1) >0 && board_1[i][j-strlen(word)-(m+1)] == '\0')
                                        {
                                                for(n = 0; n < m+1 ; n++)
                                                        formed_word[n]=sc2.s[o][n];
                                        }
                                        start=j-strlen(word)-(m+1);
                                        formed_word[n]='\0';
                                        /*strcat(formed_word,word);*/
                                        for(n = m+1; n < 15 && board_1[i][j+n-(m+1)]=='\0' && n < strlen(sc2.s[o]); n++)
                                                temp[n-(m+1)]=sc2.s[o][n];
                                        temp[n-(m+1)]='\0';
                                        end=j+n-(m+1)-1;
                                        strcat(formed_word,temp);
                                        while( start >0 && board_1[i][start-1] != '\0' ) start--;
                                        while( end <15 && board_1[i][end+1] != '\0') end++;
                                        if(start >=0 && end <15)
                                        {
                                          for(n=start,p=0;n<=end;n++)
                                              if(board_1[i][n]!='\0') final_word[n-start]=board_1[i][n];
                                              else
                                              {
                                              final_word[n-start]=formed_word[p];
                                              p++;
                                              }
                                            final_word[n-start]='\0';
                                            if((end-start+1)!=strlen(word) && (end-start+1)==strlen(final_word))
                                                {
                                                /*printf("\nfinal :%s %d %d", final_word,start,end);*/

                                                temp_int = search(final_word,indexes[final_word[0] - 'a'], indexes[final_word[0] - 'a' +1] - 1);
                                                /*temp_int=0;*/
                                                if(temp_int>weight2)
                                                        {
                                                                /*weight+=temp_int;*/
                                                                strcpy(prev_word_set[1],final_word);
                                                                pos[1][0]=i;
                                                                pos[1][1]=start;
                                                                pos[1][2]=i;
                                                                pos[1][3]=end;
								weight2=temp_int;
                                                                /*level_3(final_word,start,end,0,i);*/
                                                                /*weight-=temp_int;*/
                                                        }
                                            }

                                         }
                                }
                                formed_word [0] = '\0';
                                final_word [0] = '\0';
                                start=end=n=0;
                                /*strcat(formed_word,word);*/
                                end = strlen(sc2.s[o])+j-15;
                                if(end <=0 )
                                {
                                        strcat(formed_word,sc2.s[o]);
                                        start=j-strlen(word);
                                        end=strlen(sc2.s[o])+j-1;
                                }
                                else
                                {
                                        for(n=0;n<strlen(sc2.s[o])-end;n++)
                                                formed_word[n]=sc2.s[o][n];
                                        formed_word[n]='\0';
                                        start=j-strlen(word);
                                        end=14;
                                }
                                while( start >0 && board_1[i][start-1] != '\0' ) start--;
                                while( end <15 && board_1[i][end+1] != '\0') end++;
                                        if(start >=0 && end <15)
                                        {
                                          for(n=start,p=0;n<=end;n++)
                                              if(board_1[i][n]!='\0') final_word[n-start]=board_1[i][n];
                                              else
                                              {
                                              final_word[n-start]=formed_word[p];
                                              p++;
                                              }
                                            final_word[n-start]='\0';
                                            if((end-start+1)!=strlen(word) && (end-start+1)==strlen(final_word))
                                                {
                                                /*printf("\nfinal :%s %d %d", final_word,start,end);*/

                                                temp_int = search(final_word,indexes[final_word[0] - 'a'], indexes[final_word[0] - 'a' +1] - 1);
                                                /*temp_int=0;*/
                                                 if(temp_int>weight2)
                                                        {
                                                                /*weight+=temp_int;*/
                                                                strcpy(prev_word_set[1],final_word);
                                                                pos[1][0]=i;
                                                                pos[1][1]=start;
                                                                pos[1][2]=i;
                                                                pos[1][3]=end;
                                                                weight2=temp_int;
                                                                /*level_3(final_word,start,end,0,i);*/
                                                                /*weight-=temp_int;*/
                                                        }

                                            }

                                         }
				}
                        }
                }
        }
}

int verti_core_engine_2()
{
        int i, j, k, l, m, n,o,start,end,p,temp_int=0;
        char word[15];
        char formed_word[15] = {0}, temp[15]={0},final_word[15]={0};
        for (j = 0; j < 15; j++)
        {
                for (i = 0; i < 15; i++)
                {
                        if (board_1[i][j] != '\0')
                        {
                                k=i;
                                word[0] = '\0';
                              while( k<15 && board_1[k][j] != '\0') k++;
                              for(l=0;l<k-i;l++)
                                word[l]=board_1[i+l][j];
                              i=k;
                              word[l]='\0';
			   if(grep1(word) == 0)
                           {	
                            for(o=0;o<sc2.count;o++)
                              for(m = 0; m < strlen(sc2.s[o]); m++)
                                {
                                        formed_word [0] = '\0';
                                        final_word[0] = '\0';
                                        temp[0] = '\0';
                                        start=end=n=0;
                                        if( i-strlen(word)-(m+1) >0 && board_1[i-strlen(word)-(m+1)][j] == '\0')
                                        {
                                                for(n = 0; n < m+1 ; n++)
                                                        formed_word[n]=sc2.s[o][n];
                                        }
                                        start=i-strlen(word)-(m+1);
                                        formed_word[n]='\0';
                                        /*strcat(formed_word,word);*/
                                        for(n = m+1; n < 15 && board_1[i+n-(m+1)][j]=='\0' && n < strlen(sc2.s[o]); n++)
                                                temp[n-(m+1)]=sc2.s[o][n];
                                        temp[n-(m+1)]='\0';
                                        end=i+n-(m+1)-1;
                                        strcat(formed_word,temp);
                                        while( start >0 && board_1[start-1][j] != '\0' ) start--;
                                        while( end <15 && board_1[end+1][j] != '\0') end++;
                                        if(start >=0 && end <15)
                                        {
                                          for(n=start,p=0;n<=end;n++)
                                              if(board_1[n][j]!='\0') final_word[n-start]=board_1[n][j];
                                              else
                                              {
                                              final_word[n-start]=formed_word[p];
                                              p++;
                                              }
                                            final_word[n-start]='\0';
                                            if((end-start+1)!=strlen(word) && (end-start+1)==strlen(final_word))
                                            {
                                                /*printf("\nfinal :%s %d %d", final_word,start,end);*/
                                                temp_int = search(final_word,indexes[final_word[0] - 'a'], indexes[final_word[0] - 'a' +1] - 1);
                                                /*temp_int=10;*/
						if(temp_int>weight2)
                                                        {
                                                                /*weight+=temp_int;*/
                                                                strcpy(prev_word_set[1],final_word);
                                                                pos[1][0]=start;
                                                                pos[1][1]=j;
                                                                pos[1][2]=end;
                                                                pos[1][3]=j;
                                                                weight2=temp_int;
                                                                /*level_3(final_word,start,end,0,i);*/
                                                                /*weight-=temp_int;*/
                                                        }

                                            }
                                         }
                                }
                                formed_word [0] = '\0';
                                final_word [0] = '\0';
                                start=end=n=0;
                                /*strcat(formed_word,word);*/
                                end = strlen(sc2.s[o])+i-15;
                                if(end <=0 )
                                {
                                        strcat(formed_word,sc2.s[o]);
                                        start=i-strlen(word);
                                        end=strlen(sc2.s[o])+i-1;
                                }
                                else
                                {
                                        for(n=0;n<strlen(sc2.s[o])-end;n++)
                                                formed_word[n]=sc2.s[o][n];
                                        formed_word[n]='\0';
                                        start=i-strlen(word);
                                        end=14;
                                }
                                while( start >0 && board_1[start-1][j] != '\0' ) start--;
                                while( end <15 && board_1[end+1][j] != '\0') end++;
                                        if(start >=0 && end <15)
                                        {
                                          for(n=start,p=0;n<=end;n++)
                                              if(board_1[n][j]!='\0') final_word[n-start]=board_1[n][j];
                                              else
                                              {
                                              final_word[n-start]=formed_word[p];
                                              p++;
                                              }
                                            final_word[n-start]='\0';
                                            if((end-start+1)!=strlen(word) && (end-start+1)==strlen(final_word))
                                                {
                                                /*printf("\nfinal :%s %d %d", final_word,start,end);*/
                                                temp_int = search(final_word,indexes[final_word[0] - 'a'], indexes[final_word[0] - 'a' +1] - 1);
                                                /*temp_int=0;*/
						if(temp_int>weight2)
                                                        {
                                                                /*weight+=temp_int;*/
                                                                strcpy(prev_word_set[1],final_word);
                                                                pos[1][0]=start;
                                                                pos[1][1]=j;
                                                                pos[1][2]=end;
                                                                pos[1][3]=j;
                                                                weight2=temp_int;
                                                                /*level_3(final_word,start,end,0,i);*/
                                                                /*weight-=temp_int;*/
                                                        }

                                            }
                                         }
				}
                        }
                }
        }
}



int verti_core_engine_3()
{
        int i, j, k, l, m, n,o,start,end,p,x,y,temp_int=0;
        char word[15];
        char formed_word[15] = {0}, temp[15]={0},final_word[15]={0};
        for (j = 0; j < 15; j++)
        {
                for (i = 0; i < 15; i++)
                {
                        if (board_2[i][j] != '\0')
                        {
                                k=i;
                                word[0] = '\0';
                              while( k<15 && board_2[k][j] != '\0') k++;
                              for(l=0;l<k-i;l++)
                                word[l]=board_2[i+l][j];
                              i=k;
                              word[l]='\0';
			   if(grep1(word) == 0)
                          {	
                             for(o=0;o<sc3.count;o++)
                              for(m = 0; m < strlen(sc3.s[o]); m++)
                                {
                                        formed_word [0] = '\0';
                                        final_word[0] = '\0';
                                        temp[0] = '\0';
                                        start=end=n=0;
                                        if( i-strlen(word)-(m+1) >0 && board_2[i-strlen(word)-(m+1)][j] == '\0')
                                        {
                                                for(n = 0; n < m+1 ; n++)
                                                        formed_word[n]=sc3.s[o][n];
                                        }
                                        start=i-strlen(word)-(m+1);
                                        formed_word[n]='\0';
                                        /*strcat(formed_word,word);*/
                                        for(n = m+1; n < 15 && board_2[i+n-(m+1)][j]=='\0' && n < strlen(sc3.s[o]); n++)
                                                temp[n-(m+1)]=sc3.s[o][n];
                                        temp[n-(m+1)]='\0';
                                        end=i+n-(m+1)-1;
                                        strcat(formed_word,temp);
                                        while( start >0 && board_2[start-1][j] != '\0' ) start--;
                                        while( end <15 && board_2[end+1][j] != '\0') end++;
                                        if(start >=0 && end <15)
                                        {
                                          for(n=start,p=0;n<=end;n++)
                                              if(board_2[n][j]!='\0') final_word[n-start]=board_2[n][j];
                                              else
                                              {
                                              final_word[n-start]=formed_word[p];
                                              p++;
                                              }
                                            final_word[n-start]='\0';
                                            if((end-start+1)!=strlen(word) && (end-start+1)==strlen(final_word))
                                            {
                                                /*printf("\nfinal :%s %d %d", final_word,start,end);*/
                                                if(score_calculator(final_word)+weight > prev_weight)
						   temp_int = search(final_word,indexes[final_word[0] - 'a'], indexes[final_word[0] - 'a' +1] - 1);
                                                /*temp_int=0;*/
                                                if(temp_int>weight3)
                                                        {
                                                                strcpy(prev_word_set[2],final_word);
                                                                pos[2][0]=start;
                                                                pos[2][1]=j;
                                                                pos[2][2]=end;
                                                                pos[2][3]=j;
								weight3=temp_int;	
                                                                /*if(prev_weight <= (weight+temp_int))
                                                                {
                                                                        prev_weight=weight+temp_int;
                                                                        strcpy(word_set[0],prev_word_set[0]);
                                                                        strcpy(word_set[1],prev_word_set[1]);
                                                                        strcpy(word_set[2],prev_word_set[2]);
                                                                        for(x=0;x<3;x++)
                                                                                for(y=0;y<4;y++)
                                                                                        fin_pos[x][y]=pos[x][y];
                                                                }*/
                                                        }

                                            }
                                         }
                                }
                                formed_word [0] = '\0';
                                final_word [0] = '\0';
                                start=end=n=0;
                                /*strcat(formed_word,word);*/
                                end = strlen(sc3.s[o])+i-15;
                                if(end <=0 )
                                {
                                        strcat(formed_word,sc3.s[o]);
                                        start=i-strlen(word);
                                        end=strlen(sc3.s[o])+i-1;
                                }
                                else
                                {
                                        for(n=0;n<strlen(sc3.s[o])-end;n++)
                                                formed_word[n]=sc3.s[o][n];
                                        formed_word[n]='\0';
                                        start=i-strlen(word);
                                        end=14;
                                }
                                while( start >0 && board_2[start-1][j] != '\0' ) start--;
                                while( end <15 && board_2[end+1][j] != '\0') end++;
                                        if(start >=0 && end <15)
                                        {
                                          for(n=start,p=0;n<=end;n++)
                                              if(board_2[n][j]!='\0') final_word[n-start]=board_2[n][j];
                                              else
                                              {
                                              final_word[n-start]=formed_word[p];
                                              p++;
                                              }
                                            final_word[n-start]='\0';
                                            if((end-start+1)!=strlen(word) && (end-start+1)==strlen(final_word))
                                                {
                                                /*printf("\nfinal :%s %d %d", final_word,start,end);*/
                                                if(score_calculator(final_word)+weight > prev_weight)
							temp_int = search(final_word,indexes[final_word[0] - 'a'], indexes[final_word[0] - 'a' +1] - 1);
                                                /*temp_int=10;*/
					     if(temp_int>weight3)
                                                        {
                                                                strcpy(prev_word_set[2],final_word);
                                                                pos[2][0]=start;
                                                                pos[2][1]=j;
                                                                pos[2][2]=end;
                                                                pos[2][3]=j;
                                                                weight3=temp_int;
                                                                /*if(prev_weight <= (weight+temp_int))
                                                                {
                                                                        prev_weight=weight+temp_int;
                                                                        strcpy(word_set[0],prev_word_set[0]);
                                                                        strcpy(word_set[1],prev_word_set[1]);
                                                                        strcpy(word_set[2],prev_word_set[2]);
                                                                        for(x=0;x<3;x++)
                                                                                for(y=0;y<4;y++)
                                                                                        fin_pos[x][y]=pos[x][y];
                                                                }*/
                                                        }
	
                                            }
                                         }
				}
                        }
                }
        }
}

int hori_core_engine_3()
{
        int i, j, k, l, m, n,o,start,end,p,x,y,temp_int=0;
        char word[15];
        char formed_word[15] = {0}, temp[15]={0},final_word[15]={0};
        for (i = 0; i < 15; i++)
        {
                for (j = 0; j < 15; j++)
                {
                        if (board_2[i][j] != '\0')
                        {
                                k=j;
                                word[0] = '\0';
			    if(grep1(word) == 0)
                            	{
                              while( k<15 && board_2[i][k] != '\0') k++;
                              for(l=0;l<k-j;l++)
                                word[l]=board_2[i][j+l];
                              j=k;
                              word[l]='\0';
                             for(o=0;o<sc3.count;o++)
                              for(m = 0; m < strlen(sc3.s[o]); m++)
                                {
                                        formed_word [0] = '\0';
                                        final_word[0] = '\0';
                                        temp[0] = '\0';
                                        start=end=n=0;
                                        if( j-strlen(word)-(m+1) >0 && board_2[i][j-strlen(word)-(m+1)] == '\0')
                                        {
                                                for(n = 0; n < m+1 ; n++)
                                                        formed_word[n]=sc3.s[o][n];
                                        }
                                        start=j-strlen(word)-(m+1);
                                        formed_word[n]='\0';
                                        /*strcat(formed_word,word);*/
                                        for(n = m+1; n < 15 && board_2[i][j+n-(m+1)]=='\0' && n < strlen(sc3.s[o]); n++)
                                                temp[n-(m+1)]=sc3.s[o][n];
                                        temp[n-(m+1)]='\0';
                                        end=j+n-(m+1)-1;
                                        strcat(formed_word,temp);
                                        while( start >0 && board_2[i][start-1] != '\0' ) start--;
                                        while( end <15 && board_2[i][end+1] != '\0') end++;
                                        if(start >=0 && end <15)
                                        {
                                          for(n=start,p=0;n<=end;n++)
                                              if(board_2[i][n]!='\0') final_word[n-start]=board_2[i][n];
                                              else
                                              {
                                              final_word[n-start]=formed_word[p];
                                              p++;
                                              }
                                            final_word[n-start]='\0';
                                            if((end-start+1)!=strlen(word) && (end-start+1)==strlen(final_word))
                                                {
                                                /*printf("\nfinal :%s %d %d", final_word,start,end);*/
						
						if(score_calculator(final_word)+weight > prev_weight)	
                                                  temp_int =  search(final_word,indexes[final_word[0] - 'a'], indexes[final_word[0] - 'a' +1] - 1);
                                                /*temp_int=10;*/
                                                 if(temp_int>weight3)
                                                        {
                                                                strcpy(prev_word_set[2],final_word);
                                                                pos[2][0]=i;
                                                                pos[2][1]=start;
                                                                pos[2][2]=i;
                                                                pos[2][3]=end;
                                                                weight3=temp_int;
                                                                /*if(prev_weight <= (weight+temp_int))
                                                                {
                                                                        prev_weight=weight+temp_int;
                                                                        strcpy(word_set[0],prev_word_set[0]);
                                                                        strcpy(word_set[1],prev_word_set[1]);
                                                                        strcpy(word_set[2],prev_word_set[2]);
                                                                        for(x=0;x<3;x++)
                                                                                for(y=0;y<4;y++)
                                                                                        fin_pos[x][y]=pos[x][y];
                                                                }*/
                                                        }


                                            }

                                         }
                                }
                                formed_word [0] = '\0';
                                final_word [0] = '\0';
                                start=end=n=0;
                                /*strcat(formed_word,word);*/
                                end = strlen(sc3.s[o])+j-15;
                                if(end <=0 )
                                {
                                        strcat(formed_word,sc3.s[o]);
                                        start=j-strlen(word);
                                        end=strlen(sc3.s[o])+j-1;
                                }
                                else
                                {
                                        for(n=0;n<strlen(sc3.s[o])-end;n++)
                                                formed_word[n]=sc3.s[o][n];
                                        formed_word[n]='\0';
                                        start=j-strlen(word);
                                        end=14;
                                }
                                while( start >0 && board_2[i][start-1] != '\0' ) start--;
                                while( end <15 && board_2[i][end+1] != '\0') end++;
                                        if(start >=0 && end <15)
                                        {
                                          for(n=start,p=0;n<=end;n++)
                                              if(board_2[i][n]!='\0') final_word[n-start]=board_2[i][n];
                                              else
                                              {
                                              final_word[n-start]=formed_word[p];
                                              p++;
                                              }
                                            final_word[n-start]='\0';
                                            if((end-start+1)!=strlen(word) && (end-start+1)==strlen(final_word))
                                                {
                                                /*printf("\nfinal :%s %d %d", final_word,start,end);*/
						if(score_calculator(final_word)+weight > prev_weight)
                                              		  temp_int = search(final_word,indexes[final_word[0] - 'a'], indexes[final_word[0] - 'a' +1] - 1);
                                                /*temp_int=0;*/
                                              if(temp_int>weight3)
                                                        {
                                                                strcpy(prev_word_set[2],final_word);
                                                                pos[2][0]=i;
                                                                pos[2][1]=start;
                                                                pos[2][2]=i;
                                                                pos[2][3]=end;
								weight3=temp_int;
                                                                /*if(prev_weight <= (weight+temp_int))
                                                                {
                                                                        prev_weight=weight+temp_int;
                                                                        strcpy(word_set[0],prev_word_set[0]);
                                                                        strcpy(word_set[1],prev_word_set[1]);
                                                                        strcpy(word_set[2],prev_word_set[2]);
                                                                        for(x=0;x<3;x++)
                                                                                for(y=0;y<4;y++)
                                                                                        fin_pos[x][y]=pos[x][y];
                                                                }*/
                                                        }



                                            }

                                         }
				}
                        }
                }
        }
}

int main(int argc, char *argv[])
{
        char str[35];int i,j;
        FILE *out = fopen("output.txt", "w");
        ip = fopen(argv[1], "r");
        /*ip=fopen("board.txt","r");*/
        ip1 = fopen("wordlist.txt", "r");
        read_board(argv[2]);
        print_board();
        printf("\nreading wordlist");
	system("date");
        read_list();
	system("date");
        printf("\nafter read");
        printf("\n%s %s %s\n", set1, set2, set3);	
	printf("\n starting sort");
	system("date");
	sort(set1);
        sort(set2);
        sort(set3);
	system("date");
	printf("\n ending sort");
        printf("\n starting combis");
	system("date");
	ComputeCombinations(set1, &sc1);
	flag = 1;c_cnt = 0;
        ComputeCombinations(set2, &sc2);
	flag = 1;c_cnt = 0;
        ComputeCombinations(set3, &sc3);
        printf("\n ending combi");
	system("date");
	printf("\n%s %s %s\n", set1, set2, set3);
	printf("\n%d %d %d",sc1.count,sc2.count,sc3.count);
	printf("\nstarting core");
	system("date");
        hori_core_engine();
        printf("--------");
        verti_core_engine();
	if(pos[0][0]==pos[0][2])
                level_2(prev_word_set[0],pos[0][1],pos[0][3],0,pos[0][2]);
        else
        	level_2(prev_word_set[0],pos[0][0],pos[0][2],1,pos[0][1]);
	if(pos[1][0]==pos[1][2])
                level_3(prev_word_set[1],pos[1][1],pos[1][3],0,pos[1][2]);
        else
                level_3(prev_word_set[1],pos[1][0],pos[1][2],1,pos[1][1]);
       for (i = 0; i < 15; i++)
        {
                for (j = 0; j < 15; j++)
                {
                        if (board_2[i][j] ==  '\0')
                                printf("- ");
                        else
                                printf("%c ", board_2[i][j]);
                }
                printf("\n");
        }

	printf("\nending core"); 
	printf( "\n\nfinal result\n\n");
        printf( "Total weight:%d \n  %d %d  %d  %s--(%d,%d)->(%d,%d)  %s--(%d,%d)->(%d,%d)  %s--(%d,%d)->(%d,%d)\n",weight1+weight2+weight3, weight1,weight2,weight3,prev_word_set[0],pos[0][0],pos[0][1],pos[0][2],pos[0][3],prev_word_set[1],pos[1][0],pos[1][1],pos[1][2],pos[1][3],prev_word_set[2],pos[2][0],pos[2][1],pos[2][2],pos[2][3]);
        if(pos[0][0]==pos[0][2])
                fprintf(out,"%d,%d,%c,%s\n",pos[0][0]+1,pos[0][1]+1,'H',prev_word_set[0]);
        else
                fprintf(out,"%d,%d,%c,%s\n",pos[0][0]+1,pos[0][1]+1,'V',prev_word_set[0]);
        if(pos[1][0]==pos[1][2])
                fprintf(out,"%d,%d,%c,%s\n",pos[1][0]+1,pos[1][1]+1,'H',prev_word_set[1]);
        else
                fprintf(out,"%d,%d,%c,%s\n",pos[1][0]+1,pos[1][1]+1,'V',prev_word_set[1]);
        if(pos[2][0]==pos[2][2])
                fprintf(out,"%d,%d,%c,%s\n",pos[2][0]+1,pos[2][1]+1,'H',prev_word_set[2]);
        else
                fprintf(out,"%d,%d,%c,%s\n",pos[2][0]+1,pos[2][1]+1,'V',prev_word_set[2]);


      /*  printf("\nEnter a string to search : ");
        scanf("%s", str);
        printf("%d \n", search(str, (long)0, (long)DICT_SIZE));
        core_engine();
        getch();
        char s[8];
        sort(s);
        next_comb(s, NULL, 0, 1, out);*/
        return(0);
}

/* Sort the string. Buble sort implementation */
void sort(char *s)
{
        int i, j;
        for (i = 0; i < 6; i++)
        {
                for (j = i + 1; j < 7; j++)
                {
                        if (s[i] > s[j])
                        {
                                char t = s[j];
                                s[j] = s[i];
                                s[i] = t;
                        }
                }
        }
}

