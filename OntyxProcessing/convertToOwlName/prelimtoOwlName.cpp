

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

	OntyxKb masterKb;		// tde kb

int main (int argc, char * argv[])
{
	clock_t tstart, tend;
	tstart = clock();

	vector<string> commLine;
	for ( int i = 0; i < argc; i++)
		commLine.push_back(*argv++);

	if( commLine.size() != 2 ) {
		cerr << "Error:  incorrect number of command line parameters." << endl
			<< "\nUsage:\n\tprelimtoOwlName filename" << endl << endl;
		exit(1);
		}


 
	masterKb.readOntyxXMLFile( commLine[1] );	cerr << "Read \"" << commLine[1].c_str() << "\"" << endl;


	// KB concept iterator, used in all the processing below
	map<long, OntyxConcept>::iterator posCon;
	multimap<long, OntyxRole> roles, tmpDelRoles, tmpNewRoles;
	multimap<long, OntyxRole>::iterator pRoles;

	string all = "all";
	long roleCount;
	for( posCon = masterKb.getFirstConceptIterator(); 
				posCon != masterKb.getEndConceptIterator(); ++posCon) {
		if( posCon->second.hasRole("Disease_is_Stage") ) {
			roleCount = 0;
			tmpDelRoles.clear();
			tmpNewRoles.clear();
			roles = posCon->second.getRoles();
			pRoles = roles.begin();
			for( ; pRoles != roles.end(); ++pRoles ) {
				if( pRoles->second.getName() == 	"Disease_is_Stage" && pRoles->second.getModifier() == "some" ) {
					OntyxRole tmpDRole("Disease_is_Stage", pRoles->second.getValue(), "some");
					tmpDelRoles.insert(multimap<long, OntyxRole>::value_type(roleCount, tmpDRole));
					OntyxRole tmpNRole("Disease_is_Stage", pRoles->second.getValue(), "all");
					tmpNewRoles.insert(multimap<long, OntyxRole>::value_type(roleCount, tmpNRole));
					cout << posCon->second.getName() << '\t' << roleCount << endl;
					roleCount++;
					}
				}
			pRoles = tmpDelRoles.begin();
			for( ; pRoles != tmpDelRoles.end(); ++pRoles )
				posCon->second.deleteRole(pRoles->second);
			pRoles = tmpNewRoles.begin();
			for( ; pRoles != tmpNewRoles.end(); ++pRoles )
				posCon->second.addRole(pRoles->second);
			}
		}


	cerr << "Writing output file" << endl;
	string outFilename = "pre-owl-" + commLine[1];
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
