
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

using namespace std;

typedef struct comFlags {
	string	file1, file2;
	bool	changeSet;
	};

void ProcessCommandline (const vector<string> & commLine, comFlags & );
void mergeInKb( OntyxKb &, OntyxConcept & );
void createInKb( OntyxKb &, OntyxConcept & );

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
		; //ProcessCommandline( commLine, flags);

	else {
		cerr << "Error:  incorrect number of command line parameters." << endl
			<< "\nUsage:\n\tmergeHawaii tde_file hawaii_file" << endl << endl << endl;
		exit(0);
		}

	tstart = clock();

	OntyxKb masterKb, 		// from TDE for comparison
			hawaiiKb;		// derived from flat list

	masterKb.readOntyxXMLFile( commLine[1] );	cerr << "Read \"" << commLine[1] << "\"" << endl;
	hawaiiKb.readOntyxXMLFile( commLine[2] );	cerr << "Read \"" << commLine[2] << "\"" << endl;

	cout << "Files read -----------------------------------------------------------------" << endl;

	set<long> unmatchedID;
	map<long, OntyxConcept>::iterator posCon;
	multimap<long, OntyxRole> roles;
	multimap<long, OntyxRole>::iterator posRole;

	long propID;
	char c_maxPropCode[22];
	string s_maxPropCode;


	// copy properties to master Kb
	map<long, OntyxPropertyDef>	hawaiiProps = hawaiiKb.getPropDefs();
	map<long, OntyxPropertyDef>::iterator pHawaii = hawaiiProps.begin();
	propID = masterKb.getMaxPropID();
	string name;
	for( ; pHawaii != hawaiiProps.end(); ++pHawaii ) {
		name =  pHawaii->second.getName();
		if( !masterKb.hasPropertyDef(name) ) {
			propID++;
			sprintf(c_maxPropCode, "%ld", propID);
			s_maxPropCode = c_maxPropCode;
			OntyxPropertyDef tmpPropDef(pHawaii->second.getName(), "P" + s_maxPropCode, propID, pHawaii->second.getRange(), false, false, "NCI");
			masterKb.addPropertyDef(tmpPropDef);
			}
		}

	// change kind and namespace
	for( posCon = hawaiiKb.getFirstConceptIterator(); 
				posCon != hawaiiKb.getEndConceptIterator(); ++posCon) {
		posCon->second.setNamespace ("NCI").setKind ("Chemicals_and_Drugs_Kind");

		}


	string newname;
	for( posCon = hawaiiKb.getFirstConceptIterator(); 
				posCon != hawaiiKb.getEndConceptIterator(); ++posCon) {
		if( masterKb.conceptNameInKB(posCon->second.getName()) ) {  // the merge or creation is based on names, no comparison testing is done.
			mergeInKb( masterKb, posCon->second);
			}
		else {
			createInKb( masterKb, posCon->second);
			}
		}



 
	masterKb.writeOntyxXMLFile("hawaii_thesaurus.xml", false, true);

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

void mergeInKb( OntyxKb & KB, OntyxConcept & concept) 
{ 
	if( concept.getNumRoles() > 0 ) {
		vector<OntyxRole> lroles;
		multimap<long, OntyxRole> roles = concept.getRoles();
		multimap<long, OntyxRole>::iterator pRoles = roles.begin();
		for( ; pRoles != roles.end(); ++pRoles )
			lroles.push_back(pRoles->second);
		KB.getConcept(concept.getName()).addRoles(lroles);
		}
	if( concept.getNumProperties() > 0 ) {
		vector<OntyxProperty> lproperties;
		multimap<long, OntyxProperty> properties = concept.getProperties();
		multimap<long, OntyxProperty>::iterator pProps = properties.begin();
		for( ; pProps != properties.end(); ++pProps )
			lproperties.push_back(pProps->second);
		KB.getConcept(concept.getName()).addProperties(lproperties);
		}
	if( concept.getNumAssociations() > 0 )
		cerr << "Oops!  Concept has associations, not being dealt with." << endl;
	

}

void createInKb( OntyxKb & KB, OntyxConcept & concept) 
{ 
	KB.addConcept(concept);

}




