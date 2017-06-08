#ifndef scanner_h
#define scanner_h

#include <string>
#include <fstream>
#include "names.h"
void showError(const char* mess);
using namespace std;

typedef enum {
	namesym, numsym, devsym, xorsym, siggensym, gatesym, bitsersym, keysym, clksym, swisym,
	consym, dtypesym, outsym, insym, monsym, equals, semicol, dot, connect_, badsym, eofsym
} symbol;


class scanner {

	names* nmz;
	char curch;
	char* url;
	bool eofile;
	ifstream inf;
	string curline;
	string signalstr;
	
	void getch(void);
	/* Gets the next character from the input file						*/
	/* Counts how many lines it read from the file for error 			*/
	/* handling purposes 												*/
	
	void skipspaces(void);
	/* Skips spaces from the input file									*/
	
	void getnumber(int& num);
	/* Reads a number - unsigned int from the input file 				*/
	/* Sets the value of num equal to this value 						*/
	
	bool skipcomments(void);
	/* Ignores comments from the input file 							*/
	/* Comments start with // and finish at the end of the line 		*/
	/* It returns TRUE (1) if only one back-slash was found and not two,*/
	/* which makes the comment syntax invalid 							*/
	
	void getname(name& id);
	/* Gets a name from the input file and finds its id from the names  */
	/* table. If the name is not in the table, it automatically adds it */
	/* It also constricts the length of the string to maxlength characters */
	
	void getbitseries(string& signalstr);
	/* Reads a series of bits (0s and 1s) from the input file 			*/
	/* Stores the series in the string signalstr 						*/


public:

	int counter = 1;
	
	string geterror(void);
	/* Reads the current line from the input file 						*/
	/* and returns it as string to be printed in an error message 		*/
	 

	void setURL(const char* url);
	/* Sets the url for the read file  									*/
    
	void initialise(names* names_mod);
	/* Opens the file and reads the first character, used for errors	*/
	
	void getsymbol(symbol& s, name& id, int& num, string &signalstr);
	/* s = reads the next symbol from the file 							*/
	/* Skips comments and empty spaces						  			*/
	/* s = the type of symbol read from a file 							*/
	/* id = return the name id here if it is a name 					*/
	/* num = return the value here if it is a number 					*/

	~scanner(void); 
	/* destructor of the class, closes input file						*/

	scanner(names* names_mod, const char* defname);
	/* The constructor. This procedure is called at system initialisation*/
	/* before any of the above procedures/functions are used. 			*/
	/* Opens the file and reads first character                 		*/

};

#endif /* scanner_h */
