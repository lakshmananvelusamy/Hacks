#define TEAM   "(null)"

#include <cstdio>
#include <cstdlib>
#include <cctype>
#include <algorithm>
#include <vector>
#include <list>
#include <functional>

using namespace std;

const unsigned int SIZE   = 15; /* board size */
const unsigned int SETLEN = 7;  /* set length */
const unsigned int NOSET  = 3;  /* number of set */

const unsigned int STRSZ = 16;  /* max string size */
const unsigned int PRSSZ = 132; /* presence size, only 26 is used; rest are for opt */

const int PTS[] = {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 
   0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0,
   0,0,0,0,0, 0,0,0,0,0, 0,0,1,3,3,2, 1,4,2,4,1, 8,5,1,3,1, 1,3,10,1,1, 1,1,4,4,8, 4,10};

const int M2D[] = {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 
   0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0,
   0,0,0,0,0, 0,0,0,0,0, 0,0,1,2,3, 4,5,6,7,8, 9,10,11,12,13, 14,15,16,17,18, 19,20,21,22,23, 24,25,26};

const int M2C[] = {0, 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 
   'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};

#define POINTS(P,S) (P[S[0]]+P[S[1]]+P[S[2]]+P[S[3]]+P[S[4]]+P[S[5]]+P[S[6]]\
    +P[S[7]]+P[S[8]]+P[S[9]]+P[S[10]]+P[S[11]]+P[S[12]]+P[S[13]]+P[S[14]])

#define IS_PRESENT(A,S) (A[S[0]]&&A[S[1]]&&A[S[2]]&&A[S[3]]&&A[S[4]]&&A[S[5]]&&A[S[6]]\
    &&A[S[7]]&&A[S[8]]&&A[S[9]]&&A[S[10]]&&A[S[11]]&&A[S[12]]&&A[S[13]]&&A[S[14]])

inline int is_present(int* arr, char* str) { return IS_PRESENT(arr,str); }

inline int total_points (char* str) { return POINTS(PTS,str); }
void put_present(int* arr, const char* str);

struct dictword_t {   /* one dictionary word */
   char word[STRSZ];  /* actual word */
   int  wordlen; /* length of the word */
   int  points;
   int  pres[PRSSZ];

   dictword_t(char* str, int len) {
      memset (pres, 0, sizeof(pres));

      memcpy (word, str, STRSZ); /* copy the actual string */
      wordlen = len; /* copy the word length */
      points = POINTS(PTS, str);
   }
};

struct word_t {
   dictword_t* word;
   int row, col, ori;
   word_t* creator;

   word_t() { memset(this, 0, sizeof(word_t)); };
};

struct node_t {
   int len, alen; 
   int row, col, ori, cellno; /* row, col */
   int pres[PRSSZ];
   char word[STRSZ];
   word_t* creator;
   int points;

   node_t() { memset(this, 0, sizeof(node_t)); };
};

typedef list<dictword_t*> dict_list_t;
typedef list<dictword_t*>::iterator dict_iterator_t;

typedef vector<dictword_t*> dictionary_t;
typedef vector<node_t*> nodearr_t;
typedef vector<word_t*> matchedwords_t;

typedef char board_t[SIZE][SIZE];

struct sort_dict: public binary_function<dictword_t*, dictword_t*, bool> {
   bool operator() (dictword_t* x, dictword_t* y) { return x->points > y->points; } 
};

struct sort_word: public binary_function<dictword_t*, dictword_t*, bool> {
   bool operator() (word_t* x, word_t* y) { return x->word->points > y->word->points; } 
};

struct sort_node: public binary_function<dictword_t*, dictword_t*, bool> {
   bool operator() (node_t* x, node_t* y) { return x->points > y->points; } 
};

struct file_t { /* struct to process files */
   FILE* fptr;

   file_t() { fptr = NULL; } /* set fptr to NULL */
   ~file_t() { if (fptr) fclose(fptr); } /* close file on return */

   int file_open (char* str, char* mode) { /* open file */
      if (!(fptr = fopen(str, mode))) /* return on error */
         return fprintf (stderr, "Error: Unable to open file %s\n", str);
      else return 0;
   }
};

dict_list_t dictionary; /* master dictionary for reference */

board_t board; /* actual board */
char inputset[NOSET][STRSZ]; /* stores the set */

int charpresence[PRSSZ]; 
int boardpresence[PRSSZ]; 

inline int is_throwable (int* arr, const char* str) 
{
   int throwflag ;

   arr[str[0]]--; arr[str[1]]--; arr[str[2]]--; arr[str[3]]--;
   arr[str[4]]--; arr[str[5]]--; arr[str[6]]--; arr[str[7]]--;
   arr[str[8]]--; arr[str[9]]--; arr[str[10]]--; arr[str[11]]--;
   arr[str[12]]--; arr[str[13]]--; arr[str[14]]--; 

   throwflag =  ((arr[str[0]] < 0) || (arr[str[1]] < 0) || (arr[str[2]] < 0) || (arr[str[3]] < 0) ||
      (arr[str[4]] < 0) || (arr[str[5]] < 0) || (arr[str[6]] < 0) || (arr[str[7]] < 0) ||
      (arr[str[8]] < 0) || (arr[str[9]] < 0) || (arr[str[10]] < 0) || (arr[str[11]] < 0) ||
      (arr[str[12]] < 0) || (arr[str[13]] < 0) || (arr[str[14]] < 0));

   return throwflag;
}

inline void put_present(int* arr, const char* str)
{
   arr[str[0]]++; arr[str[1]]++; arr[str[2]]++; arr[str[3]]++;
   arr[str[4]]++; arr[str[5]]++; arr[str[6]]++; arr[str[7]]++;
   arr[str[8]]++; arr[str[9]]++; arr[str[10]]++; arr[str[11]]++;
   arr[str[12]]++; arr[str[13]]++; arr[str[14]]++;
   arr[0] = 0x0000000f; /* will be overwritten for strict presence check*/
}

inline void board_presence (int* arr, const board_t& board)
{
   put_present (arr, board[0]);  put_present (arr, board[1]);
   put_present (arr, board[2]);  put_present (arr, board[3]);
   put_present (arr, board[4]);  put_present (arr, board[5]);
   put_present (arr, board[6]);  put_present (arr, board[7]);
   put_present (arr, board[8]);  put_present (arr, board[9]);
   put_present (arr, board[10]);  put_present (arr, board[11]);
   put_present (arr, board[12]);  put_present (arr, board[13]);
   put_present (arr, board[14]);
}

void place_word (board_t* board, char* str, int r, int c, int o)
{
   int i;

   for (i = 0; i < strlen(str); i++)
      (*board)[r+((o=='V')?i:0)][c+((o=='H')?i:0)] = str[i];
}

void process_board_string (char* stri) 
{
   int r, c, l, i;
   char s, str[16];

   memset (str, 0, sizeof(str));
   sscanf (stri, "%d,%d,%c,%s", &r, &c, &s, str); r--; c--;

   place_word (&board, str, r, c, s);
}

int find_points (char *brdstr, char *set, char* dictstr, int dslen)
{
   int presence[PRSSZ];
   int throwflag;

   memset(presence, 0, sizeof(presence));
   put_present(presence, brdstr);
   put_present(presence, set);
   presence[0] = STRSZ - dslen;

   throwflag = is_throwable(presence, dictstr);
   if (throwflag)  return 0;
}

void display_board(const board_t& board)
{
   int r,c;

   int i;

   /* print the column heading */
   for (r = 0; r < SIZE; r++) {
      for (c = 0; c < SIZE; c++) {
         if (board[r][c]) printf ("%c ", board[r][c]);
         else printf ("- ", board[r][c]);
      }
      printf ("\n");
   }
}

int read_input (FILE* infile)
{
   char str[32];
   int i = 0;
   int r = 0, c = 0;

   while (!feof(infile)) { /* until end of the file */
      memset (str, 0, sizeof(str));
      fscanf (infile, "%[^\n]\n", str);
      
      if (isdigit(str[0])) process_board_string (str);
      else if (i < NOSET) { 
         memset (inputset[i], 0, sizeof(inputset[i])); sort(str, str+13);
         *(str+6) = tolower(*(str+6)); *(str+7) = tolower(*(str+7)); 
         *(str+8) = tolower(*(str+8)); *(str+9) = tolower(*(str+9)); 
         *(str+10) = tolower(*(str+10)); *(str+11) = tolower(*(str+11)); 
         *(str+12) = tolower(*(str+12)); 
         memcpy(inputset[i], str+6, SETLEN); i++; 
         put_present (charpresence, str+6);
      }
   }
   board_presence(charpresence, board);
   board_presence(boardpresence, board);
}

int process_dictionary (FILE *dictfile)
{
   char str[32];
   int len, i;
   int storeflag;
   int ttlstr = 0, stwstr = 0;
   int pres[PRSSZ];

   while (!feof(dictfile)) {
      memset (str, 0, sizeof(str));
      fscanf (dictfile, "%[^\n]\n", str);

      len = strlen(str); ttlstr++; 
      if ((len < STRSZ) && (is_present(charpresence, str))) {
         memcpy(pres, charpresence, sizeof(pres));
         if (is_throwable(pres, str)) continue;
         dictionary.push_back(new dictword_t(str, len)); stwstr++;
      }
   }
   printf ("Total words %d\nStored words %d\n", ttlstr, stwstr);
}

void consider_better_node (nodearr_t* allnodes, node_t* node, nodearr_t *nodes)
{
   unsigned int *inodes, *inode;

   if (node->creator) {
      node->points = node->creator->word->points + ((node->creator->creator)?node->creator->creator->word->points:0);
   } else node->points = 0;

   for (int i = 0; i < nodes->size(); i++) {
         inodes = (unsigned int*)(*nodes)[i]->word; inode = (unsigned int*)node->word;
        
         if ((inodes[0]==inode[0]) && (inodes[1] == inode[1]) && (inodes[2] == inode[2]) && (inodes[3] == inode[3])) {
            if (node->points < (*nodes)[i]->points) {
               delete node;
            } else {
               memcpy((*nodes)[i], node, sizeof(node_t));
            }
            return;
         }
   }
   allnodes->push_back(node);
   nodes->push_back(node); 
   return;
}

void form_nodes (board_t& board, nodearr_t* nodes, word_t* word, nodearr_t* searcharr)
{
   int i,j;
   unsigned int *iboard, *inode;

	for (i = 0; i < SIZE; i++) {
      iboard = (unsigned int*)board[i];

      if (!(iboard[0] || iboard[1] || iboard[2] || iboard[3])) continue;
      node_t* nde = new node_t();
      inode = (unsigned int*)nde->word;

      inode[0] = iboard[0]; inode[1] = iboard[1]; inode[2] = iboard[2]; inode[3] = iboard[3];

		nde->row=i; nde->col=0;
		nde->ori='H';
		nde->len=STRSZ; nde->alen=STRSZ;
      put_present(nde->pres, nde->word);
      nde->creator = word;
      nde->cellno = i * (SIZE);
      consider_better_node(nodes, nde, &searcharr[nde->cellno]);
	}

   for (j = 0; j < SIZE; j++) {

      if (!(board[0][j] || board[1][j] || board[2][j] || board[3][j] ||
            board[4][j] || board[5][j] || board[6][j] || board[7][j] ||
            board[8][j] || board[9][j] || board[10][j] || board[11][j] ||
            board[12][j] || board[13][j] || board[14][j] || board[15][j] )) continue;

      node_t* nde = new node_t();

      nde->word[0] = board[0][j]; nde->word[1] = board[1][j]; nde->word[2] = board[2][j];
      nde->word[3] = board[3][j]; nde->word[4] = board[4][j]; nde->word[5] = board[5][j];
      nde->word[6] = board[6][j]; nde->word[7] = board[7][j]; nde->word[8] = board[8][j];
      nde->word[9] = board[9][j]; nde->word[10] =  board[10][j]; nde->word[11] = board[11][j];
      nde->word[12] = board[12][j]; nde->word[13] = board[13][j]; nde->word[14] = board[14][j];
      nde->word[15] = board[15][j];

		nde->row=0; nde->col=j;
		nde->ori='V';
		nde->len=STRSZ; nde->alen=STRSZ;
      put_present(nde->pres, nde->word); 
      nde->creator = word;
      nde->cellno = j;
      consider_better_node(nodes, nde, &searcharr[nde->cellno]);
   }
}

void prune_dictionary (int* presence, dict_list_t* dict, dictionary_t* prunedict)
{
   dict_iterator_t ditr;
   int pres[PRSSZ];

   prunedict->clear();

   for (ditr = dict->begin(); ditr != dict->end(); ditr++) {
      memcpy(pres, presence, sizeof(pres));
      if (is_throwable(pres, (*ditr)->word)) continue;
      prunedict->push_back((*ditr));
   }
   sort(prunedict->begin(), prunedict->end(), sort_dict());

   printf ("\nDictionary pruned to %d\n", prunedict->size());
}

int mystrcmp(char *word,char *node,int len,int* presence)
{
	int i,alen;
   int pres[PRSSZ]; 

   memcpy(pres, presence, sizeof(pres));
	
	for(alen=0,i=0;i<len; i++) {
		if(!node[i]) {
			if(pres[word[i]]) { alen++; pres[word[i]]--;}
			else return 0;
		}
		else if(node[i] != word[i]) return 0;

	}
   if(alen == 0 || alen == len) return 0;
	return 1;
}

int match (dictword_t* dict, node_t* node, int* pres, matchedwords_t* mwords, word_t* creator)
{
	int i,j,alen, flag = 0; 

   for (i = 'a'; i <= 'z'; i++)
      if ((node->pres[i] + pres[i]) < dict->pres[i]) return 0;
	
	for(i=0; i<= SIZE - dict->wordlen; i++)
	{
			if( (i && node->word[i-1]) || (i+dict->wordlen < SIZE && node->word[i+dict->wordlen])) continue;

			if (mystrcmp(dict->word,&node->word[i],dict->wordlen,pres)) {
            word_t *word = new word_t();
				word->word = dict;
				word->ori=node->ori;
				if(node->ori=='H') {
					word->row=node->row;
					word->col=i;
				}
				else {
					word->row=i;
					word->col=node->col;
				}
            word->creator = creator;
				mwords->push_back(word);
            flag = dict->points + node->points;
			}
	}
	return flag;
}

int main(int argc, char *argv[])
{
   file_t infile, dictfile, outfile;
   char outstr[64];
   int presence[PRSSZ];
   dictionary_t* pdict = new dictionary_t;
   nodearr_t node0, node1, node2;
   nodearr_t san1[SIZE*SIZE], san2[SIZE*SIZE], san0[SIZE*SIZE]; 

   matchedwords_t level1, level2, level3;

   int i, j;
   dictword_t* dptr;
   node_t* nptr;

   /* check for the number of arguments */
   if (argc == 1) return fprintf (stderr, "Usage: %s <input file> [dict file]\n", argv[0]);

   if (infile.file_open(argv[1], "r\0")) return 1; /* input file */
   if (dictfile.file_open("wordlistscrabble.txt", "r\0")) return 2; /* dictionary file */

   sprintf (outstr, "%s.out", argv[1]); /* form output filename */
   if (outfile.file_open(outstr, "w\0")) return 3; /* output file */

   read_input(infile.fptr);
   process_dictionary(dictfile.fptr);
   display_board (board);

   put_present(boardpresence, inputset[0]);
   prune_dictionary (boardpresence, &dictionary, pdict);

   form_nodes (board, &node0, 0, san0);

   memset(presence, 0, sizeof(presence));
   put_present(presence, inputset[0]);

   for (i = 0; i < pdict->size(); i++) {
      dptr = (*pdict)[i];

      for (j = 0; j < node0.size(); j++) {
         nptr = node0[j]; 
         match(dptr, nptr, presence, &level1, 0);
      }
   }
   sort(level1.begin(), level1.end(), sort_word());

   printf ("Level 1 : %d total possibilities out of %d nodes.\n", level1.size(), node0.size());

   put_present(boardpresence, inputset[1]);
   prune_dictionary (boardpresence, &dictionary, pdict);

   memset(presence, 0, sizeof(presence));
   put_present(presence, inputset[1]);

   for (i = 0; i < level1.size(); i++) {
      board_t tbrd; memcpy (tbrd, board, sizeof(board_t));
      place_word (&tbrd, level1[i]->word->word, level1[i]->row, level1[i]->col, level1[i]->ori);
      form_nodes (tbrd, &node1, level1[i], san1);
   }

   for (i = 0; i < pdict->size(); i++) {
      dptr = (*pdict)[i];

      for (j = 0; j < node1.size(); j++) {
         nptr = node1[j]; 
         match(dptr, nptr, presence, &level2, nptr->creator);
      }
   }
   sort(level2.begin(), level2.end(), sort_word());
   printf ("Level 2 : %d total possibilities out of %d nodes.\n", level2.size(), node1.size());

   put_present(boardpresence, inputset[2]);
   prune_dictionary (boardpresence, &dictionary, pdict);

   memset(presence, 0, sizeof(presence));
   put_present(presence, inputset[2]);

   for (i = 0; i < level2.size(); i++) {
      board_t tbrd; memcpy (tbrd, board, sizeof(board_t));
      if (level2[i]->creator)
         place_word (&tbrd, level2[i]->creator->word->word, level2[i]->creator->row, level2[i]->creator->col, level2[i]->creator->ori);
      place_word (&tbrd, level2[i]->word->word, level2[i]->row, level2[i]->col, level2[i]->ori);
      form_nodes (tbrd, &node2, level2[i], san2);
   }
   sort(node2.begin(), node2.end(), sort_node());
   printf ("Level 3 :  %d sorted nodes.\n", node2.size());

   int points = 0, bestpoints = 0;

   for (i = 0; i < pdict->size(); i++) {
      dptr = (*pdict)[i]; 

      if ((node2[0]->points + dptr->points) < bestpoints) break;

      for (j = 0; j < node2.size(); j++) {
         nptr = node2[j]; 
         if ((nptr->points + dptr->points) < bestpoints) break;

         points = match(dptr, nptr, presence, &level3, nptr->creator);  
         if (points > bestpoints) bestpoints = points;
      }
   }

   word_t *bestword = level3[0];
   bestpoints = bestword->word->points + bestword->creator->word->points + bestword->creator->creator->word->points;
   for (i = 0; i < level3.size(); i++) {
      points = level3[i]->word->points + level3[i]->creator->word->points + level3[i]->creator->creator->word->points;

      if (points > bestpoints) {
         bestword = level3[i];
         bestpoints = points;
      }   
   }


   printf ("\n%s (%d) %d %d %c\n", bestword->creator->creator->word->word, bestword->creator->creator->word->points, bestword->creator->creator->row, bestword->creator->creator->col, bestword->creator->creator->ori);
   printf ("%s (%d) %d %d %c\n", bestword->creator->word->word, bestword->creator->word->points, bestword->creator->row, bestword->creator->col, bestword->creator->ori);
   printf ("%s (%d) %d %d %c\n\n", bestword->word->word, bestword->word->points, bestword->row, bestword->col, bestword->ori);

   fprintf (outfile.fptr, "%d,%d,%c,%s\n",bestword->creator->creator->row+1, bestword->creator->creator->col+1, bestword->creator->creator->ori,bestword->creator->creator->word->word);
   fprintf (outfile.fptr, "%d,%d,%c,%s\n",bestword->creator->row+1, bestword->creator->col+1, bestword->creator->ori,bestword->creator->word->word);
   fprintf (outfile.fptr, "%d,%d,%c,%s\n",bestword->row+1, bestword->col+1, bestword->ori,bestword->word->word);

   place_word (&board, bestword->creator->creator->word->word, bestword->creator->creator->row, bestword->creator->creator->col, bestword->creator->creator->ori);
   place_word (&board, bestword->creator->word->word, bestword->creator->row, bestword->creator->col, bestword->creator->ori);
   place_word (&board, bestword->word->word, bestword->row, bestword->col, bestword->ori);

   display_board(board);

   return 0;
}
