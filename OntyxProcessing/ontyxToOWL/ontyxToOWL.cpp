

#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <set>
#include <algorithm>
#include <ctime>

#include "OntyxKb.h"
#include "OntyxProperty.h"

using namespace std;

void examineClashingCodes ( OntyxKb & KB );		// outputs to stderr kinds, assoc, prop, qual that clash with concept codes
string getDayStamp (void);
string characterEscape (const string & inString);


int main (int argc, char * argv[])
{
	OntyxKb tdeKb, compareKb;		// input/output kb
	clock_t tstart, tend;

	vector<string> commLine;
	for ( int i = 0; i < argc; i++)
		commLine.push_back(*argv++);

	if( commLine.size() != 2 && commLine.size() != 3 ) {
		cerr << "Error:  incorrect number of command line parameters." << endl
			<< "\nUsage:\n\tontyxToOWL tde.xml [-C|N]" << endl << endl 
			<< "Requires ancillary header.owl, and headerCode.owl files."  << endl
			<< "Optional:  flag -C indicates output \"by code\", -N indicates output \"by name\", default is -N \"by name\"."  << endl;
		exit(0);
		}
	bool outputByCode = false;
	vector<string>::iterator pComm = commLine.begin();
	for( ; pComm != commLine.end(); ++pComm ) {
		if( *pComm == "-C" || *pComm == "-c" )
			outputByCode = true;
		}

	tstart = clock();

	tdeKb.readOntyxXMLFile( commLine[1] );	cerr << "Read \"" << commLine[1].c_str() << "\"" << endl;
	cerr << "TDE file read -----------------------------------------------------------------" << endl;


	string filename;
	string daystamp = getDayStamp();

	if( outputByCode ) { 		// WRITE BY CODE OWL FILE 
		examineClashingCodes(tdeKb);	// clashing codes output to stderr
		tdeKb.codeToXname();
		filename = commLine[1] + "-ByCode-" + daystamp + ".owl";
		cerr << "Writing output OWL file '" << filename << "', properties with role names are not exported." << endl;
		tdeKb.writeOWLFile (filename, "headerCode.owl", true); 
		} 
	else {						// WRITE BY NAME OWL FILE
		tdeKb.nameToXname();
		filename = commLine[1] + "-ByName-" + daystamp + ".owl";
		cerr << "Writing output OWL file '" << filename << "', properties with role names are not exported." << endl;
		tdeKb.writeOWLFileByName (filename, "header.owl", true); 
		}


	tend = clock();
	cerr << "Execution took " << (tend-tstart)/1000.0 << " seconds" << endl;

	return 0;
}


string getDayStamp (void)
{
	char sumFile[10] = {0};
	struct tm when;
	time_t now;   
	char *p;
	char tempS[20] = {0}, monS[3] = {0}, dayS[3] = {0}, yearS[5] = {0};

	time( &now );
	when = *localtime( &now );
	sprintf (yearS, "%d", when.tm_year);	// years since 1900
	if (strlen(yearS) > 2) {		// use only last two digits of year
		p = &yearS[strlen(yearS)-2];
		strcpy(tempS, p);
		strcpy(yearS, tempS);
		}
	sprintf (monS, "%d", when.tm_mon + 1);
	sprintf (dayS, "%d", when.tm_mday);
	if (strlen(monS) == 1) {		// use month with two digits
		strcpy(tempS, "0");
		strcat(tempS, monS);
		strcpy(monS, tempS);
		}
	if (strlen(dayS) == 1) {		// use days with two digits
		strcpy(tempS, "0");
		strcat(tempS, dayS);
		strcpy(dayS, tempS);
		}
	strcat(sumFile, yearS);
	strcat(sumFile, monS);
	strcat(sumFile, dayS);
	string stamp = sumFile;

	return stamp;
}


void examineClashingCodes ( OntyxKb & KB ) // outputs to stderr kinds, assoc, prop, qual that clash with concept codes
{
	vector<string> vKinds = KB.getKbKinds();
	vector<string> vRoles = KB.getKbRoles();
	vector<string> vAssocs = KB.getKbAssocs();
	vector<string> vQuals = KB.getKbQuals();
	vector<string> vProps = KB.getKbProps();

	vector<string>::iterator I;
	for( I = vKinds.begin(); I != vKinds.end(); ++I ) {
		OntyxKind tKind = KB.getKindDef(*I);
		if( KB.getIDFromCode(tKind.getCode()) != -1 )
			cerr << "Code from Kind " << tKind.getKindname() << " exists as concept code.  Need to edit manually."  << endl;
		}
	for( I = vRoles.begin(); I != vRoles.end(); ++I ) {
		OntyxRoleDef tRole = KB.getRoleDef(*I);
		if( KB.getIDFromCode(tRole.getCode()) != -1 )
			cerr << "Code from Role " << tRole.getName() << " exists as concept code.  Need to edit manually."  << endl;
		}
	for( I = vProps.begin(); I != vProps.end(); ++I ) {
		OntyxPropertyDef tProp = KB.getPropertyDef(*I);
		if( KB.getIDFromCode(tProp.getCode()) != -1 )
			cerr << "Code from Property " << tProp.getName() << " exists as concept code.  Need to edit manually."  << endl;
		}
	for( I = vAssocs.begin(); I != vAssocs.end(); ++I ) {
		OntyxAssociationDef tAssoc = KB.getAssociationDef(*I);
		if( KB.getIDFromCode(tAssoc.getCode()) != -1 )
			cerr << "Code from Association " << tAssoc.getName() << " exists as concept code.  Need to edit manually."  << endl;
		}
	for( I = vQuals.begin(); I != vQuals.end(); ++I ) {
		OntyxQualifierDef tQual = KB.getQualifierDef(*I);
		if( KB.getIDFromCode(tQual.getCode()) != -1 )
			cerr << "Code from Qualifier " << tQual.getName() << " exists as concept code.  Need to edit manually."  << endl;
		}

}

