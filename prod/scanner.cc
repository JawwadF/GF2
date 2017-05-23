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
      cout << "Error: cannot open file " << argv[1] << " for reading" << endl;
      exit(1);
    }
  getch()
}


//scanner::~scanner(names* names_mod, const char* defname)  /* the destructor */

//void skipcomments(void)


void getnumber(int& num)
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

void getch(void)
{
   eofile = !inf.get(curch);
}


void getname(name &id)
{
  namestring vari = "";
  namestring str;
  while (!eofile and (isalpha(curch) or isdigit(curch)))
    {
      vari += curch;
      getch();
    }
  str = vari;
  if (vari.length() > nmz->maxlength)
    {
      str.resize(maxlength);
      cout<<"Warning: the name '"<<vari<<"' was truncated to "<<str <<endl;
    }
  id = nmz->lookup(str); 
}


void skipspaces(void)
{
  while (isspace(curch) and !eofile)
    getch();
}

void scanner::getsymbol(symbol &s, name &id, int &num)
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
	  if (isalpha(curch)) //name
	    { 
	      getname(id);
	      switch (id)
		{
		case nmz->cvtname("DEVICE"): s = devsym; break;
	        case nmz->cvtname("CONNECT"): s = consym; break;
		case nmz->cvtname("XOR"): s = xorsym; break;
		case nmz->cvtname("CLOCK"): s = clksym; break;
		case nmz->cvtname("SWITCH"): s = swisym; break;
		case nmz->cvtname("DTYPE"): s = dtypesym; break;
		case nmz->cvtname("CIRCUIT"): s = cirsym; break;
		case nmz->cvtname("MONITOR"): s = monsym; break;

		case nmz->cvtname("AND"): s = gatesym; break;
		case nmz->cvtname("NAND"): s = gatesym; break; 
		case nmz->cvtname("OR"): s = gatesym; break;
		case nmz->cvtname("NOR"): s = gatesym; break;

		case nmz->cvtname("INPUTS"): s = keysym; break;
		case nmz->cvtname("VALUE"): s = keysym; break;
		case nmz->cvtname("QVAL"): s = keysym; break;
		case nmz->cvtname("NAME"): s = keysym; break;
		case nmz->cvtname("CYCLES"): s = keysym; break;
		case nmz->cvtname("START"): s = keysym; break;
		case nmz->cvtname("INCLUDES"): s = keysym; break;
		case nmz->cvtname("RECORDS"): s = keysym; break;

		case nmz->cvtname("Q"): s = outsym; break;
		case nmz->cvtname("QVAL"): s = outsym; break;

		case nmz->cvtname("I1"): s = insym; break;
		case nmz->cvtname("I2"): s = insym; break;
		case nmz->cvtname("I3"): s = insym; break;
		case nmz->cvtname("I4"): s = insym; break;
		case nmz->cvtname("I5"): s = insym; break;
		case nmz->cvtname("I6"): s = insym; break;
		case nmz->cvtname("I7"): s = insym; break;
		case nmz->cvtname("I8"): s = insym; break;
		case nmz->cvtname("I9"): s = insym; break;
		case nmz->cvtname("I10"): s = insym; break;
		case nmz->cvtname("I11"): s = insym; break;
		case nmz->cvtname("I12"): s = insym; break;
		case nmz->cvtname("I13"): s = insym; break;
		case nmz->cvtname("I14"): s = insym; break;
		case nmz->cvtname("I15"): s = insym; break;
		case nmz->cvtname("I16"): s = insym; break;

		default: s = namesym; break;
		}
	    }
	  else 
	    {
	      switch (curch) 
		{
		case '=': s = equals; break;
		case ';': s = semicol; break;
		case '.': s = dot; break;
		case '>': s = connect; break;
		default: s = badsym; break;
		}
	    }
	}
    }
}




