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

	map<long, OntyxConcept>::iterator posConPar;
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
	string sourcePT;
	string sourceCode;
	string sourceDef;
	string synonym;
	string synCode;
	string sourceSyns;
	set<string> parents;
	string parent;
	string useFor;
	string useForCode;
	string useForBlurb;
        set<string>::iterator posPars;
	string parentCode;
	string parentSourceCode;

	tdeKb0.findDescendants("Terminology_Subset", terminologies);
	for( posTerms = terminologies.begin(); posTerms != terminologies.end(); ++posTerms ) {
		subsets.push_back( *posTerms );
		sources.push_back( tdeKb0.getConcept(*posTerms).getPropertyValue("Contributing_Source") );
	}

	posSrc = sources.begin();
	posSub = subsets.begin();

        outFile << "Source\tSubset Code\tSubset Name\tNCI Concept Code\tSource (FDA CDRH) Code\tSource PT (FDA CDRH PT)\tSource Synonym(s) (FDA CDRH) SYs\tSource (FDA CDRH) Definition\tNCI Definition\tParent Concept's NCIt Concept Code\tParent Concept's Source Code (Parent Concept's FDA CDRH Code)\tParent Concept's Source PT (Parent Concept's FDA CDRH PT)\tParent Concept's NCIt PT (Only for concepts that DO NOT HAVE Source PT)";
	outFile << endl;

	while( posSrc != sources.end() && posSub != subsets.end() ) {
                source = *posSrc;
                subset = *posSub;
		if( subset == "Medical_Device_Component_Or_Accessory_Terminology_CDRH" || subset == "Medical_Device_Problem_Codes_FDA_CDRH"
				|| subset == "Patient_Problem_Codes_FDA_CDRH" ) {
			for( posCon = tdeKb0.getFirstConceptIterator(); posCon != tdeKb0.getEndConceptIterator(); ++posCon ) {
           			associations = posCon->second.getAssociations();
	         		for( posAssoc = associations.begin(); posAssoc != associations.end(); ++posAssoc ) {
		         		if( posAssoc->second.getValue() == subset ) {
                                            sourcePT = "";
                                            sourceCode = "";
	                                    sourceDef = "";
	                                    synonym = "";
	                                    synCode = "";
	                                    sourceSyns = "";
	                                    useFor = "";
	                                    useForCode = "";
	                                    useForBlurb = "";
	                                    parentCode = "";
                                            parents = posCon->second.getParents();
                                            parent = *(parents.begin());
                                            parentCode = tdeKb0.getConcept(parent).getCode();
                                            parentSourceCode = "";
                                            bool parentIsNCI = true;
					    for( posConPar = tdeKb0.getFirstConceptIterator(); posConPar != tdeKb0.getEndConceptIterator(); ++posConPar ) {
                                                if( posConPar->second.getCode() == parentCode ) {
                                                    properties = posConPar->second.getProperties();
                                                    for( posProp = properties.begin(); posProp != properties.end(); ++posProp ) {
                                                        if( posProp->second.getName() == "FULL_SYN" && posProp->second.hasQualifier() ) {
        							bool synSourceFound = false;
        							bool ptFound = false;
                                                            qualifiers = posProp->second.getQualifiers();
                                                            string tmpCode = "";
                                                            for( posQual = qualifiers.begin(); posQual != qualifiers.end(); ++posQual ) {
                                                                if( posQual->getName() == "Syn_Source" && posQual->getValue() == source )
                                                                    synSourceFound = true;
                                                                if( posQual->getName() == "Syn_Term_Type" && posQual->getValue() == "PT" )
                                                                    ptFound = true;
                                                                if( posQual->getName() == "Syn_Source_Code" )
                                                                    tmpCode = posQual->getValue();
                                                                if( synSourceFound && ptFound ) {
                                                                    parent = posProp->second.getValue();
                                                                    parentSourceCode = tmpCode;
                                                                    parentIsNCI = false;
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
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
                                                                if( ( synSourceFound && ptFound ) &&
                                                                     posQual->getName() == "Syn_Source_Code" )
                                                                        sourceCode = posQual->getValue();
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
                                                        bool synExists = false;
                                                        bool synCodeExists = false;
    							qualifiers = posProp->second.getQualifiers();
    							for( posQual = qualifiers.begin(); posQual != qualifiers.end(); ++posQual ) {
    								//022107 - added missing condition posQual->getValue == source
    								if( posQual->getName() == "Syn_Source" && posQual->getValue() == source && posProp->second.getValue() != sourcePT ) {
    									synonym = posProp->second.getValue();
    									synExists = true;
             							}
             							if( posQual->getName() == "Syn_Source_Code" ) {
                                                                        synCode = posQual->getValue();
                                                                        synCodeExists = true;
                                                                }
                                                        }
                                                        if( synExists ) {
                                                            if( sourceSyns != "" )
                                                                sourceSyns.append("|");
                                                            sourceSyns.append(synonym);
                                                        }
                                                        /*
                                                        if( synExists ) {
                                                          if( sourceSyns != "" )
                                                              sourceSyns.append("|");
                                                          sourceSyns.append(synonym);
                                                          if( synCodeExists ) {
			                                      sourceSyns.append(" (");
			                                      sourceSyns.append(synCode);
			                                      sourceSyns.append(")");
			                                  }
			                                  else
                                                              sourceSyns.append(" (NOCODE)");
                                                        }
                                                        */
                                                }
                                                /*
                                                if( posProp->second.getName() == "Use_For" && posProp->second.hasQualifier() ) {
                                                    useFor = posProp->second.getValue();
                                                    bool isSource = false;
                                                    qualifiers = posProp->second.getQualifiers();
                                                    for( posQual = qualifiers.begin(); posQual != qualifiers.end(); ++posQual ) {
                                                        if( posQual->getName() == "Use_Source" && posQual->getValue() == source )
                                                            isSource = true;
                                                        if( posQual->getName() == "Use_Code" && isSource )
                                                            useForCode = posQual->getValue();
                                                    }
                                                    if( isSource ) {
                                                        if( useForBlurb != "" )
                                                            useForBlurb.append("|");
                                                        useForBlurb.append(useFor);
                                                        if( useForCode != "" ) {
                                                            useForBlurb.append(" (");
                                                            useForBlurb.append(useForCode);
                                                            useForBlurb.append(")");
                                                        }
                                                        else
                                                            useForBlurb.append(" (NOCODE)");
                                                    }
                                                }
                                                */
                                            }
                                            outFile << source << "\t" << tdeKb0.getConcept(subset).getCode() << "\t" << subset << "\t" << posCon->second.getCode() << "\t";
                                            outFile << sourceCode << "\t" << sourcePT << "\t" << sourceSyns << "\t" << sourceDef << "\t" << posCon->second.getPropertyValue("DEFINITION") << "\t";
                                            outFile << parentCode << "\t" << parentSourceCode << "\t";
                                            if( parentIsNCI == false )
                                                outFile << parent;
                                            else {
                                                bool printed = false;
                                                properties = tdeKb0.getConcept(parent).getProperties();
                                                for( posProp = properties.begin(); posProp != properties.end(); ++posProp ) {
                                                     bool dontPrint = false;

    						     if( posProp->second.getName() == "FULL_SYN" && posProp->second.hasQualifier() ) {
    						     	qualifiers = posProp->second.getQualifiers();
    							for( posQual = qualifiers.begin(); posQual != qualifiers.end(); ++posQual ) {
                                                                if( posQual->getName() == "Syn_Source" ) {
                                                                    dontPrint = true;
                                                                }
    								if( posQual->getName() == "Syn_Term_Type" && posQual->getValue() == "PT" ) {
                                                                    if( !dontPrint )
    									outFile << "\t" << posProp->second.getValue();
    									printed = true;
    								}
    							}
    						     }
                                                }
                                                if( !printed ) {
                                                    properties = tdeKb0.getConcept(parent).getProperties();
                                                    for( posProp = properties.begin(); posProp != properties.end(); ++posProp ) {
                                                        if( posProp->second.getName() == "Preferred_Name" ) {
                                                            outFile << "\t" << posProp->second.getValue();
                                                        }
                                                    }
                                                }
                                            }
                                            outFile << endl;
                                        }
                                }
                        }
                }
              	++posSrc;
               	++posSub;
 }




/*
	tdeKb0.findDescendants("Terminology_Subset", terminologies);
	for( posTerms = terminologies.begin(); posTerms != terminologies.end(); ++posTerms ) {
		subsets.push_back( *posTerms );
		sources.push_back( tdeKb0.getConcept(*posTerms).getPropertyValue("Contributing_Source") );
	}

	posSrc = sources.begin();
	posSub = subsets.begin();

	outFile << "Source\tSubset\tConcept Code\tNCI PT\tSource PT\tNCI Definition\tSource Definition\tSource Synonym(s)" << endl;

	while( posSrc != sources.end() && posSub != subsets.end() ) {
		source = *posSrc;
		subset = *posSub;

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
			subOutFile << "Source\tSubset\tConcept Code\tUNII Code\tNCI PT\tSource PT\tNCI Definition\tSource Definition\tSource Synonym(s)" << endl;
		else
			subOutFile << "Source\tSubset\tConcept Code\tNCI PT\tSource PT\tNCI Definition\tSource Definition\tSource Synonym(s)" << endl;

		for( posCon = tdeKb0.getFirstConceptIterator(); posCon != tdeKb0.getEndConceptIterator(); ++posCon ) {
			associations = posCon->second.getAssociations();
			for( posAssoc = associations.begin(); posAssoc != associations.end(); ++posAssoc ) {
				if( posAssoc->second.getValue() == subset ) {

					//013107 added NCI PT
					outFile << source << "\t" << subset << "\t" << posCon->second.getCode() << "\t" << posCon->second.getPropertyValue("Preferred_Name") << "\t";
					if( subset == "FDA_UNII_Code_Terminology" )
						subOutFile << source << "\t" << subset << "\t" << posCon->second.getCode() << "\t" << posCon->second.getPropertyValue("FDA_UNII_Code") << "\t" << posCon->second.getPropertyValue("Preferred_Name") << "\t";
					else
						subOutFile << source << "\t" << subset << "\t" << posCon->second.getCode() << "\t" << posCon->second.getPropertyValue("Preferred_Name") << "\t";
					sourcePT = "";
					sourceDef = "";
					synonym = "";
					sourceSyns = "";
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
	++posSrc;
	++posSub;
	subOutFile << endl;
	subOutFile.close();
	}

*/

	outFile << endl;

	/* END SUBSET REPORTING */


        outFile.close();

	tend = clock();
	cerr << "Execution took " << (tend-tstart)/1000.0 << " seconds" << endl;

	return 0;
}