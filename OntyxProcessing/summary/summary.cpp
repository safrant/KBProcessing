/*
	read two files, baseline1 and baseline2

	for baseline2:  report number of concepts, roles, properties, associations, qualifiers, kinds, and namespaces
	diff baseline2 - baseline1 for the above number
	diff above per kind, use counters
*/

#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <algorithm> 
#include <ctime>
#include <iterator>

#include "fullSyn.h"
#include "OntyxKb.h"

using namespace std;


bool checkMarkup (const string & inString, const string & startTag, const string & endTag);
void checkDiffs( vector<string> & diff, vector<string> & ent0, vector<string> & ent1);


int main (int argc, char * argv[])
{
	OntyxKb tdeKb0, tdeKb1;
	clock_t tstart, tend;
	string retired = "Retired_Kind";

//	long conCount0 = 0, roleCount0 = 0, propCount0 = 0, assocCount0 = 0, qualCount0 = 0, kindCount0 = 0, nameCount0 = 0;
//	long conCount1 = 0, roleCount1 = 0, propCount1 = 0, assocCount1 = 0, qualCount1 = 0, kindCount1 = 0, nameCount1 = 0;

	vector<string> commLine;
	for ( int i = 0; i < argc; i++)
		commLine.push_back(*argv++);

	if( commLine.size() < 2 || commLine.size() > 3 ) {  // i.e. it takes two or three files
		cerr << "Error:  incorrect number of command line parameters." << endl
			<< "\nUsage:\n\tsummary tde_file_final [tde_file_initial]" << endl 
			<< "\n output to summary-tde_file_final" << endl;
		exit(0);
		}

	tstart = clock();
	tdeKb0.readOntyxXMLFile( commLine[1] );	cerr << "Read \"" << commLine[1].c_str() << "\"" << endl;
	if( commLine.size() == 3 )
		tdeKb1.readOntyxXMLFile( commLine[2] ); cerr << "Read \"" << commLine[2].c_str() << "\"" << endl;
	cerr << "Files read -----------------------------------------------------------------" << endl;


	// KB concept iterator, used in all the processing below
	map<long, OntyxConcept>::iterator posCon;


	/* START OF THE BASIC STATISTICS SECTION */
	cerr << "Writing output file" << endl;
	ofstream outFile;
	string outfilename = "summary-" + commLine[1] + ".txt";
	outFile.open(outfilename.c_str());
	if( outFile.bad() ) {
		cerr << "ERROR opening output file, exiting." << endl << endl;
		exit(0);
		}
	outFile << "KB definition counts for file " << commLine[1] << endl;
	outFile << "concepts\t" << "namespaces\t" << "kinds\t" << "roles\t" << "properties\t" << "associations\t" << "qualifiers\t" << endl;
	outFile << tdeKb0.getNumConcepts() << '\t'
		<< tdeKb0.getNumNames() << '\t'
		<< tdeKb0.getNumKinds() << '\t'
		<< tdeKb0.getNumRoles() << '\t'
		<< tdeKb0.getNumProps() << '\t'
		<< tdeKb0.getNumAssocs() << '\t'
		<< tdeKb0.getNumQuals()
		<< endl << endl;

	if( tdeKb1.getNumConcepts() > 0 ) {
		outFile << "Diff counts of KB definitions for files " << commLine[1] << " minus " << commLine[2] << endl;
		outFile << "concepts\t" << "namespaces\t" << "kinds\t" << "roles\t" << "properties\t" << "associations\t" << "qualifiers\t" << endl;
		outFile << tdeKb0.getNumConcepts() - tdeKb1.getNumConcepts() << '\t'
			<< tdeKb0.getNumNames() - tdeKb1.getNumNames() << '\t'
			<< tdeKb0.getNumKinds() - tdeKb1.getNumKinds() << '\t'
			<< tdeKb0.getNumRoles() - tdeKb1.getNumRoles() << '\t'
			<< tdeKb0.getNumProps() - tdeKb1.getNumProps() << '\t'
			<< tdeKb0.getNumAssocs() - tdeKb1.getNumAssocs() << '\t'
			<< tdeKb0.getNumQuals() - tdeKb1.getNumQuals()
			<< endl << endl;

		// if there are differences in the non-concept definitions, output the new and the deleted defs
		vector<string> diff, ent0, ent1;
		vector<string>::iterator pEnt;

		// check/output property def differences
		ent0 = tdeKb0.getKbProps();
		ent1 = tdeKb1.getKbProps();
		checkDiffs( diff, ent0, ent1);  	// in ent0 not in ent1
		pEnt = diff.begin();
		if( diff.size() > 0 ) {
			outFile << "Properties in " << commLine[1] << " not in " << commLine[2] << endl;
			for( ; pEnt != diff.end(); ++pEnt ) {
				outFile << '\t' << *pEnt << endl;
				}
			outFile << endl;		
			}
		checkDiffs( diff, ent1, ent0);  	// in ent1 not in ent0
		pEnt = diff.begin();
		if( diff.size() > 0 ) {
			outFile << "Properties in " << commLine[2] << " not in " << commLine[1] << endl;
			for( ; pEnt != diff.end(); ++pEnt ) {
				outFile << '\t' << *pEnt << endl;
				}
			outFile << endl;		
			}
		// check/output kind def differences
		ent0.clear();
		ent1.clear();
		ent0 = tdeKb0.getKbKinds();
		ent1 = tdeKb1.getKbKinds();
		checkDiffs( diff, ent0, ent1);  	// in ent0 not in ent1
		pEnt = diff.begin();
		if( diff.size() > 0 ) {
			outFile << "Kinds in " << commLine[1] << " not in " << commLine[2] << endl;
			for( ; pEnt != diff.end(); ++pEnt ) {
				outFile << '\t' << *pEnt << endl;
				}
			outFile << endl;		
			}
		checkDiffs( diff, ent1, ent0);  	// in ent1 not in ent0
		pEnt = diff.begin();
		if( diff.size() > 0 ) {
			outFile << "Kinds in " << commLine[2] << " not in " << commLine[1] << endl;
			for( ; pEnt != diff.end(); ++pEnt ) {
				outFile << '\t' << *pEnt << endl;
				}
			outFile << endl;		
			}
		// check/output role def differences
		ent0.clear();
		ent1.clear();
		ent0 = tdeKb0.getKbRoles();
		ent1 = tdeKb1.getKbRoles();
		checkDiffs( diff, ent0, ent1);  	// in ent0 not in ent1
		pEnt = diff.begin();
		if( diff.size() > 0 ) {
			outFile << "Roles in " << commLine[1] << " not in " << commLine[2] << endl;
			for( ; pEnt != diff.end(); ++pEnt ) {
				outFile << '\t' << *pEnt << endl;
				}
			outFile << endl;		
			}
		checkDiffs( diff, ent1, ent0);  	// in ent1 not in ent0
		pEnt = diff.begin();
		if( diff.size() > 0 ) {
			outFile << "Roles in " << commLine[2] << " not in " << commLine[1] << endl;
			for( ; pEnt != diff.end(); ++pEnt ) {
				outFile << '\t' << *pEnt << endl;
				}
			outFile << endl;		
			}
		// check/output association def differences
		ent0.clear();
		ent1.clear();
		ent0 = tdeKb0.getKbAssocs();
		ent1 = tdeKb1.getKbAssocs();
		checkDiffs( diff, ent0, ent1);  	// in ent0 not in ent1
		pEnt = diff.begin();
		if( diff.size() > 0 ) {
			outFile << "Associations in " << commLine[1] << " not in " << commLine[2] << endl;
			for( ; pEnt != diff.end(); ++pEnt ) {
				outFile << '\t' << *pEnt << endl;
				}
			outFile << endl;		
			}
		checkDiffs( diff, ent1, ent0);  	// in ent1 not in ent0
		pEnt = diff.begin();
		if( diff.size() > 0 ) {
			outFile << "Associations in " << commLine[2] << " not in " << commLine[1] << endl;
			for( ; pEnt != diff.end(); ++pEnt ) {
				outFile << '\t' << *pEnt << endl;
				}
			outFile << endl;		
			}
		// check/output qualifier def differences
		ent0.clear();
		ent1.clear();
		ent0 = tdeKb0.getKbQuals();
		ent1 = tdeKb1.getKbQuals();
		checkDiffs( diff, ent0, ent1);  	// in ent0 not in ent1
		pEnt = diff.begin();
		if( diff.size() > 0 ) {
			outFile << "Qualifiers in " << commLine[1] << " not in " << commLine[2] << endl;
			for( ; pEnt != diff.end(); ++pEnt ) {
				outFile << '\t' << *pEnt << endl;
				}
			outFile << endl;		
			}
		checkDiffs( diff, ent1, ent0);  	// in ent1 not in ent0
		pEnt = diff.begin();
		if( diff.size() > 0 ) {
			outFile << "Qualifiers in " << commLine[2] << " not in " << commLine[1] << endl;
			for( ; pEnt != diff.end(); ++pEnt ) {
				outFile << '\t' << *pEnt << endl;
				}
			outFile << endl;		
			}
		}  // end of optional diff section for basic def counts


	/* CONCEPT COUNT SUMMARY BY KIND */
	outFile << "Concept counts per kind for file " << commLine[1] << " (count of \"defined\" concepts in second row)" << endl;
	vector<string> kindVec0 = tdeKb0.getKbKinds();
	vector<string>::iterator pKindVec0;
	vector<string> kindVec1 = tdeKb1.getKbKinds();
	vector<string>::iterator pKindVec1;
	map<string, long> conKinds0, conKindsD0;
	map<string, long> conKinds1, conKindsD1;
	for( pKindVec0 = kindVec0.begin(); pKindVec0 != kindVec0.end(); ++pKindVec0 ) {
		conKinds0.insert(map<string,long>::value_type(*pKindVec0,0));	// initialize kind-concept map
		conKindsD0.insert(map<string,long>::value_type(*pKindVec0,0));
		outFile << *pKindVec0 << '\t';									// output column headers
		}
	outFile << endl;
	for( posCon = tdeKb0.getFirstConceptIterator(); 					// load kind-concept map
			posCon != tdeKb0.getEndConceptIterator(); ++posCon) {
		conKinds0[posCon->second.getKind()]++;
		if( posCon->second.getState() )
			conKindsD0[posCon->second.getKind()]++;
		}
	for( pKindVec0 = kindVec0.begin(); pKindVec0 != kindVec0.end(); ++pKindVec0 ) // output concept counts
		outFile << conKinds0[*pKindVec0] << '\t';
	outFile << endl;
	for( pKindVec0 = kindVec0.begin(); pKindVec0 != kindVec0.end(); ++pKindVec0 ) // output defined concept counts
		outFile << conKindsD0[*pKindVec0] << '\t';
	outFile << endl << endl;
	// concept diff summary per kind
	if( tdeKb1.getNumConcepts() > 0 ) {					// if two files, do diff
		outFile << "Concept diff per kind for files " << commLine[1] << " minus " << commLine[2] << " (diff of \"defined\" concepts in second row)" << endl;
		for( pKindVec0 = kindVec0.begin(); pKindVec0 != kindVec0.end(); ++pKindVec0 )
			outFile << *pKindVec0 << '\t';									// output column headers
		outFile << endl;
		for( pKindVec1 = kindVec1.begin(); pKindVec1 != kindVec1.end(); ++pKindVec1 ) { // initialize second kind-concept map
			conKinds1.insert(map<string,long>::value_type(*pKindVec1,0));
			conKindsD1.insert(map<string,long>::value_type(*pKindVec1,0));
			}
		for( posCon = tdeKb1.getFirstConceptIterator(); 					// load kind-concept map
				posCon != tdeKb1.getEndConceptIterator(); ++posCon) {
			conKinds1[posCon->second.getKind()]++;
			if( posCon->second.getState() )
				conKindsD1[posCon->second.getKind()]++;
			}
		for( pKindVec0 = kindVec0.begin(); pKindVec0 != kindVec0.end(); ++pKindVec0 ) {  // output diff concept counts
			if( tdeKb1.hasKind(*pKindVec0) )
				outFile << conKinds0[*pKindVec0] - conKinds1[*pKindVec0] << '\t' ;
			else
				outFile << "N.A." << '\t' ;
			}
		outFile << endl;
		for( pKindVec0 = kindVec0.begin(); pKindVec0 != kindVec0.end(); ++pKindVec0 ) {  // output defined concept diff
			if( tdeKb1.hasKind(*pKindVec0) )
				outFile << conKindsD0[*pKindVec0] - conKindsD1[*pKindVec0] << '\t' ;
			else
				outFile << "N.A." << '\t' ;
			}
		outFile << endl << endl;
		}

	string kindname0, kindname1;

	/* PROPERTY SUMMARY BY KIND */ 
	map<string, map<string,long> > kindPropertyMap0;		// maps kind to a property-counter map

	vector<string> propVec0 = tdeKb0.getKbProps();
	vector<string>::iterator pPropVec0;
	for( pKindVec0 = kindVec0.begin(); pKindVec0 != kindVec0.end(); ++pKindVec0 ) {
		map<string,long> propMap0;
		for( pPropVec0 = propVec0.begin(); pPropVec0 != propVec0.end(); ++pPropVec0 )
			propMap0.insert(map<string,long>::value_type(*pPropVec0,0));	// initialize all property counters to zero
		kindPropertyMap0.insert(map<string, map<string,long> >::value_type(*pKindVec0,propMap0));
		}
	multimap<long, OntyxProperty> conProps0;
	multimap<long, OntyxProperty>::iterator pConProps0;
	string propname0;
	for( posCon = tdeKb0.getFirstConceptIterator(); 
			posCon != tdeKb0.getEndConceptIterator(); ++posCon) {
		kindname0 = posCon->second.getKind();
		conProps0 = posCon->second.getProperties();
		for( pConProps0 = conProps0.begin(); pConProps0 != conProps0.end(); ++pConProps0 ) {
			propname0 = pConProps0->second.getName();
			kindPropertyMap0[kindname0][propname0]++;
			}
		}
	// output table for raw numbers, kinds in columns, properties in rows
	outFile << "Property counts, breakdown by kind for file " << commLine[1] << endl;
	long cum0;
	outFile << '\t' ;								// leave first column blank for propnames
	for( pKindVec0 = kindVec0.begin(); pKindVec0 != kindVec0.end(); ++pKindVec0 ) // print column headers
		outFile << *pKindVec0 << '\t';
	outFile << "cumulative" << endl;
	for( pPropVec0 = propVec0.begin(); pPropVec0 != propVec0.end(); ++pPropVec0 ) {
		cum0 = 0;
		outFile << *pPropVec0 << '\t' ;					// begin row with prop name
		for( pKindVec0 = kindVec0.begin(); pKindVec0 != kindVec0.end(); ++pKindVec0 ) {
			outFile << kindPropertyMap0[*pKindVec0][*pPropVec0] << '\t';
			cum0 += kindPropertyMap0[*pKindVec0][*pPropVec0];
			}
		outFile << cum0 << endl;								// end row
		}
	outFile << endl;
	if( tdeKb1.getNumConcepts() > 0 ) {							// two KBs, do diff of properties
		// get property summary by kind for initial file
		map<string, map<string,long> > kindPropertyMap1;		// maps kind to a property-counter map
		multimap<long, OntyxProperty> conProps1;
		multimap<long, OntyxProperty>::iterator pConProps1;
		kindVec1 = tdeKb1.getKbKinds();
		vector<string> propVec1 = tdeKb1.getKbProps();
		vector<string>::iterator pPropVec1;
		for( pKindVec1 = kindVec1.begin(); pKindVec1 != kindVec1.end(); ++pKindVec1 ) {
			map<string,long> propMap1;
			for( pPropVec1 = propVec1.begin(); pPropVec1 != propVec1.end(); ++pPropVec1 )
				propMap1.insert(map<string,long>::value_type(*pPropVec1,0));	// initialize all property counters to zero
			kindPropertyMap1.insert(map<string, map<string,long> >::value_type(*pKindVec1,propMap1));
			}
		string propname1;
		for( posCon = tdeKb1.getFirstConceptIterator(); 
				posCon != tdeKb1.getEndConceptIterator(); ++posCon) {
			kindname1 = posCon->second.getKind();
			conProps1 = posCon->second.getProperties();
			for( pConProps1 = conProps1.begin(); pConProps1 != conProps1.end(); ++pConProps1 ) {
				propname1 = pConProps1->second.getName();
				kindPropertyMap1[kindname1][propname1]++;
				}
			}
		// output table for diffs, kinds in columns, properties in rows
		outFile << "Diff properties, breakdown by kind for file " << commLine[1] << " minus " << commLine[2] << endl;
		outFile << '\t' ;								// leave first column header blank for propnames
		for( pKindVec0 = kindVec0.begin(); pKindVec0 != kindVec0.end(); ++pKindVec0 ) // print column headers from final file
			outFile << *pKindVec0 << '\t';
		outFile << endl;
		for( pPropVec0 = propVec0.begin(); pPropVec0 != propVec0.end(); ++pPropVec0 ) {
			outFile << *pPropVec0 << '\t' ;					// begin row with prop name
			for( pKindVec0 = kindVec0.begin(); pKindVec0 != kindVec0.end(); ++pKindVec0 ) {
				if( tdeKb1.hasKind(*pKindVec0) && tdeKb1.hasPropertyDef(*pPropVec0) )
					outFile << kindPropertyMap0[*pKindVec0][*pPropVec0] - kindPropertyMap1[*pKindVec0][*pPropVec0] << '\t' ;
				else
					outFile << "N.A." << '\t' ;
				}
			outFile << endl;								// end row
			}
		outFile << endl;
		}


	/* ROLE SUMMARY BY KIND */ 
	map<string, map<string,long> > kindRoleMap0;		// maps kind to a property-counter map

	vector<string> roleVec0 = tdeKb0.getKbRoles();
	vector<string>::iterator pRoleVec0;
	for( pKindVec0 = kindVec0.begin(); pKindVec0 != kindVec0.end(); ++pKindVec0 ) {
		map<string,long> roleMap0;
		for( pRoleVec0 = roleVec0.begin(); pRoleVec0 != roleVec0.end(); ++pRoleVec0 )
			roleMap0.insert(map<string,long>::value_type(*pRoleVec0,0));	// initialize all property counters to zero
		kindRoleMap0.insert(map<string, map<string,long> >::value_type(*pKindVec0,roleMap0));
		}
	multimap<long, OntyxRole> conRoles0;
	multimap<long, OntyxRole>::iterator pConRoles0;
	string rolename0;
	for( posCon = tdeKb0.getFirstConceptIterator(); 
			posCon != tdeKb0.getEndConceptIterator(); ++posCon) {
		kindname0 = posCon->second.getKind();
		conRoles0 = posCon->second.getRoles();
		for( pConRoles0 = conRoles0.begin(); pConRoles0 != conRoles0.end(); ++pConRoles0 ) {
			rolename0 = pConRoles0->second.getName();
			kindRoleMap0[kindname0][rolename0]++;
			}
		}
	// output table for raw numbers, kinds in columns, roles in rows
	outFile << "Role counts, breakdown by kind for file " << commLine[1] << endl;
	outFile << '\t' ;								// leave first column blank for rolenames
	for( pKindVec0 = kindVec0.begin(); pKindVec0 != kindVec0.end(); ++pKindVec0 ) // print column headers
		outFile << *pKindVec0 << '\t';
	outFile << endl;
	for( pRoleVec0 = roleVec0.begin(); pRoleVec0 != roleVec0.end(); ++pRoleVec0 ) {
		outFile << *pRoleVec0 << '\t' ;					// begin row with prop name
		for( pKindVec0 = kindVec0.begin(); pKindVec0 != kindVec0.end(); ++pKindVec0 ) {
			outFile << kindRoleMap0[*pKindVec0][*pRoleVec0] << '\t';
			}
		outFile << endl;								// end row
		}
	outFile << endl;
	if( tdeKb1.getNumConcepts() > 0 ) {							// two KBs, do diff of roles
		map<string, map<string,long> > kindRoleMap1;		// maps kind to a property-counter map
	
		vector<string> roleVec1 = tdeKb1.getKbRoles();
		vector<string>::iterator pRoleVec1;
		for( pKindVec1 = kindVec1.begin(); pKindVec1 != kindVec1.end(); ++pKindVec1 ) {
			map<string,long> roleMap1;
			for( pRoleVec1 = roleVec1.begin(); pRoleVec1 != roleVec1.end(); ++pRoleVec1 )
				roleMap1.insert(map<string,long>::value_type(*pRoleVec1,0));	// initialize all role counters to zero
			kindRoleMap1.insert(map<string, map<string,long> >::value_type(*pKindVec1,roleMap1));
			}
		multimap<long, OntyxRole> conRoles1;
		multimap<long, OntyxRole>::iterator pConRoles1;
		string rolename1;
		for( posCon = tdeKb1.getFirstConceptIterator(); 
				posCon != tdeKb1.getEndConceptIterator(); ++posCon) {
			kindname1 = posCon->second.getKind();
			conRoles1 = posCon->second.getRoles();
			for( pConRoles1 = conRoles1.begin(); pConRoles1 != conRoles1.end(); ++pConRoles1 ) {
				rolename1 = pConRoles1->second.getName();
				kindRoleMap1[kindname1][rolename1]++;
				}
			}
		// output table for raw numbers, kinds in columns, roles in rows
		outFile << "Role diff, breakdown by kind for file " << commLine[1] << " minus " << commLine[2] << endl;
		outFile << '\t' ;								// leave first column blank for rolenames
		for( pKindVec0 = kindVec0.begin(); pKindVec0 != kindVec0.end(); ++pKindVec0 ) // print kind column headers
			outFile << *pKindVec0 << '\t';
		outFile << endl;
		for( pRoleVec0 = roleVec0.begin(); pRoleVec0 != roleVec0.end(); ++pRoleVec0 ) {
			outFile << *pRoleVec0 << '\t' ;					// begin row with role name
			for( pKindVec0 = kindVec0.begin(); pKindVec0 != kindVec0.end(); ++pKindVec0 ) {
				if( tdeKb1.hasKind(*pKindVec0) && tdeKb1.hasRoleDef(*pRoleVec0) )
					outFile << kindRoleMap0[*pKindVec0][*pRoleVec0] - kindRoleMap1[*pKindVec0][*pRoleVec0] << '\t';
				else
					outFile << "N.A." << '\t';
				}
			outFile << endl;								// end row
			}
		outFile << endl;
		}
	//
	/* ASSOCIATION SUMMARY BY KIND */ 
	map<string, map<string,long> > kindAssocMap0;		// maps kind to a property-counter map

	vector<string> assocVec0 = tdeKb0.getKbAssocs();
	vector<string>::iterator pAssocVec0;
	for( pKindVec0 = kindVec0.begin(); pKindVec0 != kindVec0.end(); ++pKindVec0 ) {
		map<string,long> assocMap0;
		for( pAssocVec0 = assocVec0.begin(); pAssocVec0 != assocVec0.end(); ++pAssocVec0 )
			assocMap0.insert(map<string,long>::value_type(*pAssocVec0,0));	// initialize all property counters to zero
		kindAssocMap0.insert(map<string, map<string,long> >::value_type(*pKindVec0,assocMap0));
		}
	multimap<long, OntyxAssociation> conAssoc0;
	multimap<long, OntyxAssociation>::iterator pConAssoc0;
	string assocname0;
	for( posCon = tdeKb0.getFirstConceptIterator(); 
			posCon != tdeKb0.getEndConceptIterator(); ++posCon) {
		kindname0 = posCon->second.getKind();
		conAssoc0 = posCon->second.getAssociations();
		for( pConAssoc0 = conAssoc0.begin(); pConAssoc0 != conAssoc0.end(); ++pConAssoc0 ) {
			assocname0 = pConAssoc0->second.getName();
			kindAssocMap0[kindname0][assocname0]++;
			}
		}
	// output table for raw numbers, kinds in columns, associations in rows
	outFile << "Association counts, breakdown by kind for file " << commLine[1] << endl;
	outFile << '\t' ;								// leave first column blank for association names
	for( pKindVec0 = kindVec0.begin(); pKindVec0 != kindVec0.end(); ++pKindVec0 ) // print column headers
		outFile << *pKindVec0 << '\t';
	outFile << endl;
	for( pAssocVec0 = assocVec0.begin(); pAssocVec0 != assocVec0.end(); ++pAssocVec0 ) {
		outFile << *pAssocVec0 << '\t' ;					// begin row with association name
		for( pKindVec0 = kindVec0.begin(); pKindVec0 != kindVec0.end(); ++pKindVec0 ) {
			outFile << kindAssocMap0[*pKindVec0][*pAssocVec0] << '\t';
			}
		outFile << endl;								// end row
		}
	outFile << endl;
	if( tdeKb1.getNumConcepts() > 0 ) {							// two KBs, do diff of roles
		map<string, map<string,long> > kindAssocMap1;		// maps kind to a property-counter map
	
		vector<string> assocVec1 = tdeKb1.getKbAssocs();
		vector<string>::iterator pAssocVec1;
		for( pKindVec1 = kindVec1.begin(); pKindVec1 != kindVec1.end(); ++pKindVec1 ) {
			map<string,long> assocMap1;
			for( pAssocVec1 = assocVec1.begin(); pAssocVec1 != assocVec1.end(); ++pAssocVec1 )
				assocMap1.insert(map<string,long>::value_type(*pAssocVec1,0));	// initialize all counters to zero
			kindAssocMap1.insert(map<string, map<string,long> >::value_type(*pKindVec1,assocMap1));
			}
		multimap<long, OntyxAssociation> conAssoc1;
		multimap<long, OntyxAssociation>::iterator pConAssoc1;
		string assocname1;
		for( posCon = tdeKb1.getFirstConceptIterator(); 
				posCon != tdeKb1.getEndConceptIterator(); ++posCon) {
			kindname1 = posCon->second.getKind();
			conAssoc1 = posCon->second.getAssociations();
			for( pConAssoc1 = conAssoc1.begin(); pConAssoc1 != conAssoc1.end(); ++pConAssoc1 ) {
				assocname1 = pConAssoc1->second.getName();
				kindAssocMap1[kindname1][assocname1]++;
				}
			}
		// output table for raw numbers, kinds in columns, associations in rows
		outFile << "Associations diff, breakdown by kind for file " << commLine[1] << " minus " << commLine[2] << endl;
		outFile << '\t' ;								// leave first column blank for association names
		for( pKindVec0 = kindVec0.begin(); pKindVec0 != kindVec0.end(); ++pKindVec0 ) // print kind column headers
			outFile << *pKindVec0 << '\t';
		outFile << endl;
		for( pAssocVec0 = assocVec0.begin(); pAssocVec0 != assocVec0.end(); ++pAssocVec0 ) {
			outFile << *pAssocVec0 << '\t' ;					// begin row with association name
			for( pKindVec0 = kindVec0.begin(); pKindVec0 != kindVec0.end(); ++pKindVec0 ) {
				if( tdeKb1.hasKind(*pKindVec0) && tdeKb1.hasAssociationDef(*pAssocVec0) )
					outFile << kindAssocMap0[*pKindVec0][*pAssocVec0] - kindAssocMap1[*pKindVec0][*pAssocVec0] << '\t';
				else
					outFile << "N.A." << '\t';
				}
			outFile << endl;								// end row
			}
		outFile << endl;
		}
		//COUNT DEFINITIONS
		
		outFile << "Concepts with more than one definition in " << commLine[1] << endl;
		outFile << '\t' << "name" << '\t' << "kind"  << endl;
		bool found;
        found = false;
		for( posCon = tdeKb0.getFirstConceptIterator(); posCon != tdeKb0.getEndConceptIterator(); ++posCon) {
             //   long numDef = posCon->second.getPropertyNumber("DEFINITION");
                if (posCon->second.getPropertyNumber("DEFINITION") > 1) {
                      found = true;
                      outFile << '\t' << posCon->second.getName() << '\t' << posCon->second.getKind() << endl ;
                }       		
        }
		if( !found )
			outFile << "No duplicate definitions" << endl;
		outFile << endl;

	outFile.close();
	/* END OF THE BASIC STATISTICS SECTION */


	/* START SPECIALTY QUERIES, CONCEPTS WITH CHANGED TREEING, CHANGED DEFINITIONS */

	if( tdeKb1.getNumConcepts() > 0 ) {							// two KBs, do diff of roles
		cerr << "Writing \"special\" output file" << endl;
		ofstream outFile2;
		string outfilename = "special-" + commLine[1] + ".txt";
		outFile2.open(outfilename.c_str());
		if( outFile2.bad() ) {
			cerr << "ERROR opening \"special\" output file, exiting." << endl << endl;
			exit(0);
			}
		long l_ID;
		// CHECK NEW CONCEPTS
		outFile2 << "New concepts:  present in " << commLine[1] << " but not in " << commLine[2] << endl;
		outFile2 << '\t' << "name" << '\t' << "kind" << endl;
		found = false;
		for( posCon = tdeKb0.getFirstConceptIterator(); 
				posCon != tdeKb0.getEndConceptIterator(); ++posCon) {
			if( (l_ID = tdeKb1.getIDFromCode(posCon->second.getCode())) == -1 ) {
				outFile2 << '\t' << posCon->second.getName() << '\t' << posCon->second.getKind() << endl;
				found = true;
				}
			}
		if( !found )
			outFile2 << '\t' << "No \"New\" concepts found." << endl;
		outFile2 << endl;
		// CHECK DELETED CONCEPTS
		outFile2 << "Deleted concepts:  present in " << commLine[2] << " but not in " << commLine[1] << endl;
		found = false;
		for( posCon = tdeKb1.getFirstConceptIterator(); 
				posCon != tdeKb1.getEndConceptIterator(); ++posCon) {
			if( (l_ID = tdeKb0.getIDFromCode(posCon->second.getCode())) == -1 ) {
				outFile2 << '\t' << posCon->second.getName() << endl;
				found = true;
				}
			}
		if( !found )
			outFile2 << '\t' << "No deleted concepts found." << endl;
		outFile2 << endl;
		// CHECK RETIRED CONCEPTS
		set<string> unRetiredSet;
		outFile2 << "Concepts retired in " << commLine[1] << " (retirements from merges are not flagged)." << endl;
		outFile2 << '\t' << "name" << '\t' << "old kind" << endl;
		found = false;
		for( posCon = tdeKb0.getFirstConceptIterator(); 
				posCon != tdeKb0.getEndConceptIterator(); ++posCon) {
			if( (l_ID = tdeKb1.getIDFromCode(posCon->second.getCode())) != -1 ) {
				OntyxConcept tmpConcept = tdeKb1.getConcept(l_ID);
				if( tmpConcept.getKind() != retired  && posCon->second.getKind() == retired ) {
					outFile2 << '\t' << posCon->second.getName() << '\t' << tmpConcept.getKind() << endl;
					found = true;
					}
				if( tmpConcept.getKind() == retired  && posCon->second.getKind() != retired ) 
					unRetiredSet.insert(posCon->second.getName());
				}
			}
		if( !found )
			outFile2 << '\t' << "No retired concepts found."  << endl;
		outFile2 << endl;
			// check for unretirements too
		if( unRetiredSet.size() > 0 ) {
			outFile2 << "UNRETIREMENT ACTION in " << commLine[1] << ", concepts are " << endl;
			set<string>::iterator posUnretid;
			for( posUnretid = unRetiredSet.begin(); posUnretid != unRetiredSet.end(); ++posUnretid )
				outFile2 << '\t' << *posUnretid << endl;
			outFile2 << endl;
			}
		// CHECK FOR CONCEPTS WITH CHANGED KINDS
		set<string> changedKind;
		found = false;
		outFile2 << "CONCEPTS WITH CHANGED KIND (excluding retirements/unretirements) found in " << commLine[1] << ", concepts are" << endl;
		outFile2 << '\t' << "name" << '\t' << "new kind" << '\t' << "old kind" << endl;
		for( posCon = tdeKb0.getFirstConceptIterator(); 
				posCon != tdeKb0.getEndConceptIterator(); ++posCon) {
			if( (l_ID = tdeKb1.getIDFromCode(posCon->second.getCode())) != -1 ) {
				OntyxConcept tmpConcept = tdeKb1.getConcept(l_ID);
				if( tmpConcept.getKind() != posCon->second.getKind() && posCon->second.getKind() != retired && tmpConcept.getKind() != retired ) {
					found = true;
					outFile2 << '\t' << posCon->second.getName() << '\t' << posCon->second.getKind() << '\t' << tmpConcept.getKind() << endl;
					}
				}
			}
		if( !found )
			outFile2 << "No Kind-changed concepts found." << endl;
		outFile2 << endl;
		// CHECK PARENTS
		set<string> parents0, parents1; 
		set<string>::iterator posSet;
		outFile2 << "Concepts that have been retreed in " << commLine[1] << " relative to " << commLine[2] << endl;
		outFile2 << '\t' << "name" << '\t' << "kind" << '\t' << "parents in " + commLine[1] << '\t' << "parents in " + commLine[2] << endl;
		found = false;
		for( posCon = tdeKb0.getFirstConceptIterator(); 
				posCon != tdeKb0.getEndConceptIterator(); ++posCon) {
			if( (l_ID = tdeKb1.getIDFromCode(posCon->second.getCode())) != -1 ) {
				parents0 = posCon->second.getParents();
				OntyxConcept tmp1 = tdeKb1.getConcept(l_ID);
				parents1 = tmp1.getParents();
				if( parents0 != parents1 && tmp1.getKind() == posCon->second.getKind() ) {
					found = true;
					outFile2 << '\t' << posCon->second.getName() << '\t' << posCon->second.getKind() << '\t' ;
					if( parents0.size() > 0 ) {
						posSet = parents0.begin();
						outFile2 << *posSet ;
						++posSet;
						for( ; posSet != parents0.end(); ++posSet )
							outFile2 << " | " << *posSet ;
						}
					outFile2 << '\t' ;
					if( parents1.size() > 0 ) {
						posSet = parents1.begin();
						outFile2 << *posSet ;
						++posSet;
						for( ; posSet != parents1.end(); ++posSet )
							outFile2 << " | " << *posSet ;
						}
					outFile2 << endl;
					}
				}
			}
		if( !found )
			outFile2 << '\t' << "NO retreed concepts found." << endl;
		outFile2 << endl;
		// CHECK DEFINITIONS
		set<string> def0, def1, defdiff, diffdef;
		outFile2 << "Concepts with new/changed/deleted definitions in " << commLine[1] << " relative to " << commLine[2] << endl;
		outFile2 << '\t' << "name" << '\t' << "kind" << '\t' << "def in " + commLine[1] << '\t' << "def in " + commLine[2] << endl;
		found = false;
		for( posCon = tdeKb0.getFirstConceptIterator(); 
				posCon != tdeKb0.getEndConceptIterator(); ++posCon) {
			if( (l_ID = tdeKb1.getIDFromCode(posCon->second.getCode())) != -1 ) {
				def0.clear();
				def1.clear();
				defdiff.clear();
				diffdef.clear();
				conProps0 = posCon->second.getProperties();
				for( pConProps0 = conProps0.begin(); pConProps0 != conProps0.end(); ++pConProps0 ) {
					if( pConProps0->second.getName() == "DEFINITION" )
						def0.insert(pConProps0->second.getValue());
					}
				OntyxConcept tmp1 = tdeKb1.getConcept(l_ID);
				conProps0 = tmp1.getProperties();
				for( pConProps0 = conProps0.begin(); pConProps0 != conProps0.end(); ++pConProps0 ) {
					if( pConProps0->second.getName() == "DEFINITION" )
						def1.insert(pConProps0->second.getValue());
					}
				if( def0 != def1 ) { 
					found = true;
					set_difference(def0.begin(), def0.end(), def1.begin(), def1.end(), inserter(defdiff, defdiff.begin()) );
					set_difference(def1.begin(), def1.end(), def0.begin(), def0.end(), inserter(diffdef, diffdef.begin()) );
					// copy(defdiff.begin(), defdiff.end(), ostream_iterator<string>(outFile2,"\t"));
					set<string>::iterator posDef, posDiff;
					posDef = defdiff.begin();
					posDiff = diffdef.begin();
					bool endpoint = false; 
					while( !endpoint ) {
						outFile2 << '\t' << posCon->second.getName() << '\t' << posCon->second.getKind() << '\t' ;
						if( posDef != defdiff.end() ) {
							outFile2 << *posDef << '\t';
							++posDef;
							}
						else
							outFile2 << '\t';
						if( posDiff != diffdef.end() ) {
							outFile2 << *posDiff << '\t';
							++posDiff;
							}
						else
							outFile2 << '\t';
						outFile2 << endl;
						if( posDef == defdiff.end() && posDiff == diffdef.end() )
							endpoint = true;
						}
					}
				}
			}
		if( !found )
			outFile2 << "No changes in definitions." << endl;
	/* END SPECIALTY QUERIES, CONCEPTS WITH CHANGED TREEING, CHANGED DEFINITIONS */

	/* START QUERY FOR ALL CONCEPTS THAT HAVE A CONTRIBUTING SOURCE AND A DEFINITION BUT DO NOT HAVE AN ALT-DEF */
		outFile2 << endl;
		outFile2 << "Concepts with a Contributing_Source and DEFINITION without an ALT-DEFINITION in "  << commLine[1] << " :" << endl;
		outFile2 << "code" << '\t' << "name" << '\t' << "source" << '\t' << "kind" << '\t' << "comment" << endl;
		for( posCon = tdeKb0.getFirstConceptIterator();	posCon != tdeKb0.getEndConceptIterator(); ++posCon) {
			if( posCon->second.getPropertyNumber("Contributing_Source") != 0 && posCon->second.getPropertyNumber("DEFINITION") != 0
				&& posCon->second.getPropertyNumber("ALT_DEFINITION") == 0 ) {
				outFile2 << posCon->second.getCode() << "\t" << posCon->second.getName() << "\t" << posCon->second.getPropertyValue("Contributing_Source") << "\t" << posCon->second.getKind();
				if( posCon->second.getPropertyNumber("Contributing_Source") > 1 ) {
					outFile2 << "\t" << "Concept contains multiple Contributing Sources" << endl;
				}
				else {
					outFile2 << endl;
				}
			}
		}

	/* END QUERY FOR ALL CONCEPTS THAT HAVE A CONTRIBUTING SOURCE AND A DEFINITION BUT DO NOT HAVE AN ALT-DEF  */

	/* SUBSET REPORTING */

		vector<OntyxQualifier> qualifiers;
		vector<OntyxQualifier>::iterator posQual;
		multimap<long, OntyxProperty> properties;
		multimap<long, OntyxProperty>::iterator posProp;
		multimap<long, OntyxAssociation> associations;
		multimap<long, OntyxAssociation>::iterator posAssoc;
		set<string> terminologies;
		set<string>::iterator posTerms;
		set<string> subsets;
		set<string>::iterator posSub;
		string source;	
		string subset;
		tdeKb0.findChildren( "Terminology_Subset", terminologies );
		outFile2 << "Terminologies include: " << endl;
		for( posTerms = terminologies.begin(); posTerms != terminologies.end(); ++posTerms ) {
			outFile2 << *posTerms << ":" << endl;
			tdeKb0.findChildren( *posTerms, subsets);
			source = tdeKb0.getConcept(*posTerms).getPropertyValue("Contributing_Source");
			for( posSub = subsets.begin(); posSub != subsets.end(); ++posSub ) {
				subset = *posSub;
				outFile2 << "\tConcepts for " << *posSub << endl;
				for( posCon = tdeKb0.getFirstConceptIterator(); posCon != tdeKb0.getEndConceptIterator(); ++posCon ) {
					associations = posCon->second.getAssociations();
					for( posAssoc = associations.begin(); posAssoc != associations.end(); ++posAssoc ) {
						if( posAssoc->second.getValue() == subset ) {
							outFile2 << "\t\t" << posCon->second.getCode();

							properties = posCon->second.getProperties();
							for( posProp = properties.begin(); posProp != properties.end(); ++posProp ) {
								if( (posProp->second.getName() == "FULL_SYN") && (posProp->second.hasQualifier()) ) {
									qualifiers = posProp->second.getQualifiers();
									for( posQual = qualifiers.begin(); posQual != qualifiers.end(); ++posQual ) {
										if( (posQual->getName() == "Syn_Source") && (posQual->getValue() == source) ) {
											outFile2 << "\t" << posProp->second.getValue() << endl;
										}
									}
								}
							}
						}
					}
				}
				
			}
		}

		outFile2 << endl;

	/* END SUBSET REPORTING */

        outFile2.close();
  } // end if

	tend = clock();
	cerr << "Execution took " << (tend-tstart)/1000.0 << " seconds" << endl;

	return 0;
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

void checkDiffs( vector<string> & diff, vector<string> & ent0, vector<string> & ent1)
{
	diff.clear();
	vector<string>::iterator pEnt0, pEnt1;
	bool found;
	for( pEnt0 = ent0.begin(); pEnt0 != ent0.end(); ++pEnt0 ) {
		found = false;
		for( pEnt1 = ent1.begin(); pEnt1 != ent1.end(); ++pEnt1) {
			if( *pEnt0 == *pEnt1 )
				found = true;
			}
		if( !found )
			diff.push_back(*pEnt0);
		}
}

/*
	read file 0, 1

d	get num of kinds
d	get num of propertydefs
d	get num of roledefs
d	get num of assocdefs
d	get num of qualifier defs
d	get num of concept defs	


	per kind report, fileF, and diff fileF - fileI
d		num of concepts per kind, map(kind, concept_counter)
		cumulative num properties used by concepts
d		num/type property defs (based on counting per concept), array[kindname, map(propertyname, counter)]
d		num/type role defs, asserted roles
		num/type assoc defs, asserted assocs
		num/type qualifier defs
d		num defined concepts
		specialty
			change in definition property
			full_syn/synonym property
			sem_type property
			multiply-treed concepts
	
	diff per kind report, file 1 vs t0 file 0

*/
