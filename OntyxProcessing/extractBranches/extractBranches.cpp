

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <algorithm>
#include <ctime>

#include "OntyxKb.h"

using namespace std;

typedef struct comFlags {
	string	file1, file2;
	bool	changeSet;
	};

void ProcessCommandline (const vector<string> & commLine, comFlags & );
void readFlatList(const string & inFilename, set<string> & term_list);

	OntyxKb masterKb, extractKb, throwKb;

int main (int argc, char * argv[])
{
	clock_t tstart, tend;

	set<string> term_list;
	set<string> exc_list;
	vector<string> commLine;
	for ( int i = 0; i < argc; i++)
		commLine.push_back(*argv++);

	if( commLine.size() ==4 || commLine.size() == 5 )
		;  // things OK
	else {
		cerr << "Error:  incorrect number of command line parameters." << endl
			<< "\nUsage:\n\textractBranches in_tde_file out_tde_file extract_branch [exclude_branch]" << endl << endl
			<< "where extract_branch is a text file containing a list" << endl
			<< "of top nodes defining the branch(es) to extract, and"  << endl 
			<< "exclude_branch is an optional text file containing a list" << endl 
			<< "of top nodes of subranches *not* to extract." << endl << endl;
		exit(0);
		}


	tstart = clock();

	masterKb.readOntyxXMLFile( commLine[1].c_str() );	cerr << "Read \"" << commLine[1].c_str() << "\"" << endl;
	readFlatList(commLine[3].c_str(), term_list);	cerr << "Read \"" << commLine[3].c_str() << "\"" << endl;
	if( commLine.size() == 5 ) {
		readFlatList(commLine[4].c_str(), exc_list);
		cerr << "Read \"" << commLine[4].c_str() << "\"" << endl;
		}

	set<string>::iterator posSet;
	set<long> to_move;
	long l_unneedID;

	// throw out unneneded subbranches first
	for( posSet = exc_list.begin(); posSet != exc_list.end(); ++posSet ) {
		l_unneedID = masterKb.getIDFromName(*posSet);
		if( l_unneedID != -1 ) {
			cerr << "Eliminating branch " << *posSet << endl;
			to_move.clear();
			masterKb.findDescendants(l_unneedID, to_move);
			masterKb.moveSetConceptsTo(to_move, throwKb);
			}
		else
			cerr << *posSet << " not found in KB." << endl;
		}
	// extract the branch of interest
	for( posSet = term_list.begin(); posSet != term_list.end(); ++posSet ) {
		l_unneedID = masterKb.getIDFromName(*posSet);
		if( l_unneedID != -1 ) {
			cerr << "Extracting branch " << *posSet << endl;
			to_move.clear();
			masterKb.findDescendants(l_unneedID, to_move);
			masterKb.moveSetConceptsTo(to_move, extractKb);
			}
		else
			cerr << *posSet << " not found in KB." << endl;
		}

	masterKb.copyDefsTo(extractKb);
	extractKb.fixReferences();

	cerr << "Writing output file" << endl;
	extractKb.writeOntyxXMLFile(commLine[2].c_str(), false, false);

	tend = clock();
	cerr << "Execution took " << (tend-tstart)/1000.0 << " seconds" << endl;

	return 0;
}


void readFlatList(const string & inFilename, set<string> & term_list)
{
	ifstream inFile;									// VERIFY THAT FILES EXIST
	inFile.open( inFilename.c_str() );
	if( !inFile.good() ) {
		cerr << "Can't open file '" << inFilename.c_str() << "'" << endl;
		exit(0);
		}

	string inputLine;
	while( !inFile.eof() && inputLine.size() != 0 , getline(inFile, inputLine) )
		term_list.insert(inputLine);
	inFile.close();
}




