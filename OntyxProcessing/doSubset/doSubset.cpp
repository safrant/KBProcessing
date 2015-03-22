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
			<< "\nUsage:\n\tdoSubset tde_file_final" << endl
			<< "\n output to doSubset-tde_file_final" << endl;
		exit(0);
		}

	tstart = clock();
	tdeKb0.readOntyxXMLFile( commLine[1] );	cerr << "Read \"" << commLine[1].c_str() << "\"" << endl;
	cerr << "File read -----------------------------------------------------------------" << endl;


	// KB concept iterator, used in all the processing below
	map<long, OntyxConcept>::iterator posCon;
	cerr << "Writing output files" << endl;
	ofstream outFile;
	string outfilename = "Master_Subset-" + commLine[1] + ".txt";
	outFile.open(outfilename.c_str());
	if( outFile.bad() ) {
		cerr << "ERROR opening output file, exiting." << endl << endl;
		exit(0);
		}



	/* SUBSET REPORTING */

	vector<OntyxQualifier> qualifiers;
	vector<OntyxQualifier>::iterator posQual;
	multimap<long, OntyxProperty> properties;
	multimap<long, OntyxProperty>::iterator posProp;
	multimap<long, OntyxAssociation> associations;
	multimap<long, OntyxAssociation>::iterator posAssoc;
	set<string> terminologies;
	set<string>::iterator posTerms;
	vector<string> sources;
	vector<string>::iterator posSrc;
	vector<string> subsets;
	vector<string>::iterator posSub;
	string source;
	string subset;
	string subsetPT;
        string subsetCode;
	string sourcePT;
	string sourceDef;
	string synonym;
	string sourceSyns;
	string inputLine;

	tdeKb0.findDescendants("Terminology_Subset", terminologies);
	for( posTerms = terminologies.begin(); posTerms != terminologies.end(); ++posTerms ) {
		subsets.push_back( *posTerms );
		sources.push_back( tdeKb0.getConcept(*posTerms).getPropertyValue("Contributing_Source") );
	}

	posSrc = sources.begin();
	posSub = subsets.begin();

	outFile << "Source\tSubset Concept Code\tSubset PT (NCI)\tConcept Code\tNCI PT\tSource PT\tNCI Definition\tSource Definition\tSource Synonym(s)" << endl;

	while( posSrc != sources.end() && posSub != subsets.end() ) {
		source = *posSrc;
		subset = *posSub;
		subsetPT = "";
		subsetCode = "";

		// 020707 Added sub output files
		ofstream subOutFile;
		string suboutfilename = subset + "-" + commLine[1] + ".txt";
		subOutFile.open(suboutfilename.c_str());
		if( subOutFile.bad() ) {
			cerr << "ERROR opening sub output file, exiting." << endl << endl;
			exit(0);
			}
		//022007 added UNII Code
		if( subset == "FDA_UNII_Code_Terminology" )
			subOutFile << "Source\tSubset Concept Code\tSubset PT (NCI)\tConcept Code\tUNII Code\tNCI PT\tSource PT\tNCI Definition\tSource Definition\tSource Synonym(s)" << endl;
		else
			subOutFile << "Source\tSubset Concept Code\tSubset PT (NCI)\tConcept Code\tNCI PT\tSource PT\tNCI Definition\tSource Definition\tSource Synonym(s)" << endl;

                //set subsetPT value and code
                for( posCon = tdeKb0.getFirstConceptIterator(); posCon != tdeKb0.getEndConceptIterator(); ++posCon ) {
                    if( posCon->second.getName() == subset ) {
                        subsetCode = posCon->second.getCode();
                        properties = posCon->second.getProperties();
                        for( posProp = properties.begin(); posProp != properties.end(); ++posProp ) {
                             if( posProp->second.getName() == "FULL_SYN" && posProp->second.hasQualifier() ) {
                                 qualifiers = posProp->second.getQualifiers();
                                 bool synSourceFound = false;
				 bool ptFound = false;
                                 for( posQual = qualifiers.begin(); posQual != qualifiers.end(); ++posQual ) {
                                      if( posQual->getName() == "Syn_Source" && posQual->getValue() == source )
					synSourceFound = true;
				      if( posQual->getName() == "Syn_Term_Type" && posQual->getValue() == "PT" )
					ptFound = true;
				      if( !synSourceFound && ptFound )
					subsetPT = posProp->second.getValue();
                                 }
                             }
                        }
                    }
                }

                if( subsetPT == "" )
                    subsetPT = subset;

		for( posCon = tdeKb0.getFirstConceptIterator(); posCon != tdeKb0.getEndConceptIterator(); ++posCon ) {
			associations = posCon->second.getAssociations();
			for( posAssoc = associations.begin(); posAssoc != associations.end(); ++posAssoc ) {
				if( posAssoc->second.getValue() == subset ) {
                                        bool printConcept = true;
					//013107 added NCI PT
					sourcePT = "";
					sourceDef = "";
					synonym = "";
					sourceSyns = "";
					properties = posCon->second.getProperties();
					for( posProp = properties.begin(); posProp != properties.end(); ++posProp ) {
                                                if( posProp->second.getName() == "Concept_Status" && posProp->second.getValue() == "Retired_Concept" ) {
                                                    printConcept = false;
                                                }
						if( posProp->second.getName() == "FULL_SYN" && posProp->second.hasQualifier() ) {
							qualifiers = posProp->second.getQualifiers();
							bool synSourceFound = false;
							bool ptFound = false;
							for( posQual = qualifiers.begin(); posQual != qualifiers.end(); ++posQual ) {
								if( posQual->getName() == "Syn_Source" && posQual->getValue() == source )
									synSourceFound = true;
								if( posQual->getName() == "Syn_Term_Type" && posQual->getValue() == "PT" )
									ptFound = true;
								if( synSourceFound && ptFound ) {
									sourcePT = posProp->second.getValue();
								}
							}
						}
						if( posProp->second.getName() == "ALT_DEFINITION" && posProp->second.hasQualifier() ) {
							qualifiers = posProp->second.getQualifiers();
							for( posQual = qualifiers.begin(); posQual != qualifiers.end(); ++posQual )  {
								if( posQual->getName() == "Definition_Source" && posQual->getValue() == source )
									sourceDef = posProp->second.getValue();
							}
						}
						if( posProp->second.getName() == "FULL_SYN" && posProp->second.hasQualifier() ) {
							qualifiers = posProp->second.getQualifiers();
							bool synSourceFound = false;
							bool ptFound = true;
							for( posQual = qualifiers.begin(); posQual != qualifiers.end(); ++posQual ) {
								//022107 - added missing condition posQual->getValue == source
								if( posQual->getName() == "Syn_Source" && posQual->getValue() == source && posProp->second.getValue() != sourcePT ) {
									synonym = posProp->second.getValue();
									if(sourceSyns != "") {
										sourceSyns.append("|");
										sourceSyns.append(synonym);
									}
									else
										sourceSyns.append(synonym);
									synSourceFound = false;
									ptFound = true;
								}
							}
						}
					}
					if( printConcept ) {
        	                                outFile << source << "\t" << subsetCode << "\t" << subsetPT << "\t" << posCon->second.getCode() << "\t" << posCon->second.getPropertyValue("Preferred_Name") << "\t";

        					if( subset == "FDA_UNII_Code_Terminology" )
        						subOutFile << source << "\t" << subsetCode << "\t" << subsetPT << "\t" << posCon->second.getCode() << "\t" << posCon->second.getPropertyValue("FDA_UNII_Code") << "\t" << posCon->second.getPropertyValue("Preferred_Name") << "\t";
        					else
        						subOutFile << source << "\t" << subsetCode << "\t" << subsetPT << "\t" << posCon->second.getCode() << "\t" << posCon->second.getPropertyValue("Preferred_Name") << "\t";
             					if( sourcePT == "" ) {
        						outFile << "" << "\t";
        						subOutFile << "" << "\t";
        					}
        					else {
        						outFile << sourcePT << "\t";
        						subOutFile << sourcePT << "\t";
        					}

        					// 013107 Added NCI Def
        					outFile << posCon->second.getPropertyValue("DEFINITION") << "\t";
        					subOutFile << posCon->second.getPropertyValue("DEFINITION") << "\t";

        					if( sourceDef == "" ) {
        						outFile << "" << "\t";
        						subOutFile << "" << "\t";
        					}
        					else {
        						outFile << sourceDef << "\t";
        						subOutFile << sourceDef << "\t";
        					}
        					if( sourceSyns == "" ) {
        						outFile << "" << endl;
        						subOutFile << "" << endl;
        					}
        					else {
        						outFile << sourceSyns << endl;
        						subOutFile << sourceSyns << endl;
        					}
					}
				}
			}
		}
	++posSrc;
	++posSub;
	subOutFile << endl;
	subOutFile.close();
	}

	outFile << endl;

	/* END SUBSET REPORTING */


        outFile.close();

	tend = clock();
	cerr << "Execution took " << (tend-tstart)/1000.0 << " seconds" << endl;

	return 0;
}
