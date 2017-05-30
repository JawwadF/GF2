#ifndef gui_h
#define gui_h


#include <wx/wx.h>
#include <wx/glcanvas.h>
#include <wx/spinctrl.h>
#include <wx/textctrl.h>
#include <wx/choicdlg.h>
#include <wx/arrstr.h>
#include <vector>
#include "names.h"
#include "devices.h"
#include "monitor.h"
#include "network.h"
#include "scanner.h"
#include "parser.h"

enum {
  MY_SPINCNTRL_ID = wxID_HIGHEST + 1,
  MY_TEXTCTRL_ID,
  MY_BUTTON_ID,
  CONTINUE_BUTTON_ID, //added by me
  SETSWITCH_BUTTON_ID, //added by me
  SETMONITOR_BUTTON_ID, //added by me
  REMOVEMONITOR_BUTTON_ID, //added by me
}; // widget identifiers

class MyGLCanvas;

class MyFrame: public wxFrame
{
 public:
  MyFrame(wxWindow *parent, const wxString& title, const wxPoint& pos, const wxSize& size,
	  names *names_mod = NULL, devices *devices_mod = NULL, monitor *monitor_mod = NULL, parser *parser_mod = NULL, scanner *scanner_mod = NULL, network *network_mod = NULL,
	  long style = wxDEFAULT_FRAME_STYLE); // constructor
 private:
  MyGLCanvas *canvas;                     // OpenGL drawing area widget to draw traces
  wxSpinCtrl *spin;                       // control widget to select the number of cycles
  names *nmz;                             // pointer to names class
  devices *dmz;                           // pointer to devices class
  monitor *mmz;                           // pointer to monitor class
  network *netz;
  parser *pmz;
  scanner *smz;

  devlink firstDevice;
<<<<<<< HEAD
  int cyclescompleted; // how many simulation cycles have been completed
  wxArrayString wxSwitchNameArray;
  int SwitchIDArray[1000];
=======
  monitortable MonitorTable;
  int cyclescompleted; // how many simulation cycles have been completed
  wxArrayString wxSwitchNameArray;
  wxArrayString wxMonitorArray;
  int SwitchIDArray[1000];
  wxArrayInt selectedArray;
>>>>>>> 892105b984029f00e8b36688e40bdf2a366d6a20

  void runnetwork(int ncycles);           // function to run the logic network
  void OnExit(wxCommandEvent& event);     // event handler for exit menu item
  void OnAbout(wxCommandEvent& event);    // event handler for about menu item
  void OnOpen(wxCommandEvent& event); //added by me
  void OnHelp(wxCommandEvent& event); //added by me
  void OnButton(wxCommandEvent& event);   // event handler for push button
  void OnContinue(wxCommandEvent& event); //added by me - for continue button
  void OnSwitch(wxCommandEvent& event); //added by me - for set switch button
  void OnSetMon(wxCommandEvent& event); //added by me - for set monitor point button
  void OnSpin(wxSpinEvent& event);        // event handler for spin control
  void OnText(wxCommandEvent& event);     // event handler for text entry field

  // The Path to the file we have open
    wxString CurrentDocPath;
    DECLARE_EVENT_TABLE()
};

class MyGLCanvas: public wxGLCanvas
{
 public:
  MyGLCanvas(wxWindow *parent, wxWindowID id = wxID_ANY, monitor* monitor_mod = NULL, names* names_mod = NULL,
	     const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0,
	     const wxString& name = "MyGLCanvas", const wxPalette &palette=wxNullPalette); // constructor
  void Render(wxString example_text = "", int cycles = -1); // function to draw canvas contents
 private:
  wxGLContext *context;              // OpenGL rendering context
  bool init;                         // has the OpenGL context been initialised?
  int pan_x;                         // the current x pan
  int pan_y;                         // the current y pan
  double zoom;                       // the current zoom
  int cyclesdisplayed;               // how many simulation cycles have been displayed
  monitor *mmz;                      // pointer to monitor class, used to extract signal traces
  names *nmz;                        // pointer to names class, used to extract signal names
  void InitGL();                     // function to initialise OpenGL context
  void OnSize(wxSizeEvent& event);   // event handler for when canvas is resized
  void OnPaint(wxPaintEvent& event); // event handler for when canvas is exposed
  void OnMouse(wxMouseEvent& event); // event handler for mouse events inside canvas
  DECLARE_EVENT_TABLE()
};

#endif /* gui_h */
