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
  /* Used to initialise a connection between an output device and an 	*/
  /* input device. Called from readin()  								*/
  /* It returns FALSE if there is an error in the initialisation		*/
  
  bool device(void);
  /* Used for the initialisation of a device, it checks what type the 	*/
  /* new device is and calls the correct function. Called from readin() */
  /* It returns FALSE if there is an error in the initialisation		*/
  
  bool monitor_(void);
  /* Receives and checks the initialisation of a monitor point  		*/
  /* from the scanner. Called from readin() 							*/
  /* It returns FALSE if there is an error in the initialisation		*/
  
  bool xor_(void);
  /* Receives and checks the initialisation of a xor gate				*/
  /* from the scanner. Called from device() 							*/
  /* It returns FALSE if there is an error in the initialisation 		*/
  
  bool gate(void);
  /* Receives and checks the initialisation of an AND/NAND/OR/NOR gate	*/
  /* from the scanner. Called from device() 							*/
  /* It returns FALSE if there is an error in the initialisation 		*/
  
  bool clock(void);
  /* Receives and checks the initialisation of a clock device			*/
  /* from the scanner. Called from device() 							*/
  /* It returns FALSE if there is an error in the initialisation 		*/
  
  bool switch_(void);
  /* Receives and checks the initialisation of a switch  				*/
  /* from the scanner. Called from device() 							*/
  /* It returns FALSE if there is an error in the initialisation 		*/
  
  bool dtype_(void);
  /* Receives and checks the initialisation of a dtype device			*/
  /* from the scanner. Called from device() 							*/
  /* It returns FALSE if there is an error in the initialisation 		*/
  
  bool siggen_(void);
  /* Receives and checks the initialisation of a siggen device			*/
  /* from the scanner. Called from device() 							*/
  /* It returns FALSE if there is an error in the initialisation 		*/
  
  void skip(void);
  /* If an error has been detected in the input file, this function 	*/
  /* skips reading the input file until the next semcolon, where it 	*/
  /* resumes normal operation. Used for error handling.					*/
  
  bool readname(void);
  /* Used during the initialisation of a device or monitor to read the  */
  /* the name of the new variable from the definition file				*/
  /* It returns FALSE if there is an error with the name initialisation	*/



 public:
  bool readin ();
    /* Reads the definition of the logic system and builds the          */
    /* corresponding internal representation via calls to the 'Network' */
    /* module and the 'Devices' module.                                 */

  parser (network* network_mod, devices* devices_mod,
	  monitor* monitor_mod, scanner* scanner_mod, names* names_mod);
    /* the constructor takes pointers to various other classes as parameters */
};

#endif /* parser_h */
