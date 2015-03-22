

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
			<< "\nUsage:\n\treadNCIOBO obo_file" << endl << endl;
		exit(0);
		}
	else
		ProcessCommandline( commLine, flags);

	tstart = clock();





	ifstream inFile;									// VERIFY THAT FILES EXIST
	inFile.open( commLine[1].c_str() );
	if( inFile.bad() ) {
		cerr << "Can't open file '" << commLine[1].c_str() << "'" << endl;
		exit(0);
		}
	bool isTerm = false, hasTerm = false;
	string i_name = "name: ";
	string i_isa = "is_a: ";
	string i_rela = "relationship: ";
	string i_id = "id: ";
	string i_syn = "synonym: ";
	
	string c_name, code;
	long count = 0;
	vector<string> parents;
	vector<OntyxRole> relations;
	vector<OntyxProperty> synonyms;
	vector<string>::iterator posVec;

	OntyxName tmpName("nci", 1, "N1");
	tdeKb.addNamespace(tmpName);

	OntyxKind tmpKind("nci_anatomy", 1, "K1", "nci", false);
	tdeKb.addKind(tmpKind);

	OntyxRoleDef tmpRole("part_of", "R1", 1, "nci", "nci_anatomy", "nci_anatomy", "", "");
	tdeKb.addRoleDef(tmpRole);

	OntyxPropertyDef tmpProp("synonym", "P1", 1, "string", false, false, "nci");
	tdeKb.addPropertyDef(tmpProp);

	OntyxPropertyDef tmpProp1("Preferred_Name", "P2", 2, "string", false, false, "nci");
	tdeKb.addPropertyDef(tmpProp1);

	string inputLine;
	while( getline(inFile, inputLine), !inFile.eof()) {
		if( inputLine == "[Term]" ) {
			isTerm = true;
			count++;
			if( count > 1 ) {
				tString t_id = code;
				t_id.tokenize(":");
				//Convert GO:code to GO_code for rdf:iD.  Add second "code" property for GO:code
				OntyxConcept tmpConcept(c_name, code, atoi(t_id.getTokenNumber(1).c_str()), "nci", "nci_anatomy", false);
				tmpConcept.addParents(parents).addRoles(relations).addProperties(synonyms);
				tdeKb.addConcept(tmpConcept);

				relations.clear();
				synonyms.clear();
				parents.clear();
				hasTerm = false;
				}
			}
		else if( inputLine == "[Typedef]" || inFile.eof() ) {
			isTerm = false;
			if( hasTerm ) {
				tString t_id = code;
				t_id.tokenize(":");
				OntyxConcept tmpConcept(c_name, code, atoi(t_id.getTokenNumber(1).c_str()), "nci", "nci_anatomy", false);
				tmpConcept.addParents(parents).addRoles(relations).addProperties(synonyms);
				tdeKb.addConcept(tmpConcept);

				relations.clear();
				synonyms.clear();
				parents.clear();
				hasTerm = false;
				}
			}
		else if( inputLine.substr(0, i_name.size() ) == i_name && isTerm ) {
			string tmp(inputLine, i_name.size());
			OntyxProperty tmpProperty("Preferred_Name", tmp, "");
			synonyms.push_back(tmpProperty);
			hasTerm = true;
			}
		else if( inputLine.substr(0, i_id.size() ) == i_id && isTerm ) {
			string tmp(inputLine, i_id.size());
			code = tmp;
			tString t_id = code;
			t_id.tokenize(":");
			c_name = t_id.getTokenNumber(0) + "_" + t_id.getTokenNumber(1);
			}
		else if( inputLine.substr(0, i_isa.size() ) == i_isa && isTerm ) {
			tString tmpTStr = inputLine;
			tmpTStr.tokenize(" ");
			tString tmp2T = tmpTStr.getTokenNumber(1);
			tString p_id = tmp2T;
			p_id.tokenize(":");
			string reference = p_id.getTokenNumber(0) + "_" + p_id.getTokenNumber(1);
			parents.push_back(reference); //.getTokenSequence());
			}
		else if( inputLine.substr(0, i_rela.size() ) == i_rela && isTerm ) {
			tString tmpTStr = inputLine;
			tmpTStr.tokenize(" ");
			tString tmp2T = tmpTStr.getTokenNumber(2);
			tString p_id = tmp2T;
			p_id.tokenize(":");
			string reference = p_id.getTokenNumber(0) + "_" + p_id.getTokenNumber(1);
			OntyxRole tmpRole("part_of", reference, "some");
			relations.push_back(tmpRole);
			parents.push_back(reference); // .getTokenSequence());	// for the jax owl work, create isas out of roles
			}
		else if( inputLine.substr(0, i_syn.size() ) == i_syn && isTerm ) {
			tString tmpTStr = inputLine;
			tmpTStr.tokenize("\"");
			OntyxProperty tmpProperty("synonym", tmpTStr.getTokenNumber(1), "");
			synonyms.push_back(tmpProperty);
			}
		}
	inFile.close();


	string filename, daystamp = getDayStamp();


	// ************* WRITE OWL FILE *************
	filename = "NCI_Anatomy-" + daystamp + ".owl";
	cerr << "Writing output OWL file '" << filename << "', properties with role names are not exported." << endl;
	tdeKb.writeOWLFile (filename, "header.owl", true);
	string makecopy = "copy " + filename + " NCI_Anatomy.owl";
	system(makecopy.c_str()); 
	// if noProps is true, properties with names derived from roles are not exported
	// ************* END WRITE OWL FILE *************


	
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

void readFlatList(const string & inFilename, vector<string> & term_list)
{
	ifstream inFile;									// VERIFY THAT FILES EXIST
	inFile.open( inFilename.c_str() );
	if( inFile.bad() ) {
		cerr << "Can't open file '" << inFilename.c_str() << "'" << endl;
		exit(0);
		}

	string inputLine;
	while( getline(inFile, inputLine), !inFile.eof())
		term_list.push_back(inputLine);
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

