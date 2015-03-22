

#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <algorithm>
#include <ctime>

#include "OntyxKb.h"
#include "OntyxProperty.h"
#include "tString.h"
#include "fullSyn.h"

using namespace std;

typedef struct comFlags {
	string	file1, file2;
	bool	changeSet;
	};

void ProcessCommandline (const vector<string> & commLine, comFlags & );
void reportSimpleMatch (OntyxKb & fromKb, OntyxKb & toKb, set<long> & unmatchedIDs);
void reportAlphaStemMatch (OntyxKb & fromKb, OntyxKb & toKb, set<long> & unmatchedIDs);


	OntyxKb tdeKb, 		// from TDE for comparison
			anatomyKb,		// tde subset
			masterKb,
			flatKb;			// derived from flat list

int main (int argc, char * argv[])
{
	clock_t tstart, tend;

	vector<string> commLine;
	for ( int i = 0; i < argc; i++)
		commLine.push_back(*argv++);

	comFlags flags;
	flags.file1 = "";
	flags.file2 = "";
	if( commLine.size() == 3 || commLine.size() == 4 )
		ProcessCommandline( commLine, flags);

	else {
		cerr << "Error:  incorrect number of command line parameters." << endl
			<< "\nUsage:\n\tcompareFlat tde-xml_file flat_list-txt_file [-s]" << endl << endl
			<< "where \"-s\" indicates simple matching.  Omitting -s results in" << endl
			<< "normalized matching." << endl << endl 
			<< "The input tde-xml_file is a processed baseline from tde" << endl
			<< "containing the filled-in Synonym property.  The input file can" << endl
			<< "be generated with the restoreFullSyn.exe the program." << endl;
		exit(0);
		}

	tstart = clock();
	tdeKb.readOntyxXMLFile( flags.file1 );	cerr << "Read \"" << flags.file1 << "\"" << endl;
	flatKb.readFlatFile( flags.file2, "FLAT-NAMESPACE", "Flat_Kind", "Flat_Type", "Flat Parent", "FLATSOURCE" );
		cerr << "Read \"" << flags.file2 << "\"" << endl;

	cout << "Files read -----------------------------------------------------------------" << endl;

	set<long> unmatchedID;


	if( flags.changeSet ) // -s:imple has been specified
		reportSimpleMatch (flatKb, tdeKb, unmatchedID);
	else
		reportAlphaStemMatch (flatKb, tdeKb, unmatchedID);

 
	tend = clock();
	cerr << "Execution took " << (tend-tstart)/1000.0 << " seconds" << endl;

	return 0;
}


void ProcessCommandline (const vector<string> & commLine, struct comFlags & flags)
{
	flags.changeSet = false;
	for( unsigned i = 1; i < commLine.size(); i++) {
		if( commLine[i] == "-s" || commLine[i] == "-S" )
			flags.changeSet = true;
		else if( flags.file1 == "" )
			flags.file1 = commLine[i];
		else
			flags.file2 = commLine[i];
		}
}


void reportSimpleMatch (OntyxKb & fromKb, OntyxKb & toKb, set<long> & unmatchedIDs)
// does a merge if all terms match, or if name matches, returns set of unmatched fromKb IDs
{
	long matchResult;
	set<long> s_myID;
	set<long>::const_iterator pSmyID;
	map<long, OntyxConcept>::iterator posCon;
	long matchConcept = 0, totalConcept = 0;
	unmatchedIDs.clear();
	for( posCon = fromKb.getFirstConceptIterator(); 
				posCon != fromKb.getEndConceptIterator(); ++posCon) {
		totalConcept++;
		matchResult = fromKb.simpleMatchKB (toKb, posCon->second.getName(), s_myID);
		if( matchResult == 0 ) {  									// unambiguous match, rename
			matchConcept++;
			cout << posCon->second.getName() << "\tsingle-match\t" 
				<< toKb.getConcept(*(s_myID.begin())).getCode() << '\t'
				<< toKb.getConcept(*(s_myID.begin())).getName() << '\t'
				<< toKb.getConcept(*(s_myID.begin())).getKind()
				<< endl;
			}
		else if( matchResult != 5 ) {	// merge, split, & unknown if name-match
			matchConcept++;
			cout << posCon->second.getName() << "\tmulti-match" ;
			pSmyID = s_myID.begin();
			for( ; pSmyID != s_myID.end(); ++pSmyID ) {
				cout << '\t' 
					<< toKb.getConcept(*pSmyID).getCode() << '\t'
					<< toKb.getConcept(*pSmyID).getName() << '\t'
					<< toKb.getConcept(*pSmyID).getKind();
				}
			cout  << endl;
			}
		else {																// find parent and tree
			cout << posCon->second.getName() << "\tno-match" << endl;
			}
		}
}

void reportAlphaStemMatch (OntyxKb & fromKb, OntyxKb & toKb, set<long> & unmatchedIDs)
// does a merge if all terms match, or if name matches, returns set of unmatched fromKb IDs
{
	long matchResult;
	set<long> s_myID;
	set<long>::const_iterator pSmyID;
	map<long, OntyxConcept>::iterator posCon;
	long matchConcept = 0, totalConcept = 0;
	unmatchedIDs.clear();
	for( posCon = fromKb.getFirstConceptIterator(); 
				posCon != fromKb.getEndConceptIterator(); ++posCon) {
		totalConcept++;
		matchResult = fromKb.alphaStemMatchKB (toKb, posCon->second.getName(), s_myID);
		if( matchResult == 0 ) {  									// unambiguous match, rename
			matchConcept++;
			cout << posCon->second.getName() << "\tsingle-match\t" 
				<< toKb.getConcept(*(s_myID.begin())).getCode() << '\t' 
				<< toKb.getConcept(*(s_myID.begin())).getName() << '\t'
				<< toKb.getConcept(*(s_myID.begin())).getKind() << endl;
			}
		else if( matchResult != 5 ) {								// merge, split, & unknown
			matchConcept++;
			cout << posCon->second.getName() << "\tmulti-match" ;
			pSmyID = s_myID.begin();
			for( ; pSmyID != s_myID.end(); ++pSmyID ) {
				cout << '\t' 
					<< toKb.getConcept(*pSmyID).getCode() << '\t'
					<< toKb.getConcept(*pSmyID).getName() << '\t'
					<< toKb.getConcept(*pSmyID).getKind();
				}
			cout  << endl;
			}
		else {																// find parent and tree
			cout << posCon->second.getName() << "\tno-match" << endl;
			}
		}
}




