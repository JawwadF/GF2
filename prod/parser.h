#ifndef parser_h
#define parser_h

#include "names.h"
#include "scanner.h"
#include "network.h"
#include "devices.h"
#include "monitor.h"


using namespace std;

class parser {
  network* netz; // instantiations of various classes for parser to use.
  devices* dmz; 
  monitor* mmz;
  scanner* smz;

  /* put other stuff that the class uses internally here */
  /* also declare internal functions                     */


 public:
  bool readin ();
    /* Reads the definition of the logic system and builds the             */
    /* corresponding internal representation via calls to the 'Network'    */
    /* module and the 'Devices' module.                                    */
<<<<<<< HEAD
<<<<<<< HEAD
  void parser::connection(void);
  void parser::device(void);
  void parser::xor (void);
  void parser::gate(void);
  void parser::clock(void);
  void parser::switch_(void);
  void parser::dtype(void);
=======
=======
>>>>>>> 3fdbb80d5fbd9ab04929e250e13a63e2a265b576
  void connection(void);
  void device(void);
  void circuit(void);
  void monitor_(void);
  void xor_(void);
  void gate(void);
  void clock(void);
  void switch_(void);
  void dtype(void);
<<<<<<< HEAD
  //void monitor(void);
>>>>>>> origin/bianca
=======
>>>>>>> 3fdbb80d5fbd9ab04929e250e13a63e2a265b576
  parser (network* network_mod, devices* devices_mod,
	  monitor* monitor_mod, scanner* scanner_mod);
    /* the constructor takes pointers to various other classes as parameters */
};

#endif /* parser_h */
