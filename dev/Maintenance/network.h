#ifndef network_h
#define network_h

#include "names.h"
#include <string>

/* Network specification */

typedef enum {falling, low, rising, high} asignal;
typedef enum {aswitch, aclock, andgate, nandgate, orgate,
	      norgate, xorgate, dtype, asiggen, baddevice} devicekind;

struct outputrec {
  name       id;
  name 		 devid;
  asignal    sig;
  outputrec* next;
};
typedef outputrec* outplink;
struct inputrec {
  name      id;
  outplink  connect;
  inputrec* next;
};
typedef inputrec* inplink;
struct devicerec {
  name id;
  inplink ilist;
  outplink olist;
  devicerec* next;
  devicekind kind;
  /* the next elements are only used by some of the device kinds */
  asignal swstate;      // used when kind == aswitch
  int frequency;        // used when kind == aclock || asiggen
  int counter;          // used when kind == aclock ||asiggen
  asignal memory;       // used when kind == dtype
  string bitstring;     // used when kind == asiggen
};
typedef devicerec* devlink;

class network {
  names* nmz;  // the instatiation of the names class that we are going to use.

 public:
  devlink devicelist (void);
    /* Returns list of devices                                             */

  void writeconnections(devlink d);
    /* TEST FUNCTION: Prints out all the connections in the network */
 
  devlink finddevice (name id);
   /* Returns link to device with specified name. Returns NULL if not       */
   /* found.                                                               */
 
  inplink findinput (devlink dev, name id);
    /* Returns link to input of device pointed to by dev with specified    */
    /* name.  Returns NULL if not found.                                    */
 
  outplink findoutput (devlink dev, name id);
    /* Returns link to output of device pointed to by dev with specified   */
    /* name.  Returns NULL if not found.                                    */
 
  void adddevice (devicekind dkind, name did, devlink& dev);
    /* Adds a device to the device list with given name and returns a link */
    /* to it via 'dev'.                                                    */
 
  void addinput (devlink dev, name iid);
    /* Adds an input to the device pointed to by 'dev' with the specified  */
    /* name.                                                               */
 
  void addoutput (devlink dev, name oid, name tdevid);
    /* Adds an output to the device pointed to by 'dev' with the specified */
    /* name.                                                               */

  void makeconnection (name idev, name inp, name odev, name outp, bool& ok);
    /* Makes a connection between the 'inp' input of device 'idev' and the */
    /* 'outp' output of device 'odev'. 'ok' is set true if operation       */
    /* succeeds.                                                           */
    
    
  void deleteconnection (name idev, name inp);
	/* Deletes the connection that exists at the 'inp' input of device  */
	/* 'idev' . 										                */


 
  void checknetwork (bool& ok, string& mess);
    /* Checks that all inputs are connected to an output.                  */
 
  network (names* names_mod);
  /* Called on system initialisation.                                      */

 private:
  devlink devs;          // the list of devices
  devlink lastdev;       // last device in list of devices

};

#endif /* network_h */
