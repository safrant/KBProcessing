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

OntyxKb tdeKb, compareKb;		// input/output kb

typedef struct ctrmSyn {
  string term;
  string type;
};

typedef struct comFlags {
	string	file1, file2, file3, file4, file5, file6;
	bool	changeSet;
};

void ProcessCommandline (const vector<string> & commLine, comFlags & );
void readFlatList(const string &, set<string> & term_list);
void reportAlphaStemMatch (OntyxKb & fromKb, OntyxKb & toKb, set<long> & unmatchedIDs);
bool hasCtrm(map<long, OntyxConcept>::iterator &);
bool hasNciPt(map<long, OntyxConcept>::iterator &);
vector<ctrmSyn> getCtrmSyns(map<long, OntyxConcept>::iterator &);

int main (int argc, char * argv[])
{

	clock_t tstart, tend;

	vector<string> commLine;
	for ( int i = 0; i < argc; i++)
		commLine.push_back(*argv++);

	comFlags flags;
	flags.file1 = "";
	flags.file2 = "";
	if( commLine.size() != 2 ) {
		cerr << "Error:  incorrect number of command line parameters." << endl
			<< "\nUsage:\n\tctrmReport tde.xml" << endl
			<< endl << "Requires ancillary branch_to_delete file."  << endl;
		exit(0);
		}
	else
		ProcessCommandline( commLine, flags);

	tstart = clock();

	tdeKb.readOntyxXMLFile( flags.file1 );	cerr << "Read \"" << flags.file1 << "\"" << endl;

	set<string>::iterator posDelete;

	// file of branches to exclude
	set<string> branch_delete;
	readFlatList("branch_to_delete.TXT", branch_delete);

	cout << "TDE file read -----------------------------------------------------------------" << endl;


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

   cout << "Code\tConcept Name\tCTRM Term\tCTRM Term Type\tNCI PT?\tMatching NCI Terms" << endl;

   map<long, OntyxConcept>::iterator posCon;
   for( posCon = tdeKb.getFirstConceptIterator(); posCon != tdeKb.getEndConceptIterator(); ++posCon ) {
     if( hasCtrm(posCon) ) {
       vector<ctrmSyn> ctrmSyns = getCtrmSyns(posCon);
       vector<ctrmSyn>::iterator syns;
       bool hasNci = hasNciPt(posCon);

       syns = ctrmSyns.begin();
       while( syns != ctrmSyns.end() ) {
         ctrmSyn syn = *syns;
         cout << posCon->second.getCode() << "\t" << posCon->second.getName();
         cout << "\t" << syn.term << "\t" << syn.type;
         if( !hasNci ) {
           cout << "\tNO NCI PT";
           OntyxKb flatKb;
           ofstream kbFlatFile;
	   string kbflatfilename = "flat.txt";
           kbFlatFile.open(kbflatfilename.c_str());
           if( kbFlatFile.bad() ) {
               cerr << "ERROR opening kb output file, exiting." << endl << endl;
               exit(0);
           }
           kbFlatFile << syn.term << endl;
           kbFlatFile.close();
           flatKb.readFlatFile( kbflatfilename, "FLAT-NAMESPACE", "Flat_Kind", "Flat_Type", "Flat Parent", "FLATSOURCE" );

           set<long> unmatchedID;
           reportAlphaStemMatch( flatKb, tdeKb, unmatchedID );
         }
         else
             cout << endl;
         ++syns;
       }
     }
   }



   tend = clock();
   cerr << "Execution took " << (tend-tstart)/1000.0 << " seconds" << endl;
   return 0;
}

vector<ctrmSyn> getCtrmSyns( map<long, OntyxConcept>::iterator & posCon ) {
     multimap<long, OntyxProperty> properties;
     multimap<long, OntyxProperty>::iterator posProp;
     vector<OntyxQualifier> qualifiers;
     vector<OntyxQualifier>::iterator posQual;

     properties = posCon->second.getProperties();
     vector<ctrmSyn> v;
     for( posProp = properties.begin(); posProp != properties.end(); ++posProp ) {
       if( posProp->second.getName() == "FULL_SYN" && posProp->second.hasQualifier() ) {
         qualifiers = posProp->second.getQualifiers();
         ctrmSyn tmpSyn;
         tmpSyn.term = "";
         tmpSyn.type = "";
         for( posQual = qualifiers.begin(); posQual != qualifiers.end(); ++posQual ) {
           if( posQual->getName() == "Syn_Source"  && posQual->getValue() == "CTRM" )
               tmpSyn.term = posProp->second.getValue();
           if( posQual->getName() == "Syn_Term_Type" )
               tmpSyn.type = posQual->getValue();
         }
         if( tmpSyn.term != "" && tmpSyn.type != "" ) {
           v.push_back( tmpSyn );
         }
       }
     }
     return v;
}

bool hasNciPt( map<long, OntyxConcept>::iterator & posCon ) {
     multimap<long, OntyxProperty> properties;
     multimap<long, OntyxProperty>::iterator posProp;
     vector<OntyxQualifier> qualifiers;
     vector<OntyxQualifier>::iterator posQual;

     properties = posCon->second.getProperties();
     for( posProp = properties.begin(); posProp != properties.end(); ++posProp ) {
       if( posProp->second.getName() == "FULL_SYN" && posProp->second.hasQualifier() ) {
         bool hasSourceNCI = false;
         bool isPt = false;
         qualifiers = posProp->second.getQualifiers();
         for( posQual = qualifiers.begin(); posQual != qualifiers.end(); ++posQual ) {
           if( posQual->getName() == "Syn_Source" && posQual->getValue() == "NCI" )
               hasSourceNCI = true;
           if( posQual->getName() == "Syn_Term_Type" && posQual->getValue() == "PT" )
               isPt = true;
         }
         if( hasSourceNCI && isPt )
             return true;
       }
     }

     return false;
}

bool hasCtrm( map<long, OntyxConcept>::iterator & posCon ) {
     multimap<long, OntyxProperty> properties;
     multimap<long, OntyxProperty>::iterator posProp;
     vector<OntyxQualifier> qualifiers;
     vector<OntyxQualifier>::iterator posQual;

     properties = posCon->second.getProperties();
     for( posProp = properties.begin(); posProp != properties.end(); ++posProp ) {
       if( posProp->second.getName() == "FULL_SYN" && posProp->second.hasQualifier() ) {
         qualifiers = posProp->second.getQualifiers();
         for( posQual = qualifiers.begin(); posQual != qualifiers.end(); ++posQual ) {
           if( posQual->getName() == "Syn_Source" && posQual->getValue() == "CTRM" )
               return true;
         }
       }
     }
     return false;
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

void reportAlphaStemMatch (OntyxKb & fromKb, OntyxKb & toKb, set<long> & unmatchedIDs)
// does a merge if all terms match, or if name matches, returns set of unmatched fromKb IDs
{
	long matchResult;
	set<long> s_myID;
	set<long>::const_iterator pSmyID;
	map<long, OntyxConcept>::iterator posCon;
	long matchConcept = 0, totalConcept = 0;
	unmatchedIDs.clear();
	for( posCon = fromKb.getFirstConceptIterator(); 
				posCon != fromKb.getEndConceptIterator(); ++posCon) {
		totalConcept++;
		matchResult = fromKb.alphaStemMatchKB (toKb, posCon->second.getName(), s_myID);
		if( matchResult == 0 ) {  									// unambiguous match, rename
			matchConcept++;
			cout << "\tsingle-match\t"
				<< toKb.getConcept(*(s_myID.begin())).getCode() << '\t' 
				<< toKb.getConcept(*(s_myID.begin())).getName() << '\t'
				<< toKb.getConcept(*(s_myID.begin())).getKind() << endl;
			}
		else if( matchResult != 5 ) {								// merge, split, & unknown
			matchConcept++;
			cout << posCon->second.getName() << "\tmulti-match" ;
			pSmyID = s_myID.begin();
			for( ; pSmyID != s_myID.end(); ++pSmyID ) {
				cout << '\t' 
					<< toKb.getConcept(*pSmyID).getCode() << '\t'
					<< toKb.getConcept(*pSmyID).getName() << '\t'
					<< toKb.getConcept(*pSmyID).getKind();
				}
			cout  << endl;
			}
		else {																// find parent and tree
			cout << posCon->second.getName() << "\tno-match" << endl;
			}
		}
}
