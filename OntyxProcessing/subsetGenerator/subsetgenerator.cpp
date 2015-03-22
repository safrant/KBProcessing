/*
#######################################
# Subset report generator
#
# Requires TDEByName.xml and config.file
# of type:
# concept\hierarchy\t
# association\t
# assoc_value\t
# termsource\t
# defsource\t
# reportname.txt\n
#######################################
*/

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
#include "OntyxQualifier.h"
#include "tString.h"

using namespace std;

typedef struct comFlags {
	string	file1, file2, file3, file4, file5, file6;
    bool	changeSet;
};

typedef struct reportParams {
	string repType, assoc, assocValue, termSource, defSource, reportName;
	bool   valid;
};

void processCommandline(const vector<string> &, comFlags &); 		// there will be 2 arguments, one for XML and one for config.file
void readConfig(const string &, set<string> &); 					// passes the name of the file, followed by a set<string> to be returned
void tokenize(const string &, vector<string> &, const string &); 	// tokenize a line of input
bool valid(const vector<string> &, const OntyxKb &, reportParams &);					// validate input

int main (int argc, char * argv [] )
{
	OntyxKb refKb, outKb;
	vector<string> commLine;
    
	for ( int i = 0; i < argc; i++)
    	commLine.push_back(*argv++);

	comFlags flags;
	flags.file1 = "";
	flags.file2 = "";

	reportParams params;

	//ensure proper input
	if( argc != 3 )
	{
		cerr << "Error:  incorrect number of command line parameters." << endl
			<< "Usage: subset\ttde.xml\tconfig.file" << endl;
		exit(0);
	}

	// read TCL
	processCommandline( commLine, flags);

	// read TDEByName.xml
	refKb.readOntyxXMLFile( flags.file1 );
	cerr << "Read \"" << flags.file1 << "\"" << endl;

	// read config.file
	set<string> inputLines;
	readConfig( flags.file2, inputLines );

	// initialize and set pointer
	set<string>::iterator inputPos;
	inputPos = inputLines.begin();

	while( !inputPos->empty() )
	{
		vector<string> s1;
		tokenize( *inputPos, s1, "" );				// use tString tokenizer

		if( valid( s1, refKb, params ) )			// validate and set report parameters
		{
			set<string> targetConcept;
			if( params.repType == "concept" )   {
				targetConcept.insert(params.assocValue);
			}
			else if( params.repType == "hierarchy" ) {
				refKb.findChildren (params.assocValue, targetConcept);
				targetConcept.insert(params.assocValue);
			}
		}
		else 
			cerr << "ERROR:  " << params.repType << " is not a valid report type." << endl;


		set<string>::iterator posChild;
		multimap<long, OntyxConcept>::iterator posCon;
		for( posCon = refKb.getFirstConceptIterator(); posCon != refKb.getEndConceptIterator();
				++posCon ) {
                             // UNTESTED, no DataStucture implemented...needs review.
                             // get all properties
                             // for each property
                             //     if qualifier value of Syn_Source == defSource {
                             //          skip to associations, record them
                             //     }
                             //     else {
                             //          do nothing;
                             //     }
         }
		inputPos++;
		params.valid = false;
     }
}


void processCommandline (const vector<string> & commLine, struct comFlags & flags)
{
     for( unsigned i = 1; i < commLine.size(); i++) {
		if( flags.file1 == "" )
			flags.file1 = commLine[i];
		else
			flags.file2 = commLine[i];
      }
}

void readConfig (const string & inFilename, set<string> & term_list)
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

void tokenize(const string & str, vector<string> & tokens, const string& delimiters = "\t" )
{
     // Skip delimiters at beginning
     string::size_type lastPos =  str.find_first_not_of(delimiters, 0);
     // Find first "non-delimiter"
     string::size_type pos =      str.find_first_of(delimiters, lastPos);

     while( string::npos != pos || string::npos != lastPos )
     {
       // Found a token, add it to the vector
       tokens.push_back( str.substr(lastPos, pos - lastPos));
       // Skip delimiters
       lastPos = str.find_first_not_of(delimiters, pos);
       // Find next "non-delimiter"
       pos = str.find_first_of(delimiters, lastPos);
     }
}

bool valid( const vector<string> & s1, const OntyxKb & refKb, reportParams & params )
{
	params.repType     = s1[0];				// concept || hierarchy
	params.assoc       = s1[1];				// hasAssociationDef(params.assoc) == true
	params.assocValue  = s1[2];				// conceptNameInKB(params.assocValue)...
	params.termSource  = s1[3];				// valid source in a picklist
	params.defSource   = s1[4];				// valid source in a picklist
	params.reportName  = s1[5];
	//params.propertyName = something;  	// an optional property of interest to the user

	if( params.repType != "concept" || params.repType != "hierarchy" ) return false;
	if( !refKb.hasAssociationDef( params.assoc ) ) return false;
	if( !refKb.conceptNameInKB( params.assocValue) ) return false;
//	if( !refKb.stringInPickList( params.termSource ) ) return false;
//	if( !refKb.OntyxQualifierDef::stringInPickList( params.defSource ) ) return false;
	
	return true;
}


