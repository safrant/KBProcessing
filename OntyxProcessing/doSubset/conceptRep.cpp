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

	tstart = clock();
	tdeKb0.readOntyxXMLFile( commLine[1] );	cerr << "Read \"" << commLine[1].c_str() << "\"" << endl;
	cerr << "File read -----------------------------------------------------------------" << endl;


	// KB concept iterator, used in all the processing below

	map<long, OntyxConcept>::iterator posCon;
	multimap<long, OntyxProperty> properties;
	vector<OntyxQualifier> qualifiers;
	vector<OntyxQualifier>::iterator posQual;
	multimap<long, OntyxProperty>::iterator posProp;

	cerr << "Writing output files" << endl;
	ofstream outFile;
	string outfilename = "Concept_Report.txt";
	outFile.open(outfilename.c_str());
	if( outFile.bad() ) {
		cerr << "ERROR opening output file, exiting." << endl << endl;
		exit(0);
		}

  	string conceptCode;
	ifstream inFile ("input.txt");
  	if (inFile.is_open())
  	{
		outFile << "Concept Code\tConcept Name\tPreferred Name\tDefinition\tFull Syn" << endl;
    		while (! inFile.eof() )
    		{
      			getline (inFile,conceptCode);
			bool foundConcept = false;
      			for( posCon = tdeKb0.getFirstConceptIterator(); posCon != tdeKb0.getEndConceptIterator(); ++posCon ) {
				if( posCon->second.getCode() == conceptCode ) {
					outFile << posCon->second.getCode() << "\t" << posCon->second.getName() << "\t"	<< posCon->second.getPropertyValue("Preferred_Name") << "\t" << posCon->second.getPropertyValue("DEFINITION");
					properties = posCon->second.getProperties();
					for( posProp = properties.begin(); posProp != properties.end(); ++posProp ) {
						if( posProp->second.getName() == "FULL_SYN" ) {					
							outFile << "\t" << posProp->second.getValue();
							qualifiers = posProp->second.getQualifiers();
							for( posQual = qualifiers.begin(); posQual != qualifiers.end(); ++posQual ) {
								if( posQual->getName() == "Syn_Source" )
									outFile << " " << "(" << posQual->getValue() << ")";
							}
						}
					}
					outFile << endl;
					foundConcept = true;
				}
			}
			if( !foundConcept )
				outFile << conceptCode << "\t" << "NO CONCEPT" << endl;
    		}
    		inFile.close();
  	}


	outFile << endl;

        outFile.close();

	tend = clock();
	cerr << "Execution took " << (tend-tstart)/1000.0 << " seconds" << endl;

	return 0;
}
