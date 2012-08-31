#include <map>
#include <string>
#include <vector>
#include <list>
#include <utility>
#include <algorithm>

#include <cstdlib>
#include <cstdio>
#include <cstring>

const unsigned int MIN_ARG = 3; // Minimum number of argument
const unsigned int ARG_DICTFILE = 1; // argv[1] => decoder file
const unsigned int ARG_INFILE = 2; // argv[2] => input file to be unscrambled 

const unsigned int STR_LIMIT = 256; // Max characters in a line

const unsigned int MAPS = 8; // number of maps.

const unsigned int DECODER_ALLOC_SIZE = 1024; // Alloc size of  decoder words at one new operator;
const unsigned int CODED_ALLOC_SIZE = 4096; // Alloc size of  coded words at one new operator;

using namespace std; //use standard namespace

// macro for checking the new return pointer
#define CHK_ALLOC(X) { if(!(X)) { fprintf (stderr, "Not enough memory\n"); exit(3);}}

// macro for map finder
#define H_MAPS(X)   ((X)%MAPS)

struct chararray_t {
   char string[STR_LIMIT]; //store a string

   chararray_t(void) { // constructor 
      memset(string, 0, STR_LIMIT); // init everything with 0
   }

   chararray_t(const char* str) { // constructor for STL use
      memset(string, 0, STR_LIMIT);
      strcpy(string, str); // copy target string to its string array
   }

   chararray_t(const chararray_t& carr) {
      memset(string, 0, STR_LIMIT);
      strcpy(string, carr.string); // copy target string to its string array
   }
}; 

struct decoder_word_t {
   chararray_t word; // Actual dictionary word
   vector<chararray_t> splits; // each part of split as a element in vector
   bool is_splited; // flags whether the word is splited or not; stored for faster processing
};

struct coded_word_t;

struct mapper_t { // meta data for the same sorted string; liar and rail has the same sorted string. Such string will be here.
   vector<decoder_word_t*> decoder; // list of decoder words that have the same sorted string
   vector<coded_word_t*> unmatched; // list of unmatched coded words that have the same sorted string

   int unsplited; // count of unsplited words that have the same sorted string

   mapper_t (void) { unsplited = 0; } // constructor; what can I say more about it.
};

struct coded_word_t {
   chararray_t word; // actual coded word

   mapper_t* map_ptr; // pointer to the mapper structure
   decoder_word_t* dcdr_ptr; // pointer to the decoded word, if any 
   
   coded_word_t(void) { map_ptr = 0; dcdr_ptr = 0;}  // constructor as usual. 
};

struct cmpstr // helper functor or function object for map
{ bool operator()(const string s1, const string s2) const {
    return (s1 < s2);
  }
};

map<string, mapper_t,  cmpstr> alldata[MAPS]; // map for storing the mapping between the sorted string and mapper meta data
list<coded_word_t*> coded_text; // list of coded words
typedef map<string, mapper_t,  cmpstr>::iterator map_iterator_t; // type defs for the map iterator

void vectorize_split(char* splitstr, vector<chararray_t>& vsplit) // vectorize the ab-c-d split as a vector of string like ab,c,d
{
   char *str, *tok; 
   
   str = splitstr; tok = strtok(str, "-\0"); // tokenize word based on -
   while (tok != NULL) {
      vsplit.push_back(tok); // push the word into the vector
      tok = strtok(NULL, "-\0");  // pick next token
   }
}

void process_decoder_file(char *filename)
{
   char word[STR_LIMIT], split[STR_LIMIT], *splited;
   FILE *fptr;
   decoder_word_t *dictword, *dictword_grp;
   mapper_t* maptr;
   int i = 0;
   int wordct =0, unsplit_ct = 0;
   int wordlen = 0;

   fptr = fopen(filename, "r\0");
   if (fptr == NULL) { 
      fprintf (stderr, "Unable to read file %s\n", filename);
      exit(2);
   }

   while (!feof(fptr)) {
      memset(word, 0, STR_LIMIT); memset (split, 0, STR_LIMIT);

      fscanf (fptr, "%s %s\n", word, split); word[STR_LIMIT-1]='\0'; split[STR_LIMIT-1]='\0';
      if (!strlen(word) || word[0] == '\n') continue; //because of uncertainty in the text file format (unix / dos)

      if (!i) { // call new operator once for DECODER_ALLOC_SIZE words. (C) (null) Team 2008
         CHK_ALLOC(dictword_grp = new (nothrow) decoder_word_t[DECODER_ALLOC_SIZE]);
         i = DECODER_ALLOC_SIZE;
      }
      dictword = (decoder_word_t *)(&dictword_grp[--i]); // pointer to one of the pre allocated decoder word

      strcpy(dictword->word.string, word);
      dictword->is_splited = (split[0]=='?')?false:true; // find splited or not

      if (dictword->is_splited) vectorize_split(split, dictword->splits); 
      else unsplit_ct++;

      wordct++; wordlen = strlen(word);
      sort(word, word+wordlen); // sort word

      maptr = &((alldata[H_MAPS(wordlen)])[word]); // get a pointer to the mapper object
      maptr->decoder.push_back(dictword);  // push decoder word pointer into the mapper object's decoder vector
      if (!dictword->is_splited) maptr->unsplited++; // increment the mapper's unsplited count if unsplited
   }

   fclose (fptr);
   
   printf ("Processing decoder file (%s) - Completed successfully. Total words read is %d and unsplit ones read is %d\n", filename, wordct, unsplit_ct);
}

bool find_length_splits (char* word, vector<chararray_t>& scrambles,  char* splitstr, bool &is_ambiguous)
{
   int i, j, k;

   bool is_splited = false;
   
   char *tword = strdup(word); // temp string for easy processing
   char *tstr;
   int wlen = strlen(word);
   int tlen;

   for (i=0; i < wlen; i++) { // loop for all letters in the target word or the decoder word
      for (j = wlen; j > i; j-- ) { // come from tail end so that lengthy split parts are easily found.
         tword[j] = '\0';
         for (k=0; ((k<scrambles.size()) && (strstr(scrambles[k].string, &tword[i]) !=NULL)); k++);  // split part should occur in all coded words
         
         if (k == scrambles.size()) { // split part occurs in all coded words
            sprintf(splitstr, "%s-%s",splitstr, &tword[i]); // append split part to the split string
            tlen = strlen(&tword[i]); 

            for (k=0; k<scrambles.size(); k++) { // replace the splited parts with the - in the scrambles
               tstr =  strstr(scrambles[k].string, &tword[i]);
               memset (tstr, '-', tlen);
            }

            i += (tlen - 1); // move pointer to the start of the new probable split part. -1 will be compensated by the for loop i++.
            if (tlen >= 2) is_splited = true; // only a non singular char splits are valid
            if (tlen > 2) is_ambiguous = true; // mark ambiguous if a part of the split has more than 2 chars. 
            break; 
         }
      }

      strcpy(tword, word);
   }

   free (tword); // free whatever you have allocated
   memcpy (splitstr, &splitstr[1], strlen(splitstr)); // crude way to remove the first - in the split string eg. -ab-c-d to ab-c-d;

   if (is_splited || (strlen(word) == 1)) return true; // consider only the valid splits
   else return false;
}

bool is_match (char* word, vector<chararray_t>& splits, int splitnum)
{
   if (splitnum == 0) return true;

   const char *splitptr = splits[splitnum-1].string;
   char *ptr = word;
   int len = strlen(splitptr);


   while ((ptr = strstr (ptr, splitptr)) != NULL) {
      memset (ptr, '-', len); // replace the matched string with -
      if(is_match (word, splits, splitnum-1)) return true; // recurs for the splitnum-1 split parts 
      else { memcpy (ptr, splitptr, len); ptr++; } // put things back, replace - with the matched split part
   }
 
   return false;
}

void process_coded_file (char *filename)
{
   char word[STR_LIMIT], sortwd[STR_LIMIT];
   FILE *fptr;
   coded_word_t *coded_word, *coded_word_grp;
   int i=0, wordlen =0, wordct = 0;
   map_iterator_t itr;

   fptr = fopen(filename, "r\0");
   if (fptr == NULL) {
      fprintf (stderr, "Unable to read file %s\n", filename);
      exit(2);
   }

   while (!feof(fptr)) {
      memset (word, 0, STR_LIMIT);
      fscanf (fptr, "%s\n", word); word[STR_LIMIT-1]='\0';

      if (!strlen(word) || word[0] == '\n') continue; //because of uncertainty in the text file format (unix / dos)

      if (!i) { // call new operator once for CODED_ALLOC_SIZE words. (C) (null) Team 2008
         CHK_ALLOC(coded_word_grp = new (nothrow) coded_word_t[CODED_ALLOC_SIZE]);
         i = CODED_ALLOC_SIZE;
      }
      coded_word = (coded_word_t *)(&coded_word_grp[--i]);
      strcpy(coded_word->word.string, word);

      strcpy(sortwd, word);
      wordlen = strlen(word);
      sort(sortwd, sortwd+wordlen);

      wordct++; 

      itr = alldata[H_MAPS(wordlen)].find(sortwd);
      if (itr != alldata[H_MAPS(wordlen)].end()) { // sorted string is there in the map of the mapper meta data.
         coded_word->map_ptr = &(itr->second);
         int j;
         for (j=0;j<coded_word->map_ptr->decoder.size(); j++)   {
            decoder_word_t *dcoder = coded_word->map_ptr->decoder[j]; // loop thru each decoder word in the mapper meta data
            if (!dcoder->is_splited) continue; // consider only the splited decoder words

            if (is_match(word, dcoder->splits, dcoder->splits.size())) { // hmm, matched with one decoder word
               coded_word->dcdr_ptr = dcoder;  // store the matched decoder word pointer for later processing
               break;
            }
         }
         
         if (!coded_word->dcdr_ptr && (coded_word->map_ptr->unsplited == 1)) { // store as unmatched only is there is only one unsplited decoder
            coded_word->map_ptr->unmatched.push_back(coded_word);
         } else if (!coded_word->dcdr_ptr) {// unmatched and there is no or more than 1 unsplited decoder; so it is a invalid word 
            coded_word->map_ptr = NULL; // reset the map_ptr
         }
      }

      coded_text.push_back(coded_word); // have this in the list of coded word irrespective of any other details
   }

   fclose(fptr);
   printf ("Pre processing encoded file (%s) - Completed successfully. Total scrambled words read is %d\n", filename, wordct);
}

int main(int argc, char *argv[])
{
   list<coded_word_t*>::iterator itr;
   bool is_ambiguous = false;
   FILE *unscram_fptr, *split_fptr, *invld_fptr, *ambi_fptr;
   int unscram_ct = 0, split_ct = 0, invld_ct = 0, ambi_ct = 0;

   if (argc < MIN_ARG) {
      fprintf (stderr, "Usage: %s <decoder file> <inputfile>\n", argv[0]);
      exit (1);  
   }

   unscram_fptr = fopen("unscrambled.txt", "w\0");
   if (unscram_fptr == NULL) {
      fprintf (stderr, "Unable to write file unscrambled.txt\n");
      exit(2);
   }
 
   split_fptr = fopen("splited.txt", "w\0");
   if (unscram_fptr == NULL) {
      fprintf (stderr, "Unable to write file splited.txt\n");
      exit(2);
   }

   invld_fptr = fopen("invalid.txt", "w\0");
   if (invld_fptr == NULL) {
      fprintf (stderr, "Unable to write file invalid.txt\n");
      exit(2);
   }
 
   ambi_fptr = fopen("ambiguous.txt", "w\0");
   if (ambi_fptr == NULL) {
      fprintf (stderr, "Unable to write file ambiguous.txt\n");
      exit(2);
   }

   process_decoder_file (argv[ARG_DICTFILE]);
   process_coded_file (argv[ARG_INFILE]);

   for (itr = coded_text.begin(); itr != coded_text.end(); itr++) { // list thru coded words for post processing
      if ((*itr)->map_ptr && !((*itr)->dcdr_ptr)) { // mapper is there but no decoder matched. probably they decoder is not splited
         mapper_t* mptr = (*itr)->map_ptr;
             
         if (mptr->unsplited > 1) { // more than one decoder words are unsplited. so this coded word is considered invalid 
            invld_ct++;
            fprintf (invld_fptr, "%s\n", (*itr)->word.string);
            continue;
         }

         vector<chararray_t> scrambles;
         vector<chararray_t> solution;
         chararray_t target, splitstr;
         decoder_word_t* ditr;
         coded_word_t* citr;
         int d, c;

         for (c = 0; c < mptr->unmatched.size(); c++) {
            citr =  mptr->unmatched[c];
            scrambles.push_back(citr->word.string); // prepare a list of scrambled words
         }

         for (d = 0; d < mptr->decoder.size(); d++) {
            ditr = mptr->decoder[d];
            if (!ditr->is_splited) {  // take the unsplited word as the target.
               strcpy(target.string, ditr->word.string);
               break;
            }
         }

         if (find_length_splits(target.string, scrambles, splitstr.string, is_ambiguous)) { // decoder word is splited
            for (c = 0; c < mptr->unmatched.size(); c++) {
               citr =  mptr->unmatched[c];
               citr->dcdr_ptr = ditr; // map unmatched coded word to the recently splited decoder word
            }

            fprintf (unscram_fptr, "%s\n", ditr->word.string); unscram_ct++;
            fprintf (split_fptr, "%s %s\n", ditr->word.string, splitstr.string); split_ct++;
         } else {
            fprintf (invld_fptr, "%s\n", (*itr)->word.string); invld_ct++;
         }
      
         if (is_ambiguous) { // write ambiguous words into more file
            fprintf (ambi_fptr, "%s\n", (*itr)->dcdr_ptr->word.string); ambi_ct++;
            is_ambiguous = false;
         }

      } else if ((*itr)->map_ptr && ((*itr)->dcdr_ptr))  { // matched word
         fprintf (unscram_fptr, "%s\n", (*itr)->dcdr_ptr->word.string); unscram_ct++;
      } else if (!(*itr)->map_ptr) { // clearly invalid coded word as there is no mapper found
         fprintf (invld_fptr, "%s\n", (*itr)->word.string); invld_ct++;
      }
   }

   printf ("Total of %d words are unscrambled and written to unscrambled.txt\n", unscram_ct);
   printf ("Total of %d words are splited and written to splited.txt\n", split_ct);
   printf ("\tTotal of %d words are rated as ambiguous splits and written to ambiguous.txt\n", ambi_ct);
   printf ("Total of %d words are found invalid and written to invalid.txt\n", invld_ct);
}
