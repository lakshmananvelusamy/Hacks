To Run the program :
 
Important:"The Program runs only in unix/linux platforms"


1. Unzip the submitted zip file.
 
2. Copy the file named "script" and the three c files "main.c" "splitfinder.c" "ascii.c" to a directory.

3. The script is a shell script and it takes two arguments

        1) Decoder.txt
	2) CodedFile.txt

4. Provide execute permission to script. ( $chmod +x script ).
                          
5. Run the script. At the shell prompt $./script Decoder.txt CodedFile.txt


All C files and thier binaries and the script file is included with this.


Output:


The output is written to three files :

 1. output1.txt --> The unscrambled file containing the decoded text, including all the words that could not be decoded and invalid 			            strings

 2.output2.txt -->  Unknown splits that were discovered

 
 3.output3.txt -->  The invalid strings that were found, in the same order in which they appear in the input file.	 		


Program Explanation:
              
         1. First, we put line numbers for the codedfile.
	 2. Find the length of the scrambled words and write it to file and sort it according to length.
	 3. Find the length of the correct words in decoder file and sort it according to length.
	 4. A "C" program, (ascii.c) calculates the ascii sum of each word in coded file and decoder file and writes it to file.
	 5. Sort according to the ascii sum, maintaining the sorted order of length and write it to file.
	   (i.e., within the sorted length, sort according to ascii sum).
	 6. "Main.c" reads from the sorted files and searches only in blocks where length and ascii sum matches.
	 7. If Split is present, determines whether the scrambled word can be changed to correct word or not, based on that write it
	    as invalid string or valid string.
	 8. If split is not present, determine whether a split can be formed, if so write that word to split.txt
	 9. "Split.txt" contains all words that can be made into valid strings but yet to find the split.
 	 10."Split.c" : Based on a set of scrambled words (whose right word is known) a common split is found out such that, that                       particular set of scarmbled words can be transformed into valid strings using that particular one split.
	 11.Everything is written to files.
	 12.Sorting according to the given order is done finally.