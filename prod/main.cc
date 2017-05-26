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
	cout << "The content of the lookup table is " << endl;
	for (int i = 0; i < nmz->length_of_table; i++) {
<<<<<<< HEAD
<<<<<<< HEAD
	cout << "id "<< i << " name: "<< nmz->table[i] << endl;
	}
	return 0;
}
=======
	  cout << "id "<< i << " name: ";
	  nmz->writename(i) ;
	  cout<<endl;
	}
	return 0;
}
>>>>>>> origin/bianca
=======
	  cout << "id "<< i << " name: ";
	  nmz->writename(i) ;
	  cout<<endl;
	}
	return 0;
}
>>>>>>> 3fdbb80d5fbd9ab04929e250e13a63e2a265b576
