#include "scanner.h"
#include <iostream>
#include <string>
#include <cstdlib>
#include <cmath>

using namespace std;

/***********************************************************************
 * 
 * The constructor. This procedure is called at system initialisation
 * before any of the above procedures/functions are used. 
 * Opens the file and reads first character                 	
 *
 */
scanner::scanner(names* names_mod, const char* defname)
{
  nmz = names_mod;
  counter = 1;
  curline = "";
  inf.open(defname);
  if (!inf) 
    {
      cout << "Error: cannot open file " << defname<< " for reading" << endl;
      exit(1);
    }
  getch(); 
}

/***********************************************************************
 * 
 * Sets the url for the read file  
 *
 */
void scanner::setURL(const char* ur) {
	url = (char*) ur;
}

/***********************************************************************
 * 
 * Opens the file and reads the first character, used for errors
 *
 */
void scanner::initialise(names* names_mod) {
	nmz = names_mod;
	inf.close();
	inf.open(url);
	if (!inf)
	{
		cout << "Error: cannot open file " << url << " for reading" << endl;
		exit(1);
	}
	getch();
}

/***********************************************************************
 * 
 * Destructor of the class, closes input file
 *
 */
scanner::~scanner(void)
{
  inf.close();
}

/***********************************************************************
 * 
 * Ignores comments from the input file 							
 * Comments start with // and finish at the end of the line 
 *  It returns TRUE (1) if only one back-slash was found and not two,
 *  which makes the comment syntax invalid 
 *
 */
bool scanner::skipcomments(void) 
{ 
  bool i = 0;
  if (curch == '/')
    {
      getch();
      if (curch == '/')
	while (!eofile && curch != '\n')
	  getch();
      else 
	i = 1;
    }
  return i;
}

/***********************************************************************
 * 
 * Reads the current line from the input file 			
 * and returns it as string to be printed in an error message 
 *
 */
string scanner::geterror(void) {
	char ch = curch;
	string line = "";
	int errorpos = curline.size() - 1;
	while (curline != "") {
		line = curline;
		getch();
	}
	curch = ch;
	int size = line.size();

	string spaces(2*size , ' ');
	double total = pow(2, ((errorpos*1.0)/ (size - 1)));
	spaces[round(total*errorpos)] = '^';
	line = line + "\n" + spaces;
	return line;
}

/***********************************************************************
 * 
 * Reads a number - unsigned int from the input file 	
 *  Sets the value of num equal to this value 						
 * 
*/
void scanner::getnumber(int& num)
{
  string numstr;
  while (!eofile) {
    if (!isdigit(curch))
      break;
    numstr += curch;
    getch();
  }
  num = atoi(numstr.c_str());
}

/***********************************************************************
 * 
 * Gets the next character from the input file						
 * Counts how many lines it read from the file for error 		
 * handling purposes
 * 
 */ 
void scanner::getch(void)
{
   eofile = !inf.get(curch);
   curline = curline + curch;
   if (curch == '\n') {
	   counter++;
	   curline = "";
   }
}

/***********************************************************************
 * 
 * Gets a name from the input file and finds its id from the names 
 * table. If the name is not in the table, it automatically adds it 
 * It also constricts the length of the string to maxlength characters 
 * 
 */ 
void scanner:: getname(name &id)
{
  namestring vari = "";
  namestring str;
  while (!eofile && (isalpha(curch) || isdigit(curch)))
    {
      vari += curch;
      getch();
    }
  str = vari;
  if (vari.length() > maxlength)
    {
      str.resize(maxlength);
      cout<<"Warning: the name '"<<vari<<"' was truncated to "<<str <<endl;
    }
  id = nmz->lookup(str); 
}

/***********************************************************************
 * 
 * Skips spaces from the input file	
 * 
 */
void scanner::skipspaces(void)
{
  while (isspace(curch) && !eofile)
    getch();
}

/***********************************************************************
 * 
 * s = reads the next symbol from the file 			
 * Skips comments and empty spaces						  		
 * s = the type of symbol read from a file 					
 * id = return the name id here if it is a name 				
 * num = return the value here if it is a number 				
 * 
 */
void scanner::getsymbol(symbol &s, name &id, int &num)
{
  bool i = 0;
  skipspaces();

  while (curch == '/')
    {
      i = skipcomments(); //skips the comments
      skipspaces(); //skips spaces
    }

  if (i == 1) //if it found just a back slash instead of 2 it returns an error
    {
      id = -1;
      num = -1;
      s = badsym;
    }
  else
    {    
      skipspaces(); //skips spaces
      if (eofile)
		{
		  s = eofsym;
		  id = -1;
		  num = -1;
		}
      else 
		{
		  if (isdigit(curch)) // if the first character is a number, it reads a number into num
	    {
	      s = numsym;
	      getnumber(num);
	      id = -1;
	    }
	  else 
	    {
	      num = -1;
	      if (isalpha(curch)) // if the first character is a letter, it reads a name and returns its id
		    { 
		      getname(id);
		      if (id == nmz->cvtname("DEVICE")) s = devsym;
			  else if (id == nmz->cvtname("CONNECT")) s = consym;
			  else if (id == nmz->cvtname("XOR")) s = xorsym;
			  else if (id == nmz->cvtname("CLOCK")) s = clksym;
			  else if (id == nmz->cvtname("SWITCH")) s = swisym;
			  else if (id == nmz->cvtname("DTYPE")) s = dtypesym;
			  else if (id == nmz->cvtname("MONITOR")) s = monsym;

			  else if (id == nmz->cvtname("AND")) s = gatesym;
			  else if (id == nmz->cvtname("NAND")) s = gatesym;
			  else if (id == nmz->cvtname("OR")) s = gatesym;
			  else if (id == nmz->cvtname("NOR")) s = gatesym;

			  else if (id == nmz->cvtname("INPUTS")) s = keysym;
			  else if (id == nmz->cvtname("VALUE")) s = keysym;
			  else if (id == nmz->cvtname("QVAL")) s = keysym;
			  else if (id == nmz->cvtname("NAME")) s = keysym;
			  else if (id == nmz->cvtname("CYCLES")) s = keysym;
			  else if (id == nmz->cvtname("START")) s = keysym;
			  else if (id == nmz->cvtname("INCLUDES")) s = keysym;
			  else if (id == nmz->cvtname("RECORDS")) s = keysym;

			  else if (id == nmz->cvtname("Q")) s = outsym;
			  else if (id == nmz->cvtname("QBAR")) s = outsym;

			  else if (id == nmz->cvtname("I1")) s = insym;
			  else if (id == nmz->cvtname("I2")) s = insym;
			  else if (id == nmz->cvtname("I3")) s = insym;
			  else if (id == nmz->cvtname("I4")) s = insym;
			  else if (id == nmz->cvtname("I5")) s = insym;
			  else if (id == nmz->cvtname("I6")) s = insym;
			  else if (id == nmz->cvtname("I7")) s = insym;
			  else if (id == nmz->cvtname("I8")) s = insym;
			  else if (id == nmz->cvtname("I9")) s = insym;
			  else if (id == nmz->cvtname("I10")) s = insym;
			  else if (id == nmz->cvtname("I11")) s = insym;
			  else if (id == nmz->cvtname("I12")) s = insym;
			  else if (id == nmz->cvtname("I13")) s = insym;
			  else if (id == nmz->cvtname("I14")) s = insym;
			  else if (id == nmz->cvtname("I15")) s = insym;
			  else if (id == nmz->cvtname("I16")) s = insym;

			  else if (id == nmz->cvtname("DATA")) s = insym;
			  else if (id == nmz->cvtname("SET")) s = insym;
			  else if (id == nmz->cvtname("CLEAR")) s = insym;
			  else if (id == nmz->cvtname("CLK")) s = insym;

			  else s = namesym;
			}
	      else 
			{
			  id = -1;
		      switch (curch) //symbols
		        {
		          case '=': s = equals; break;
				  case ';': s = semicol; break;
			  	  case '.': s = dot; break;
				  case '>': s = connect_; break;
				  default: s = badsym; break;
				}
			  getch();
		    }
	    }
	}
    }
}




