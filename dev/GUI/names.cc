#include "names.h"
#include <iostream>
#include <string>
#include <cstdlib>

using namespace std;

/***********************************************************************
 *
 * The constructor, table is initialised empty 
 * Initilises the lenght_of_table to 0
 *
 */
names::names(void)
{
  length_of_table = 0;
}

/***********************************************************************
 *
 * Takes a string as argument 
 * Searches through the table for this string
 * 
 * If the string exists in the table, 
 * returns its ID (the index in the array)
 * 
 * If the string is not in the table, 
 * it inserts it in the table and returns its ID
 * 
 * The string can be maxlength characters long, 
 * otherwise it gets truncated
 *
 */
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

/***********************************************************************
 *
 * Takes an ID as argument
 * Searches through the table for this ID and returns the string in 
 * character form.
 *
 */
namestring names::get_str (name id)
{
  if (id == -1)
    return "Blankname";
  else if (id>=0 && id<length_of_table)
    return table[id];
  else 
    {
      cout<<"Warning, this id does not exist";
      return "";
    }
}

/***********************************************************************
 *
 * Takes a string as argument 
 * Searches through the table for this string
 * If the string exists in the table, returns its ID (the index in the array)
 * If the string is not in the table, it returns blankname
 *
 */
name names::cvtname (namestring str)
{
  name t=blankname;
  for (int i=0; i<length_of_table; i++)
    if (table[i]==str)
      t = i;
  return t;
}

/***********************************************************************
 *
 * Takes an ID as argument
 * Prints the string in character form on the console.
 *
 */
void names::writename (name id)
{
  if (id >= 0 && id<length_of_table)
    cout<<table[id];
  else
  {
	  if (id == -1) {
		  cout << "Blankname";
		  return ;
	  }
	  cout << "Warning, this id does not exist";
	  return;
  }
}

/***********************************************************************
 *
 * Takes an ID as argument
 * Returns the length (ie number of characters) of the string
 *
 */
int names::namelength (name id)
{
  if (id >= 0 && id<length_of_table)
    return table[id].length();
  else
    {
	  if (id == -1) {
		  cout << "Blankname";
		  return 0;
	  }
      cout<<"Warning, this id does not exist";
      return 0;
    }
}




