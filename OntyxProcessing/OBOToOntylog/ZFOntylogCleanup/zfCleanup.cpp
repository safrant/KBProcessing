
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
	OntyxKb tdeKb;		// input/output kb
	clock_t tstart, tend;

	vector<string> commLine;
	for ( int i = 0; i < argc; i++)
		commLine.push_back(*argv++);

	if( commLine.size() != 2 && commLine.size() != 3 ) {
		cerr << "Error:  incorrect number of command line parameters." << endl
			<< "\nUsage:\n\tontyxToOWL tde.xml [-C|N]" << endl << endl 
			<< "Requires ancillary header.owl, and headerCode.owl files."  << endl
			<< "Optional:  flag -C indicates output \"by code\", -N indicates output \"by name\", default is -N \"by name\"."  << endl;
		exit(0);
		}
	bool outputByCode = false;
	vector<string>::iterator pComm = commLine.begin();
	for( ; pComm != commLine.end(); ++pComm ) {
		if( *pComm == "-C" || *pComm == "-c" )
			outputByCode = true;
		}

	tstart = clock();

	tdeKb.readOntyxXMLFile( commLine[1] );	cerr << "Read \"" << commLine[1].c_str() << "\"" << endl;
	cerr << "TDE file read -----------------------------------------------------------------" << endl;

	long propID;
	propID = tdeKb.getMaxPropID();
	char c_maxPropCode[22];
	string s_maxPropCode;
	vector<string> nspacevec = tdeKb.getKbNames();
	string nspace = *(nspacevec.begin());  // apelon...  multiple namespaces...

	string propname = "display_name";
	if( !tdeKb.hasPropertyDef(propname) ) {
		propID++;
		sprintf(c_maxPropCode, "%ld", propID);
		s_maxPropCode = c_maxPropCode;
		OntyxPropertyDef tmpPropDef(propname, "P" + s_maxPropCode, propID, "string", false, false, nspace);
		tdeKb.addPropertyDef(tmpPropDef);
		}

	map<long, OntyxConcept>::iterator posCon;
	multimap<long, OntyxProperty> properties;
	multimap<long, OntyxProperty >::iterator pProp;
	multimap<long, OntyxRole> roles;
	multimap<long, OntyxRole >::const_iterator pRole;
	string startValue = "";
	string endValue = "";
	string partOfValue = "";
	string developsFrom = "";
	string changeValue = "";

	for( posCon = tdeKb.getFirstConceptIterator(); posCon != tdeKb.getEndConceptIterator(); ++posCon ) {
		startValue = "";
		endValue = "";
		partOfValue = "";
		developsFrom = "";
		properties = posCon->second.getProperties();
		roles = posCon->second.getRoles();
		pRole = roles.begin();
		for( ; pRole != roles.end(); ++pRole ) {
			if( pRole->second.getName() == "start" )
				startValue = pRole->second.getValue();
			else if( pRole->second.getName() == "end" )
				endValue = pRole->second.getValue();
			else if( pRole->second.getName() == "part_of" )
				partOfValue = pRole->second.getValue();
			else if( pRole->second.getName() == "develops_from" )
				developsFrom = pRole->second.getValue();
			}
		if( ( startValue != "" ) && ( endValue != "" ) ) {
			changeValue = " (" + startValue + " to " + endValue + ")";
			pProp = properties.begin();
			for( ; pProp != properties.end(); ++pProp ) {
				if( pProp->second.getName() == "Preferred_Name" ) {
					changeValue = pProp->second.getValue() + changeValue;
					OntyxProperty tmpProperty(propname,changeValue,"");
					posCon->second.addProperty(tmpProperty);
					}
				}
			}
		else {
			pProp = properties.begin();
			for( ; pProp != properties.end(); ++pProp ) {
				if( pProp->second.getName() == "Preferred_Name" ) {
					changeValue = pProp->second.getValue();
					OntyxProperty tmpProperty(propname,changeValue,"");
					posCon->second.addProperty(tmpProperty);
					}
				}
			}
/*		if( posCon->second.getNumParents() == 0 ) {
			if( partOfValue != "" ) 
				posCon->second.addParent( partOfValue );
			if( developsFrom != "" ) 
				posCon->second.addParent( developsFrom );
			}
*/
		}



	string filename;
	string daystamp = getDayStamp();

	filename = commLine[1] + "-ByName-" + daystamp + ".xml" ;
	tdeKb.writeOntyxXMLFile(filename, false, true);

//	tdeKb.codeToXname();

//	tdeKb.nameToXname();


/*	filename = commLine[1] + "-ByCode-" + daystamp + ".owl";
	cerr << "Writing output OWL file '" << filename << "', properties with role names are not exported." << endl;
	tdeKb.writeOWLFile (filename, "headerCode.owl", true);

	filename = commLine[1] + "-ByName-" + daystamp + ".owl";
	cerr << "Writing output OWL file '" << filename << "', properties with role names are not exported." << endl;
	tdeKb.writeOWLFileByName (filename, "header.owl", true);
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






