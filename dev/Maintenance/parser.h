#ifndef parser_h
#define parser_h
#include "names.h"
#include "scanner.h"
#include "network.h"
#include "devices.h"
#include "monitor.h"
void showError(const char* mess);
using namespace std;

class parser {
  network* netz; // instantiations of various classes for parser to use.
  devices* dmz; 
  monitor* mmz;
  scanner* smz;
  names* nmz;

  bool connection(void);
  /*  */
  /* */
  
  bool device(void);
  bool monitor_(void);
  bool xor_(void);
  bool gate(void);
  bool clock(void);
  bool switch_(void);
  bool dtype_(void);
  bool siggen_(void);
  
  void skip(void);
  bool readname(void);


 public:
  bool readin ();
    /* Reads the definition of the logic system and builds the             */
    /* corresponding internal representation via calls to the 'Network'    */
    /* module and the 'Devices' module.                                    */

  parser (network* network_mod, devices* devices_mod,
	  monitor* monitor_mod, scanner* scanner_mod, names* names_mod);
    /* the constructor takes pointers to various other classes as parameters */
};

#endif /* parser_h */
