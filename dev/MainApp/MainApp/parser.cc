#include <iostream>
#include "parser.h"

using namespace std;

/* The parser for the circuit definition files */

symbol cursym;
name id;
int num;

bool parser::readin(void)
{
	symbol tempsym;
	smz->getsymbol(cursym, id, num);
	while (cursym != eofsym) {
		tempsym = cursym;
		smz->getsymbol(cursym, id, num);
		//cout << id << endl;
		if (cursym != equals) {
			cout << "ERROR expecting the equal sign" << endl;
			return false;
		}
		switch (tempsym)
		{
		case devsym:
			device();
			break;
		case monsym:
			//monitor_();
			break;
		case cirsym:
			//circuit();
			break;
		case consym:
			connection();
			break;
		default:
			break;
		}
		if (tempsym != consym) {

			smz->getsymbol(cursym, id, num);
			if (cursym != keysym) { //here we shold check that the word is actually 'NAME'
				cout << "ERROR: expecting the keyword 'NAME'" << endl;
				return false;
			}
			//cout << "["<< id << "] ";
			cout << "assigned to a variable ";
			smz->getsymbol(cursym, id, num);
			if (cursym != namesym) {
				cout << "ERROR: expecting name" << cursym << endl;
				return false;
			}
			cout << "[" << id << "] ";
		}
		smz->getsymbol(cursym, id, num);
		if (cursym != semicol) {
			cout << "ERROR missing semicolon" << id << endl;
			return false;
		}
		smz->getsymbol(cursym, id, num);
		cout << endl;
	}
	return true;

}

void parser::connection(void) {
	smz->getsymbol(cursym, id, num);
	if (cursym != namesym) {
		cout << "ERROR in the name" << endl;
		return;
	}
	cout << "There is a connection from " << id << " ";
	smz->getsymbol(cursym, id, num);

	if (cursym == dot) { //here we should check if we expect a dot
		cout << "output ";
		smz->getsymbol(cursym, id, num);
		if (cursym != outsym) {
			cout << "ERROR in the output" << endl;
		}
		cout << id << " ";
		smz->getsymbol(cursym, id, num);
	}
	if (cursym != connect) {
		cout << "ERROR missing >" << endl;
	}
	cout << "To input ";
	smz->getsymbol(cursym, id, num);
	if (cursym != namesym) {
		cout << "ERROR in the name " << cursym << endl;
	}
	cout << id << " ";

	smz->getsymbol(cursym, id, num);
	if (cursym != dot) {
		cout << "ERROR in the dot" << endl;
	}
	smz->getsymbol(cursym, id, num);
	if (cursym != insym) {
		cout << "ERROR in the input" << endl;
	}

	cout << id;

}

void parser::device(void) {
	smz->getsymbol(cursym, id, num);
	if (cursym == xorsym) {
		xor_();
	}
	else if (cursym == gatesym) {
		gate();
	}
	else {
		cout << "ERROR expecting a logic gate. Recieved " << cursym << endl;
	}
}

void parser::xor_(void) {
	cout << "Creating a XOR gate ";
}
void parser::gate(void) {

	switch (id)
	{
	case 2:
		cout << "Creating an AND gate ";
		break;
	case 3:
		cout << "Creating a NAND gate ";
		break;
	case 4:
		cout << "Creating a OR gate ";
		break;
	case 5:
		cout << "Creating a NOR gate ";
		break;
	default:
		cout << "ERROR expecting a AND/NAND/OR/NOR gate" << endl;
		break;
	}
	smz->getsymbol(cursym, id, num);
	if (cursym == keysym || id == 6) {
		//cout << id << " ";
		cout << "That has ";
		smz->getsymbol(cursym, id, num);
		if (num == -1) {
			cout << "ERROR expecting the number of inputs" << endl;
		}
		cout << num << " inputs ";
	}
	else {
		cout << "ERROR expecting keyword INPUTS" << endl;
	}

}
void parser::clock(void) {

	cout << "Creating a Clock";
	smz->getsymbol(cursym, id, num);
	if (cursym == keysym) //here we should check that the keywok is cycles
	{
		cout << " that changes state every ";
		smz->getsymbol(cursym, id, num);
		if (cursym == numsym)
		{
			if (num > 0)
			{
				cout << num << " cyles";
				smz->getsymbol(cursym, id, num);
				if (cursym == keysym) //here we should check that the keywod is start
				{
					cout << " starting with value ";
					smz->getsymbol(cursym, id, num);
					if (cursym == numsym)
					{
						if (num == 0 || num == 1)
							cout << num << " ";
						else cout << endl << "ERROR: initial value of clock can be eiher 0 or 1 " << endl;

					}
					else  cout << endl << "ERROR: expected initial value of clock, 0/1 " << endl;
				}
				else  cout << endl << "ERROR: expecting keyword 'START'" << endl;
			}
			else  cout << endl << "ERROR: number of cycles has to be integer greater than 0";
		}
		else  cout << endl << "ERROR: expected number of cycles";
	}
	else  cout << endl << "ERROR: expecting keyword 'CYCLES'" << endl;
}



void parser::switch_(void)
{
	cout << "Creating a SWITCH";
	smz->getsymbol(cursym, id, num);
	if (cursym == keysym) //here we should check the keyword is VALUE
	{
		cout << " starting with value ";
		smz->getsymbol(cursym, id, num);
		if (cursym == numsym)
		{
			if (num == 0 || num == 1)
				cout << num << " ";
			else cout << endl << "ERROR: initial value of switch can be eiher 0 or 1 " << endl;
		}
		else  cout << endl << "ERROR: expected initial value of switch, 0/1 " << endl;
	}
	else  cout << endl << "ERROR: expecting keyword 'VALUE'" << endl;
}

void parser::dtype(void) {
	cout << "Creating a DTYPE";
	smz->getsymbol(cursym, id, num);
	if (cursym == keysym) //here we should check the keyword is QVAL
	{
		cout << " starting with Q value ";
		smz->getsymbol(cursym, id, num);
		if (cursym == numsym)
		{
			if (num == 0 || num == 1)
				cout << num << " ";
			else cout << endl << "ERROR: initial value of Q can be eiher 0 or 1 " << endl;
		}
		else  cout << endl << "ERROR: expected initial value of Q, 0/1 " << endl;
	}
	else  cout << endl << "ERROR: expecting keyword 'QVAL'" << endl;
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
