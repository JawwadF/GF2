#ifndef names_h
#define names_h

#include <string>

using namespace std;

const int maxnames  = 200;  /* max number of distinct names */
const int maxlength = 8;    /* max chars in a name string   */
const int blankname = -1;   /* special name                 */

typedef int name;
typedef string namestring;

class names{

  namestring table[maxnames];

public:
  int length_of_table;
  
  name lookup (namestring str);
    /* Returns the internal representation of the name given in character*/
    /* form.  If the name is not already in the name table, it is       */
    /* automatically inserted.                                          */

  name cvtname (namestring str);
    /* Returns the internal representation of the name given in character*/
    /* form.  If the name is not in the name table then 'blankname' is  */
    /* returned.                                                        */

  void writename (name id);
    /* Prints out the given name on the console                         */

  namestring get_str (name id);
    /* Returns the stored namestring for a given id                     */

  int namelength (name id);
    /* Returns length ie number of characters in given name             */

  names (void);
  /* Called on class initialisation.                                    */
};

#endif /* names_h */
