#include "guitest.h"
#include "gui.h"
#include <GL/glut.h>

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
  // This function is automatically called when the application starts
{
  // Construct the four classes required by the innards of the logic simulator
  nmz = new names();
  netz = new network(nmz);
  dmz = new devices(nmz, netz);
  mmz = new monitor(nmz, netz);
  smz = new scanner(nmz, "file.txt"); // second arg wxString(argv[1]).mb_str()
  pmz = new parser(netz, dmz, mmz, smz);
  //pmz->readin();
  // glutInit cannot cope with Unicode command line arguments, so we pass
  // it some fake ASCII ones instead
  char **tmp1; int tmp2 = 0; glutInit(&tmp2, tmp1);
  // Construct the GUI
  MyFrame *frame = new MyFrame(NULL, "Logic simulator", wxDefaultPosition,  wxSize(800, 600), nmz, dmz, mmz, pmz, smz, netz);
  frame->Show(true);
  return(true); // enter the GUI event loop

}
