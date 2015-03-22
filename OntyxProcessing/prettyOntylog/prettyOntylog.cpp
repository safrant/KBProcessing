 

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
#include "tString.h"
#include "fullSyn.h"

using namespace std;


int main (int argc, char * argv[])
{
	OntyxKb tdeKb, compareKb;		// input/output kb
	clock_t tstart, tend;

	vector<string> commLine;
	for ( int i = 0; i < argc; i++)
		commLine.push_back(*argv++);

	if( commLine.size() != 2 ) {
		cerr << "Error:  incorrect number of command line parameters." << endl
			<< "\nUsage:\n\tprettyOntylog tde_input_file" << endl
			<< "\nOutput filename prefixed with \"p-\"." << endl;
		exit(0);
		}

	tstart = clock();

	tdeKb.readOntyxXMLFile( commLine[1].c_str() );	cerr << "Read \"" << commLine[1].c_str() << "\"" << endl;
	cerr << "TDE file read" << endl;

	string filename = "p-" + commLine[1];
	cerr << "Writing output ontylog file '" << filename << "', properties with role names are not exported." << endl;
	tdeKb.writeOntyxXMLFile(filename, false, true);
	// void OntyxKb::writeOntyxXMLFile (const string & filename, bool flag, bool noProps) // flag = true:renumber
	// if noProps is true, properties with names derived from roles are not exported
	// ************* END WRITE ONTYLOG FILE *************
	

	tend = clock();
	cerr << "Execution took " << (tend-tstart)/1000.0 << " seconds" << endl;

	return 0;
}



