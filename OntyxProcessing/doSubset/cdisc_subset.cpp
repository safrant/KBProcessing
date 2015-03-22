#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <algorithm> 
#include <ctime>
#include <iterator>

#include "fullSyn.h"
#include "OntyxKb.h"

using namespace std;
int main (int argc, char * argv[])
{
	OntyxKb tdeKb0, tdeKb1;
	clock_t tstart, tend;

	vector<string> commLine;
	for ( int i = 0; i < argc; i++)
		commLine.push_back(*argv++);

	if( commLine.size() < 2 || commLine.size() > 3 ) {  // i.e. it takes two or three files
		cerr << "Error:  incorrect number of command line parameters." << endl
			<< "\nUsage:\n\tsummary tde_file_final" << endl 
			<< "\n output to summary-tde_file_final" << endl;
		exit(0);
		}

	tstart = clock();
	tdeKb0.readOntyxXMLFile( commLine[1] );	cerr << "Read \"" << commLine[1].c_str() << "\"" << endl;
	cerr << "File read -----------------------------------------------------------------" << endl;


	// KB concept iterator, used in all the processing below
	map<long, OntyxConcept>::iterator posCon;
	cerr << "Writing output file" << endl;
	ofstream outFile;
	string outfilename = "subset-" + commLine[1] + ".txt";
	outFile.open(outfilename.c_str());
	if( outFile.bad() ) {
		cerr << "ERROR opening output file, exiting." << endl << endl;
		exit(0);
		}

	vector<OntyxQualifier> qualifiers;
	vector<OntyxQualifier>::iterator posQual;
	multimap<long, OntyxProperty> properties;
	multimap<long, OntyxProperty>::iterator posProp;
	multimap<long, OntyxAssociation> associations;
	multimap<long, OntyxAssociation>::iterator posAssoc;
	
	for( posCon = tdeKb0.getFirstConceptIterator(); posCon != tdeKb0.getEndConceptIterator(); ++posCon ) {
		properties = posCon->second.getProperties();
		bool cdiscProperty = false;
		bool cdiscSource = false;
		string property = "";
		for( posProp = properties.begin(); posProp != properties.end(); ++posProp ) {
			property = posProp->second.getName();
			if( property == "FULL_SYN" || property == "ALT_DEFINITION" ) {
				qualifiers = posProp->second.getQualifiers();
				for( posQual = qualifiers.begin(); posQual != qualifiers.end(); ++posQual ) {
					if( posQual->getValue() == "CDISC" )
						cdiscProperty = true;
				}
			}
		}
		associations = posCon->second.getAssociations();
		for( posAssoc = associations.begin(); posAssoc != associations.end(); ++posAssoc ) {
			if( posAssoc->second.getValue() == "Clinical_Data_Interchange_Standards_Consortium" )
				cdiscSource = true;
		}		
		if( cdiscProperty && !cdiscSource )
			outFile << posCon->second.getCode() << endl;
	}

	outFile.close();

	tend = clock();
	cerr << "Execution took " << (tend-tstart)/1000.0 << " seconds" << endl;

	return 0;
}
