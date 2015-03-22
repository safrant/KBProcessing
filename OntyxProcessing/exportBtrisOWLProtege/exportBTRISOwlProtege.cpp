 

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

string getDayStamp (void);
string characterEscape (const string & inString);


int main (int argc, char * argv[])
{
	OntyxKb tdeKb, compareKb;		// input/output kb
	clock_t tstart, tend;

	vector<string> commLine;
	for ( int i = 0; i < argc; i++)
		commLine.push_back(*argv++);

	if( commLine.size() != 2 ) {
		cerr << "Error:  incorrect number of command line parameters." << endl
			<< "\nUsage:\n\texportOwlProtege tde.xml" << endl 
			<< endl << "Requires ancillary header.owl, and headerCode.owl files."  << endl;
		exit(0);
		}

	tstart = clock();

	tdeKb.readOntyxXMLFile( commLine[1] );	cerr << "Read \"" << commLine[1].c_str() << "\"" << endl;
	cerr << "TDE file read -----------------------------------------------------------------" << endl;


	// KB concept iterator, used in all the processing below
	map<long, OntyxConcept>::iterator posCon;
	vector<OntyxProperty> propVecDel, propVecAdd;
	vector<OntyxProperty>::iterator pPropVec;
	multimap<long, OntyxProperty> conPropMap;
	multimap<long, OntyxProperty>::iterator pConProp;
	bool good, hasProp;
	string newValue;



	// ************* CONVERT SYNONYMS TO FULLSYNS ************* 
	// ************* Use_Code qualifier is dismissed
	for( posCon = tdeKb.getFirstConceptIterator(); 
				posCon != tdeKb.getEndConceptIterator(); ++posCon) {
		good = false;
		hasProp = false;
		propVecDel.clear();
		propVecAdd.clear();

		conPropMap = posCon->second.getProperties();
		for( pConProp = conPropMap.begin(); pConProp != conPropMap.end(); ++pConProp) {
			if( pConProp->second.getName()== "Synonym" ) {
				hasProp = true;

				propVecDel.push_back( pConProp->second );
				OntyxProperty tmpProp("FULL_SYN", pConProp->second.getValue());

				if( pConProp->second.hasQualifier() ) {
					vector<OntyxQualifier> quals = pConProp->second.getQualifiers();
					vector<OntyxQualifier>::const_iterator pQuals = quals.begin();
					for( ; pQuals != quals.end(); ++pQuals ) {						// dismiss Use_Code
						if( 	pQuals->getName() == "Syn_Source" ||
								pQuals->getName() == "Syn_Source_Local_Code" ||
								pQuals->getName() == "Syn_Term_Type" ) {
							OntyxQualifier qual( pQuals->getName(), pQuals->getValue() );
							tmpProp.addQualifier(qual);
							}
						}
					}
				propVecAdd.push_back(tmpProp);
				}
			}
		if( hasProp ) {
			pPropVec = propVecDel.begin();
			for( ; pPropVec != propVecDel.end(); ++pPropVec ) {
				posCon->second.deleteProperty( *pPropVec );
				}
			pPropVec = propVecAdd.begin();
			for( ; pPropVec != propVecAdd.end(); ++pPropVec ) {
				posCon->second.addProperty( *pPropVec );
				}
			}
		// ************* END CONVERT SYNONYMS TO FULLSYNS ************* 
		// ************* posCon ITERATION CONTINUES *******************
		


		// ************* CONVERT LONG_DEFINITION TO DEFINITION ********
		// ************* no qualifiers are dismissed
		good = false;
		hasProp = false;
		propVecDel.clear();
		propVecAdd.clear();

		for( pConProp = conPropMap.begin(); pConProp != conPropMap.end(); ++pConProp) {
			if( pConProp->second.getName()== "LONG_DEFINITION" ) {
				hasProp = true;

				propVecDel.push_back( pConProp->second );
				OntyxProperty tmpProp("DEFINITION", pConProp->second.getValue());

				if( pConProp->second.hasQualifier() ) {
					vector<OntyxQualifier> quals = pConProp->second.getQualifiers();
					vector<OntyxQualifier>::const_iterator pQuals = quals.begin();
					for( ; pQuals != quals.end(); ++pQuals ) {					// no qualifiers dismissed, add all
						OntyxQualifier qual( pQuals->getName(), pQuals->getValue() );
						tmpProp.addQualifier(qual);
						}
					}
				propVecAdd.push_back(tmpProp);
				}
			}
		if( hasProp ) {
			pPropVec = propVecDel.begin();
			for( ; pPropVec != propVecDel.end(); ++pPropVec ) {
				posCon->second.deleteProperty( *pPropVec );
				}
			pPropVec = propVecAdd.begin();
			for( ; pPropVec != propVecAdd.end(); ++pPropVec ) {
				posCon->second.addProperty( *pPropVec );
				}
			}
		// ************* END CONVERT LONG_DEFINITION TO DEFINITION ****
		// ************* posCon ITERATION CONTINUES *******************

		

		// ************* PROCESS DEFINITION AND ALT_DEFINITION ********
		// ************* syn_term_type is dismissed
		// ************* syn_source is converted to def-source
		good = false;
		hasProp = false;
		propVecDel.clear();
		propVecAdd.clear();
		newValue = "";

		for( pConProp = conPropMap.begin(); pConProp != conPropMap.end(); ++pConProp) {
			if( pConProp->second.getName()== "DEFINITION" || pConProp->second.getName()== "ALT_DEFINITION") {
				if( pConProp->second.hasQualifier() ) {								// don't process ncit definitions
					hasProp = true;

					propVecDel.push_back( pConProp->second );						// will need to delete the qualified value
					newValue = "<def-definition>" + characterEscape(pConProp->second.getValue()) + "</def-definition>";

					vector<OntyxQualifier> quals = pConProp->second.getQualifiers();
					vector<OntyxQualifier>::const_iterator pQuals = quals.begin();
					for( ; pQuals != quals.end(); ++pQuals ) {					// syn_term_type is dismissed, the rest are added
						if( pQuals->getName() == "Definition_Source" || pQuals->getName() == "Syn_Source" )
							newValue = newValue + "<def-source>" + characterEscape(pQuals->getValue()) + "</def-source>";
						else if( pQuals->getName() == "Definition_Attribution" )
							newValue = newValue + "<attr>" + characterEscape(pQuals->getValue()) + "</attr>";
						}
					OntyxProperty tmpProp( pConProp->second.getName(), newValue );
					propVecAdd.push_back(tmpProp);
					}
				else if( pConProp->second.getValue().find("def-source") == string::npos) {		// doesn't have microsyntax either
					hasProp = true;
					propVecDel.push_back( pConProp->second );
					newValue = "<def-definition>" + characterEscape(pConProp->second.getValue()) + "</def-definition>>";
					newValue = newValue + "<def-source>RED</def-source>";
					OntyxProperty tmpProp( pConProp->second.getName(), newValue );
					propVecAdd.push_back(tmpProp);
					}
				}
			}
		if( hasProp ) {
			pPropVec = propVecDel.begin();
			for( ; pPropVec != propVecDel.end(); ++pPropVec ) {
				posCon->second.deleteProperty( *pPropVec );
				}
			pPropVec = propVecAdd.begin();
			for( ; pPropVec != propVecAdd.end(); ++pPropVec ) {
				posCon->second.addProperty( *pPropVec );
				}
			}
		// ************* END PROCESS DEFINITION AND ALT_DEFINITION ****
		// ************* posCon ITERATION CONTINUES *******************



		// ************* PROCESS FULL_SYN *****************************
		// ************* convert or pass-thru, listing elsewhere (but can be read from code below)
		// ************* 
		good = false;
		hasProp = false;
		propVecDel.clear();
		propVecAdd.clear();
		newValue = "";

		for( pConProp = conPropMap.begin(); pConProp != conPropMap.end(); ++pConProp) {
			if( pConProp->second.getName()== "FULL_SYN" ) {
				if( pConProp->second.hasQualifier() ) {								// don't process ncit-derived full_syns
					hasProp = true;

					propVecDel.push_back( pConProp->second );						// will need to delete the qualified value
					newValue = "<term-name>" + characterEscape(pConProp->second.getValue()) + "</term-name>";

					vector<OntyxQualifier> quals = pConProp->second.getQualifiers();
					vector<OntyxQualifier>::const_iterator pQuals = quals.begin();
					for( ; pQuals != quals.end(); ++pQuals ) {	
						if( pQuals->getName() == "Syn_Abbreviation" )
							newValue = newValue + "<term-name>" + characterEscape(pQuals->getValue()) + "</term-name>";

						else if( pQuals->getName() == "Syn_Source" || pQuals->getName() == "Definition_Source" )
							newValue = newValue + "<term-source>" + characterEscape(pQuals->getValue()) + "</term-source>";

						else if( pQuals->getName() == "Syn_Source_Local_Code" || pQuals->getName() == "Syn_Source_Second_ID" )
							newValue = newValue + "<source-code>" + characterEscape(pQuals->getValue()) + "</source-code>";

						else if( pQuals->getName() == "Syn_Term_Type" )
							newValue = newValue + "<term-group>" + characterEscape(pQuals->getValue()) + "</term-group>";

						else
							newValue = newValue + "<" + pQuals->getName() + ">" + characterEscape(pQuals->getValue()) + "</" + pQuals->getName() + ">";
						}
					OntyxProperty tmpProp( pConProp->second.getName(), newValue );
					propVecAdd.push_back(tmpProp);
					}
				else if( pConProp->second.getValue().find("term-name") == string::npos ) {			// it doesn't have microsyntax either
					hasProp = true;
					propVecDel.push_back( pConProp->second );
					newValue = "<term-name>" + characterEscape(pConProp->second.getValue()) + "</term-name>";
					newValue = newValue + "<term-source>RED</term-source><term-group>SY</term-group>";
					OntyxProperty tmpProp( pConProp->second.getName(), newValue );
					propVecAdd.push_back(tmpProp);
					}
				}
			}
		if( hasProp ) {
			pPropVec = propVecDel.begin();
			for( ; pPropVec != propVecDel.end(); ++pPropVec ) {
				posCon->second.deleteProperty( *pPropVec );
				}
			pPropVec = propVecAdd.begin();
			for( ; pPropVec != propVecAdd.end(); ++pPropVec ) {
				posCon->second.addProperty( *pPropVec );
				}
			}
		// ************* END PROCESS FULL_SYN *************************
		// ************* posCon ITERATION CONTINUES *******************



		// ************* PROCESS Reference_Source AND Lab_Test_Status 
		// ************* dismiss Use_Code
		good = false;
		hasProp = false;
		propVecDel.clear();
		propVecAdd.clear();
		newValue = "";

		for( pConProp = conPropMap.begin(); pConProp != conPropMap.end(); ++pConProp) {
			if( pConProp->second.getName()== "Reference_Source" || pConProp->second.getName()== "Lab_Test_Status" ) {
				hasProp = true;
				propVecDel.push_back( pConProp->second );						// will need to delete the qualified value
				if( pConProp->second.getName()== "Reference_Source" )
					newValue = "<rs-value>" + characterEscape(pConProp->second.getValue()) + "</rs-value>";
				else
					newValue = "<lts-value>" + characterEscape(pConProp->second.getValue()) + "</lts-value>";

				if( pConProp->second.hasQualifier() ) {								
					vector<OntyxQualifier> quals = pConProp->second.getQualifiers();
					vector<OntyxQualifier>::const_iterator pQuals = quals.begin();
					for( ; pQuals != quals.end(); ++pQuals ) {	
						if( pQuals->getName() != "Use_Code" )
							newValue = newValue + "<" + pQuals->getName() + ">" + characterEscape(pQuals->getValue()) + "</" + pQuals->getName() + ">";
						}
					}
				OntyxProperty tmpProp( pConProp->second.getName(), newValue );
				propVecAdd.push_back(tmpProp);
				}
			}
		if( hasProp ) {
			pPropVec = propVecDel.begin();
			for( ; pPropVec != propVecDel.end(); ++pPropVec ) {
				posCon->second.deleteProperty( *pPropVec );
				}
			pPropVec = propVecAdd.begin();
			for( ; pPropVec != propVecAdd.end(); ++pPropVec ) {
				posCon->second.addProperty( *pPropVec );
				}
			}
		// ************* END PROCESS Reference_Source AND Lab_Test_Status 
		// ************* posCon ITERATION CONTINUES *******************




		// ************* PROCESS Contributing_Source_Local_Code 
		// ************* convert Syn_Source to Use_Source
		good = false;
		hasProp = false;
		propVecDel.clear();
		propVecAdd.clear();
		newValue = "";

		for( pConProp = conPropMap.begin(); pConProp != conPropMap.end(); ++pConProp) {
			if( pConProp->second.getName()== "Contributing_Source_Local_Code" ) {
				hasProp = true;
				propVecDel.push_back( pConProp->second );						// will need to delete the qualified value
				newValue = "<cslc-value>" + characterEscape(pConProp->second.getValue()) + "</cslc-value>";

				if( pConProp->second.hasQualifier() ) {								
					vector<OntyxQualifier> quals = pConProp->second.getQualifiers();
					vector<OntyxQualifier>::const_iterator pQuals = quals.begin();
					for( ; pQuals != quals.end(); ++pQuals ) {	
						if( pQuals->getName() != "Syn_Source" )
							newValue = newValue + "<Use_Source>" + characterEscape(pQuals->getValue()) + "</Use_Source>";
						else
							newValue = newValue + "<" + pQuals->getName() + ">" + characterEscape(pQuals->getValue()) + "</" + pQuals->getName() + ">";
						}
					}
				OntyxProperty tmpProp( pConProp->second.getName(), newValue );
				propVecAdd.push_back(tmpProp);
				}
			}
		if( hasProp ) {
			pPropVec = propVecDel.begin();
			for( ; pPropVec != propVecDel.end(); ++pPropVec ) {
				posCon->second.deleteProperty( *pPropVec );
				}
			pPropVec = propVecAdd.begin();
			for( ; pPropVec != propVecAdd.end(); ++pPropVec ) {
				posCon->second.addProperty( *pPropVec );
				}
			}
		// ************* END PROCESS Contributing_Source_Local_Code 


	} // ************* END posCon ITERATION


	string filename;
	string daystamp = getDayStamp();




	// ************* WRITE BY NAME OWL FILE *************
	filename = "Red-" + daystamp + ".owl";
	cerr << "Writing output OWL file '" << filename << "', properties with role names are not exported." << endl;
	tdeKb.writeOWLFileByName (filename, "header.owl", true); 
	// if noProps is true, properties with names derived from roles are not exported
	// ************* END WRITE OWL FILE *************

/*
	// ************* WRITE BY CODE OWL FILE *************
	filename = "Thesaurus-ByCode-" + daystamp + ".owl";
	cerr << "Writing output OWL file '" << filename << "', properties with role names are not exported." << endl;
	tdeKb.writeOWLFile (filename, "headerCode.owl", true); 
	// if noProps is true, properties with names derived from roles are not exported
	// ************* END WRITE OWL FILE *************

*/


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


string characterEscape (const string & inString)
{
	string searVec[] = { "&", "<" };
	string repVec[] = { "&amp;", "&lt;" };
	unsigned pos; // startPos, endPos, 
	unsigned i;
	string tmpString = inString;
	if( tmpString.find_first_of("<&",0) != string::npos ) {
		for( i = 0; i < sizeof(searVec)/sizeof(string); i++ ) {
			pos = 0;
			while ( (pos = tmpString.find(searVec[i], pos)) != string::npos ) {
				tmpString = tmpString.replace(pos++, searVec[i].size(), repVec[i]);
				++pos;
				}
			}
		}
	return tmpString;
}





