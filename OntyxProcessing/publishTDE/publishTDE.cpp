 

#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <set>
#include <algorithm>
#include <ctime>

#include "OntyxKb.h"
#include "OntyxKBQA.h"
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
string characterEscape (const string & inString);


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
			<< "\nUsage:\n\tpublishTDE tde.xml" << endl 
			<< endl << "Requires ancillary prop_to_delete, header.owl, and branch_to_delete files."  << endl;
		exit(0);
		}
	else
		ProcessCommandline( commLine, flags);

	tstart = clock();

	tdeKb.readOntyxXMLFile( flags.file1 );	cerr << "Read \"" << flags.file1 << "\"" << endl;

	set<string>::iterator posDelete;

	// file of properties to delete
	set<string> prop_delete;
	readFlatList("prop_del.txt", prop_delete);
	
    //file of qualifiers to delete
	set<string> qual_delete;
	readFlatList("qual_del.txt", qual_delete);

	// file of branches to exclude
	set<string> branch_delete;
	readFlatList("branch_to_delete_publish_DTS.TXT", branch_delete);

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
			}
			*/
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


	// ************* CONVERT FULLSYNS *************
	// CONVERT FULL_SYN FROM QUALIFIED TO XML, CREATES SYNONYM PROPERTY
	multimap<long, OntyxProperty> conPropMap;
	multimap<long, OntyxProperty>::iterator pConProp;
	set<string> tmpPropSet;
	set<string>::iterator pTmpPropSet;
	set<string> oldPropSet;
	set<string> newPropSet;	

	string termTag = "term-name",
			typeTag = "term-group",
			sourceTag = "term-source",
			codeTag = "source-code",
			typeQual = "Syn_Term_Type",
			sourceQual = "Syn_Source",
			codeQual = "Syn_Source_Code",
			goIDTag = "GO_ID",
			goEviTag = "GO_Evidence",
			goSrcTag = "GO_Source",
			goDateTag = "GO_Source_Date";

	string defaultType = "SY";
	string defaultSource = "NCI";

	string value, tagVal, typestr, defstr, sourstr, codestr, datestr, newVal;

	bool good, hasSource, hasType; // hasQual, 
	cerr << "Converting full_syns and creating Synonyms" << endl;
	for( posCon = tdeKb.getFirstConceptIterator(); 
				posCon != tdeKb.getEndConceptIterator(); ++posCon) {
		tmpPropSet.clear();
		newPropSet.clear();
		good = false;
		conPropMap = posCon->second.getProperties();
		for( pConProp = conPropMap.begin(); pConProp != conPropMap.end(); ++pConProp) {
			if( pConProp->second.getName() == "FULL_SYN") {
				newVal = tagVal = typestr = sourstr = codestr = "";
				hasSource = hasType = false;					
				value = characterEscape(pConProp->second.getValue());
				tagVal = "<" + termTag + ">" + value + "</" + termTag + ">";
				if( !pConProp->second.hasQualifier() ) {
					typestr = "<" + typeTag + ">" + defaultType + "</" + typeTag + ">";
					sourstr = "<" + sourceTag + ">" + defaultSource + "</" + sourceTag + ">";
					}
				else {
					vector<OntyxQualifier> quals = pConProp->second.getQualifiers();
					vector<OntyxQualifier>::const_iterator pQuals = quals.begin();
					for( ; pQuals != quals.end(); ++pQuals ) {
						if( pQuals->getName() == typeQual ) {
							if( pQuals->getValue() == "AQS" )
								typestr = "<" + typeTag + ">" + "AQ" + "</" + typeTag + ">";
							else 
								typestr = "<" + typeTag + ">" + characterEscape(pQuals->getValue()) + "</" + typeTag + ">";
							hasType = true;
							}
						else if( pQuals->getName() == sourceQual ) {
							sourstr = "<" + sourceTag + ">" + characterEscape(pQuals->getValue()) + "</" + sourceTag + ">";
							hasSource = true;
							}
						else if( pQuals->getName() == codeQual )
							codestr = "<" + codeTag + ">" + characterEscape(pQuals->getValue()) + "</" + codeTag + ">";
						}
					if( !hasType )					
						typestr = "<" + typeTag + ">" + defaultType + "</" + typeTag + ">";
					if( !hasSource )
						sourstr = "<" + sourceTag + ">" + defaultSource + "</" + sourceTag + ">";

					}
				newVal = tagVal + typestr + sourstr + codestr;					
				newPropSet.insert(newVal);
				tmpPropSet.insert(value);
				}
			}
		pTmpPropSet = tmpPropSet.begin();								// synonyms for publication purpose only
		for( ; pTmpPropSet != tmpPropSet.end(); ++pTmpPropSet ) {
			OntyxProperty tmpProp("Synonym", *pTmpPropSet);
			posCon->second.addProperty(tmpProp);
			}
		posCon->second.deleteProperty("FULL_SYN");
		pTmpPropSet = newPropSet.begin();
		for( ; pTmpPropSet != newPropSet.end(); ++pTmpPropSet ) {
			OntyxProperty tmpProp("FULL_SYN", *pTmpPropSet);
			posCon->second.addProperty(tmpProp);
			}
		}
	// ************* END CONVERT FULLSYNS *************

	// ************* CONVERT GO ANNOTATION TO XML *************
	
	cerr << "Converting go_annotation to xml" << endl;
	for( posCon = tdeKb.getFirstConceptIterator(); 
				posCon != tdeKb.getEndConceptIterator(); ++posCon) {
		tmpPropSet.clear();
		oldPropSet.clear();
		newPropSet.clear();
		good = false;
		conPropMap = posCon->second.getProperties();
		for( pConProp = conPropMap.begin(); pConProp != conPropMap.end(); ++pConProp) {
			if( pConProp->second.getName() == "GO_Annotation") {
				good = true;
				value = pConProp->second.getValue();
				tagVal = "<go-term>" + value + "</go-term>";
				vector<OntyxQualifier> quals = pConProp->second.getQualifiers();
				vector<OntyxQualifier>::const_iterator pQuals = quals.begin();
				for( ; pQuals != quals.end(); ++pQuals ) {
					if( pQuals->getName() == goIDTag )
						codestr = "<go-id>" + pQuals->getValue() + "</go-id>";
					else if( pQuals->getName() == goSrcTag )
						sourstr = "<go-source>" + pQuals->getValue() + "</go-source>";
					else if( pQuals->getName() == goDateTag )
						datestr = "<source-date>" + pQuals->getValue() + "</source-date>";
					else if( pQuals->getName() == goEviTag )
						typestr = "<go-evi>" + pQuals->getValue() + "</go-evi>";
					}
				newVal = codestr + tagVal + typestr + sourstr + datestr;
				newPropSet.insert(newVal);
				}
			}
		if( good ) {
			posCon->second.deleteProperty("GO_Annotation");
			pTmpPropSet = newPropSet.begin();
			for( ; pTmpPropSet != newPropSet.end(); ++pTmpPropSet ) {
				OntyxProperty tmpProp("GO_Annotation", *pTmpPropSet);
				posCon->second.addProperty(tmpProp);
				}
			}
		}
	// ************* END CONVERT GO ANNOTATION TO XML *************


	// ************* START CONVERT DEFINITIONS *************
	string defTag = "def-definition",
			attrQual = "Definition_Attribution",
			attrTag = "attr";
	// unused for publication export Definition_Review_Date and Definition_Reviewer_Name

	sourceTag = "def-source";
	sourceQual = "Definition_Source";
	vector<OntyxProperty> vecProp;

	string attrstr, revnamestr, revdatestr;
	string propName;
	bool hassource, hasdef, hasattr, hasrevname, hasrevdate;
	
	multimap<long, OntyxProperty> conProps; 
	multimap<long, OntyxProperty>::iterator pProps;
	for( posCon = tdeKb.getFirstConceptIterator(); 
				posCon != tdeKb.getEndConceptIterator(); ++posCon) {
		conProps = posCon->second.getProperties();
		attrstr = sourstr = revdatestr = revnamestr = "";
		hasdef = false;
		vecProp.clear();
		pProps = conProps.begin();
		for( ; pProps != conProps.end(); ++pProps) {
			propName = pProps->second.getName();
			if( propName == "DEFINITION" || propName == "LONG_DEFINITION" || propName == "ALT_DEFINITION" || propName == "ALT_LONG_DEFINITION") {
				hasdef = true;
				attrstr = sourstr = revdatestr = revnamestr = "";
				hassource = hasattr = hasrevdate = hasrevname = false;
				value = characterEscape(pProps->second.getValue());
				tagVal = "<" + defTag + ">" + value + "</" + defTag + ">";
				newVal = tagVal;
				if( pProps->second.hasQualifier() ) {
					vector<OntyxQualifier> quals = pProps->second.getQualifiers();
					vector<OntyxQualifier>::const_iterator pQuals = quals.begin();
					for( ; pQuals != quals.end(); ++pQuals ) {
						if( pQuals->getName() == attrQual ) {
							attrstr = "<" + attrTag + ">" + characterEscape(pQuals->getValue()) + "</" + attrTag + ">";
							hasattr = true;
							}
						else if( pQuals->getName() == sourceQual ) {
							sourstr = "<" + sourceTag + ">" + characterEscape(pQuals->getValue()) + "</" + sourceTag + ">";
							hassource = true;
							}
						}
					}
				if( hassource )							
					newVal = sourstr + newVal;
				else								// missing source defaults to adding "NCI"
					newVal = "<" + sourceTag + ">NCI</" + sourceTag + ">" + newVal;
				if( hasattr )
					newVal = newVal + attrstr;
				OntyxProperty tmpProp(propName, newVal);
				vecProp.push_back(tmpProp);
				}
			}
		if( hasdef ) {
			posCon->second.deleteProperty("DEFINITION");
			posCon->second.deleteProperty("LONG_DEFINITION");
			posCon->second.deleteProperty("ALT_DEFINITION");
			posCon->second.deleteProperty("ALT_LONG_DEFINITION");
			posCon->second.addProperties(vecProp);
			}
		}
	// ************* END CONVERT DEFINITIONS *************



	string filename;
	string daystamp = getDayStamp();

	// ************* KIM's QA START *************
    OntyxKBQA *qa = new OntyxKBQA(tdeKb);
	string qa_outfile = "ThesaurusQA-" + daystamp + ".txt";
	qa->performQA(qa_outfile);

	delete qa;	// new line

	// ************* KIM's QA END *************

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


	// ************* WRITE ONTYLOG FILE *************
	filename = "Thesaurus-" + daystamp + ".xml";
	cerr << "Writing output ontylog file '" << filename << "', properties with role names are not exported." << endl;
	tdeKb.writeOntyxXMLFile(filename, false, true);
	// void OntyxKb::writeOntyxXMLFile (const string & filename, bool flag, bool noProps) // flag = true:renumber
	// if noProps is true, properties with names derived from roles are not exported
	// ************* END WRITE ONTYLOG FILE *************
	
    // ************* BEGIN ADD SILOS **************
    
   	long propID;
	char c_maxPropCode[22];
	string s_maxPropCode;
	
	map<long, OntyxPropertyDef>	NCIProps = tdeKb.getPropDefs();
	map<long, OntyxPropertyDef>::iterator pNCI = NCIProps.begin();
	propID = tdeKb.getMaxPropID();
	string name = "Syn2";
	string range = "long_string";
		if( !tdeKb.hasPropertyDef(name) ) {
			propID++;
			sprintf(c_maxPropCode, "%ld", propID);
			s_maxPropCode = c_maxPropCode;
			OntyxPropertyDef tmpPropDef(name, "P" + s_maxPropCode, propID, range, false, false, "NCI");
			tdeKb.addPropertyDef(tmpPropDef);
			}
			
	// COPY SYNONYM TO SYN2
	cerr << "Copying Synonyms and creating Syn2" << endl;
	for( posCon = tdeKb.getFirstConceptIterator(); 
				posCon != tdeKb.getEndConceptIterator(); ++posCon) {
		tmpPropSet.clear();
		oldPropSet.clear();
		newPropSet.clear();
		good = false;
		conPropMap = posCon->second.getProperties();
		for( pConProp = conPropMap.begin(); pConProp != conPropMap.end(); ++pConProp) {
			if( pConProp->second.getName() == "Synonym") {
					good = true;
					//string tmpFSS = pConProp->second.getValue();
					//newPropSet.insert(tmpFSS);
					//oldPropSet.insert(pConProp->second.getValue());
					tmpPropSet.insert(pConProp->second.getValue());
				}
			}
		if( good ) {
            const string conName = posCon->second.getName();
            OntyxProperty conNameProp("Syn2",conName);
            posCon->second.addProperty(conNameProp);
			pTmpPropSet = tmpPropSet.begin();
			for( ; pTmpPropSet != tmpPropSet.end(); ++pTmpPropSet ) {
				OntyxProperty tmpProp("Syn2", *pTmpPropSet);
				posCon->second.addProperty(tmpProp);
				}
			}
		}
            
	//  WRITE SILOS FILE 
	filename = "Silos-" + daystamp + ".xml";
	cerr << "Writing output ontylog file '" << filename << "', properties with role names are not exported." << endl;
	tdeKb.writeOntyxXMLFile(filename, false, true);
	
    //  REMOVE THE SYN2 PROPERTIES FOR FURTHER PROCESSING
    name = "Syn2";
    //OntyxPropertyDef tmpPropDef = tdeKb.getPropertyDef(name);
    tdeKb.removePropertyDef(name);
    for( posCon = tdeKb.getFirstConceptIterator(); 
	     posCon != tdeKb.getEndConceptIterator(); ++posCon) {
         posCon->second.deleteProperty(name);              
     }
    //  ************ END ADD SILOS  ***************
    
	// ************* WRITE PUBLICATION OWL FILE *************
	filename = "Thesaurus-" + daystamp + ".owl";
	cerr << "Writing output OWL file '" << filename << "', properties with role names are not exported." << endl;
	tdeKb.writeOWLFileByName (filename, "header.owl", true); 
	// if noProps is true, properties with names derived from roles are not exported
	// ************* END WRITE OWL FILE *************


	// ************* WRITE FLAT FILE *************
	filename = "Thesaurus-" + daystamp + ".txt";
	cerr << "Writing output flat file \"" << filename << "\"" << endl;
	ofstream outFile;
	outFile.open(filename.c_str());
	set<string> conSyns, conParents, conDefs;
	set<string>::iterator pStringSet;
	string prefName;
	string def;
	string startTag = "<def-definition>";
	string endTag = "</def-definition>";
	for( posCon = tdeKb.getFirstConceptIterator(); 
				posCon != tdeKb.getEndConceptIterator(); ++posCon) {
		conSyns.clear();
		conParents.clear();
		conDefs.clear();
		conProps = posCon->second.getProperties();
		conParents = posCon->second.getParents();
		pProps = conProps.begin();
		for( ; pProps != conProps.end(); ++pProps) {
			if( pProps->second.getName() == "Synonym")
				conSyns.insert(pProps->second.getValue());
			if( pProps->second.getName() == "DEFINITION") {
				def = pProps->second.getValue();
				if( def.find("<def-source>MSH",0) == string::npos )
					conDefs.insert( extractFromMarkup(def, startTag, endTag));
				}
			}
		outFile << posCon->second.getCode()  << '\t' << posCon->second.getName() << '\t' ;
		if( conParents.size() != 0 ) {
			pStringSet = conParents.begin();
			outFile << *pStringSet;
			for( ++pStringSet ; pStringSet != conParents.end(); ++pStringSet)
				outFile << '|' << *pStringSet;
			}
		else
			outFile << "root_node" ;
		outFile << '\t' ;
		if( (prefName = posCon->second.getPropertyValue("Preferred_Name")) != OntyxTerminology::emptyString )
			outFile << prefName ;
		else
			outFile << posCon->second.getName() ; 
		if( conSyns.size() != 0 ) {
			pStringSet = conSyns.begin();
			for( ; pStringSet != conSyns.end(); ++pStringSet) {
				if( *pStringSet != prefName )
					outFile << '|' << *pStringSet;
				}
			}
		if( conDefs.size() != 0 )
			outFile << '\t' << *(conDefs.begin());
		outFile << endl;
		}
	outFile.close();
	// ************* END WRITE FLAT FILE *************

    

	
	
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



