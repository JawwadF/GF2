#ifndef scanner_h
#define scanner_h

#include <string>
#include <fstream>
#include "names.h"

//using namespace std;

typedef enum {namesym, numsym, devsym, xorsym,gatesym,keysym,clksym,swisym,consym,dtypesym,cirsym,outsym,insym,monsym,equals,semicol,dot,connect,badsym,eofsym} symbol;

class scanner{

  /* put stuff that the class uses internally here */
  names* nmz;
  char curch;   /*current character*/
  bool eofile;  /*TRUE when end of file is reached */
  ifstream inf; /*text file that is read */
  
  void getch(void);
    /* gets the next character from the file */
    /* and stores it in curch */
    /* it also updates eofile */

  void skipspaces(void);
    /* skips the spaces and new lines in the input file*/

  void getnumber(int& num);
    /* reads the number (integer greater than 1) from the file */
    /* and returns it as an 'int' in variable num */

  bool skipcomments(void);
    /* skips comments from the input file */
    /* comments start with // and finish at the end of line */  

  void getname(name& id);
    /* reads a name from the input file */
    /* names start with a letter and continue with letters or digits */
    /* names finish at the space */
    /* it returns the id from the names class */

public:

  void getsymbol(symbol& s, name& id, int& num);
    /* s = the sort of symbol read from a file */
    /* id = return the name id here if it is a name */
    /* num = return the value here if it is a number */

  ~scanner(void); /* destructor of the class, closes input file */

  scanner (names* names_mod, const char* defname);
    /* names initialises the name table.  This procedure is called at      */
    /* system initialisation before any of the above procedures/functions  */
    /* are used.                                                           */
};

#endif /* scanner_h */
