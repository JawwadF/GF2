#include <iostream>
#include "parser.h"

using namespace std;

int main(void) {
	// Construct the six classes required by the innards of the logic simulator
	names* nmz = new names();
	network* netz = new network(nmz);
	devices* dmz = new devices(nmz, netz);
	monitor* mmz = new monitor(nmz, netz);
	scanner* smz = new scanner(nmz, "file.txt");
	parser* pmz = new parser(netz, dmz, mmz, smz);
	pmz->readin();
	bool ok = false;
	bool ok2 = false;
	//dmz->debug(true);
	devlink devices = netz->devicelist();
	netz->checknetwork(ok);
	
	
	if (ok) {
		mmz->resetmonitor();
		dmz->executedevices(ok2);
		cout << "The network is successful? " << (ok && ok2) << endl;
		mmz->recordsignals();
		//dmz->setswitch(43, high, ok);
		for (int i = 0; i < 20; i++) {
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
		
	} 
	
	cout << "The content of the lookup table is " << endl;
	for (int i = 0; i < nmz->length_of_table; i++) {
	  cout << "id "<< i << " name: ";
	  nmz->writename(i) ;
	  cout<<endl;
	}
	return 0;
}
