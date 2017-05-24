#include "scanner.h"
#include <iostream>
#include <string>
#include <cstdlib>

using namespace std;


scanner::scanner(names* names_mod, const char* defname)  /* the constructor */
{
  nmz = names_mod;
  inf.open(defname);
  if (!inf) 
    {
      cout << "Error: cannot open file " << defname<< " for reading" << endl;
      exit(1);
    }
  getch();
  
}


scanner::~scanner(void)  /* the destructor */
{
  inf.close();
}

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

void scanner::getch(void)
{
   eofile = !inf.get(curch);
}


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


void scanner::skipspaces(void)
{
  while (isspace(curch) && !eofile)
    getch();
}

void scanner::getsymbol(symbol &s, name &id, int &num)
{
  bool i = 0;
  skipspaces();
  i = skipcomments();
  if (i == 1)
    {
      id = -1;
      num = -1;
      s = badsym;
    }
  else
    {    
      skipspaces();
      if (eofile)
	{
	  s = eofsym;
	  id = -1;
	  num = -1;
	}
      else 
	{
	  if (isdigit(curch)) //number 
	    {
	      s = numsym;
	      getnumber(num);
	      id = -1;
	    }
	  else 
	    {
	      num = -1;
	      if (isalpha(curch)) //name
		{ 
		  getname(id);
		  if (id == nmz->cvtname("DEVICE")) s = devsym;
		  else if (id == nmz->cvtname("CONNECT")) s = consym;
		  else if (id == nmz->cvtname("XOR")) s = xorsym;
		  else if (id == nmz->cvtname("CLOCK")) s = clksym;
		  else if (id == nmz->cvtname("SWITCH")) s = swisym;
		  else if (id == nmz->cvtname("DTYPE")) s = dtypesym;
		  else if (id == nmz->cvtname("CIRCUIT")) s = cirsym;
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
		  else s = namesym;
		}
	      else 
		{
		  id = -1;
		  switch (curch) 
		    {
		    case '=': s = equals; break;
		    case ';': s = semicol; break;
		    case '.': s = dot; break;
		    case '>': s = connect; break;
		    default: s = badsym; break;
		    }
		  getch();
		    }
	    }
	}
    }
}




