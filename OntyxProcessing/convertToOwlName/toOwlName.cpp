

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <algorithm>
#include <ctime>
#include "tString.h"

#include "OntyxKb.h"

using namespace std;


string xName( const string & inString );
string revertXName( const string & inString );
string cleanTextForInsertion(const string & inString);
void retireConceptsInKb (OntyxKb & inKb, const string & filename, OntyxKb & parentKb);
void moveConceptsFromToKb (const string & conName, OntyxKb & fromKb, OntyxKb & toKb);

	OntyxKb masterKb,		// tde kb
			tempKb;

int main (int argc, char * argv[])
{
	clock_t tstart, tend;
	tstart = clock();

	vector<string> commLine;
	for ( int i = 0; i < argc; i++)
		commLine.push_back(*argv++);

	if( commLine.size() != 2 && commLine.size() != 4 ) {
		cerr << "Error:  incorrect number of command line parameters." << endl
			<< "\nUsage:\n\t file [-f name_map.txt]" << endl << endl;
		exit(1);
		}


	masterKb.readOntyxXMLFile( commLine[1] );	cerr << "Read \"" << commLine[1].c_str() << "\"" << endl;

	// KB concept iterator, used in all the processing below
	map<long, OntyxConcept>::iterator posCon;

/*	for( posCon = masterKb.getFirstConceptIterator(); 
				posCon != masterKb.getEndConceptIterator(); ++posCon) {
		if( posCon->second.getName() != xName(posCon->second.getName()) )
			cerr << "'" << posCon->second.getName() << "' is malformed" << endl;
		}
*/

	// construct renamePairs with the user-selected names
	map<string, string> renamePairs, renamePairs2;
	set<string> ncname;
	string input_mapfile,
			inputLine;
	bool foundflag = false;
	if( commLine.size() == 4 ) {
		unsigned i = 0;
		for( ; i < commLine.size(); ++i ) {
			if( commLine[i] == "-f" || commLine[i] == "-F" ) {
				if( i+1 < commLine.size() ) {
					input_mapfile = commLine[i+1];
					foundflag = true;
					break;
					}
				}
			}
		if( !foundflag ) {
			cerr << "Error:  incorrect number of command line parameters; -f switch not found." << endl
				<< "\nUsage:\n\t file [-f name_map.txt]" << endl << endl;
			exit(2);
			}
		ifstream mapFile;
		mapFile.open(input_mapfile.c_str());
		string tmp1S, tmp2S;
		set<string> ncname;
		tString tokstring;
		if( mapFile.good() ) {
			while( getline(mapFile, inputLine), !mapFile.eof() ) {
				tokstring = inputLine;
				tokstring.tokenize("\t");
				tmp1S = tokstring.getToken();
				if( tokstring.nextToken() )	{	// it better have a next token!
					tmp2S = xName(tokstring.getToken());
					if( tmp2S[tmp2S.size()-1] == '_' && ncname.count(tmp2S.substr(0, tmp2S.size()-1)) == 0 ) 				// strip trailing underscore
						tmp2S = tmp2S.substr(0, tmp2S.size()-1);
					}
				else {
					cerr << "second token missing for '\t" << inputLine << "', will exit." << endl;
					exit(3);
					}
				if( ncname.count(tmp2S) == 0 					// the suggested ncname is not a duplicate
					&& masterKb.conceptNameInKB(tmp1S) ) {		// and the name exists in kb
					renamePairs.insert(map<string, string>::value_type(tmp1S, tmp2S));
					ncname.insert(tmp2S);
					}
				}
			mapFile.close();
			}
		}
 
	// construct a set of the existing concept names
	string name = "", newName, tmpString;

	set<string> conceptSet;
	for( posCon = masterKb.getFirstConceptIterator(); 
				posCon != masterKb.getEndConceptIterator(); ++posCon) {
		name = posCon->second.getName();
		if( conceptSet.count(name) > 0 )
			cerr << "ERROR: name = " << name << " is duplicated." << endl;
		else
			conceptSet.insert(name);
		}


	// construct the actual mapping table renamePairs2 for the renaming
	renamePairs2 = renamePairs;
	for( posCon = masterKb.getFirstConceptIterator(); 
				posCon != masterKb.getEndConceptIterator(); ++posCon) {
		name = posCon->second.getName();
		newName = xName(name);
		if( newName[newName.size()-1] == '_' ) 				// strip trailing underscore
			newName = newName.substr(0, newName.size()-1);
		if( ncname.count(newName) > 0 || conceptSet.count(newName) > 0) {	// check ncname is unique, modify as necessary
			while( ncname.count(newName) > 0 || conceptSet.count(newName) > 0 )
				newName += "_";
			}
		// see if it has been entered already
		if( renamePairs2.count(name) == 0 ) {				// ok, it hasn't been entered yet
			if( newName != name ) {							// enter it in the map if ncname differs from name
				renamePairs2.insert(map<string, string>::value_type(name, newName));
				ncname.insert(newName);
				}
			}
		}
	cerr << "done writing map" << endl;

	map<string, string>::iterator posString = renamePairs2.begin();
	for( ; posString != renamePairs2.end(); ++posString ) {
		if( !masterKb.conceptNameInKB(posString->first) )
			cerr << posString->first << " doesn't exist in kb" << endl;
		if( masterKb.getIDFromName(posString->first) == -1 )
			cerr << posString->first << " has nonexistent ID in kb" << endl;
//		if( posString->second != xName(posString->second) )
//			cerr << posString->second << " has non-ncname characters" << endl;
		}

	// output table of old vs new names
	ofstream outFile;
	outFile.open("name_map.txt");
	if( outFile.good() ) {
		string tmpPT;
		bool ptNameFlag = false;
		bool isReversible = false;
		posString = renamePairs2.begin();
		for( ; posString != renamePairs2.end(); ++posString ) {
			tmpPT = masterKb.getConcept(posString->first).getPropertyValue("Preferred_Name");
			if( tmpPT == OntyxTerminology::emptyString )
				tmpPT = "NO PREFERRED NAME STATED";
			if( tmpPT == masterKb.getConcept(posString->first).getName() )
				ptNameFlag = true;
			else
				ptNameFlag = false;
			if( revertXName(posString->second) == posString->first )
				isReversible = true;
			else
				isReversible = false;
			outFile << masterKb.getConcept(posString->first).getKind() << '\t' << tmpPT << '\t' << ptNameFlag << '\t'
				<< posString->first << '\t' << posString->second << '\t' << isReversible << endl;
			}
		}


	outFile.close();

	cerr << "concept renaming" << endl;



	masterKb.renameConcept(renamePairs2);
/*
	cerr << "retiring the mouse stuff" << endl;
	// retire the mouse stuff
	moveConceptsFromToKb ("Retired_Organism_Concepts", masterKb, tempKb);
	// but remember to not retire top node, written into the routine
	retireConceptsInKb (tempKb, "second_B_mouse_retires.txt", masterKb);
	// then bring back to main kb
	moveConceptsFromToKb ("Retired_Organism_Concepts", tempKb, masterKb);
*/

	cerr << "Writing output file" << endl;
	string outFilename = "owl-" + commLine[1];
	masterKb.writeOntyxXMLFile(outFilename, false, false);


	tend = clock();
	cerr << "Execution took " << (tend-tstart)/1000.0 << " seconds" << endl;

	return 0;
}



string xName( const string & inString )
{
 
	unsigned i;
	string tmpString, 
			tmpInString = inString, 
			outString = "";
	string plus = "+", 
			plus_string = "_plus_";
	outString.reserve(256);

	// replace '+' character with "plus" string
	unsigned startpos;
//	tmpInString = inString;
	if( tmpInString.find("+") != string::npos ) {
		while( (startpos = tmpInString.find(plus, 0)) != string::npos )
			tmpInString.replace(startpos, plus.size(), plus_string);
		}
	// prefix string with an underscore if first character is illegal
	if( !isalpha(tmpInString[0]) && tmpInString[0] != '_' )
		tmpString = "_";
	// eliminate illegal characters, string will become a xName
	for( i = 0; i < tmpInString.size(); ++i ) {
		if( isalnum(tmpInString[i]) || tmpInString[i] == '_' || tmpInString[i] == '-')
			tmpString.push_back(tmpInString[i]);
		else
			tmpString.push_back('_');
		}
	// "beautify" xName now
	outString.push_back(tmpString[0]);
	for( i = 1; i < tmpString.size(); ++i ) {
		if( tmpString[i] != '_' && tmpString[i] != '-' )
			outString.push_back(tmpString[i]);
		else if( tmpString[i] == '_' && tmpString[i-1] != '_' && tmpString[i-1] != '-')
			outString.push_back(tmpString[i]);
		else if( tmpString[i] == '-' && tmpString[i-1] != '_' && tmpString[i-1] != '-')
			outString.push_back(tmpString[i]);
		}
	return outString;
}

string revertXName( const string & inString )
{
	// converts '_' to space ' ', the dash and period are left untouched
	// no trimming as leading underscores might be of interest
	string outString = "";
	unsigned i;
	for( i = 0; i < inString.size(); ++i ) {
		if( inString[i] == '_' )
			outString.push_back(' ');
		else
			outString.push_back(inString[i]);
		}
	return outString;
}

void retireConceptsInKb (OntyxKb & inKb, const string & filename, OntyxKb & parentKb) 
{
/*	the parentKb is just to find the parents of the root nodes sent to process in inKb
	as the sent root nodes in inKb are not necessarily roots in parentKb

	store children names in OLD_CHILD property
	store role name|role value in OLD_ROLE
	store kind in OLD_KIND, set kind to "Retired_Kind"
	if concept is defined, make it primitive and store Defined in OLD_STATE

	store parent names in OLD_PARENT, need to do alongside retirement script, e.g. for concept_history: 
		insert into concept_history(concept, editaction, editdate, reference)
		values ('Cxxx', 'modify', '18-JUN-03', null);

	insert into evs_history(CONCEPTCODE, CONCEPTNAME, ACTION, EDITNAME, HOST, REFERENCECODE, PUBLISHED)
	"values ('" + posCon->second.getCode() + "', '" + posCon->second.getName() +
	", 'Retire', 'gilberto', '6116-fragosog-1.nci.nih.gov', " + need reference code of parent + "', 0);";
*/
	string tmpString;
	map<long, OntyxConcept>::iterator posCon;

	multimap<long, OntyxRole> lrole;
	multimap<long, OntyxRole>::iterator pRole;

	multimap<long, OntyxProperty> lproperty;
	multimap<long, OntyxProperty>::iterator pProperty;

	set<string> lchildren;
	set<string>::iterator pChildren;

	posCon = inKb.getFirstConceptIterator();
	for( ; posCon != inKb.getEndConceptIterator(); ++posCon) {
		if( posCon->second.getName() == "Retired_Organism_Concepts" )
			continue;
		lchildren.clear();
		inKb.findChildren(posCon->second, lchildren);
		pChildren = lchildren.begin();
		for( ; pChildren != lchildren.end(); ++pChildren) {
			inKb.getConcept(*pChildren).getName();
			OntyxProperty tmpProp("OLD_CHILD", inKb.getConcept(*pChildren).getName(),"");
			posCon->second.addProperty(tmpProp);
			}

		lrole = posCon->second.getRoles();
		pRole = lrole.begin();
		for( ; pRole != lrole.end(); ++pRole) {
			tmpString = pRole->second.getName() + "|" + pRole->second.getValue();
			OntyxProperty tmpProp("OLD_ROLE", tmpString,"");
			posCon->second.addProperty(tmpProp);
			}
		posCon->second.deleteAllRoles();

		OntyxProperty tmpProp("OLD_KIND", posCon->second.getKind(),"");
		posCon->second.addProperty(tmpProp);
		posCon->second.setKind("Retired_Kind");

		if( posCon->second.getState() ) {
			OntyxProperty tmpProp("OLD_STATE", "Defined","");
			posCon->second.addProperty(tmpProp);
			posCon->second.setPrimitive();
			}
		}

	// have to process in two steps, since "children" are derived from the parents table, modifying parents
	// would have corrupted the children

	// second iteration of routine, had to be modified to deal with the "pre-retirements" that were done
	// in mouse strains without the double-treeing as I had requested
	ofstream outFile;
	outFile.open(filename.c_str());
	outFile << "SET FEEDBACK OFF;" << endl
			<< "CREATE TRIGGER my_evs_trig_history" << endl
			<< "BEFORE INSERT ON evs_history" << endl
			<< "FOR EACH ROW when (new.historyid is null)" << endl
			<< "BEGIN" << endl
			<< "SELECT evs_history_id_seq.nextval INTO :new.historyid FROM dual;" << endl
			<< "END;" << endl
			<< "/" << endl;
	set<string> lparent;
	set<string>::iterator pParent;
	string tmpCode;
	posCon = inKb.getFirstConceptIterator();
	for( ; posCon != inKb.getEndConceptIterator(); ++posCon) {
		if( posCon->second.getName() == "Retired_Organism_Concepts" )
			continue;
		lparent.clear();				// need to do this for the situation found retiring cons during ncname conversion
		tmpCode = posCon->second.getCode();
		lparent = posCon->second.getParents();
		pParent = lparent.begin();
		if( *pParent == "Retired_Organism_Concepts" ) {
			posCon->second.deleteParent("Retired_Organism_Concepts");
			lproperty = posCon->second.getProperties();
			pProperty = lproperty.begin();
			for( ; pProperty != lproperty.end(); ++pProperty) {
				if( pProperty->second.getName() == "OLD_PARENT" ) {
					if( parentKb.conceptNameInKB(pProperty->second.getValue()) )
						tmpString = parentKb.getConcept(pProperty->second.getValue()).getCode();
					else if( inKb.conceptNameInKB(pProperty->second.getValue()) )
						tmpString = inKb.getConcept(pProperty->second.getValue()).getCode();
					else
						tmpString = "";
					outFile << "insert into evs_history(CONCEPTCODE, CONCEPTNAME, ACTION, EDITNAME, HOST, REFERENCECODE, PUBLISHED)" << endl
							<< "values ('" << tmpCode << "', '" << cleanTextForInsertion(posCon->second.getName())
							<< "', 'Retire', 'gilberto', '6116-fragosog-1.nci.nih.gov', '" << tmpString << "', 0);" << endl;
					}
				}
			}
		else {
			for( ; pParent != lparent.end(); ++pParent) {
				OntyxProperty tmpProp("OLD_PARENT", *pParent,"");
				posCon->second.addProperty(tmpProp);
				posCon->second.deleteParent(*pParent);
				if( inKb.conceptNameInKB(*pParent) )
					tmpString = inKb.getConcept(*pParent).getCode();
				else
					tmpString = parentKb.getConcept(*pParent).getCode();
				outFile << "insert into evs_history(CONCEPTCODE, CONCEPTNAME, ACTION, EDITNAME, HOST, REFERENCECODE, PUBLISHED)" << endl
						<< "values ('" << tmpCode << "', '" << cleanTextForInsertion(posCon->second.getName())
						<< "', 'Retire', 'gilberto', '6116-fragosog-1.nci.nih.gov', '" << tmpString << "', 0);" << endl;
				}
			}
		posCon->second.addParent("Retired_Concepts");
		}
	outFile << "commit;" << endl
			<< "DROP TRIGGER my_evs_trig_history;" << endl;
	outFile.close();
}

string cleanTextForInsertion(const string & inString)
// careful, haven't tested to see if startPos > tmpString.size() breaks find
{
	string tmpString = inString;
	if( tmpString.find("'") != string::npos ) {
		cerr << "concept name string modified for db insertion: " << tmpString << endl;
		unsigned startPos = 0;
		while ( (startPos = tmpString.find("'",startPos)) != string::npos ) {
			tmpString = tmpString.replace(startPos, 1, "''");
			startPos +=2;
			}
		}
	return tmpString;
}


void moveConceptsFromToKb (const string & conName, OntyxKb & fromKb, OntyxKb & toKb)
{
	set<long> to_move;
	long l_unneedID = fromKb.getIDFromName(conName);
	fromKb.findDescendants(l_unneedID, to_move);
	fromKb.moveSetConceptsTo(to_move, toKb);

}

