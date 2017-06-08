#include <iostream>
#include "parser.h"

using namespace std;

devlink dev;
symbol cursym;
name id;
string errorMessage = "";

bool noerrors = true;
int num;
string signalstr = "";

/***********************************************************************
 * 
 * Reads the definition of the logic system and builds the          
 * corresponding internal representation via calls to the 'Network' 
 * module and the 'Devices' module.                                 
 *
 */
bool parser::readin(void) {
	symbol tempsym;
	bool noerror;
	noerrors = true;
	bool noMonitor = true;
	errorMessage = "";
	smz->getsymbol(cursym, id, num, signalstr);
	while (cursym != eofsym) {
		noerror = true;
		tempsym = cursym;
		smz->getsymbol(cursym, id, num, signalstr);
		if (cursym != equals) {
			errorMessage = errorMessage + "Line " + to_string(smz->counter) + ": ERROR expecting the equal sign\n";
			cout << "SYNTATIC ERROR expecting the equal sign" << endl;
			errorMessage = errorMessage + smz->geterror() + "\n";
			noerror =  false;
		}
		bool hasMonitor;
		switch (tempsym)
		{
		case devsym:
			noerror = noerror && device();
			break;
		case monsym:
			hasMonitor = monitor_();
			noerror = noerror && hasMonitor;
			noMonitor = noMonitor && !hasMonitor;
			break;
		case consym:
			noerror = noerror && connection();
			break;
		default:
			noerror = false;
			errorMessage = errorMessage + "Line " + to_string(smz->counter) + ": ERROR "
				"expected keyword DEVICE/CONNECTION/MONITOR or CIRCUIT\n";
			cout << "SYNTATIC ERROR expected keyword DEVICE/CONNECTION/MONITOR or CIRCUIT" << endl;
			errorMessage = errorMessage + smz->geterror() + "\n";
			break;
		}

		if (!noerror) {
			skip();
			
			continue;
		}

		smz->getsymbol(cursym, id, num, signalstr);
		if (cursym != semicol) {
			cout << "SYNTATIC ERROR missing semicolon, got " << id << endl;
			errorMessage = errorMessage + "Line " + to_string(smz->counter) + ": ERROR missing a semicolon\n";
			errorMessage = errorMessage + smz->geterror() + "\n";
			noerror = false;
			skip();
			continue;
		}
		smz->getsymbol(cursym, id, num, signalstr);
		noerrors = noerrors && noerror;
	}
	if (noerrors) {
		string mess;
		netz->checknetwork(noerror, mess);
		if (!noerror) {
			errorMessage = errorMessage + "Line " + to_string(smz->counter) + ": ERROR \n" + mess;
			errorMessage = errorMessage + smz->geterror() + "\n";
			noerrors = false;
		}
		else if (noMonitor) {
			errorMessage = errorMessage + "Line " + to_string(smz->counter) + ": ERROR " + 
				"The circuit must have at least one monitor";
			errorMessage = errorMessage + smz->geterror() + "\n";
			noerrors = false;
		}
	}
	if (!noerrors) showError(errorMessage.c_str());
	return noerrors;

}


/***********************************************************************
 * 
 * If an error has been detected in the input file, this function 	
 * skips reading the input file until the next semcolon, where it 	
 * resumes normal operation. Used for error handling.				
 *
 */
void parser::skip(void) {
	while (cursym != eofsym && cursym != semicol) {
		smz->getsymbol(cursym, id, num, signalstr);
	}
	smz->getsymbol(cursym, id, num, signalstr);
	noerrors = false;
}


/***********************************************************************
 * 
 * Used to initialise a connection between an output device and an 	
 * input device. Called from readin()  								
 * It returns FALSE if there is an error in the initialisation		
 * 	
 */
bool parser::connection(void) {
	smz->getsymbol(cursym, id, num, signalstr);
	name indev = -1, outdev = -1, insig = -1, outsig = -1;
	if (cursym != namesym) {
		cout << "SYNTATIC ERROR in the name" << endl;
		errorMessage = errorMessage + "Line " + to_string(smz->counter) + ": ERROR in the name\n";
		errorMessage = errorMessage + smz->geterror() + "\n";
		return false;
	}
	cout << "There is a connection from " << id << " ";
	outdev = id;
	smz->getsymbol(cursym, id, num, signalstr);
	
	//here we check if the ouput device is a dtype and we are expecting a dot
	
	devlink dd;
	dd=netz->finddevice(outdev);
	if (dd != NULL && dd->kind == dtype)
	{
		if (cursym != dot)
		{
			cout << "SYNTATIC ERROR missing '.'" << endl;
			errorMessage = errorMessage + "Line " + to_string(smz->counter) + ": ERROR missing '.'\n";
			errorMessage = errorMessage + smz->geterror() + "\n";
			return false;
		}
		smz->getsymbol(cursym, id, num, signalstr);
		cout << "output ";
		if (cursym != outsym) {
			cout << "SYNTATIC ERROR in the output" << endl;
			errorMessage = errorMessage + "Line " + to_string(smz->counter) + ": ERROR in the output\n";
			errorMessage = errorMessage + smz->geterror() + "\n";
			return false;
		}
		cout << id << " ";
		outsig = id;
		smz->getsymbol(cursym, id, num, signalstr);
	}
	else {
		if (cursym == dot)
		{
			string outstr = nmz->get_str(outdev);
			cout << "ERROR: device "<<outstr<<" does not have multiple outputs" << endl;
			errorMessage = errorMessage + "Line " + to_string(smz->counter) + ": ERROR: device "+ outstr + " does not have multiple outputs\n";
			errorMessage = errorMessage + smz->geterror() + "\n";
			return false;
		}
		outsig = -1;
		devlink o = netz->finddevice(outdev);
		if (noerrors &&  o != NULL && netz->findoutput(o, -1) == NULL) {
			netz->addoutput(netz->finddevice(outdev), outsig, outdev);
		}
	}
	
	if (cursym != connect_) {
		cout << "SYNTATIC ERROR missing >" << endl;
		errorMessage = errorMessage + "Line " + to_string(smz->counter) + ": ERROR missing >\n";
		errorMessage = errorMessage + smz->geterror() + "\n";
		return false;
	}
	cout << "To input ";
	smz->getsymbol(cursym, id, num, signalstr);
	if (cursym != namesym) {
		cout << "SYNTATIC ERROR in the name " << cursym << endl;
		errorMessage = errorMessage + "Line " + to_string(smz->counter) + ": ERROR missing the name\n";
		errorMessage = errorMessage + smz->geterror() + "\n";
		return false;
	}
	cout << id << " ";
	indev = id;
	smz->getsymbol(cursym, id, num, signalstr);
	if (cursym != dot) {
		cout << "SYNTATIC ERROR in the dot" << endl;
		errorMessage = errorMessage + "Line " + to_string(smz->counter) + ": ERROR missing the dot\n";
		errorMessage = errorMessage + smz->geterror() + "\n";
		return false;
	}
	smz->getsymbol(cursym, id, num, signalstr);
	if (cursym != insym) {
		cout << "SYNTATIC ERROR in the input" << endl;
		errorMessage = errorMessage + "Line " + to_string(smz->counter) + ": ERROR in the input\n";
		errorMessage = errorMessage + smz->geterror() + "\n";
		return false;
	}
	cout << id <<endl;
	insig = id;
	bool noerror = true;
	if (noerrors) {
		
		devlink din = netz->finddevice(indev);
		devlink dout = netz->finddevice(outdev);
		
			if (din == NULL) {
				errorMessage = errorMessage + "Line " + to_string(smz->counter) + ": ERROR Input device "+
					nmz->get_str(indev) +  " does not exist\n";
				errorMessage = errorMessage + smz->geterror() + "\n";
				noerror = false;
			}
			else if (netz->findinput(din, insig) == NULL) {
				errorMessage = errorMessage + "Line " + to_string(smz->counter) + ": ERROR Input device " +
					nmz->get_str(indev) + " does not have the input "+ nmz->get_str(insig)+ "\n";
				errorMessage = errorMessage + smz->geterror() + "\n";
				noerror = false;
			}
			if (dout == NULL) {
				errorMessage = errorMessage + "Line " + to_string(smz->counter) + ": ERROR Output device " +
					nmz->get_str(outdev) + " does not exist\n";
				errorMessage = errorMessage + smz->geterror() + "\n";
				noerror = false;
			} 
			else if (dout->kind == dtype && netz->findoutput(dout, outsig) == NULL) {
				errorMessage = errorMessage + "Line " + to_string(smz->counter) + ": ERROR The DTYPE " +
					nmz->get_str(outdev) + " must have a valid output (Q or QBAR)\n";
				errorMessage = errorMessage + smz->geterror() + "\n";
				noerror = false;
			}
			if (noerror) {
				netz->makeconnection(indev, insig, outdev, outsig, noerror);
				if (!noerror) {
					errorMessage = errorMessage + "Line " + to_string(smz->counter) + ": ERROR Cannot establish a " +
						"connection from " + nmz->get_str(indev) + " to "+ nmz->get_str(outdev) + "\n";
					errorMessage = errorMessage + smz->geterror() + "\n";
				}
			}

	}
	return noerror;
}


/***********************************************************************
 * 
 * Used during the initialisation of a device or monitor to read the  
 * the name of the new variable from the definition file				
 * It returns FALSE if there is an error with the name initialisation	
 *
 */
bool parser::readname(void) {

	if (id != 37) smz->getsymbol(cursym, id, num, signalstr);
	if (cursym != keysym || id != 37) {
		cout << "SYNTATIC ERROR: expecting the keyword 'NAME' but getting" <<id<< endl;
		errorMessage = errorMessage + "Line " + to_string(smz->counter) + ": ERROR expecting the keyword 'NAME'\n";
		errorMessage = errorMessage + smz->geterror() + "\n";
		return false;
	}
	
	smz->getsymbol(cursym, id, num, signalstr);
	if (cursym != namesym) {
		cout << "SYNTATIC ERROR: expecting name" << cursym << endl;
		errorMessage = errorMessage + "Line " + to_string(smz->counter) + ": ERROR expecting a name\n";
		errorMessage = errorMessage + smz->geterror() + "\n";
		return false;
	}
	cout << "assigned to a variable " << id << endl;

	return true;
}


/***********************************************************************
 * 
 * Used for the initialisation of a device, it checks what type the 	
 * new device is and calls the correct function. Called from readin() 
 * It returns FALSE if there is an error in the initialisation		
 * 	
 */
bool parser::device(void) {
	smz->getsymbol(cursym, id, num, signalstr);
	switch (cursym)
	{
	case xorsym:
		return xor_();
	case gatesym:
		return gate();
	case clksym:
		return clock();
	case swisym:
		return switch_();
	case dtypesym:
		return dtype_();
	case siggensym:
		return siggen_();
	default:
		cout << "SYNTATIC ERROR expecting a device. Received " << cursym << endl;
		errorMessage = errorMessage + "Line " + to_string(smz->counter) + ": ERROR expecting a device\n";
		errorMessage = errorMessage + smz->geterror() + "\n";
		return false;
	}
	return false;
}


/***********************************************************************
 * 
 * Receives and checks the initialisation of a xor gate		
 * from the scanner. Called from device() 							
 * It returns FALSE if there is an error in the initialisation 	
 * 	
 */
bool parser::xor_(void) {
	cout << "Creating a XOR gate ";
	
	if (!readname()) {
		return false;
	}

	bool noerror;
	dmz->makedevice(xorgate, id, 2, noerror, "");
	return noerror;
}



/***********************************************************************
 * 
 * Receives and checks the initialisation of an AND/NAND/OR/NOR gate		
 * from the scanner. Called from device() 							
 * It returns FALSE if there is an error in the initialisation 	
 * 	
 */
bool parser::gate(void) {
	devicekind gatetype;
	switch (id)
	{
	case 2:
		cout << "Creating an AND gate ";
		gatetype = andgate;
		break;
	case 3:
		cout << "Creating a NAND gate ";
		gatetype = nandgate;
		break;
	case 4:
		cout << "Creating a OR gate ";
		gatetype = orgate;
		break;
	case 5:
		cout << "Creating a NOR gate ";
		gatetype = norgate;
		break;
	default:
		cout << "SYNTATIC ERROR expecting a AND/NAND/OR/NOR gate" << endl;
		errorMessage = errorMessage + "Line " + to_string(smz->counter) + ": ERROR expecting a AND/NAND/OR/NOR gate\n";
		errorMessage = errorMessage + smz->geterror() + "\n";
		return false;
		break;
	}
	smz->getsymbol(cursym, id, num, signalstr);
	if (cursym == keysym && id == 18) {
		cout << "that has ";
		smz->getsymbol(cursym, id, num, signalstr);
		if (cursym != numsym) {
			cout << endl << "SYNTATIC ERROR expecting the number of inputs" << endl;
			errorMessage = errorMessage + "Line " + to_string(smz->counter) + ": ERROR expecting the number of inputs\n";
			errorMessage = errorMessage + smz->geterror() + "\n";
			return false;
		}
		else
		{
			if (num > 1 && num <= 16)
				cout << num << " inputs ";
			else {
				cout << endl << "SYNTATIC ERROR: number of inputs has to be integer greater than 1 and not more than 16" << endl;
				errorMessage = errorMessage + "Line " + to_string(smz->counter) + ": ERROR "
					"number of inputs has to be integer greater than 1 and not more than 16\n";
				errorMessage = errorMessage + smz->geterror() + "\n";
				return false;
			}
		}
	}
	else {
		cout << endl << "SYNTATIC ERROR expecting keyword INPUTS" << endl;
		errorMessage = errorMessage + "Line " + to_string(smz->counter) + ": ERROR expecting keyword INPUTS\n";
		errorMessage = errorMessage + smz->geterror() + "\n";
		return false;
	}
	int numberInputs = num;
	if (!readname()) {
		return false;
	}
	bool noerror;
	dmz->makedevice(gatetype, id, numberInputs, noerror, "");
	return noerror;
}


/***********************************************************************
 * 
 * Receives and checks the initialisation of a clock device		
 * from the scanner. Called from device() 							
 * It returns FALSE if there is an error in the initialisation 	
 * 	
 */
bool parser::clock(void) {
	int numOfcycles = -1;
	cout << "Creating a Clock";
	smz->getsymbol(cursym, id, num, signalstr);
	if (cursym == keysym && id == 38)
	{
		cout << " that changes state every ";
		smz->getsymbol(cursym, id, num, signalstr);
		if (cursym == numsym)
		{
			if (num > 0)
			{
				cout << num << " cyles ";
				numOfcycles = num;
			}
			else {
				cout << endl << "SYNTATIC ERROR: number of cycles has to be integer greater than 0";
				errorMessage = errorMessage + "Line " + to_string(smz->counter) +
					": ERROR number of cycles has to be integer greater than 0\n";
				errorMessage = errorMessage + smz->geterror() + "\n";
				return false;
			}
		}
		else  {
			cout << endl << "SYNTATIC ERROR: expected number of cycles";
			errorMessage = errorMessage + "Line " + to_string(smz->counter) +
				": ERROR expected number of cycles\n";
			errorMessage = errorMessage + smz->geterror() + "\n";
			return false;
		}
	}
	else  {
		cout << endl << "SYNTATIC ERROR: expecting keyword 'CYCLES'" << endl;
		errorMessage = errorMessage + "Line " + to_string(smz->counter) +
			": ERROR expecting keyword 'CYCLES'\n";
		errorMessage = errorMessage + smz->geterror() + "\n";
		return false;
	}
	
	if (!readname()) {
		return false;
	}
	bool noerror;
	dmz->makedevice(aclock, id, numOfcycles, noerror, "");
	return noerror;
}


/***********************************************************************
 * 
 * Receives and checks the initialisation of a switch		
 * from the scanner. Called from device() 							
 * It returns FALSE if there is an error in the initialisation 	
 * 	
 */
bool parser::switch_(void) {
	int switchvalue = -1;
	cout << "Creating a SWITCH";
	smz->getsymbol(cursym, id, num, signalstr);
	if (cursym == keysym && id == 19)
	{
		cout << " starting with value ";
		smz->getsymbol(cursym, id, num, signalstr);
		if (cursym == numsym)
		{
			if (num == 0 || num == 1) {
				switchvalue = num;
				cout << num << " ";
			}
			else{
				cout << endl << "SYNTATIC ERROR: initial value of switch can be eiher 0 or 1 " << endl;
				errorMessage = errorMessage + "Line " + to_string(smz->counter) +
					": ERROR initial value of switch can be eiher 0 or 1\n";
				errorMessage = errorMessage + smz->geterror() + "\n";
				return false;
			}
		}
		else  {
			cout << endl << "SYNTATIC ERROR: expected initial value of switch, 0/1 " << endl;
			errorMessage = errorMessage + "Line " + to_string(smz->counter) +
				": ERROR expected initial value of switch, 0/1 \n";
			errorMessage = errorMessage + smz->geterror() + "\n";
			return false;
		}
	}
	else {
		cout << endl << "SYNTATIC ERROR: expecting keyword 'VALUE'" << endl;
		errorMessage = errorMessage + "Line " + to_string(smz->counter) +
			": ERROR expecting keyword 'VALUE' \n";
		errorMessage = errorMessage + smz->geterror() + "\n";
		return false;
	}
	if (!readname()) {
		return false;
	}
	bool noerror;
	dmz->makedevice(aswitch, id, switchvalue, noerror, "");
	return noerror;
}

/***********************************************************************
 * 
 * Receives and checks the initialisation of a dtype device			
 * from the scanner. Called from device() 							
 * It returns FALSE if there is an error in the initialisation 	
 * 	
 */
bool parser::dtype_(void) {
	cout << "Creating a DTYPE ";

	if (!readname()) {
		return false;
	}

	bool noerror;
	dmz->makedevice(dtype, id, -1, noerror, "");
	return noerror;
}


/***********************************************************************
 * 
 * Receives and checks the initialisation of a siggen device			
 * from the scanner. Called from device() 							
 * It returns FALSE if there is an error in the initialisation 		
 * 
 */
bool parser::siggen_(void) {
	string InputSeries = "";
	cout<<"Creating a SIGGEN";
	smz->getsymbol(cursym, id, num, signalstr);
	if (cursym == bitsersym)
	{
		if (signalstr.length() > 0)
		{
			cout<<" with signal "<<signalstr<<" ";
			InputSeries = signalstr;
		}
		else {
			cout << endl << "SYNTATIC ERROR: binary signal has to be at least one cycles long";
			errorMessage = errorMessage + "Line " + to_string(smz->counter) + 
				": ERROR binary signal has to be at least one cycles long \n";
			errorMessage = errorMessage + smz->geterror() + "\n";
			return false;
		}
	}
	else {
		cout << endl << "SYNTATIC ERROR: expected '#'";
		errorMessage = errorMessage + "Line " + to_string(smz->counter) + 
		    ": ERROR expected '#' \n";
		errorMessage = errorMessage + smz->geterror() + "\n";
		return false;
	}
	if (!readname()) {
		return false;
		}
		
	bool noerror;
	dmz->makedevice(asiggen, id, -1, noerror, InputSeries);
	return noerror;
}


/***********************************************************************
 * 
 * Receives and checks the initialisation of a monitor point  
 * from the scanner. Called from readin() 										
 * It returns FALSE if there is an error in the initialisation 	
 */
bool parser::monitor_(void) {
	cout << "Creating a MONITOR";
	smz->getsymbol(cursym, id, num, signalstr);
	name devicename;
	name output = -1;
	if (cursym == keysym && id == 41)
	{
		cout << " that records the output of ";
		smz->getsymbol(cursym, id, num, signalstr);
		if (cursym == namesym)
		{
			cout << id << " ";
			devicename = id;
			
			devlink dd;
			dd=netz->finddevice(id);
			if (dd != NULL && dd->kind == dtype)
			{
				smz->getsymbol(cursym, id, num, signalstr);
				if (cursym == dot)
				{
					cout << "output ";
					smz->getsymbol(cursym, id, num, signalstr);
					if (cursym != outsym) {
						cout << "SYNTATIC ERROR: expected output" << endl;
						errorMessage = errorMessage + "Line " + to_string(smz->counter) +
							": ERROR expected an output\n";
						errorMessage = errorMessage + smz->geterror() + "\n";
						return false;
					}
					else {
						cout << id << " ";
						output = id;
					}
				}
				else 
				{
					cout << "SYNTATIC ERROR: expected a dot '.'" << endl;
						errorMessage = errorMessage + "Line " + to_string(smz->counter) +
							": ERROR expected a dot\n";
						errorMessage = errorMessage + smz->geterror() + "\n";
						return false;
				}
			}
		}
		else {
			cout << endl << "SYNTATIC ERROR: expecting a name" << endl;
			errorMessage = errorMessage + "Line " + to_string(smz->counter) +
				": ERROR expecting a name \n";
			errorMessage = errorMessage + smz->geterror() + "\n";
			return false;
		}
	}
	else {
		cout << endl << "SYNTATIC ERROR: expecting keyword 'RECORDS'" << endl;
		errorMessage = errorMessage + "Line " + to_string(smz->counter) +
			": ERROR expecting keyword 'RECORDS' \n";
		errorMessage = errorMessage + smz->geterror() + "\n";
		return false;
	}
	
	if (!readname()) {
		return false;
	}
	bool noerror;
	
	if (noerrors) {
		mmz->makemonitor(devicename, output, noerror);
		devlink dout = netz->finddevice(devicename);
		if (!noerror) {
			if (dout == NULL) {
				errorMessage = errorMessage + "Line " + to_string(smz->counter) + ": ERROR Output device " +
					nmz->get_str(devicename) + " does not exist\n";
				errorMessage = errorMessage + smz->geterror() + "\n";
			}
			else if (dout->kind == dtype) {
				errorMessage = errorMessage + "Line " + to_string(smz->counter) + ": ERROR The DTYPE " +
					nmz->get_str(devicename) + " must have a valid output (Q or QBAR)\n";
				errorMessage = errorMessage + smz->geterror() + "\n";
			}
		}
	}
	else {
		noerror = false;
	}
	
	
	return noerror;

}


/***********************************************************************
 *
 * The constructor, takes pointers to various other classes as parameters     
 *
 */
parser::parser(network* network_mod, devices* devices_mod,
	monitor* monitor_mod, scanner* scanner_mod, names* names_mod)
{
	netz = network_mod; 
	dmz = devices_mod;   
	mmz = monitor_mod;   
	smz = scanner_mod;   
	nmz = names_mod;	

}

