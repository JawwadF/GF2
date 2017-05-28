#include "names.h"
#include <iostream>
#include <string>
#include <cstdlib>

using namespace std;


/* Name storage and retrieval routines */

names::names(void)  /* the constructor */
{
  length_of_table = 0;
}

name names::lookup (namestring str)
{
  name i;
  bool check = 0;
  namestring vari = str;
  //first check if the str has less than maxlength characters
  if (str.length() > maxlength)
    {
       str.resize(maxlength);
       cout<<"Warning: the string '"<<vari<<"' was truncated to '"<<str<<"'"<<endl;
    }

  for (i = 0; i<length_of_table; i++)
      if (table[i] == str)
	{
	  check = 1;
	  return i;
	}
  if (check == 0)
    {
      table[length_of_table] = str;
      length_of_table +=1;
      return (length_of_table-1);
    }
}

name names::cvtname (namestring str)
{
  name t=blankname;
  for (int i=0; i<length_of_table; i++)
    if (table[i]==str)
      t = i;
  return t;
}

void names::writename (name id)
{
  if (id<length_of_table)
    cout<<table[id];
  else
    cout<<"Warning, this id does not exist";
}

int names::namelength (name id)
{
  if (id<length_of_table)
    return table[id].length();
  else
    {
      cout<<"Warning, this id does not exist";
      return 0;
    }

}
