#include "gui.h"
#include <GL/glut.h>
#include "wx_icon.xpm"
#include <iostream>
#include <sstream>
#include <algorithm>
using namespace std;

// MyGLCanvas ////////////////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(MyGLCanvas, wxGLCanvas)
EVT_SIZE(MyGLCanvas::OnSize)
EVT_PAINT(MyGLCanvas::OnPaint)
EVT_MOUSE_EVENTS(MyGLCanvas::OnMouse)
END_EVENT_TABLE()

void showError(const char* mess) {
	wxMessageBox(mess, wxT("Error"), wxICON_ERROR);
}

void MyGLCanvas::reset(monitor* mm, names* nm) {
	mmz = mm;
	nmz = nm;
}


int wxglcanvas_attrib_list[5] = { WX_GL_RGBA, WX_GL_DOUBLEBUFFER, WX_GL_DEPTH_SIZE, 16, 0 };

MyGLCanvas::MyGLCanvas(wxWindow *parent, wxWindowID id, monitor* monitor_mod, names* names_mod, const wxPoint& pos,
	const wxSize& size, long style, const wxString& name, const wxPalette& palette) :
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

	text_to_print = "No file selected, please select input file";
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
	
	int w, h;
	GetClientSize(&w, &h);

	if (cycles >= 0) cyclesdisplayed = cycles;

	SetCurrent(*context);
	if (!init) {
		InitGL();
		init = true;
	}
	glClear(GL_COLOR_BUFFER_BIT);

	int square_size = 30; //this is the size of one square on the trace
	int start_corner_x = 100; //this is the corner size that is left empty on the top left part of the canvas
	int start_corner_y = 130; //this is the corner size that is left empty on the top left part of the canvas


	if ((cyclesdisplayed >= 0) && (mmz->moncount() > 0) && cycles != -10) { // draw all the monitor traces

	// here create the big square trace
		
		glLineWidth(1.0);
		glColor3f(0.0, 0.0, 1.0); //blue
		
		int ij = 0;
		for (i = 0; i < mmz->moncount(); i++) //horizontal lines
		{ 
			if (mmz->usedMonitors[i])
			{
				glBegin(GL_LINE_STRIP);
				glVertex2f(start_corner_x, h - start_corner_y - ij*square_size * 2);
				glVertex2f(cyclesdisplayed*square_size + start_corner_x, h - start_corner_y - ij*square_size * 2);
				glEnd();
				glBegin(GL_LINE_STRIP);
				glVertex2f(start_corner_x, h - start_corner_y - square_size - ij*square_size * 2);
				glVertex2f(cyclesdisplayed*square_size + start_corner_x, h - start_corner_y - square_size - ij*square_size * 2);
				glEnd();
				ij = ij+1;
			}
		}

		for (i = 0; i < cyclesdisplayed + 1; i++) //vertical lines
		{
			//string mystr;
			//stringstream iout;
			//iout << i;
			//mystr = iout.str();

			wxString mystring = wxString::Format(wxT("%i"), i);

			PrintOnCanvas(mystring, start_corner_x + i*square_size, h - start_corner_y + square_size*2.5);

			ij = 0;

			glBegin(GL_LINE_STRIP);
			glVertex2f(start_corner_x + i*square_size, h - start_corner_y + square_size * 2);
			glVertex2f(start_corner_x + i*square_size, h - start_corner_y + square_size );
			glEnd();

			for (int j = 0; j < mmz->moncount(); j++)
			{		
				if (mmz->usedMonitors[j])
				{
					glBegin(GL_LINE_STRIP);
					glVertex2f(start_corner_x + i*square_size, h - start_corner_y - ij*square_size * 2);
					glVertex2f(start_corner_x + i*square_size, h - start_corner_y - square_size - ij*square_size * 2);
					glEnd();
					ij = ij+1;
				}
			}

			glBegin(GL_LINE_STRIP);
			glVertex2f(start_corner_x + i*square_size, h - start_corner_y - ij*square_size * 2);
			glVertex2f(start_corner_x + i*square_size, h - start_corner_y - square_size - ij*square_size * 2);
			glEnd();

			PrintOnCanvas(mystring, start_corner_x + i*square_size, h - start_corner_y - (ij)*square_size * 2 - square_size*1.5);
		}

		ij = 0;
		glLineWidth(2.0);
		for (int j = 0; j < mmz->moncount(); j++)
		{
			if (mmz->usedMonitors[j])
			{	
				name id1, id2;
				string out_str = "";
		    	mmz->getmonname(j, id1,id2);
		    	out_str.append(nmz->get_str(id1));
		    
		    if (id2 != -1){ //check if device is dtype
				out_str.append(".");
				out_str.append(nmz->get_str(id2));
				}
			wxString myoutputname(out_str);
			PrintOnCanvas(myoutputname, 10, h - start_corner_y - square_size / 2 - ij*square_size * 2);
			
			
			
			glBegin(GL_LINE_STRIP);
			for (i = 0; i < cyclesdisplayed; i++) {
				if (mmz->getsignaltrace(j, i, s)) {
					if (s == low) { y = h - start_corner_y - square_size - ij*square_size * 2;   glColor3f(1.0, 0.0, 0.0); } //red 
					if (s == high) { y = h - start_corner_y - ij*square_size * 2;   glColor3f(1.0, 0.0, 0.0); } //red
					if (s == rising) { y = h - start_corner_y - square_size / 2 - ij*square_size * 2;   glColor3f(1.0, 0.8, 0.8); } //pink
					if (s == falling) { y = h - start_corner_y - square_size / 2 - ij*square_size * 2;   glColor3f(1.0, 0.8, 0.8); } //pink
					glVertex2f(start_corner_x + i*square_size, y);
					glVertex2f(start_corner_x + square_size + i*square_size, y);
				}
			}
			glEnd();
			ij = ij+1;
		}
		}


	}

	else if (cycles == -10)
	{
		SwapBuffers();
		cyclesdisplayed = -1;	
	}

	PrintOnCanvas(example_text,10,h-15);

	// We've been drawing to the back buffer, flush the graphics pipeline and swap the back buffer to the front
	glFlush();
	SwapBuffers();
}

void MyGLCanvas::resetView() {
	pan_x = 0;
	pan_y = 0;
	zoom = 1.0;
	init = false;
	Render(text_to_print);
}
void MyGLCanvas::InitGL()
// Function to initialise the GL context
{
	int w, h;

	GetClientSize(&w, &h);
	SetCurrent(*context);
	glDrawBuffer(GL_BACK);
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glViewport(0, 0, (GLint)w, (GLint)h);
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

void MyGLCanvas::setCanv(int cyclescompleted) {
	int w, h;
	GetClientSize(&w, &h);
	if (100 + cyclescompleted*30 + pan_x > w) {
		pan_x = 100 - cyclescompleted*30 + 0.5 * w;
		init = false;
	}
}
void MyGLCanvas::OnPaint(wxPaintEvent& event)
// Event handler for when the canvas is exposed
{
	int w, h;
	wxPaintDC dc(this); // required for correct refreshing under MS windows
	GetClientSize(&w, &h);
	Render(text_to_print, -1);
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
		text.Printf("Mouse button %d pressed at %d %d", event.GetButton(), event.m_x, h - event.m_y);
	}
	if (event.ButtonUp()) text.Printf("Mouse button %d released at %d %d", event.GetButton(), event.m_x, h - event.m_y);
	if (event.Dragging()) {
		pan_x += event.m_x - last_x;
		pan_y -= event.m_y - last_y;
		last_x = event.m_x;
		last_y = event.m_y;
		init = false;
		text.Printf("Mouse dragged to %d %d, pan now %d %d", event.m_x, h - event.m_y, pan_x, pan_y);
	}
	if (event.Leaving()) text.Printf("Mouse left window at %d %d", event.m_x, h - event.m_y);
	if (event.GetWheelRotation() < 0) {
		zoom = zoom * (1.0 - (double)event.GetWheelRotation() / (20 * event.GetWheelDelta()));
		init = false;
		text.Printf("Negative mouse wheel rotation, zoom now %f", zoom);
	}
	if (event.GetWheelRotation() > 0) {
		zoom = zoom / (1.0 + (double)event.GetWheelRotation() / (20 * event.GetWheelDelta()));
		init = false;
		text.Printf("Positive mouse wheel rotation, zoom now %f", zoom);
	}

	if (event.GetWheelRotation() || event.ButtonDown() || event.ButtonUp() || event.Dragging() || event.Leaving()) Render(text_to_print);
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
EVT_BUTTON(MAKECONNECTION_BUTTON_ID, MyFrame::OnMakeCon) //added by me
EVT_BUTTON(REMOVECONNECTION_BUTTON_ID, MyFrame::OnRemCon) //added by me
EVT_BUTTON(RESTCANVAS_BUTTON_ID, MyFrame::OnRestCav)
EVT_SPINCTRL(MY_SPINCNTRL_ID, MyFrame::OnSpin)
END_EVENT_TABLE()

MyFrame::MyFrame(wxWindow *parent, const wxString& title, const wxPoint& pos, const wxSize& size,
	names *names_mod, devices *devices_mod, monitor *monitor_mod, parser *parser_mod, scanner *scanner_mod, network *network_mod, long style) :
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

	wxMenu *fileMenu = new wxMenu;
	
  	fileMenu->Append(wxID_OPEN, "&Open");
	fileMenu->Append(wxID_ABOUT, "&About");
	fileMenu->Append(wxID_HELP, "&Help");
	fileMenu->Append(wxID_EXIT, "&Quit");
	wxMenuBar *menuBar = new wxMenuBar;
	menuBar->Append(fileMenu, "&File");
	SetMenuBar(menuBar);

	wxBoxSizer *topsizer = new wxBoxSizer(wxHORIZONTAL);
	canvas = new MyGLCanvas(this, wxID_ANY, monitor_mod, names_mod);
	topsizer->Add(canvas, 1, wxEXPAND | wxALL, 10);

	wxBoxSizer *button_sizer = new wxBoxSizer(wxVERTICAL);
	const wxSize* switchStateBoxSize = new wxSize(150, 100);
	
	switchesList = new wxListBox(this, wxID_ANY, wxDefaultPosition, *switchStateBoxSize, 0, NULL, 0);
	button_sizer->Add(new wxStaticText(this, wxID_ANY, "Switches States:"), 0, wxTOP | wxLEFT | wxRIGHT, 10);
	button_sizer->Add(switchesList, 0, wxALL, 10);
	button_sizer->Add(new wxButton(this, MY_BUTTON_ID, "Run"), 0, wxALL, 10);
	button_sizer->Add(new wxButton(this, CONTINUE_BUTTON_ID, "Continue"), 0, wxALL, 10); //added by me
	button_sizer->Add(new wxButton(this, RESTCANVAS_BUTTON_ID, "Reset View"), 0, wxALL, 10); //added by me
	button_sizer->Add(new wxButton(this, SETSWITCH_BUTTON_ID, "Set Switch"), 0, wxALL, 10); //added by me
	button_sizer->Add(new wxButton(this, SETMONITOR_BUTTON_ID, "Set Monitor point"), 0, wxALL, 10); //added by me
	button_sizer->Add(new wxButton(this, MAKECONNECTION_BUTTON_ID, "Make Connection"), 0, wxALL, 10); //added by me
	button_sizer->Add(new wxButton(this, REMOVECONNECTION_BUTTON_ID, "Remove Connection"), 0, wxALL, 10); //added by me
	button_sizer->Add(new wxStaticText(this, wxID_ANY, "Cycles"), 0, wxTOP | wxLEFT | wxRIGHT, 10);
	spin = new wxSpinCtrl(this, MY_SPINCNTRL_ID, wxString("10"));
	button_sizer->Add(spin, 0, wxALL, 10);
	topsizer->Add(button_sizer, 0, wxALIGN_CENTER);
	SetSizeHints(750, 500);
	SetSizer(topsizer);
}

void MyFrame::OnRestCav(wxCommandEvent &event) {
	canvas->resetView();
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
		SetTitle(wxString("Circuit from - ") << OpenDialog->GetFilename());
		
		canvas->resetView();
		canvas->text_to_print.Printf("File selected, press 'Run' to start simulation");
		switchesList->Clear();
		cyclescompleted = 0;
		canvas->Render(canvas->text_to_print, -10);
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
	if (CurrentDocPath == "") {
		showError("Need to select the logic description file first.");
		canvas->text_to_print.Printf("No file selected, please select input file");
		canvas->Render(canvas->text_to_print);
		return;
	}

	wxMultiChoiceDialog dialog(this,
		wxT("Check the switches you wish to set to high from the list below"),
		wxT("Set switches"),
		wxSwitchNameArray);

	dialog.SetSelections(selectedSwitchArray);

	if (dialog.ShowModal() == wxID_OK)
	{
		bool cmdok = true;
		wxArrayInt selections = dialog.GetSelections(); //array of indices of selected switches
		for (int i = 0; i < wxSwitchNameArray.size(); i++) {//set all switches to low
			dmz->setswitch(SwitchIDArray[i], low, cmdok);
		}
		selectedSwitchArray.clear();
		
		for (size_t n = 0; n < selections.GetCount(); n++)//set selected switches to high
		{
			// msg += wxString::Format(wxT("\t%d: %d (%s)\n"),
			// 	int(n), int(selections[n]),
			// 	wxSwitchNameArray[selections[n]].c_str());
			dmz->setswitch(SwitchIDArray[selections[n]], high, cmdok);
			selectedSwitchArray.push_back(selections[n]);
		}
		//wxMessageBox(msg, wxT("Got selections"));
		updateSwitchList();

		devlink devicesList = firstDevice;
		canvas->text_to_print.Printf("Switches set");
		canvas->Render(canvas->text_to_print);

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
}

void MyFrame::OnSetMon(wxCommandEvent &event)
// Event handler for the set monitor button
{

	if (CurrentDocPath == "") {
		showError("Need to select the logic description file first.");
		canvas->text_to_print.Printf("No file selected, please select input file");
		canvas->Render(canvas->text_to_print);
		return;
	}

	wxMultiChoiceDialog dialog(this,
		wxT("Check the device outputs you wish to monitor from the list below"),
		wxT("Set monitor points"),
		wxMonitorArray);

	dialog.SetSelections(selectedArray);
	
	

	if (dialog.ShowModal() == wxID_OK)
	{

		selectedArray.clear();
		bool cmdok = true;
		wxArrayInt selections = dialog.GetSelections();
		selectedArray = selections;
		for(int i = 0; i < 1000; i++){
			mmz->usedMonitors[i] = false;
		}
		for (size_t n = 0; n < selections.GetCount(); n++)
		{
			int value = selections[n];
			mmz->usedMonitors[value] = true;
		}
 
    //int ncycles = spin->GetValue()
    wxString text;
    text.Printf("Set monitor points.");
    //mmz->resetmonitor();
    //runnetwork(ncycles);/////////THIS CAUSES THE NEXT SET OF CYCLES TO RUN! BUT CANT REMOVE
    canvas->Render(text, cyclescompleted);
	}
}


void MyFrame::OnMakeCon(wxCommandEvent &event)
// Event handler for the make connection button
{

	if (CurrentDocPath == "") {
		showError("Need to select the logic description file first.");
		canvas->text_to_print.Printf("No file selected, please select input file");
		canvas->Render(canvas->text_to_print);
		return;
	}

	wxArrayString wxDeviceArray = wxMonitorArray;

	wxSingleChoiceDialog dialog(this,
		wxT("Select the device output you wish to connect to another device."),
		wxT("Make connection: Select Output"),
		wxDeviceArray);

	if (dialog.ShowModal() == wxID_OK)
	{
		bool cmdok = true;
		int selectedOutDeviceIndex = dialog.GetSelection();
		cout << "SELECTED CONNECTION OUTPUT: " << nmz->get_str(MonitorIDArray[selectedOutDeviceIndex]);
		int selectedOutDeviceID = MonitorOutIDArray[selectedOutDeviceIndex];
		cout << "Selected device ID for connection: " << selectedOutDeviceID << endl;
		cout << "Selected device name for connection: " << nmz->get_str(selectedOutDeviceID) << endl;

		wxSingleChoiceDialog dialog2(this,
		wxT("Select the device input you wish to connect the previously selected output to."),
		wxT("Make connection: Select Input"),
		DeviceInArray);
		if(dialog2.ShowModal() == wxID_OK){
			int selectedInDeviceIndex = dialog2.GetSelection();
			int selectedInDeviceID = DeviceInInputIDArray[selectedInDeviceIndex];

			// cout << "Selected input device name ID: " << DeviceInIDArray[selectedInDeviceIndex] << endl;
			// cout << "Selected input name ID: " << selectedInDeviceID << endl;
			// cout << "Selected output device name ID: " << MonitorIDArray[selectedOutDeviceIndex] << endl;
			// cout << "Selected output name ID: " << selectedOutDeviceID << endl;
			netz->makeconnection(DeviceInIDArray[selectedInDeviceIndex], selectedInDeviceID, MonitorIDArray[selectedOutDeviceIndex], selectedOutDeviceID, cmdok);
		}
		
	// for (devlink d = firstDevice; d != NULL; d = d->next){
 //    		for (inplink i = d->ilist; i != NULL; i = i->next){
 //      			namestring devicename = nmz->get_str(d->id);
 //      			namestring deviceinputname = nmz->get_str(i->id);
 //      			namestring deviceconnection = nmz->get_str(i->connect->devid);
 //      			cout << "Name of device: " << devicename << endl;
 //      			cout << "Name of input: " << deviceinputname << endl;
 //      			cout << "Output its connected to: " << deviceconnection << endl;
 //      		}
	// 	}
	
      

	}
}

void MyFrame::OnRemCon(wxCommandEvent &event)
// Event handler for the remove connection button
{

	if (CurrentDocPath == "") {
		showError("Need to select the logic description file first.");
		canvas->text_to_print.Printf("No file selected, please select input file");
		canvas->Render(canvas->text_to_print);
		return;
	}

	wxConnectionArray.clear();

		for (devlink d = firstDevice; d != NULL; d = d->next){
    		for (inplink i = d->ilist; i != NULL; i = i->next){
      			namestring devicename = nmz->get_str(d->id);
      			removeConnectionInputIDArray.push_back(d->id);
      			removeConnectionDevIDArray.push_back(i->id);
      			namestring deviceinputname = nmz->get_str(i->id);
      			namestring deviceconnection = nmz->get_str(i->connect->devid);
      			namestring deviceoutputname = nmz->get_str(i->connect->id);
      			wxString connectionArrayEntry;
      			if(deviceoutputname == "Blankname"){
      				connectionArrayEntry = deviceconnection + " -> " + devicename + ":" + deviceinputname; 
      			}
      			else{
      				connectionArrayEntry = deviceconnection + ":" + deviceoutputname + " -> " + devicename + ":" + deviceinputname; 
      			}
      			wxConnectionArray.push_back(connectionArrayEntry);
      			// cout << "Name of device: " << devicename << endl;
      			// cout << "Name of input: " << deviceinputname << endl;
      			// cout << "Output its connected to: " << deviceconnection << endl;
      		}
		}


	wxSingleChoiceDialog dialog(this,
		wxT("Select the connection/s you wish to remove"),
		wxT("Remove connections"),
		wxConnectionArray);


	if (dialog.ShowModal() == wxID_OK)
	{
		bool cmdok = true;
		int selectedConnectionIndex = dialog.GetSelection();
		cout << "SELECTED REMOVE CONNECTION device name: ";
		nmz->writename(removeConnectionDevIDArray[selectedConnectionIndex]);
		cout << endl;
		cout << "SELECTED REMOVE CONNECTION input name: ";
		nmz->writename(removeConnectionInputIDArray[selectedConnectionIndex]);
		cout << endl;
		//int selectedInDeviceID = Monit
		//netz->makeconnection(selectedInDeviceIndex, selectedInDeviceID, selectedOutDeviceIndex, selectedOutDeviceID, cmdok);
		
		
		// for (devlink d = firstDevice; d != NULL; d = d->next){
  //   		for (inplink i = d->ilist; i != NULL; i = i->next){
  //     			namestring devicename = nmz->get_str(d->id);
  //     			namestring deviceinputname = nmz->get_str(i->id);
  //     			namestring deviceconnection = nmz->get_str(i->connect->id);
  //     			cout << "Name of device: " << devicename << endl;
  //     			cout << "Name of input: " << deviceinputname << endl;
  //     			cout << "Output its connected to: " << deviceconnection << endl;
  //     		}
		// }
      

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

	wxMessageDialog help(this, "Logic simulator \nUse 'OPEN' to select the desired definition file - see user manual for correct syntax \n \n'Run' - run the simulation for n cycles \n'Continue' - continue to run the simulation for another n cycles \n'Set Switch' - list of all switches and their current state (low/high), which can be modified \n'Set Monitor Point' - list of all outputs in the system that can be monitored \n'Cycles' - select the number of cycles to run/continue", "Help window", wxICON_INFORMATION | wxOK);
	help.ShowModal();
}

void MyFrame::updateSwitchList(void) {
	int k = 0;
	wxArrayString switchListArray;
	int size = selectedSwitchArray.size();
	string state;
	for (int i = 0; i < wxSwitchNameArray.size(); i++) {
		state = ": LOW";
		if (k < size &&  selectedSwitchArray[k] == i) {
			state = ": HIGH";
			k++;
		}
		switchListArray.push_back(wxSwitchNameArray[i] + state);
	}
	switchesList->Clear();
	switchesList->InsertItems(switchListArray, 0);
	
}

void MyFrame::OnButton(wxCommandEvent &event)
// Event handler for the push button
{
	if (CurrentDocPath == "") {
		showError("Need to select the logic description file first.");
		canvas->text_to_print.Printf("No file selected, please select input file");
		canvas->Render(canvas->text_to_print);
		return;
	}

	nmz = new names();
	netz = new network(nmz);
	dmz = new devices(nmz, netz);
	mmz = new monitor(nmz, netz);
	smz = new scanner(nmz, CurrentDocPath.mb_str());
	pmz = new parser(netz, dmz, mmz, smz, nmz);
	canvas->reset(mmz, nmz);
	if (!pmz->readin()) return;
	//bool ok = false;
	firstDevice = netz->devicelist();
	mmz->MonitorTable = mmz->getmontable();
	devlink devicesList = firstDevice;
	bool cmdok = true;

	// for (devlink d = firstDevice; d != NULL; d = d->next){
 //    		for (inplink i = d->ilist; i != NULL; i = i->next){
 //      			namestring devicename = nmz->get_str(d->id);
 //      			namestring deviceinputname = nmz->get_str(i->id);
 //      			namestring deviceconnection = nmz->get_str(i->connect->devid);
 //      			cout << "Name of device: " << devicename << endl;
 //      			cout << "Name of input: " << deviceinputname << endl;
 //      			cout << "Output its connected to: " << deviceconnection << endl;
 //      		}
	// 	}
	

	for(int i = 0; i < 1000; i++){
		mmz->usedMonitors[i] = false;
	}
	wxMonitorArray.clear();//MonitorArray contains strings of Monitor name
	MonitorIDArray.clear();//MonitorIDArray contains ID of device
	MonitorOutIDArray.clear();//MonitorOutIDArray contains ID of device output
	for(int i = 0; i < mmz->MonitorTable.used; i++){//populate arrays with existing monitors defined in config file
	namestring MonName = nmz->get_str(mmz->MonitorTable.sigs[i].devid);
	namestring MonOutput = nmz->get_str(mmz->MonitorTable.sigs[i].op->id);
	string MonListString;
	if(MonOutput == "Blankname"){
	  MonListString = MonName;
	}
	else{
	  MonListString = MonName + ": " + MonOutput;
	}
	wxMonitorArray.push_back(wxString(MonListString));
	MonitorIDArray.push_back(mmz->MonitorTable.sigs[i].devid);
	MonitorOutIDArray.push_back(mmz->MonitorTable.sigs[i].op->id);
	}
	DeviceNameArray.clear();//contains namestrings of devices
	DeviceOutArray.clear();//contains namesstrings of outputs of devices
	DeviceInArray.clear();
  	while (devicesList != NULL) {
	    namestring DevName = nmz->get_str(devicesList->id);//get name string of device
	    DeviceNameArray.push_back(DevName);
	    namestring DevOutName = nmz->get_str(devicesList->olist->id);
	    //namestring DevInName = nmz->get_str(devicesList->ilist->id);
	    
	   	    DeviceOutArray.push_back(wxString(DevOutName));
	    string MonListString;
	    if(DevOutName == "Blankname"){
	      MonListString = DevName;
	    }
	    else{
	      MonListString = DevName + ": " + DevOutName;
	    }
	    if(std::find(wxMonitorArray.begin(), wxMonitorArray.end(), MonListString) != wxMonitorArray.end()){//check if device is already in monitor array
	      devicesList = devicesList->next;
	    }
	    else{
	      wxMonitorArray.push_back(wxString(MonListString));
	      MonitorIDArray.push_back(devicesList->id);
	      MonitorOutIDArray.push_back(devicesList->olist->id);
	    	}
  	}

  	for (int i = 0; i < mmz->MonitorTable.used; i++) {//remove all used monitors
		mmz->remmonitor(mmz->MonitorTable.sigs[i].devid, mmz->MonitorTable.sigs[i].op->id, cmdok);
		mmz->usedMonitors[i] = true;
	}
  	for (size_t n = 0; n < MonitorIDArray.size(); n++)
	{
		mmz->makemonitor(MonitorIDArray[n], MonitorOutIDArray[n], cmdok);
	}


	devicesList = firstDevice;

	//CREATE LIST OF SWITCHES
	wxSwitchNameArray.clear();
	selectedSwitchArray.clear();
	wxArrayString switchListArray;

	int i = 0;
	while (devicesList != NULL) {
		if (devicesList->kind == aswitch) {
			int ID = devicesList->id;
	    	asignal SwitchState = devicesList->swstate;
	    	if(SwitchState == high){
	        	selectedSwitchArray.push_back(i);//save the index of high switch
	    	}
			namestring SwitchName = nmz->get_str(ID);
			wxSwitchNameArray.push_back(wxString(SwitchName));
			SwitchIDArray[i] = ID;
			i++;
		}    
		devicesList = devicesList->next;
		
	}
devicesList = firstDevice;
while(devicesList != NULL){
	    inplink  i;
		for (i = devicesList->ilist; i != NULL; i = i->next) {
			//nmz->writename (i->id);
			namestring DevInName = nmz->get_str(devicesList->id);
			DeviceInIDArray.push_back(devicesList->id);
			namestring DevInputName = nmz->get_str(i->id);
			DeviceInInputIDArray.push_back(i->id);
			wxString DevInputArrayString = DevInName + ": " + DevInputName;
			DeviceInArray.push_back(DevInputArrayString);
		}
		devicesList = devicesList->next;
}

	selectedArray.clear();
	for (int i = 0; i < mmz->MonitorTable.used; i++) {
		selectedArray.push_back(i);
	}
	updateSwitchList();

	//netz->checknetwork(ok);
	int n, ncycles;
	cyclescompleted = 0;
	ncycles = spin->GetValue();


	canvas->text_to_print.Printf("Run button pressed: Running for %d cycles", ncycles);
	mmz->resetmonitor();
	cout << "Running for " << ncycles << " cycles" << endl;
	runnetwork(ncycles);
	canvas->Render(canvas->text_to_print, cyclescompleted);

	canvas->resetView();
	cout << "The content of the lookup table is " << endl;
	for (int i = 0; i < nmz->length_of_table; i++) {
		cout << "id " << i << " name: ";
		nmz->writename(i);
		cout << endl;
	}



	return;

}

void MyFrame::OnContinue(wxCommandEvent &event)
// Event handler for continue button
{
	if (CurrentDocPath == "") {
		showError("Need to select the logic description file first.");
		canvas->text_to_print.Printf("No file selected, please select input file");
		canvas->Render(canvas->text_to_print);
		return;
	}

	int ncycles;
	ncycles = spin->GetValue();
	if (cyclescompleted > 0) {
		if ((ncycles + cyclescompleted) > maxcycles)
			ncycles = maxcycles - cyclescompleted;

		cout << "Continuing for " << ncycles << " cycles" << endl;
		canvas->text_to_print.Printf("Continuing for %d cycles", ncycles);
		runnetwork(ncycles);
		canvas->setCanv(cyclescompleted);
		canvas->Render(canvas->text_to_print, cyclescompleted);
	}
	else
	{
		canvas->text_to_print.Printf("Error: nothing to continue!");
		canvas->Render(canvas->text_to_print);
		cout << "Error: nothing to continue!" << endl;
	}	
	
}

void MyFrame::OnSpin(wxSpinEvent &event)
// Event handler for the spin control
{
	canvas->text_to_print.Printf("Simulation Cycles: %d", event.GetPosition());
	canvas->Render(canvas->text_to_print);
}


void MyFrame::runnetwork(int ncycles)
// Function to run the network, derived from corresponding function in userint.cc
{
	bool ok = true;
	int n = ncycles;

	while ((n > 0) && ok) {
		dmz->executedevices(ok);
		if (ok) {
			n--;
			mmz->recordsignals();
		}
		else{
			cout << "Error: network is oscillating" << endl;
			canvas->text_to_print.Printf("Error: network is oscillating");
			canvas->Render(canvas->text_to_print);
		}
	}
	if (ok) {
		mmz->displaysignals();
		cyclescompleted = cyclescompleted + ncycles;
	}
	else cyclescompleted = 0;
}
