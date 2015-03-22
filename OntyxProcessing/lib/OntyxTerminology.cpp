//*******************************************************************
// Implementation of Ontyx class OntyxTerminology
// 		Defines Terminology constants of the Ontylog KB terminology
//
//		Initial class definition 12/17/2001
//
// Dependencies		<iostream>
// Dependencies		STL <string> class
// Dependencies		C <stdlib>
//
// Virtual:			~OntyxTerminology()
//
//*******************************************************************

#include "OntyxTerminology.h"
#include <cstdlib>
#include <iostream>
#include <string>
using namespace std;


ref_by OntyxTerminology::reference = name;

	// order of strings in array follows declaration of enumeration types
const string OntyxTerminology::refStrings[] = {"name","id","code"};

const string OntyxTerminology::emptyString = "";

const long OntyxTerminology::CODE_EXISTS =			0x00000001;
const long OntyxTerminology::ID_EXISTS =			0x00000010;
const long OntyxTerminology::NAME_EXISTS =			0x00000100;
const long OntyxTerminology::NO_NAMESPACE =			0x00001000;
const long OntyxTerminology::NO_KIND = 				0x00010000;
const long OntyxTerminology::NO_PROPDEF = 			0x00100000;
const long OntyxTerminology::NO_ROLEDEF = 			0x01000000;
const long OntyxTerminology::NO_CONCEPTDEF = 		0x10000000;

const long OntyxTerminology::errorArray[] = {
							CODE_EXISTS,
							ID_EXISTS,
							NAME_EXISTS,
							NO_NAMESPACE,
							NO_KIND,
							NO_PROPDEF,
							NO_ROLEDEF,
							NO_CONCEPTDEF
							};
const long OntyxTerminology::numErrors = sizeof errorArray / sizeof (long);

const string OntyxTerminology::errorNames[] = {
							"CODE_EXISTS",
							"ID_EXISTS",
							"NAME_EXISTS",
							"NO_NAMESPACE",
							"NO_KIND",
							"NO_PROPDEF",
							"NO_ROLEDEF",
							"NO_CONCEPTDEF"
							};


void OntyxTerminology::setReference(string inRef)
{
	bool found = false;
	for(unsigned index = 0; index < (sizeof refStrings / sizeof (string)); index++) {
		if( inRef == refStrings[index] ) {
			reference = static_cast<ref_by> (index);
			found = true;
			}
		}
	if( !found ) {
		cerr << "Terminology ref_by string not found.  Will EXIT." << endl;
		exit(0);
		}
}
	
const string & OntyxTerminology::getReference(void)
	{ return refStrings[ static_cast<int> (reference) ]; }

const ref_by OntyxTerminology::getReference(char c)		// takes any char for distinct signature
	{ return reference; }

OntyxTerminology::OntyxTerminology(void) {}
OntyxTerminology::~OntyxTerminology() {}


