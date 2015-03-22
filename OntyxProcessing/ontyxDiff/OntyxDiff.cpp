

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <ctime>

#include "OntyxKb.h"

using namespace std;

typedef struct comFlags {
	string	file1, file2;
	bool	changeSet;
	};

void ProcessCommandline (const vector<string> & commLine, comFlags & );

int main (int argc, char * argv[])
{
	clock_t tstart, tend;

	vector<string> commLine;
	for ( int i = 0; i < argc; i++)
		commLine.push_back(*argv++);

	comFlags flags;
	flags.file1 = "";
	flags.file2 = "";
	if( commLine.size() != 4 ) {
		cerr << "Error:  incorrect number of command line parameters." << endl
			<< "\nUsage:\n\tontyxdiff file1 file2 -c|u" << endl << endl
			<< "\twhere 'c' indicates changeset and 'u' indicates unique" << endl
			<< "\tconcepts in file1 and file2." << endl;
		exit(0);
		}
	else
		ProcessCommandline( commLine, flags);


	OntyxKb masterKb, compareKb, resultKb;

	tstart = clock();

	masterKb.readOntyxXMLFile( flags.file1 );	cerr << "Read \"" << flags.file1 << "\"" << endl;
	compareKb.readOntyxXMLFile( flags.file2 );	cerr << "Read \"" << flags.file2 << "\"" << endl;
	if( flags.changeSet ) {
		masterKb.diff_Concepts(compareKb, resultKb);
		resultKb.printKbDiffConcepts();
		}
	else {
		masterKb.unique_Concepts(compareKb, resultKb);
		resultKb.printKbConcepts();
		}

	tend = clock();
	cerr << "Execution took " << (tend-tstart)/1000.0 << " seconds" << endl;


	return 0;
}


void ProcessCommandline (const vector<string> & commLine, struct comFlags & flags)
{
	int numFlag = 0;
	for( unsigned i = 1; i < commLine.size(); i++) {
		if( commLine[i][0] == '-' ) {
			if( commLine[i][1] == 'u' ) {
				flags.changeSet = false;
				numFlag++;
				}
			else if( commLine[i][1] == 'c' ) {
				flags.changeSet = true;
				numFlag++;
				}
			}
		else {
			if( flags.file1 == "" )
				flags.file1 = commLine[i];
			else
				flags.file2 = commLine[i];
			}
		}

	if( numFlag != 1) {
		cerr << "Error:  wrong number of flags in command line." << endl
			<< "\nUsage:\n\tontyxcmp file1 file2 -c|u" << endl << endl
			<< "\twhere 'c' indicates changeset and 'u' indicates unique" << endl
			<< "\tconcepts in file1 and file2." << endl;
		exit(0);
		}
}




