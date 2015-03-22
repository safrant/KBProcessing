 

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



	// ************* CONVERT FULLSYNS *************
	// CONVERT FULL_SYN FROM QUALIFIED TO XML
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
	cerr << "Converting full_syns" << endl;
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

	/* for conversion to protege */
	string reviewDateQual = "Definition_Review_Date";
	string reviewNameQual = "Definition_Reviewer_Name";
	string reviewDateTag = reviewDateQual;
	string reviewNameTag = reviewNameQual;

	string propName;

	sourceTag = "def-source";
	sourceQual = "Definition_Source";
	vector<OntyxProperty> vecProp;

	string attrstr, revnamestr, revdatestr;
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
			if( propName == "DEFINITION" || propName == "LONG_DEFINITION" || propName == "ALT_DEFINITION" || propName == "ALT_LONG_DEFINITION" ) {
				if( propName == "LONG_DEFINITION" )
					propName = "DEFINITION";
				else if( propName == "ALT_LONG_DEFINITION" )
					propName = "ALT_DEFINITION";
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
						else if( pQuals->getName() == reviewDateQual ) {
							revdatestr = "<" + reviewDateTag + ">" + characterEscape(pQuals->getValue()) + "</" + reviewDateTag + ">";
							hasrevdate = true;
							}
						else if( pQuals->getName() == reviewNameQual ) {
							revnamestr = "<" + reviewNameTag + ">" + characterEscape(pQuals->getValue()) + "</" + reviewNameTag + ">";
							hasrevname = true;
							}
						}
					}
				if( hassource )							
					newVal = sourstr + newVal;
				else
					newVal = "<" + sourceTag + ">NCI</" + sourceTag + ">" + newVal;
				if( hasattr )								// missing source defaults to adding "NCI"
					newVal = newVal + attrstr;
				if( hasrevdate )
					newVal = newVal + revdatestr;
				if( hasrevname )
					newVal = newVal + revnamestr;
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




	// ************* WRITE BY NAME OWL FILE *************
	filename = "Thesaurus-ByName-" + daystamp + ".owl";
	cerr << "Writing output OWL file '" << filename << "', properties with role names are not exported." << endl;
	tdeKb.writeOWLFileByName (filename, "header.owl", true); 
	// if noProps is true, properties with names derived from roles are not exported
	// ************* END WRITE OWL FILE *************


	// ************* WRITE BY CODE OWL FILE *************
	filename = "Thesaurus-ByCode-" + daystamp + ".owl";
	cerr << "Writing output OWL file '" << filename << "', properties with role names are not exported." << endl;
	tdeKb.writeOWLFile (filename, "headerCode.owl", true); 
	// if noProps is true, properties with names derived from roles are not exported
	// ************* END WRITE OWL FILE *************




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





