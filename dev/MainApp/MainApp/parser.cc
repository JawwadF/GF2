#include <iostream>
#include "parser.h"

using namespace std;

void skip(scanner* smz);
bool getname(scanner* smz);
/* The parser for the circuit definition files */
devlink dev;
symbol cursym;
name id;
bool noerrors = true;
int num;

bool parser::readin(void)
{
	symbol tempsym;
	bool noerror;
	smz->getsymbol(cursym, id, num);
	while (cursym != eofsym) {
		noerror = true;
		tempsym = cursym;
		smz->getsymbol(cursym, id, num);
		//cout << id << endl;
		if (cursym != equals) {
			cout << "SYNTATIC ERROR expecting the equal sign" << endl;
			noerror =  false;
		}
		switch (tempsym)
		{
		case devsym:
			noerror = noerror && device();
			break;
		case monsym:
			noerror = noerror && monitor_();
			break;
		case cirsym:
			noerror = noerror && circuit();
			break;
		case consym:
			noerror = noerror && connection();
			break;
		default:
			noerror = false;
			cout << "SYNTATIC ERROR expected keyword DEVICE/CONNECTION/MONITOR or CIRCUIT" << endl;
			break;
		}

		if (!noerror) {
			skip(smz);
			continue;
		}

		smz->getsymbol(cursym, id, num);
		if (cursym != semicol) {
			cout << "SYNTATIC ERROR missing semicolon, got " << id << endl;
			noerror = false;
			skip(smz);
			continue;
		}
		smz->getsymbol(cursym, id, num);
	}
	return noerrors;

}

void skip(scanner* smz) {
	while (cursym != eofsym && cursym != semicol) {
		smz->getsymbol(cursym, id, num);
	}
	smz->getsymbol(cursym, id, num);
	noerrors = false;
}

bool parser::connection(void) {
	smz->getsymbol(cursym, id, num);
	name indev = -1, outdev = -1, insig = -1, outsig = -1;
	if (cursym != namesym) {
		cout << "SYNTATIC ERROR in the name" << endl;
		return false;
	}
	cout << "There is a connection from " << id << " ";
	outdev = id;
	smz->getsymbol(cursym, id, num);
	
	if (cursym == dot) { //here we should check if we expect a dot
		cout << "output ";
		smz->getsymbol(cursym, id, num);
		if (cursym != outsym) {
			cout << "SYNTATIC ERROR in the output" << endl;
			return false;
		}
		cout << id << " ";
		outsig = id;
		smz->getsymbol(cursym, id, num);
	}
	else {
		outsig = -1;
		netz->addoutput(netz->finddevice(outdev), outsig);
	}
	if (cursym != connect) {
		cout << "SYNTATIC ERROR missing >" << endl;
		return false;
	}
	cout << "To input ";
	smz->getsymbol(cursym, id, num);
	if (cursym != namesym) {
		cout << "SYNTATIC ERROR in the name " << cursym << endl;
		return false;
	}
	cout << id << " ";
	indev = id;
	smz->getsymbol(cursym, id, num);
	if (cursym != dot) {
		cout << "SYNTATIC ERROR in the dot" << endl;
		return false;
	}
	smz->getsymbol(cursym, id, num);
	if (cursym != insym) {
		cout << "SYNTATIC ERROR in the input" << endl;
		return false;
	}
	cout << id <<endl;
	insig = id;
	bool noerror;
	netz->makeconnection(indev, insig, outdev, outsig, noerror);
	return noerror;

}
bool getname(scanner* smz) {

	if (id != 37) smz->getsymbol(cursym, id, num);
	if (cursym != keysym || id != 37) {
		cout << "SYNTATIC ERROR: expecting the keyword 'NAME' but getting" <<id<< endl;
		return false;
	}
	//cout << "["<< id << "] ";
	
	smz->getsymbol(cursym, id, num);
	if (cursym != namesym) {
		cout << "SYNTATIC ERROR: expecting name" << cursym << endl;
		return false;
	}
	cout << "assigned to a variable " << id << endl;

	return true;
}

bool parser::device(void) {
	smz->getsymbol(cursym, id, num);
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
	default:
		cout << "SYNTATIC ERROR expecting a device. Received " << cursym << endl;
		return false;
	}

	return false;
}
bool parser::xor_(void) {
	if (!getname(smz)) {
		return false;
	}
	cout << "[" << id << "] ";
	cout << "Creating a XOR gate ";
	bool noerror;
	dmz->makedevice(xorgate, id, 2, noerror);
	return noerror;
}
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
		return false;
		break;
	}
	smz->getsymbol(cursym, id, num);
	if (cursym == keysym && id == 18) {
		//cout << id << " ";
		cout << "That has ";
		smz->getsymbol(cursym, id, num);
		if (cursym != numsym) {
			cout << endl << "SYNTATIC ERROR expecting the number of inputs" << endl;
			return false;
		}
		else
		{
			if (num > 1)
				cout << num << " inputs ";
			else {
				cout << endl << "SYNTATIC ERROR: number of inputs has to be integer greater than 1" << endl;
				return false;
			}
		}
	}
	else {
		cout << endl << "SYNTATIC ERROR expecting keyword INPUTS" << endl;
		return false;
	}
	int numberInputs = num;
	if (!getname(smz)) {
		return false;
	}
	bool noerror;
	dmz->makedevice(gatetype, id, numberInputs, noerror);
	return noerror;
}

bool parser::clock(void) {
	int numOfcycles = -1;
	cout << "Creating a Clock";
	smz->getsymbol(cursym, id, num);
	if (cursym == keysym && id == 38)
	{
		cout << " that changes state every ";
		smz->getsymbol(cursym, id, num);
		if (cursym == numsym)
		{
			if (num > 0)
			{
				cout << num << " cyles";
				numOfcycles = num;
				smz->getsymbol(cursym, id, num);
				if (cursym == keysym && id == 39)
				{
					cout << " starting with value ";
					smz->getsymbol(cursym, id, num);
					if (cursym == numsym)
					{
						if (num == 0 || num == 1)
							cout << num << " ";
						else {
							cout << endl << "SYNTATIC ERROR: initial value of clock can be eiher 0 or 1 " << endl;
							return false;
						}

					}
					else { 
						cout << endl << "SYNTATIC ERROR: expected initial value of clock, 0/1 " << endl;
						return false;
					}
				}
				else {
					cout << endl << "SYNTATIC ERROR: expecting keyword 'START'" << endl;
					return false;
				}
			}
			else {
				cout << endl << "SYNTATIC ERROR: number of cycles has to be integer greater than 0";
				return false;
			}
		}
		else  {
			cout << endl << "SYNTATIC ERROR: expected number of cycles";
			return false;
		}
	}
	else  {
		cout << endl << "SYNTATIC ERROR: expecting keyword 'CYCLES'" << endl;
		return false;
	}
	
	if (!getname(smz)) {
		return false;
	}
	bool noerror;
	dmz->makedevice(aclock, id, numOfcycles, noerror);
	return noerror;
}



bool parser::switch_(void)
{
	int switchvalue = -1;
	cout << "Creating a SWITCH";
	smz->getsymbol(cursym, id, num);
	if (cursym == keysym && id == 19)
	{
		cout << " starting with value ";
		smz->getsymbol(cursym, id, num);
		if (cursym == numsym)
		{
			if (num == 0 || num == 1) {
				switchvalue = num;
				cout << num << " ";
			}
			else{
				cout << endl << "SYNTATIC ERROR: initial value of switch can be eiher 0 or 1 " << endl;
				return false;
			}
		}
		else  {
			cout << endl << "SYNTATIC ERROR: expected initial value of switch, 0/1 " << endl;
			return false;
		}
	}
	else {
		cout << endl << "SYNTATIC ERROR: expecting keyword 'VALUE'" << endl;
		return false;
	}
	if (!getname(smz)) {
		return false;
	}
	bool noerror;
	dmz->makedevice(aswitch, id, switchvalue, noerror);
	return noerror;
}

bool parser::dtype_(void) {
	cout << "Creating a DTYPE";
	smz->getsymbol(cursym, id, num);
	if (cursym == keysym && id == 20)
	{
		cout << " starting with Q value ";
		smz->getsymbol(cursym, id, num);
		if (cursym == numsym)
		{
			if (num == 0 || num == 1)
				cout << num << " ";
			else {
				cout << endl << "SYNTATIC ERROR: initial value of Q can be eiher 0 or 1 " << endl;
				return false;
			}
		}
		else  {
			cout << endl << "SYNTATIC ERROR: expected initial value of Q, 0/1 " << endl;
			return false;
		}
	}
	else  {
		cout << endl << "SYNTATIC ERROR: expecting keyword 'QVAL'" << endl;
		return false;
	}

	if (!getname(smz)) {
		return false;
	}

	bool noerror;
	dmz->makedevice(dtype, id, -1, noerror);
	return noerror;
}


bool parser::circuit(void) {
	cout << "Creating a CIRCUIT";
	smz->getsymbol(cursym, id, num);
	if (cursym == keysym && id == 40)
	{
		cout << " that includes devices ";
		smz->getsymbol(cursym, id, num);
		while (cursym == namesym)
		{
			cout << id << ", ";
			smz->getsymbol(cursym, id, num);
		}
	}
	else {
		cout << endl << "SYNTATIC ERROR: expecting keyword 'INCLUDES'" << endl;
		return false;
	}
	return true;

}

bool parser::monitor_(void) {
	cout << "Creating a MONITOR";
	smz->getsymbol(cursym, id, num);
	name devicename;
	name output = -1;
	if (cursym == keysym && id == 41)
	{
		cout << " that records the output of ";
		smz->getsymbol(cursym, id, num);
		if (cursym == namesym)
		{
			cout << id << " ";
			devicename = id;
			smz->getsymbol(cursym, id, num);
			if (cursym == dot) //here we should check if we expect a dot
			{
				cout << "output ";
				smz->getsymbol(cursym, id, num);
				if (cursym != outsym) {
					cout << "SYNTATIC ERROR: expected output" << endl;
					return false;
				}
				else {
					cout << id << " ";
					output = id;
				}
				smz->getsymbol(cursym, id, num);
			}
		}
		else {
			cout << endl << "SYNTATIC ERROR: expecting a name" << endl;
			return false;
		}
	}
	else {
		cout << endl << "SYNTATIC ERROR: expecting keyword 'RECORDS'" << endl;
		return false;
	}
	if (!getname(smz)) {
		return false;
	}

	bool noerror;
	mmz->makemonitor(devicename, output, noerror);

	return noerror;

}



parser::parser(network* network_mod, devices* devices_mod,
	monitor* monitor_mod, scanner* scanner_mod)
{
	netz = network_mod;  /* make internal copies of these class pointers */
	dmz = devices_mod;   /* so we can call functions from these classes  */
	mmz = monitor_mod;   /* eg. to call makeconnection from the network  */
	smz = scanner_mod;   /* class you say:                               */
						 /* netz->makeconnection (i1, i2, o1, o2, ok);   */

	/* any other initialisation you want to do? */

}
