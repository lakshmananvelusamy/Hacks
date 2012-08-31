#include <cstdio>
#include <cstdlib>
#include <set>
#include <list>
#include <map>
#include <vector>
#include <algorithm>

using namespace std;

const int SHP = 0; /* shape */
const int CLR = 1; /* color */
const int SPC = 2;

#define INBOARD(R,C) (((R) >= 0) && ((R) < brdsz) && ((C) >= 0) && ((C) < brdsz))
#define ASSEMBLE(R,C) ( (((R) << 4) + (C)) & 0xff )
#define DISASSEMBLE(X,R,C) { (C) = (X) & 0xf; (R) = ((X) >> 4) & 0xf; }

int brdsz;  /* board size */

typedef vector<unsigned char> adjacent_t;
typedef list<unsigned short> path_t;
typedef set<unsigned char> graph_t;

struct cell_t {
   unsigned char coin[2];
   adjacent_t adj[3];
   graph_t* grf[2];

   cell_t() { grf[SHP] = 0; grf[CLR] = 0; }
   cell_t(cell_t& cl) { coin[0] = cl.coin[0]; coin[1] = cl.coin[1]; grf[0] = 0; grf[1] = 0; }
   cell_t& operator= (cell_t& cl) { coin[0] = cl.coin[0]; coin[1] = cl.coin[1]; grf[0] = 0; grf[1] = 0; return cl;}
};

struct board_t {
   cell_t cells[16][16];
   set<unsigned char> nearspace[26];
   graph_t spcgraph;

   list<graph_t*> graphs[2];

   int coincount[26];

   board_t() { memset(&coincount, 0, sizeof(int) * 26); }

   board_t(board_t& bd) { 
      for (int r=0;r<16;r++) for (int c=0;c<16;c++) cells[r][c] = bd.cells[r][c];
      for (int i=0;i<26;i++) nearspace[i]=bd.nearspace[i];
      memcpy(coincount, bd.coincount, sizeof(int)*26);
      spcgraph = bd.spcgraph;
   }

   board_t& operator=(board_t& bd) {
      for (int r=0;r<16;r++) for (int c=0;c<16;c++) cells[r][c] = bd.cells[r][c];
      for (int i=0;i<26;i++) nearspace[i]=bd.nearspace[i];
      memcpy(coincount, bd.coincount, sizeof(int)*26);
      spcgraph = bd.spcgraph; return bd;
   }

   ~board_t() {
      for (list<graph_t*>::iterator gitr = graphs[0].begin(); gitr != graphs[0].end(); gitr++) delete(*gitr);
      for (list<graph_t*>::iterator gitr = graphs[1].begin(); gitr != graphs[1].end(); gitr++) delete(*gitr);
   }
};

int get_board_score (board_t& brd)
{
   int score = 0;
   for (int i = 0; i < 26; i++) {
      score = score + (brd.coincount[i] * 3);
   }
   return score/2;
}

void reset_board (board_t& brd)
{
   list<graph_t*>::iterator glitr;
   int r, c;

   graph_t* spcgraph;

   /* clear the graphs */
   for (glitr = brd.graphs[SHP].begin(); glitr != brd.graphs[SHP].end(); glitr++) delete (*glitr);
   brd.graphs[SHP].clear();
   for (glitr = brd.graphs[CLR].begin(); glitr != brd.graphs[CLR].end(); glitr++) delete (*glitr);
   brd.graphs[CLR].clear();
   
   /* now clean and form the graphs from the board */
   for (r = 0; r < brdsz; r++) {
      for (c = 0; c < brdsz; c++) {
         /* clear the adjacency list and the graphs */
         brd.cells[r][c].adj[SHP].clear(); brd.cells[r][c].adj[CLR].clear(); brd.cells[r][c].adj[SPC].clear();
         brd.cells[r][c].grf[SHP] = 0; brd.cells[r][c].grf[CLR] = 0; 

         if (brd.cells[r][c].coin[SHP] == 'B') continue;

         if (brd.cells[r][c].coin[SHP]) {
            if (INBOARD(r,c-1)) {
               if (brd.cells[r][c].coin[SHP] == brd.cells[r][c-1].coin[SHP]) { /* same shape */
                  brd.cells[r][c].adj[SHP].push_back(ASSEMBLE(r,c-1));
                  brd.cells[r][c-1].adj[SHP].push_back(ASSEMBLE(r,c));

                  if (!(brd.cells[r][c-1].grf[SHP])) {
                     brd.graphs[SHP].push_back( brd.cells[r][c-1].grf[SHP] = new graph_t());
                     brd.cells[r][c-1].grf[SHP]->insert(ASSEMBLE(r,c-1));
                  }
                  brd.cells[r][c].grf[SHP] = brd.cells[r][c-1].grf[SHP];
                  brd.cells[r][c].grf[SHP]->insert(ASSEMBLE(r,c));
               }

               if (brd.cells[r][c].coin[CLR] == brd.cells[r][c-1].coin[CLR]) { /* same color */
                  brd.cells[r][c].adj[CLR].push_back(ASSEMBLE(r,c-1));
                  brd.cells[r][c-1].adj[CLR].push_back(ASSEMBLE(r,c));

                  if (!(brd.cells[r][c-1].grf[CLR])) {
                     brd.graphs[CLR].push_back( brd.cells[r][c-1].grf[CLR] = new graph_t());
                     brd.cells[r][c-1].grf[CLR]->insert(ASSEMBLE(r,c-1));
                  }
                  brd.cells[r][c].grf[CLR] = brd.cells[r][c-1].grf[CLR];
                  brd.cells[r][c].grf[CLR]->insert(ASSEMBLE(r,c));
               }

               if (brd.cells[r][c-1].coin[SHP] == 0) { /* neighbouring cell is an empty cell */
                  brd.cells[r][c].adj[SPC].push_back(ASSEMBLE(r,c-1));
                  brd.cells[r][c-1].adj[SPC].push_back(ASSEMBLE(r,c));
               }
            }

            if (INBOARD(r-1,c)) {
               if (brd.cells[r][c].coin[SHP] == brd.cells[r-1][c].coin[SHP]) { /* same shape */
                  brd.cells[r][c].adj[SHP].push_back(ASSEMBLE(r-1,c));
                  brd.cells[r-1][c].adj[SHP].push_back(ASSEMBLE(r,c));

                  if (!(brd.cells[r-1][c].grf[SHP])) {
                     brd.graphs[SHP].push_back( brd.cells[r-1][c].grf[SHP] = new graph_t());
                     brd.cells[r-1][c].grf[SHP]->insert(ASSEMBLE(r-1,c));
                  }
                  brd.cells[r][c].grf[SHP] = brd.cells[r-1][c].grf[SHP];
                  brd.cells[r][c].grf[SHP]->insert(ASSEMBLE(r,c));
               }

               if (brd.cells[r][c].coin[CLR] == brd.cells[r-1][c].coin[CLR]) { /* same color */
                  brd.cells[r][c].adj[CLR].push_back(ASSEMBLE(r-1,c));
                  brd.cells[r-1][c].adj[CLR].push_back(ASSEMBLE(r,c));

                  if (!(brd.cells[r-1][c].grf[CLR])) {
                     brd.graphs[CLR].push_back( brd.cells[r-1][c].grf[CLR] = new graph_t());
                     brd.cells[r-1][c].grf[CLR]->insert(ASSEMBLE(r-1,c));
                  }
                  brd.cells[r][c].grf[CLR] = brd.cells[r-1][c].grf[CLR];
                  brd.cells[r][c].grf[CLR]->insert(ASSEMBLE(r,c));
               }

               if (brd.cells[r-1][c].coin[SHP] == 0) { /* neighbouring cell is an empty cell */
                  brd.cells[r][c].adj[SPC].push_back(ASSEMBLE(r-1,c));
                  brd.cells[r-1][c].adj[SPC].push_back(ASSEMBLE(r,c));
               }
            }

         } else { /* cell is empty */
            if (INBOARD(r-1,c)) { 
               brd.cells[r][c].adj[SPC].push_back(ASSEMBLE(r-1,c));
               brd.cells[r-1][c].adj[SPC].push_back(ASSEMBLE(r,c));
            }
            if (INBOARD(r,c-1)) { 
               brd.cells[r][c].adj[SPC].push_back(ASSEMBLE(r,c-1));
               brd.cells[r][c-1].adj[SPC].push_back(ASSEMBLE(r,c));
            }
         }
      }
   }
}

int read_board_file (board_t& brd, char *file)
{
   FILE* fptr = fopen(file, "r\0");
   int c = 0, r = 0, i;
   unsigned char shp, clr;

   if (!fptr) return -2;

   for (r = 0; r < brdsz; r++) {
      for (c = 0; c < brdsz; c++) {
         if (fscanf(fptr, "%c,%c;", &shp, &clr) != 2) return -1;

         if (shp == 'A') shp = 0;
         if (clr == 'A') clr = 0;

         brd.cells[r][c].coin[SHP] = shp; brd.coincount[brd.cells[r][c].coin[SHP]-'A']++; 
         brd.cells[r][c].coin[CLR] = clr; brd.coincount[brd.cells[r][c].coin[CLR]-'A']++; 
      }
      fscanf(fptr, "\n");
   }

   return 0;
}
/*
void get_longest_paths_node_r (board_t& brd, int type, list<path_t*>& bestpaths, path_t& stack, char *stkset)
{
   char stk_top = *stack.begin();
   char r,c;
   adjacent_t::iterator itr;

   DISASSEMBLE(stk_top,r,c);
   for (itr = brd.cells[r][c].adj[type].begin(); itr != brd.cells[r][c].adj[type].end(); itr++) {
      if (stkset[(*itr).first]) continue;

      stack.push_front((*itr).first); stkset[(*itr).first] = 1;
      get_longest_paths_node_r(brd, type, bestpaths, stack, stkset);
      stack.pop_front(); stkset[(*itr).first] = 0;
   }

   if (bestpaths.empty() || (stack.size() > (*bestpaths.begin())->size())) {
      for (list<path_t*>::iterator litr = bestpaths.begin(); litr != bestpaths.end(); litr++)
         delete (*litr);
      bestpaths.clear();
      bestpaths.push_back(new path_t(stack));
   } else if (stack.size() == (*bestpaths.begin())->size()) 
     bestpaths.push_back(new path_t(stack)); 
}

void get_longest_paths_node(board_t& brd, int type, char node, list<path_t*>& bestpaths)
{
   path_t stack;
   char stkset[1024];

   stack.push_back(node);
   memset(stkset, 0, 256);
   stkset[node] = 1;
   get_longest_paths_node_r(brd, type, bestpaths,stack, stkset);
}
*/

void get_longest_paths_node (board_t& brd, int type, char node, list<path_t*>& bestpaths)
{
   path_t *stack, *best;
   char stk_top, trow, tcol, new_stk_top;
   set<char> visited;

   new_stk_top = stk_top = node;
   stack = new path_t(); stack->push_front(node);
   best = new path_t(); best->push_back(node); 

   while (1) {
      visited.insert(stk_top);
      DISASSEMBLE(stk_top, trow, tcol);
      for (adjacent_t::iterator itr = brd.cells[trow][tcol].adj[type].begin(); itr !=  brd.cells[trow][tcol].adj[type].end(); itr++) {
         if (visited.find(*itr) == visited.end()) { new_stk_top = *itr; break; }   
      }
      if (new_stk_top == stk_top) {
         if (best->size() == stack->size()) {
            bestpaths.push_back(new path_t(*stack)); 
         } else if (best->size() < stack->size()) {
            delete best; best = new path_t(*stack);
            for (list<path_t*>::iterator litr = bestpaths.begin(); litr != bestpaths.end(); litr++)
               delete (*litr);
            bestpaths.clear();
            bestpaths.push_back(new path_t(*stack)); 
         } 
         stack->pop_front(); 

         if (stack->empty()) break;
         else new_stk_top = stk_top = *(stack->begin());
      } else stack->push_front(stk_top = new_stk_top);
   } 
}

void get_longest_paths (board_t& brd, int type, list<path_t*>& bestpaths)
{
   list<graph_t*>::iterator gitr;
   graph_t::iterator itr;

   for (gitr = brd.graphs[type].begin(); gitr != brd.graphs[type].end(); gitr++) {
      list<path_t*> bpaths;
      for (itr = (*gitr)->begin(); itr != (*gitr)->end(); itr++) {
         list<path_t*> paths;
         get_longest_paths_node(brd, type, (*itr), paths);
         if ((*paths.begin())->size() == (*brd.graphs[type].begin())->size()) {
            for (list<path_t*>::iterator litr = bpaths.begin(); litr != bpaths.end(); litr++) delete (*litr);
            bpaths = paths; break;
         } else if (bpaths.empty()) {
            bpaths = paths;
         } else if ((*bpaths.begin())->size() <= (*paths.begin())->size()) {
            for (list<path_t*>::iterator litr = bpaths.begin(); litr != bpaths.end(); litr++) delete (*litr);
            bpaths = paths;
         } else 
            for (list<path_t*>::iterator litr = paths.begin(); litr != paths.end(); litr++) delete (*litr);
      }
      bestpaths.merge(bpaths);
   }
}

void remove_path (board_t& brd, path_t& pth)
{
   path_t::iterator pitr;
   adjacent_t::iterator aitr;
   graph_t::iterator gitr;
   unsigned char r, c, ar, ac;

   for (pitr = pth.begin(); pitr != pth.end(); pitr++) {
      DISASSEMBLE((*pitr), r, c);
      brd.coincount[brd.cells[r][c].coin[SHP]-'A']--; brd.coincount[brd.cells[r][c].coin[CLR]-'A']--;

      if (INBOARD(r-1,c) && brd.cells[r-1][c].coin[SHP]) {
         brd.nearspace[brd.cells[r-1][c].coin[SHP]-'A'].insert(ASSEMBLE(r-1,c)); 
         brd.nearspace[brd.cells[r-1][c].coin[CLR]-'A'].insert(ASSEMBLE(r-1,c)); 
      }

      if (INBOARD(r+1,c) && brd.cells[r+1][c].coin[SHP]) {
         brd.nearspace[brd.cells[r+1][c].coin[SHP]-'A'].insert(ASSEMBLE(r+1,c));
         brd.nearspace[brd.cells[r+1][c].coin[CLR]-'A'].insert(ASSEMBLE(r+1,c));
      }

      if (INBOARD(r,c-1) && brd.cells[r][c-1].coin[SHP]) {
         brd.nearspace[brd.cells[r][c-1].coin[SHP]-'A'].insert(ASSEMBLE(r,c-1)); 
         brd.nearspace[brd.cells[r][c-1].coin[CLR]-'A'].insert(ASSEMBLE(r,c-1)); 
      }

      if (INBOARD(r,c+1) && brd.cells[r][c+1].coin[SHP]) {
         brd.nearspace[brd.cells[r][c+1].coin[SHP]-'A'].insert(ASSEMBLE(r,c+1)); 
         brd.nearspace[brd.cells[r][c+1].coin[CLR]-'A'].insert(ASSEMBLE(r,c+1)); 
      }

      brd.nearspace[brd.cells[r][c].coin[SHP]-'A'].erase(ASSEMBLE(r,c));
      brd.nearspace[brd.cells[r][c].coin[CLR]-'A'].erase(ASSEMBLE(r,c));
      brd.cells[r][c].coin[SHP] = 0; brd.cells[r][c].coin[CLR] = 0;
      brd.spcgraph.insert(ASSEMBLE(r,c));
   }
}

bool is_col_empty(board_t& brd, int r1, int r2, int c)
{
   int i;
   
   if (r1 > r2) { i = r2; r2 = r1; r1 = i; }

   for (i = r1; i <= r2; i++) 
      if (brd.cells[i][c].coin[SHP]) return false;
   return true;
}

bool is_row_empty(board_t& brd, int r, int c1, int c2)
{
   int i;

   if (c1 > c2) { i = c2; c2 = c1; c1 = i; }

   for (i = c1; i <= c2; i++) 
      if (brd.cells[r][i].coin[SHP]) return false;
   return true;
}

bool is_adjacent(board_t& brd, unsigned char node1, unsigned char node2)
{
   int r1,c1,r2,c2;
   int i;

   DISASSEMBLE(node1,r1,c1); DISASSEMBLE(node2,r2,c2);

   if (r1 == r2) if (is_row_empty(brd, r1, ((c1>c2)?c2:c1)+1, ((c1>c2)?c1:c2)-1)) return true;
   if (c1 == c2) if (is_col_empty(brd, ((r1>r2)?r2:r1)+1, ((r1>r2)?r1:r2)-1, c1)) return true;

   if (c1 > c2)  { i = r2; r2 = r1; r1 = i; i = c2; c2 = c1; c1 = i; }

   /* check for ninety degree */
   if (r1 != r2 && c1 != c2) {
      if ( is_row_empty (brd, r1, c1+1, c2-1) && (!brd.cells[r1][c2].coin[SHP]) &&
           is_col_empty (brd, ((r1>r2)?r2:r1)+1, ((r1>r2)?r1:r2)-1, c2)) return true;

      if ( is_row_empty (brd, r2, c1+1, c2-1) && (!brd.cells[r2][c1].coin[SHP]) &&
           is_col_empty (brd, ((r1>r2)?r2:r1)+1, ((r1>r2)?r1:r2)-1, c1)) return true;
   }

   for (i = 0; i < brdsz; i++) {
      if (i == r1 || i == r2 ) continue;
      if (is_row_empty(brd, i, ((c1>c2)?c2:c1), ((c1>c2)?c1:c2))) {
         if (i < r1 && i < r2) {
            if (is_col_empty(brd, i, r1-1,c1) && is_col_empty(brd,i,r2-1,c2)) return true;
         }
         if (i > r1 && i > r2) {
            if (is_col_empty(brd,r1+1,i,c1) && is_col_empty(brd,r2+1,i,c2)) return true;
         }
         if (r1 < r2 && i > r1 && i < r2) {
            if (is_col_empty(brd,r1+1,i,c1) && is_col_empty(brd,i,r2-1,c2)) return true;
         }
         if (r1 > r2 && i > r2 && i < r1) {
            if (is_col_empty(brd,i,r1-1, c1) && is_col_empty(brd,r2+1,i,c2)) return true;
         }
      }
   }

   for (i = 0; i < brdsz; i++) {
      if (i == c1 || i == c2 ) continue;
      if (is_col_empty(brd, ((r1>r2)?r2:r1), ((r1>r2)?r1:r2), i)) {
         if (i < c1 && i < c2) {
            if (is_row_empty(brd, r1,i,c1) && is_col_empty(brd,r2,i,c2)) return true;
         }
         if (i > c1 && i > c2) {
            if (is_row_empty(brd,r1,c1,i) && is_col_empty(brd,r2,c2,i)) return true;
         }
         if (i > c1 && i < c2) {
            if (is_row_empty(brd,r1,c1-1,i) && is_col_empty(brd,r2,i,c2+1)) return true;
         }
      }
   }

   return false;
}

void reset_graph_space (board_t& brd)
{
   int r, c, i;
   brd.spcgraph.clear();
   for (i = 0; i < 26; i++) brd.nearspace[i].clear();

   for (r = 0; r < brdsz; r++) {
      for (c = 0; c < brdsz; c++) {
         if (brd.cells[r][c].coin[SHP]) continue;
         if (INBOARD(r-1,c) && brd.cells[r-1][c].coin[SHP]) {
            brd.nearspace[brd.cells[r-1][c].coin[SHP]-'A'].insert(ASSEMBLE(r-1,c));
            brd.nearspace[brd.cells[r-1][c].coin[CLR]-'A'].insert(ASSEMBLE(r-1,c));
         }
         if (INBOARD(r+1,c) && brd.cells[r+1][c].coin[SHP]) {
            brd.nearspace[brd.cells[r+1][c].coin[SHP]-'A'].insert(ASSEMBLE(r+1,c));
            brd.nearspace[brd.cells[r+1][c].coin[CLR]-'A'].insert(ASSEMBLE(r+1,c));
         }
         if (INBOARD(r,c-1) && brd.cells[r][c-1].coin[SHP]) {
            brd.nearspace[brd.cells[r][c-1].coin[SHP]-'A'].insert(ASSEMBLE(r,c-1));
            brd.nearspace[brd.cells[r][c-1].coin[CLR]-'A'].insert(ASSEMBLE(r,c-1));
         }
         if (INBOARD(r,c+1) && brd.cells[r][c+1].coin[SHP]) {
            brd.nearspace[brd.cells[r][c+1].coin[SHP]-'A'].insert(ASSEMBLE(r,c+1));
            brd.nearspace[brd.cells[r][c+1].coin[CLR]-'A'].insert(ASSEMBLE(r,c+1));
         }
         brd.spcgraph.insert(ASSEMBLE(r,c));
      }
   }
}

void form_new_graphs (board_t& brd)
{
   int i, r1, c1, r2, c2, r3, c3;
   set<unsigned char>::iterator sitr1, sitr2;
   graph_t::iterator gitr;
   graph_t* grf;


   for (i = ('R'-'A'); i <= ('Z'-'A'); i++) {
      if (brd.nearspace[i].size() <= 1) continue;
      for (sitr1 = brd.nearspace[i].begin(); sitr1 != brd.nearspace[i].end(); sitr1++) {
         sitr2 = sitr1; sitr2++;
         DISASSEMBLE((*sitr1), r1, c1);
         for (; sitr2 != brd.nearspace[i].end(); sitr2++) {
            DISASSEMBLE((*sitr2), r2, c2);
            if (find(brd.cells[r1][c1].adj[SHP].begin(), brd.cells[r1][c1].adj[SHP].end(), (*sitr2)) != brd.cells[r1][c1].adj[SHP].end())  continue;
            if (!is_adjacent(brd,(*sitr1), (*sitr2))) continue;
            brd.cells[r1][c1].adj[SHP].push_back(*sitr2); brd.cells[r2][c2].adj[SHP].push_back(*sitr1);
            if (brd.cells[r1][c1].grf[SHP] && brd.cells[r1][c1].grf[SHP] == brd.cells[r2][c2].grf[SHP]) continue;
            grf = brd.cells[r2][c2].grf[SHP];
            if (!brd.cells[r1][c1].grf[SHP]) {
               brd.graphs[SHP].push_back(brd.cells[r1][c1].grf[SHP] = new graph_t());
               brd.cells[r1][c1].grf[SHP]->insert((*sitr1));
            }
            if (!grf) {
               brd.cells[r2][c2].grf[SHP] = brd.cells[r1][c1].grf[SHP];
               brd.cells[r2][c2].grf[SHP]->insert(*sitr2); continue;
            }
            for (gitr = grf->begin(); gitr != grf->end(); gitr++) {
               brd.cells[r1][c1].grf[SHP]->insert((*gitr)); DISASSEMBLE((*gitr), r3,c3);
               brd.cells[r3][c3].grf[SHP] = brd.cells[r1][c1].grf[SHP];
            }
            brd.graphs[SHP].remove(grf); delete grf;
         }
      }
   }

   for (i = ('E'-'A'); i <= ('K'-'A'); i++) {
      if (brd.nearspace[i].size() <= 1) continue;
      for (sitr1 = brd.nearspace[i].begin(); sitr1 != brd.nearspace[i].end(); sitr1++) {
         sitr2 = sitr1; sitr2++;
         DISASSEMBLE((*sitr1), r1, c1);
         for (; sitr2 != brd.nearspace[i].end(); sitr2++) {
            DISASSEMBLE((*sitr2), r2, c2);
            if (find(brd.cells[r1][c1].adj[CLR].begin(), brd.cells[r1][c1].adj[CLR].end(), (*sitr2)) != brd.cells[r1][c1].adj[CLR].end())  continue;
            if (!is_adjacent(brd, (*sitr1), (*sitr2))) continue;
            brd.cells[r1][c1].adj[CLR].push_back(*sitr2); brd.cells[r2][c2].adj[CLR].push_back(*sitr1);
            if (brd.cells[r1][c1].grf[CLR] && brd.cells[r1][c1].grf[CLR] == brd.cells[r2][c2].grf[CLR]) continue;
            grf = brd.cells[r2][c2].grf[CLR];
            if (!brd.cells[r1][c1].grf[CLR]) {
               brd.graphs[CLR].push_back(brd.cells[r1][c1].grf[CLR] = new graph_t());
               brd.cells[r1][c1].grf[CLR]->insert((*sitr1));
            }
            if (!grf) {
               brd.cells[r2][c2].grf[CLR] = brd.cells[r1][c1].grf[CLR];
               brd.cells[r2][c2].grf[CLR]->insert(*sitr2); continue;
            }
            for (gitr = grf->begin(); gitr != grf->end(); gitr++) {
               brd.cells[r1][c1].grf[CLR]->insert((*gitr)); DISASSEMBLE((*gitr), r3,c3);
               brd.cells[r3][c3].grf[CLR] = brd.cells[r1][c1].grf[CLR];
            }
            brd.graphs[CLR].remove(grf); delete grf;
         }
      }
   }
}

void display_board(board_t& brd)
{
   int r, c;

   for (c = 0; c < 26; c++) {
      if (brd.coincount[c]) printf ("%c(%d) ", c + 'A', brd.coincount[c]);
   }

   printf ("\n");
   for (c = 0; c < brdsz; c++) {
      printf ("-----");
   }
   printf ("-\n");

   for (r = 0; r < brdsz; r++) {
      for (c = 0; c < brdsz; c++) {
         printf ("| %c%c ", brd.cells[r][c].coin[SHP], brd.cells[r][c].coin[CLR]);
      }
      printf ("|\n");
      for (c = 0; c < brdsz; c++) {
         printf ("-----");
      }
      printf ("-\n");
   }
   printf ("graphs(%d,%d) score(%d)\n\n", brd.graphs[SHP].size(), brd.graphs[CLR].size(), get_board_score(brd));
}

void display_path (path_t& pth, FILE *fptr)
{
   path_t::iterator pitr;
   int r,c;

   if (!fptr) printf ("path: ");
   for (pitr = pth.begin(); pitr != pth.end(); pitr++) {
      DISASSEMBLE((*pitr), r,c);
      printf ("%d,%d; ", r,c); 
      if (fptr) fprintf (fptr, "%d,%d;", r+1, c+1);
   }
   printf ("\n\n");
   if(fptr) fprintf (fptr, "\n");
}

void clean_paths(list<path_t*>& paths)
{
   list<path_t*>::iterator itr;

   for (itr = paths.begin(); itr != paths.end(); itr++)
      delete (*itr);

   paths.clear();
}

bool get_longest_path(list<path_t*>& paths, path_t& bestpath)
{
   list<path_t*>::iterator itr;
   path_t* bpath = *paths.begin();

   if (paths.empty()) return false;

   for (itr = paths.begin(); itr != paths.end(); itr++) {
      if (bpath->size() < (*itr)->size()) bpath = (*itr); 
   }

   bestpath = (*bpath);

   return true;
}


bool get_best_path (board_t& inbrd, int* bestscore, int myscore, int remaininglevel, list<path_t*>& paths, path_t& bestpath) 
{
   bool flag = false;

   remaininglevel--; 
   for (list<path_t*>::iterator pitr = paths.begin(); pitr != paths.end(); pitr++) {
      board_t brd = inbrd;
      list<path_t*> bpaths;

      remove_path(brd, (**pitr));
      reset_board(brd);
      form_new_graphs(brd);

      get_longest_paths (brd, SHP, bpaths);
      get_longest_paths (brd, CLR, bpaths);

      if (bpaths.empty() || remaininglevel == 0) {
         myscore = myscore + get_board_score(inbrd);
         if (remaininglevel == 0) myscore = myscore - ((**pitr).size() * 3);
         if (*bestscore > myscore) { 
            bestpath.clear(); bestpath = (**pitr);
            *bestscore = myscore; return true;
         } return false;
      } else {
         if (get_best_path (brd, bestscore, myscore+5, remaininglevel,bpaths,bestpath)) {
            bestpath.clear();
            bestpath = (**pitr);
            flag = true;
         }
         clean_paths(bpaths);
      }
   }

   return flag;
}

int main(int argc, char* argv[])
{
   board_t brd, bbrd;
   list<path_t*> bpaths;
   path_t bestpath;
   int pct = 0;
   int bestscore = 0x0fffffff;
   char ofname[256];
   FILE *fptr;

   path_t emptypath;

   if (argc != 3) {
      fprintf (stderr, "Usage : %s <board_size> <board_file>\n", argv[0]);
      return -1;
   }

   brdsz = atoi(argv[1]);

   if (read_board_file(brd, argv[2])) {
      fprintf (stderr, "Error reading the file\n");
      return -1;
   }
   
   sprintf (ofname, "%s.out\0", argv[2]);
   if ((fptr = fopen(ofname,"w\0")) == NULL) {
      fprintf (stderr, "Error opening file %s for writing\n", ofname);
      return -3;
   }

   bbrd = brd;

   reset_graph_space(brd);
   reset_board(brd);
   form_new_graphs(brd);
   display_board(brd);

   get_longest_paths (brd, CLR, bpaths);
   get_longest_paths (brd, SHP, bpaths);

 //  while (get_best_path(brd, &bestscore, 0, 3, bpaths, bestpath)) {
  while (get_longest_path(bpaths, bestpath)) {
      display_path(bestpath, fptr);
      remove_path(brd, bestpath); pct++;
      clean_paths(bpaths);
      reset_board(brd);
      form_new_graphs(brd);
      display_board(brd); 
      get_longest_paths (brd, CLR, bpaths);
      get_longest_paths (brd, SHP, bpaths);
      bestscore = 0x0fffffff;
   }
   printf ("Total_score(%d) Chains(%d) Coins_in_board(%d)\n\n", get_board_score(brd)+pct*5, pct, get_board_score(brd)/3);
   printf ("****** REFER %s FOR OUTPUT ****** \n", ofname);
}
