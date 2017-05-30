#ifndef scanner_h
#define scanner_h

#include <string>
#include <fstream>
#include "names.h"

//using namespace std;

typedef enum {namesym, numsym, devsym, xorsym,gatesym,keysym,clksym,swisym,
consym,dtypesym,cirsym,outsym,insym,monsym,equals,semicol,dot,connect_,badsym,eofsym} symbol;

class scanner{

  /* put stuff that the class uses internally here */
  names* nmz;
  char curch;
  bool eofile;
  ifstream inf;
  
  void getch(void);
  void skipspaces(void);
  void getnumber(int& num);
  bool skipcomments(void);
  void getname(name& id);


public:

  void getsymbol(symbol& s, name& id, int& num);
    /* s = the sort of symbol read from a file */
    /* id = return the name id here if it is a name */
    /* num = return the value here if it is a number */

   ~scanner(void); //destructor of the class, closes input file

  scanner (names* names_mod, const char* defname);
    /* names initialises the name table.  This procedure is called at      */
    /* system initialisation before any of the above procedures/functions  */
    /* are used.                                                           */
};

#endif /* scanner_h */
