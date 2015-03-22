 

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
#include "tString.h"
#include "fullSyn.h"

using namespace std;

typedef struct comFlags {
	string	file1, file2, file3, file4, file5, file6;
	bool	changeSet;
	};

void ProcessCommandline (const vector<string> & commLine, comFlags & );
void readFlatList(const string &, set<string> & term_list);
bool checkMarkup (const string & inString, const string & startTag, const string & endTag);
string extractFromMarkup (const string &, const string &, const string &);
string getDayStamp (void);


int main (int argc, char * argv[])
{
	OntyxKb tdeKb, compareKb;		// input/output kb
	clock_t tstart, tend;

	vector<string> commLine;
	for ( int i = 0; i < argc; i++)
		commLine.push_back(*argv++);

	comFlags flags;
	flags.file1 = "";
	flags.file2 = "";
	if( commLine.size() != 2 ) {
		cerr << "Error:  incorrect number of command line parameters." << endl
			<< "\nUsage:\n\tpublishtomemefinal tde.xml" << endl 
			<< endl << "Requires ancillary prop_to_delete and branch_to_delete files."  << endl;
		exit(0);
		}
	else
		ProcessCommandline( commLine, flags);

	tstart = clock();

	tdeKb.readOntyxXMLFile( flags.file1 );	cerr << "Read \"" << flags.file1 << "\"" << endl;

	set<string>::iterator posDelete;

	// file of properties to delete
	set<string> prop_delete;
	readFlatList("prop_del_Meme.txt", prop_delete);
	
    //file of qualifiers to delete
	set<string> qual_delete;
	readFlatList("qual_del_Meme.txt", qual_delete);

	// file of branches to exclude
	set<string> branch_delete;
	readFlatList("branch_to_delete_publish_Meme.TXT", branch_delete);

	cout << "TDE file read -----------------------------------------------------------------" << endl;


	// KB concept iterator, used in all the processing below
	map<long, OntyxConcept>::iterator posCon;

	// ************* DELETE PROPERTIES *************
	if( prop_delete.size() > 0 ) {
		cerr << "Eliminating unpublishable properties." << endl;
		for( posCon = tdeKb.getFirstConceptIterator(); 
					posCon != tdeKb.getEndConceptIterator(); ++posCon) {
				posDelete = prop_delete.begin();
				for( ; posDelete != prop_delete.end(); ++posDelete )
					posCon->second.deleteProperty(*posDelete);
			}
		posDelete = prop_delete.begin();
		for( ; posDelete != prop_delete.end(); ++posDelete )
			tdeKb.removePropertyDef (*posDelete);
		}
	// ************* END DELETE PROPERTIES *************

	// ************* DELETE QUALIFIERS *************
	if( qual_delete.size() > 0 ) {
		cerr << "Eliminating unpublishable qualifiers." << endl;
/*		for( posCon = tdeKb.getFirstConceptIterator(); 
					posCon != tdeKb.getEndConceptIterator(); ++posCon) {
				posDelete = qual_delete.begin();
				for( ; posDelete != qual_delete.end(); ++posDelete )
					posCon->second.deleteQualifier(*posDelete);
			}*/
		posDelete = qual_delete.begin();
		for( ; posDelete != qual_delete.end(); ++posDelete )
			tdeKb.removeQualifierDef (*posDelete);
		}
	// ************* END DELETE QUALIFIERS *************
	// ************* EXCLUDE BRANCHES *************
	unsigned numconcept0 = static_cast<unsigned> (tdeKb.getNumConcepts());
	if( branch_delete.size() > 0 ) {
		cerr << "Extracting hierarchies to exclude, top nodes:" << endl;
		posDelete = branch_delete.begin();
		for( ; posDelete != branch_delete.end(); ++posDelete)
			cout << '\t' << *posDelete << endl;

		long excludeID;
		set<long> to_move;
		for(posDelete = branch_delete.begin(); posDelete != branch_delete.end(); ++posDelete) {
			to_move.clear();
			excludeID = tdeKb.getIDFromName(*posDelete);
			tdeKb.findDescendants(excludeID, to_move);
			tdeKb.moveSetConceptsTo(to_move, compareKb);
			}
		tdeKb.fixReferences();
		}
	if( numconcept0 !=  static_cast<unsigned> (tdeKb.getNumConcepts()) + branch_delete.size() )
		cerr << "WARNING:  Excluded hierarchies have children nodes." << endl;
	// ************* END EXCLUDE BRANCHES *************


	// ************* FILL IN FULL_SYN DEFAULT VALUES *************
	cerr << "Adding default qualifiers to full_syns and creating Synonyms" << endl;
	multimap<long, OntyxProperty> conPropMap;
	multimap<long, OntyxProperty>::iterator pConProp;
	set<string> tmpPropSet;
	set<string>::iterator pTmpPropSet;
	set<string> oldPropSet;
	vector<OntyxProperty> newPropVec;	
	vector<OntyxProperty>::iterator pNewPropVec;

	string 	typeQual = "Syn_Term_Type",
			sourceQual = "Syn_Source",
			codeQual = "Syn_Source_Code";

	string defaultType = "SY";
	string defaultSource = "NCI";

	string value, tagVal, typestr, defstr, sourstr, codestr, datestr, newVal;

	bool hasSource, hasType; // hasQual, 
	for( posCon = tdeKb.getFirstConceptIterator(); 
				posCon != tdeKb.getEndConceptIterator(); ++posCon) {
		tmpPropSet.clear();
		newPropVec.clear();
		conPropMap = posCon->second.getProperties();
		for( pConProp = conPropMap.begin(); pConProp != conPropMap.end(); ++pConProp) {
			if( pConProp->second.getName() == "FULL_SYN") {
				hasSource = hasType = false;					
				vector<OntyxQualifier> lquals;
				value = pConProp->second.getValue();
				if( !pConProp->second.hasQualifier() ) {
					OntyxQualifier ltype(typeQual,defaultType);
					OntyxQualifier lsource(sourceQual,defaultSource);
					lquals.push_back(ltype);
					lquals.push_back(lsource);
					}
				else {
					vector<OntyxQualifier> quals = pConProp->second.getQualifiers();
					vector<OntyxQualifier>::const_iterator pQuals = quals.begin();
					for( ; pQuals != quals.end(); ++pQuals ) {
						if( pQuals->getName() == typeQual ) {
							if( pQuals->getValue() == "AQS" ) {			// Hack to solve a business rule issue in tde
								OntyxQualifier ltype(typeQual, "AQ");
								lquals.push_back(ltype);
								hasType = true;
								}
							else {
								OntyxQualifier ltype(typeQual,pQuals->getValue());
								lquals.push_back(ltype);
								hasType = true;
								}
							}
						else if( pQuals->getName() == sourceQual ) {
							OntyxQualifier lsource(sourceQual,pQuals->getValue());
							lquals.push_back(lsource);
							hasSource = true;
							}
						else if( pQuals->getName() == codeQual ) {
							OntyxQualifier lcode(codeQual, pQuals->getValue());
							lquals.push_back(lcode);
							}
						}
					if( !hasType ) {					
						OntyxQualifier ltype(typeQual,defaultType);
						lquals.push_back(ltype);
						}
					if( !hasSource ) {
						OntyxQualifier lsource(sourceQual,defaultSource);
						lquals.push_back(lsource);
						}
					}
				OntyxProperty newProp("FULL_SYN", value);
				newProp.setQualifier(lquals);

				newPropVec.push_back(newProp);
				tmpPropSet.insert(value);
				}
			}
//		pTmpPropSet = tmpPropSet.begin();
//		for( ; pTmpPropSet != tmpPropSet.end(); ++pTmpPropSet ) {
//			OntyxProperty tmpProp("Synonym", *pTmpPropSet);
//			posCon->second.addProperty(tmpProp);
//			}
		tdeKb.removePropertyDef ("Synonym");
		posCon->second.deleteProperty("FULL_SYN");
		pNewPropVec = newPropVec.begin();
		for( ; pNewPropVec != newPropVec.end(); ++pNewPropVec ) {
			posCon->second.addProperty(*pNewPropVec);
			}
		}
	// ************* END FILL IN FULL_SYN DEFAULT VALUES *************



	// ************* START CONVERT DEFINITIONS *************
	string attrQual = "Definition_Attribution";
	sourceQual = "Definition_Source";
	// unused for export Definition_Review_Date and Definition_Reviewer_Name

	string propname;
	bool hassource, hasdef;
	
	multimap<long, OntyxProperty> conProps; 
	multimap<long, OntyxProperty>::iterator pProps;
	for( posCon = tdeKb.getFirstConceptIterator(); 
				posCon != tdeKb.getEndConceptIterator(); ++posCon) {
		conProps = posCon->second.getProperties();
		hasdef = false;
		newPropVec.clear();
		pProps = conProps.begin();
		for( ; pProps != conProps.end(); ++pProps) {
			if( pProps->second.getName() == "DEFINITION" ||  pProps->second.getName() == "LONG_DEFINITION" 
					||  pProps->second.getName() == "ALT_DEFINITION" ||  pProps->second.getName() == "ALT_LONG_DEFINITION") {
				propname = pProps->second.getName();
				value = pProps->second.getValue();
				vector<OntyxQualifier> lquals;
				hasdef = true;
				hassource = false;
				if( pProps->second.hasQualifier() ) {
					vector<OntyxQualifier> quals = pProps->second.getQualifiers();
					vector<OntyxQualifier>::const_iterator pQuals = quals.begin();
					for( ; pQuals != quals.end(); ++pQuals ) {
						if( pQuals->getName() == attrQual ) {
							OntyxQualifier lattr(attrQual, pQuals->getValue());
							lquals.push_back(lattr);
							}
						else if( pQuals->getName() == sourceQual ) {
							OntyxQualifier lsource(sourceQual, pQuals->getValue());
							lquals.push_back(lsource);
							hassource = true;
							}
						}
					}
				if( !hassource ) {
					if( propname == "DEFINITION" || propname == "LONG_DEFINITION" ) {
						OntyxQualifier lsource(sourceQual,"NCI");
						lquals.push_back(lsource);
						}
					else { // oops!  alt_definition doesn't have a source
						cerr << "ERROR:  " << propname << " in concept " << posCon->second.getName() 
							<< " doesn't have a source; will continue processing." << endl;
						}
					}

				OntyxProperty tmpProp(propname, value);
				tmpProp.setQualifier(lquals);
				newPropVec.push_back(tmpProp);
				}
			}
		if( hasdef ) {
			posCon->second.deleteProperty("DEFINITION");
			posCon->second.deleteProperty("LONG_DEFINITION");
			posCon->second.deleteProperty("ALT_DEFINITION");
			posCon->second.deleteProperty("ALT_LONG_DEFINITION");
			posCon->second.addProperties(newPropVec);
			}
		}

	// ************* END CONVERT DEFINITIONS *************


	// ************* VALIDATE PROPERTY LENGTH START *************
	cerr << "Validating property length." << endl;
	map<string, unsigned> str_len;
	str_len["string"] = 256;
	str_len["long_string"] = 1024;
	str_len["real_long_string"] = 0;  // really, no limit here

	string str_range;
	for( posCon = tdeKb.getFirstConceptIterator(); 
				posCon != tdeKb.getEndConceptIterator(); ++posCon) {
		conPropMap = posCon->second.getProperties();
		for( pConProp = conPropMap.begin(); pConProp != conPropMap.end(); ++pConProp) {
			str_range = tdeKb.getPropertyDefRange(pConProp->second.getName()) ;
			if( pConProp->second.getValue().size() > str_len[str_range] )
				if( str_range != "real_long_string" )
					cerr << "ERROR:  Property value length exceeded in concept '" << posCon->second.getName() << "'" << endl
							<< '\t' << "property name = " <<  pConProp->second.getName() << ", length = " << pConProp->second.getValue().size() << endl;
			}
		}

	// ************* VALIDATE PROPERTY LENGTH END *************


	string filename;
	string daystamp = getDayStamp();


	// ************* WRITE ONTYLOG FILE *************
	filename = "Thesaurus-MEME-" + daystamp + ".xml";
	cerr << "Writing output ontylog file '" << filename << "', properties with role names are not exported." << endl;
	tdeKb.writeOntyxXMLFile(filename, false, true);
	// void OntyxKb::writeOntyxXMLFile (const string & filename, bool flag, bool noProps) // flag = true:renumber
	// if noProps is true, properties with names derived from roles are not exported
	// ************* END WRITE ONTYLOG FILE *************
	



	tend = clock();
	cerr << "Execution took " << (tend-tstart)/1000.0 << " seconds" << endl;

	return 0;
}


void ProcessCommandline (const vector<string> & commLine, struct comFlags & flags)
{
	for( unsigned i = 1; i < commLine.size(); i++) {
		if( flags.file1 == "" )
			flags.file1 = commLine[i];
		else
			flags.file2 = commLine[i];
		}
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
	while( getline(inFile, inputLine), !inFile.eof())
		term_list.insert(inputLine);
	inFile.close();
}

bool checkMarkup (const string & inString, 
							const string & startTag,
							const string & endTag)
{
	bool returnFlag = true;
	string newString = "";
	unsigned startPos = inString.find(startTag, 0);
	if( startPos == string::npos) {
		cout << "Bad initial markup \"" << startTag << "\" in '" << inString << "'." << endl;
		returnFlag = false;
		startPos = 0;
		}
	unsigned endPos = inString.find(endTag, startPos);
	if( endPos == string::npos ) {
		cout << "Unterminated markup \"" << endTag << "\" in '" << inString << "'." << endl;
		returnFlag = false;
		}
	return returnFlag;
}


string extractFromMarkup (const string & inString, 
							const string & startTag,
							const string & endTag)
{
	string newString = "";
	unsigned startPos = inString.find(startTag, 0);
	if( startPos != string::npos) {
		unsigned endPos = inString.find(endTag, startPos);
		if( endPos != string::npos )
			newString = inString.substr(startPos + startTag.size(), endPos - startPos - startTag.size());
		else
			cout << "Unterminated markup in '" << inString << "'." << endl;
		}
	return newString;
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



