#include "gui.h"
#include <GL/glut.h>
#include "wx_icon.xpm"
#include <iostream>
#include <sstream>
using namespace std;

// MyGLCanvas ////////////////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(MyGLCanvas, wxGLCanvas)
  EVT_SIZE(MyGLCanvas::OnSize)
  EVT_PAINT(MyGLCanvas::OnPaint)
  EVT_MOUSE_EVENTS(MyGLCanvas::OnMouse)
END_EVENT_TABLE()

void showError(const char* mess) {
	wxMessageBox(mess, wxT("Syntactic Error"), wxICON_ERROR);
}

int wxglcanvas_attrib_list[5] = {WX_GL_RGBA, WX_GL_DOUBLEBUFFER, WX_GL_DEPTH_SIZE, 16, 0};

MyGLCanvas::MyGLCanvas(wxWindow *parent, wxWindowID id, monitor* monitor_mod, names* names_mod, const wxPoint& pos,
		       const wxSize& size, long style, const wxString& name, const wxPalette& palette):
  wxGLCanvas(parent, id, wxglcanvas_attrib_list, pos, size, style, name, palette)
  // Constructor - initialises private variables
{
  context = new wxGLContext(this);
  mmz = monitor_mod;
  nmz = names_mod;
  init = false;
  pan_x = 0;
  pan_y = 0;
  zoom = 1.0;
  cyclesdisplayed = -1;
}

void MyGLCanvas::Render(wxString example_text, int cycles)
  // Draws canvas contents - the following example writes the string "example text" onto the canvas
  // and draws a signal trace. The trace is artificial if the simulator has not yet been run.
  // When the simulator is run, the number of cycles is passed as a parameter and the first monitor
  // trace is displayed.
{
  float y;
  unsigned int i;
  asignal s;

  if (cycles >= 0) cyclesdisplayed = cycles;

  SetCurrent(*context);
  if (!init) {
    InitGL();
    init = true;
  }
  glClear(GL_COLOR_BUFFER_BIT);

  int square_size = 30; //this is the size of one square on the trace
  int start_corner = 100; //this is the corner size that is left empty on the top left part of the canvas

  if ((cyclesdisplayed >= 0) && (mmz->moncount() > 0)) { // draw all the monitor traces

  // here create the big square trace
  int w, h;
  GetClientSize (&w ,&h);
  glLineWidth(1.0);
  glColor3f(0.0, 0.0, 1.0); //blue
  for (i=0; i< mmz->moncount(); i++) //horizontal lines
    {
      glBegin(GL_LINE_STRIP);
      glVertex2f(start_corner, h-start_corner-i*square_size*2);
      glVertex2f(maxcycles*square_size+start_corner, h-start_corner-i*square_size*2);
      glEnd();
      glBegin(GL_LINE_STRIP);
      glVertex2f(start_corner, h-start_corner-square_size-i*square_size*2);
      glVertex2f(maxcycles*square_size+start_corner, h-start_corner-square_size-i*square_size*2);
      glEnd();
    } 

  for (i=0; i< maxcycles+1; i++) //vertical lines
    {
      //string mystr;
      //stringstream iout;
      //iout << i;
      //mystr = iout.str();

      wxString mystring = wxString::Format(wxT("%i"),i);

      string stlstring;
      //stlstring= nmz->get_str(MonitorTable.sigs[i].devid);//name of device
       //nmz->get_str(MonitorTable.sigs[i].op->id)//name of output
      //wxString mystring(stlstring);

      PrintOnCanvas(mystring, start_corner+i*square_size , h-start_corner+square_size*2.5);

      for (int j=0; j<= mmz->moncount() + 1; j++)
        {
          glBegin(GL_LINE_STRIP);
          glVertex2f(start_corner+i*square_size, h-start_corner+square_size*2-j*square_size*2);
          glVertex2f(start_corner+i*square_size, h-start_corner+square_size-j*square_size*2);
          glEnd(); 
        }
      PrintOnCanvas(mystring, start_corner+i*square_size , h-start_corner-(mmz->moncount())*square_size*2-square_size*1.5);
    }
  

    glLineWidth(2.0);
    for (int j=0; j<mmz->moncount(); j++)
      {
	glBegin(GL_LINE_STRIP);
         for (i=0; i<cyclesdisplayed; i++) {
             if (mmz->getsignaltrace(j, i, s)) {
	       if (s==low) {y = h-start_corner-square_size-j*square_size*2;   glColor3f(1.0, 0.0, 0.0);} //red 
                if (s==high) {y = h-start_corner-j*square_size*2;   glColor3f(1.0, 0.0, 0.0);} //red
                if (s==rising) {y = h-start_corner-square_size/2-j*square_size*2;   glColor3f(1.0, 0.8, 0.8);} //pink
                if (s==falling) {y = h-start_corner-square_size/2-j*square_size*2;   glColor3f(1.0, 0.8, 0.8);} //pink
	        glVertex2f(start_corner+i*square_size, y);
	        glVertex2f(start_corner+square_size+i*square_size, y);
         }
    }
      	glEnd();
}


  } else { // draw an artificial trace

    /*glColor3f(1.0, 1.0, 0.0);
    glBegin(GL_LINE_STRIP);
    for (i=0; i<5; i++) {
      if (i%2) y = 10.0;
      else y = 30.0;
      glVertex2f(20*i+10.0, y);
      glVertex2f(20*i+30.0, y);
    }
    glEnd();*/
    
  }

  PrintOnCanvas(example_text, 10, 10);


  // We've been drawing to the back buffer, flush the graphics pipeline and swap the back buffer to the front
  glFlush();
  SwapBuffers();
}

void MyGLCanvas::InitGL()
  // Function to initialise the GL context
{
  int w, h;

  GetClientSize(&w, &h);
  SetCurrent(*context);
  glDrawBuffer(GL_BACK);
  glClearColor(1.0, 1.0, 1.0, 0.0);
  glViewport(0, 0, (GLint) w, (GLint) h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w, 0, h, -1, 1);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslated(pan_x, pan_y, 0.0);
  glScaled(zoom, zoom, zoom);
}

void MyGLCanvas::PrintOnCanvas(wxString example_text, int xaxis, int yaxis) //prints text on canvas
{
  // Example of how to use GLUT to draw text on the canvas
  glColor3f(0.0, 0.0, 1.0);
  glRasterPos2f(xaxis, yaxis);
  for (int i = 0; i < example_text.Len(); i++) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, example_text[i]);

}


void MyGLCanvas::OnPaint(wxPaintEvent& event)
  // Event handler for when the canvas is exposed
{
  int w, h;
  wxString text;

  wxPaintDC dc(this); // required for correct refreshing under MS windows
  GetClientSize(&w, &h);
  text.Printf("Canvas redrawn by OnPaint event handler, canvas size is %d by %d", w, h);
  Render(text);
}

void MyGLCanvas::OnSize(wxSizeEvent& event)
  // Event handler for when the canvas is resized
{
  init = false;; // this will force the viewport and projection matrices to be reconfigured on the next paint
}

void MyGLCanvas::OnMouse(wxMouseEvent& event)
  // Event handler for mouse events inside the GL canvas
{
  wxString text;
  int w, h;;
  static int last_x, last_y;

  GetClientSize(&w, &h);
  if (event.ButtonDown()) {
    last_x = event.m_x;
    last_y = event.m_y;
    text.Printf("Mouse button %d pressed at %d %d", event.GetButton(), event.m_x, h-event.m_y);
  }
  if (event.ButtonUp()) text.Printf("Mouse button %d released at %d %d", event.GetButton(), event.m_x, h-event.m_y);
  if (event.Dragging()) {
    pan_x += event.m_x - last_x;
    pan_y -= event.m_y - last_y;
    last_x = event.m_x;
    last_y = event.m_y;
    init = false;
    text.Printf("Mouse dragged to %d %d, pan now %d %d", event.m_x, h-event.m_y, pan_x, pan_y);
  }
  if (event.Leaving()) text.Printf("Mouse left window at %d %d", event.m_x, h-event.m_y);
  if (event.GetWheelRotation() < 0) {
    zoom = zoom * (1.0 - (double)event.GetWheelRotation()/(20*event.GetWheelDelta()));
    init = false;
    text.Printf("Negative mouse wheel rotation, zoom now %f", zoom);
  }
  if (event.GetWheelRotation() > 0) {
    zoom = zoom / (1.0 + (double)event.GetWheelRotation()/(20*event.GetWheelDelta()));
    init = false;
    text.Printf("Positive mouse wheel rotation, zoom now %f", zoom);
  }

  if (event.GetWheelRotation() || event.ButtonDown() || event.ButtonUp() || event.Dragging() || event.Leaving()) Render(text);
}

// MyFrame ///////////////////////////////////////////////////////////////////////////////////////


BEGIN_EVENT_TABLE(MyFrame, wxFrame)
  EVT_MENU(wxID_EXIT, MyFrame::OnExit)
  EVT_MENU(wxID_ABOUT, MyFrame::OnAbout)
  EVT_MENU(wxID_HELP, MyFrame::OnHelp)
  EVT_MENU(wxID_OPEN, MyFrame::OnOpen)
  EVT_BUTTON(MY_BUTTON_ID, MyFrame::OnButton)
  EVT_BUTTON(CONTINUE_BUTTON_ID, MyFrame::OnContinue) //added by me
  EVT_BUTTON(SETSWITCH_BUTTON_ID, MyFrame::OnSwitch) //added by me
  EVT_BUTTON(SETMONITOR_BUTTON_ID, MyFrame::OnSetMon) //added by me
  EVT_BUTTON(REMOVEMONITOR_BUTTON_ID, MyFrame::OnButton) //added by me
  EVT_SPINCTRL(MY_SPINCNTRL_ID, MyFrame::OnSpin)
  EVT_TEXT_ENTER(MY_TEXTCTRL_ID, MyFrame::OnText)
END_EVENT_TABLE()

MyFrame::MyFrame(wxWindow *parent, const wxString& title, const wxPoint& pos, const wxSize& size,
		 names *names_mod, devices *devices_mod, monitor *monitor_mod, parser *parser_mod, scanner *scanner_mod, network *network_mod, long style):
  wxFrame(parent, wxID_ANY, title, pos, size, style)
  // Constructor - initialises pointers to names, devices and monitor classes, lays out widgets
  // using sizers
{
  SetIcon(wxIcon(wx_icon));
  cyclescompleted = 0;
  nmz = names_mod;
  dmz = devices_mod;
  mmz = monitor_mod;
  pmz = parser_mod;
  smz = scanner_mod;
  netz = network_mod;
  if (nmz == NULL || dmz == NULL || mmz == NULL) {
    cout << "Cannot operate GUI without names, devices and monitor classes" << endl;
    exit(1);
  }

  wxMenu *fileMenu = new wxMenu;
  fileMenu->Append(wxID_ABOUT, "&About");
  fileMenu->Append(wxID_HELP, "&Help");
  fileMenu->Append(wxID_EXIT, "&Quit");
fileMenu->Append(wxID_OPEN, "&Open");
  wxMenuBar *menuBar = new wxMenuBar;
  menuBar->Append(fileMenu, "&File");
  SetMenuBar(menuBar);

  wxBoxSizer *topsizer = new wxBoxSizer(wxHORIZONTAL);
  canvas = new MyGLCanvas(this, wxID_ANY, monitor_mod, names_mod);
  topsizer->Add(canvas, 1, wxEXPAND | wxALL, 10);

  wxBoxSizer *button_sizer = new wxBoxSizer(wxVERTICAL);
  button_sizer->Add(new wxButton(this, MY_BUTTON_ID, "Run"), 0, wxALL, 10);
  button_sizer->Add(new wxButton(this, CONTINUE_BUTTON_ID, "Continue"), 0, wxALL, 10); //added by me
  button_sizer->Add(new wxButton(this, SETSWITCH_BUTTON_ID, "Set Switch"), 0, wxALL, 10); //added by me
  button_sizer->Add(new wxButton(this, SETMONITOR_BUTTON_ID, "Set Monitor point"), 0, wxALL, 10); //added by me
  button_sizer->Add(new wxButton(this, REMOVEMONITOR_BUTTON_ID, "Remove Monitor point"), 0, wxALL, 10); //added by me
  button_sizer->Add(new wxStaticText(this, wxID_ANY, "Cycles"), 0, wxTOP|wxLEFT|wxRIGHT, 10);
  spin = new wxSpinCtrl(this, MY_SPINCNTRL_ID, wxString("10"));
  button_sizer->Add(spin, 0 , wxALL, 10);

  button_sizer->Add(new wxTextCtrl(this, MY_TEXTCTRL_ID, "", wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER), 0 , wxALL, 10);
  topsizer->Add(button_sizer, 0, wxALIGN_CENTER);

  SetSizeHints(750, 500);
  SetSizer(topsizer);
}



void MyFrame::OnExit(wxCommandEvent &event)
  // Event handler for the exit menu item
{
  Close(true);
}

void MyFrame::OnOpen(wxCommandEvent &event)
  // Event handler for the open menu item
{
    wxFileDialog* OpenDialog = new wxFileDialog(
		this, _("Choose a file to open"), wxEmptyString, wxEmptyString,
		_("Text files (*.txt)|*.txt|C++ Source Files (*.cpp, *.cxx)|*.cpp;*.cxx"),
		wxFD_OPEN, wxDefaultPosition);
    if (OpenDialog->ShowModal() == wxID_OK) // if the user click "Open" instead of "Cancel"
	{
		CurrentDocPath = OpenDialog->GetPath();
		SetTitle(wxString("Cicruit from - ") << OpenDialog->GetFilename());
	}

//    if (!input_stream.IsOk())
//    {
//        wxLogError("Cannot open file '%s'.", openFileDialog.GetPath());
//        return;
//    }
}

void MyFrame::OnSwitch(wxCommandEvent &event)
  // Event handler for the switch button
{

  
  // wxArrayString choices;
  // choices.Add(wxT("One"));
  // choices.Add(wxT("Two"));
  // choices.Add(wxT("Three"));
  // choices.Add(wxT("Four"));
  // choices.Add(wxT("Five"));

  // for(int i = 0; i < StringArray.size(); i++){
  //   wxStringArray.Add(wxT(StringArray[i]));
  // }

  wxMultiChoiceDialog dialog(this,
  wxT("A multi-choice convenience dialog"),
  wxT("Please select several values"),
  wxSwitchNameArray);

  if (dialog.ShowModal() == wxID_OK)
  {
    bool cmdok = true;
    wxArrayInt selections = dialog.GetSelections();
    wxString msg;
    msg.Printf(wxT("You selected %i items:\n"),
    int(selections.GetCount()));
    for(int i = 0; i<wxSwitchNameArray.size(); i++){
      dmz->setswitch (SwitchIDArray[i], low, cmdok);
    }
    for ( size_t n = 0; n < selections.GetCount(); n++ )
    {
    msg += wxString::Format(wxT("\t%d: %d (%s)\n"),
    int(n), int(selections[n]),
    wxSwitchNameArray[selections[n]].c_str());
    dmz->setswitch (SwitchIDArray[selections[n]], high, cmdok);
    }
    wxMessageBox(msg, wxT("Got selections"));

    devlink devicesList = firstDevice;
    
    //THIS IS FOR CHECKING THE STATES OF THE SWITCHES
    // int j = 0;
    // while(devicesList->next != NULL){
    //   if(devicesList->kind == aswitch){
    //     asignal SwitchState = devicesList->swstate;
    //     cout << "SWITCH STATE: " << SwitchState << endl;
    //   }
    //   devicesList = devicesList->next;
    //   j++;
    // }
  }


  //SINGLE CHOICE STUFF
  // const wxString choices[] = { wxT("One"), wxT("Two"), wxT("Three"), wxT("Four"), wxT("Five") } ;

  //   wxSingleChoiceDialog dialog(this,
  //                               wxT("This is a small sample\n")
  //                               wxT("A single-choice convenience dialog"),
  //                               wxT("Please select a value"),
  //                               WXSIZEOF(choices), choices);

  //   dialog.SetSelection(2);

  //   if (dialog.ShowModal() == wxID_OK)
  //   {
  //       wxMessageDialog dialog2(this, dialog.GetStringSelection(), wxT("Got string"));
  //       dialog2.ShowModal();
  //   }
}

void MyFrame::OnSetMon(wxCommandEvent &event)
  // Event handler for the set monitor button
{

  wxArrayString choices;
  choices.Add(wxT("One"));
  choices.Add(wxT("Two"));
  choices.Add(wxT("Three"));
  choices.Add(wxT("Four"));
  choices.Add(wxT("Five"));

  wxMonitorArray.clear();
  for( int i = 0; i<MonitorTable.used; i++){
    namestring MonName = nmz->get_str(MonitorTable.sigs[i].devid);
    namestring MonOutput = nmz->get_str(MonitorTable.sigs[i].op->id);
    string MonListString = MonName + ": " + MonOutput;
    wxMonitorArray.push_back(wxString(MonListString));
  }

  wxMultiChoiceDialog dialog(this,
  wxT("A multi-choice convenience dialog"),
  wxT("Please select several values"),
  wxMonitorArray);
  
  dialog.SetSelections(selectedArray);

  if (dialog.ShowModal() == wxID_OK)
  {

    selectedArray.clear();
    bool cmdok = true;
    wxArrayInt selections = dialog.GetSelections();
    wxString msg;
    msg.Printf(wxT("You selected %i items:\n"),
    int(selections.GetCount()));

    for(int i = 0; i<MonitorTable.used; i++){
      mmz->remmonitor (MonitorTable.sigs[i].devid, MonitorTable.sigs[i].op->id, cmdok);
    }
    for ( size_t n = 0; n < selections.GetCount(); n++ )
    {
    msg += wxString::Format(wxT("\t%d: %d (%s)\n"),
    int(n), int(selections[n]),
    wxMonitorArray[selections[n]].c_str());
    mmz->makemonitor(MonitorTable.sigs[selections[n]].devid, MonitorTable.sigs[selections[n]].op->id, cmdok);
    selectedArray.push_back(selections[n]);
    }
    cyclescompleted = 0;


// if (dialog.ShowModal() == wxID_OK)
//   {
//     bool cmdok = true;
//     wxArrayInt selections = dialog.GetSelections();
//     wxString msg;
//     msg.Printf(wxT("You selected %i items:\n"),
//     int(selections.GetCount()));
//     for(int i = 0; i<wxSwitchNameArray.size(); i++){
//       dmz->setswitch (SwitchIDArray[i], low, cmdok);
//     }
//     for ( size_t n = 0; n < selections.GetCount(); n++ )
//     {
//     msg += wxString::Format(wxT("\t%d: %d (%s)\n"),
//     int(n), int(selections[n]),
//     wxSwitchNameArray[selections[n]].c_str());
//     dmz->setswitch (SwitchIDArray[selections[n]], high, cmdok);
//     }
//     wxMessageBox(msg, wxT("Got selections"));

//     devlink devicesList = firstDevice;




    wxMessageBox(msg, wxT("Got selections"));
  }
}

void MyFrame::OnAbout(wxCommandEvent &event)
  // Event handler for the about menu item
{
  wxMessageDialog about(this, "Logic simulator GUI written by \nBianca Bilovolschi - bab42 \nJawwad Farid - jmf81 \nOsman Ramadan - oior2 \nMay-June 2017", "About Logsim", wxICON_INFORMATION | wxOK);
  about.ShowModal();
}

void MyFrame::OnHelp(wxCommandEvent &event) //added by me
  // Event handler for the help menu item
{
  wxMessageDialog help(this, "Help window", "Help window", wxICON_INFORMATION | wxOK);
  help.ShowModal();
}


void MyFrame::OnButton(wxCommandEvent &event)
  // Event handler for the push button
{
  pmz->readin();
  bool ok = false;
  firstDevice = netz->devicelist();
  MonitorTable = mmz->getmontable();
  devlink devicesList = firstDevice;

  // for(int i = 0; i < 5; i++){
  //   cout << "i" << endl; 
  //   if(devices->kind == aswitch){
  //     cout << "Found a switch" << endl;
  //   }
  //   devices = devices->next;
  // }

  //CREATE LIST OF SWITCHES
  int i = 0;
  while(devicesList->next != NULL){
    if(devicesList->kind == aswitch){
      int ID = devicesList->id;
      namestring SwitchName = nmz->get_str(ID);
      wxSwitchNameArray.push_back(wxString(SwitchName));
      SwitchIDArray[i] = ID;
    }
    devicesList = devicesList->next;
    i++;
  }

  for( int i = 0; i<MonitorTable.used; i++){
      selectedArray.push_back(i);
  }

  netz->checknetwork(ok);
  int n, ncycles;
  cyclescompleted = 0;
  ncycles = spin->GetValue();
  wxString text;
  text.Printf("Run button pressed: Running for %d cycles", ncycles);
  mmz->resetmonitor ();
  cout << "Running for " << ncycles << " cycles" << endl;
  runnetwork(ncycles);
  canvas->Render(text, cyclescompleted);
  


/*    if (ok) {
    mmz->resetmonitor();
    dmz->executedevices(ok2);
    cout << "The network is successful? " << (ok && ok2) << endl;
    mmz->recordsignals();
    //dmz->setswitch(43, high, ok);
    for (int i = 0; i < 10; i++) {
      dmz->executedevices(ok2);
      mmz->recordsignals();
      dmz->executedevices(ok2);
      mmz->recordsignals();
      dmz->executedevices(ok2);
      mmz->recordsignals();
      dmz->executedevices(ok2);
      mmz->recordsignals();
      dmz->executedevices(ok2);
      mmz->recordsignals();

    }
    
    mmz->displaysignals();
    
  }*/
  
  cout << "The content of the lookup table is " << endl;
  for (int i = 0; i < nmz->length_of_table; i++) {
    cout << "id "<< i << " name: ";
    nmz->writename(i) ;
    cout<<endl;
  }
  return;

}

void MyFrame::OnContinue(wxCommandEvent &event)
  // Event handler for continue button
{
  int ncycles;
  ncycles = spin->GetValue();
  if (cyclescompleted > 0) {
    if ((ncycles + cyclescompleted) > maxcycles)
      ncycles = maxcycles - cyclescompleted;
      cout << "Continuing for " << ncycles << " cycles" << endl;
      wxString text;
      text.Printf("Continuing for %d cycles", ncycles);
      runnetwork (ncycles);
      canvas->Render(text, cyclescompleted);
  } else
    cout << "Error: nothing to continue!" << endl;
}

void MyFrame::OnSpin(wxSpinEvent &event)
  // Event handler for the spin control
{
  wxString text;

  text.Printf("New spinctrl value %d", event.GetPosition());
  canvas->Render(text);
}

void MyFrame::OnText(wxCommandEvent &event)
  // Event handler for the text entry field
{
  wxString text;

  text.Printf("New text entered %s", event.GetString().c_str());
  canvas->Render(text);
}

void MyFrame::runnetwork(int ncycles)
  // Function to run the network, derived from corresponding function in userint.cc
{
  bool ok = true;
  int n = ncycles;

  while ((n > 0) && ok) {
    dmz->executedevices (ok);
    if (ok) {
      n--;
      mmz->recordsignals ();
    } else
      cout << "Error: network is oscillating" << endl;
  }
  if (ok) {
    mmz->displaysignals ();
    cyclescompleted = cyclescompleted + ncycles;
  }
  else cyclescompleted = 0;
}