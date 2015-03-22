//*******************************************************************
// Definition of the Ontyx class OntyxKb
// 		Definition of Ontyx kb
//		Initial class definition 4/10/2001
//
// Dependencies:	STL <string>, <set>, <algorithm>, <map>, <vector>
// Dependencies:	iostream, fstream, strstream
// Dependencies:	OntyxName, OntyxKind, OntyxRoleDef,OntyxPropertyDef
// Virtual:			~OntyxKb()
//*******************************************************************

#ifndef _ONTYX_KB_H
#define _ONTYX_KB_H

#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <set>
#include <algorithm>
#include <string>

#include "OntyxConcept.h"
#include "OntyxRoleDef.h"
#include "OntyxPropertyDef.h"
#include "OntyxName.h"
#include "OntyxKind.h"
#include "OntyxProperty.h"
#include "OntyxAssociationDef.h"
#include "OntyxQualifierDef.h"
#include "OntyxRole.h"
#include "tString.h"		// deals with stemming and tokenizing
#include "OntylogHandler.h"	


using namespace std;

class CompareKb : public binary_function<OntyxConcept, OntyxConcept, bool>
{
	public:
		CompareKb() {}
		bool operator() (const OntyxConcept & a, const OntyxConcept & b) {return a.isSame(b);}
};

class OntyxKb
{
	private:

			// ********** Data Members

		string						m_kbname;			// name for kb, can be anything, use prob for filename
		map<long, OntyxName>		m_namespace;
		map<long, OntyxKind>		m_kind;
		map<long, OntyxPropertyDef>	m_property;
		map<long, OntyxRoleDef>		m_role;
		map<long, OntyxAssociationDef>	m_association;
		map<long, OntyxQualifierDef> m_qualifier;
		map<long, OntyxConcept> 	m_concept;			// uses internal id for map, use getMapID()

		map<string, long>			m_conName;			// concept name registry, names are trimmed and
														// lower case
		multimap<string, long>		m_terms;			// terms registry, names are trimmed and lower case
		multimap<string, long>		m_alphastemterms;	// alpha-ordered & stemmed terms

		multimap<string, long>		m_parents;			// parent registry
		map<string, long>			m_cuis;				// cui registry
		map<string, long>			m_codes;			// concept code registry
		multimap<string, long>		m_alphastem;		// stemmed alpha name registry

		static long 				s_conMapID;			// internal id counter


	public:

			// ********** Constructor/Destructor

		OntyxKb ();
		OntyxKb (const OntyxKb &);

		virtual ~OntyxKb();

			// ********** Mutator Functions

		void clearKb (void);

		OntyxKb & setKbName(string &);

		OntyxKb & addNamespace(const OntyxName &);
		OntyxKb & addKind (const OntyxKind &);
		OntyxKb & addPropertyDef (const OntyxPropertyDef &);
		OntyxKb & addRoleDef (const OntyxRoleDef & );
		OntyxKb & addConcept (const OntyxConcept & );
		OntyxKb & addAssociationDef (const OntyxAssociationDef &);
		OntyxKb & addQualifierDef (const OntyxQualifierDef &);

		void copyDefsTo (OntyxKb & ) const;
		OntyxKb & createPropertiesFromRoles (void);

		bool copyConceptTo (long conceptID, OntyxKb & outKB);	// returns false if target KB has copy 
		bool copySetConceptsTo (const set<long> & conceptIDs, OntyxKb & outKB);	// returns false if target KB has copy 
		bool moveConceptTo (long conceptID, OntyxKb & outKB);	// id
		bool moveSetConceptsTo (const set<long> & conceptIDs, OntyxKb & outKB);
		bool moveAllConceptsTo (OntyxKb & outKB);

		bool removeConcept (long conceptID);
		OntyxKb & removePropertyDef (const string & inPropDefName);
		OntyxKb & removeQualifierDef (const string & inPropDefName);
		OntyxKb & codeToXname (void);							// converts all codes in KB to valid xml ncnames
		OntyxKb & nameToXname (void);					// converts all names in KB to valid xml ncnames


			// ********** Accessor Functions
		string getKbName (void) const;

		map<long, OntyxConcept>::iterator getFirstConceptIterator (void) ;
		map<long, OntyxConcept>::iterator getEndConceptIterator (void) ;

		long getMaxKindID (void) const;
		long getMaxPropID (void) const;
		long getMaxConID (void) const;
		long getMaxRoleID (void) const;

		long getNumConcepts (void) const;
		long getNumKinds (void) const;
		long getNumRoles (void) const;
		long getNumProps (void) const;
		long getNumNames (void) const;
		long getNumAssocs (void) const;
		long getNumQuals (void) const;

		vector<string> getKbKinds (void) const;
		vector<string> getKbRoles (void) const;
		vector<string> getKbNames (void) const;
		vector<string> getKbAssocs (void) const;
		vector<string> getKbQuals (void) const;
		vector<string> getKbProps (void) const;

		OntyxKind getKindDef (string & name) const;
		OntyxRoleDef getRoleDef (string & name) const;
		OntyxPropertyDef getPropertyDef (string & name) const;
		OntyxAssociationDef getAssociationDef (string & name) const;
		OntyxQualifierDef getQualifierDef (string & name) const;

		map<long, OntyxPropertyDef>	getPropDefs (void);
		const string & getPropertyDefRange (const string & propName );
		const string & getRoleDefRangeKind (const string & testRoleDefName ) ;
		const string & getRoleDefDomainKind (const string & testRoleDefName );

		const string & getConceptKind (const OntyxConcept & ) const;
		const string & getConceptKind (long conceptID) ; // const;
		OntyxConcept & getConcept (const string & conName) ;
		OntyxConcept & getConcept (long conceptID) ;
		const string & getConceptName (long conceptID) const;


			// ********** Utility Functions

		bool hasKbName (void) const;
		bool hasAssociations (void) const;
		bool hasQualifiers (void) const;
		bool hasAssociationDef (const string & assocname) const;
		bool hasPropertyDef (string & propertyname) const;
		bool hasKind (string & kindname) const;
		bool hasRoleDef (string & rolename) const;		

		bool verifyRegistries(long callnumber) const;

		void findChildren (const OntyxConcept & , set<string> & outChildren);
		void findChildren (const string & conceptName, set<string> & outChildren);
		void findDescendants ( const string & conceptName, set<string> & outDescendants) ; // const;
		void findDescendants (long conceptID, set<long> & outDescendantIDs) ; // const;
		void clearKindsOfDescendants(const string & name);
		void assignKindsToDescendants(const string & name, const string & kind); // call clearkinds... first

		void reportOrphans (void) const;							// previously findOrphans()
		void findRootConcepts (set<string> & outRoots) const;
		void findRootConceptsInKb (set<string> & outRoots) const;	// returns all roots, stated as well as "orphans"

		void assignParentToOrphans (const string & name);
		bool isParentReferencedInRole (const OntyxConcept & refCon) ;
		void reportIfParentReferencedInRole (void) ;

		void getParentSet (const OntyxConcept & refCon, set<string> & outParentSet) ;
		void getConceptTerms (const OntyxConcept &, set<string> & outSet) const;	// "terms" are hard-coded
		void getAlphaStemConceptTerms (const OntyxConcept & inConcept, set<string> & outSet) const;

		string toLower(const string & ) const;					// maintained to support older programs, use tString now

		bool resolveCui (OntyxConcept & toCon, OntyxConcept & fromCon);

		void renameConcept(long conceptID, const string & newName);	// id
		void renameConcept(map<string, string> & renamePairs);	// {old name, new name} pairs

		bool parentNameInKb (const OntyxConcept & refCon) const;
		bool conceptNameInKB (const string & conName) const;	// case-insensitive search 
		bool alphastemtermInKB (const string & term) const;		// case-insensitive search 
		bool termInKB (const string & term) const;				// case-insensitive

		long getIDFromName (const string & conceptName) const;	// returns ID or -1 if not found
		long getIDFromCui (const string & cui ) const;			// returns ID or -1 if not found
		void getKbIDSet (set<long> & idSet);
		long getIDFromCode (const string & code ) const;			// returns ID or -1 if not found
		void getIDFromTerm (const string & term, set<long> & outSet) const;
		void getIDFromAlphaStemTerm (const string & term, set<long> & outSet) const;	// input is term, checks name terms
		void getIDFromAlphaStemTermsTerm (const string & term, set<long> & outSet) const; // checks pref_name & syn terms
																						// it gets stemmed by routine, case-insensitive search

		long simpleMatchKB (OntyxKb & inKB, const string & term, set<long> & ID); // -1 if merge, 0 if one term, 1 if split
		long alphaStemMatchKB (OntyxKb & inKB, const string & conceptName, set<long> & termID);

		void findMultiKind (set<string> & outSet) ;
		void findKinds (const OntyxConcept &, set<string> & outSet) const;

		vector<string> findConceptNamesByProperty (const string & propertyName, const string & propertyValue) const; // returns vector of concept names
		vector<string> findConceptNamesByProperty (const string & propertyName) const; 	// returns vector of concept names

		bool verifyReferences(const long refnumber) const;	// refnumber allows you to distinguish calling routine
		void fixReferences (void);
		void remapRoleReferences (map<string, string> & inMap);
		void remapRoleReferences (map<string, string> & inMap, OntyxKb & otherKb);


		string verifyOutString (const string & inString);  // for CDATA
		bool isValidOWL (const string & inString );
		string xName( const string & inString );			// returns an xml ncname given an input string


			// ********** Comparison Functions 

		// computes a kb difference-set of concepts & places output in diffKb
		// name, kinds, and defs are copied from *this
		void diff_Concepts ( OntyxKb & compareKb, OntyxKb & diffKb) const;
		void unique_Concepts (const OntyxKb & compareKb, OntyxKb & diffKb) const;

			// ********** Print Functions

		void printKbConcepts (void) const;					// prints concepts in a kb
		void printKbDiffConcepts (void) const;

			// ********** File I/O Functions

		void readOntyxXMLFile (const string & inFile);
		void writeOntyxXMLFile (const string & outFile, bool flag, bool noProps);
			 // flag => if true, renumber, noProps => if true, properties with role names excluded
		void writeOWLFile (const string & filename, const string headerFile, bool noProps);
		void writeOWLFileByName (const string & filename, const string headerFile, bool noProps);  // original export routine
		void writeNDRFT2OWLFile (string & filename, vector<string> & header, string & locnamespace );
		void writeFlatFile (const string & filename, bool flag, bool noProps); // not yet implemented
		void readFlatFile (const string & inFile, const string & inNamespace, const string & kind,
				const string & sem_type, const string & parent, const string & source);



private:

		ifstream & getTopLevelTag (ifstream &, string &) const;
		ifstream & getTag (ifstream &, string &) const;
		ifstream & getValue (ifstream &, string &) const;
		ifstream & readConceptDef (ifstream &);
		ifstream & readPropertyDef (ifstream &);
		ifstream & readRoleDef (ifstream &);
		ifstream & readKindDef (ifstream &);
		ifstream & readNamespaceDef (ifstream &);


};

#endif




