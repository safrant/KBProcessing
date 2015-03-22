//*******************************************************************
// Definition of the Ontyx association definition class OntyxAssociationDef
// 		Defines the property attributes of concepts in the Ontyx terminology kb
//		Based on xml dtd for ontylog declared in "ontylog.dtd"
//		Initial class definition 3/5/2005
//		Based on "ontylog.dtd" from TDE 3, and various other changes
//		similar to those documented in "OntyxName.h"
//		
//
// Dependencies:	<iostream>
// 					STL <string>, <map>, <set>
//					<cstdlib> - exit()
//					OntyxName - uses NamespaceID registry
//					OntyxTerminology - uses error codes
//
// Virtual:			~OntyxAssociationDef()
//
// Misc:		1	Constructor sets error flag true if name, code, or id already exist;
//					error constants defined in OntyxTerminology.
//				2	Constructs static maps of <string, long> and <long, string>
//					for clients to search for registered Association Definitions names/ids.
//				3	The code and id are not used for searching, searches
//					of the maps by id are done with internal ids generated and supplied 
//					by the class.  
//				4	A client needs to call 
//					OntyxAssociationDef::getAssocDefMapID(string assoc_def_name) to get the internal 
//					id stored for the property definition prior to constructing, or in the 
//					constructor of, the client object
//				5	Relational comparisons of OntyxAssociationDef (prop_def_name) objects take 
//					into account only the association definition name.
//				6	Comparison method isSame() takes into account all attributes.
//				7	Exits if memory allocation for maps or sets fails
//
//*******************************************************************

#ifndef _ONTYX_ASSOCIATION_DEF_
#define _ONTYX_ASSOCIATION_DEF_

#include "OntyxName.h"
#include "OntyxTerminology.h"

#include <cstdlib>
#include <iostream>
#include <set>
#include <map>
#include <string>

using namespace std;


class OntyxAssociationDef 
{
	private:

		string m_associationname;					// string name of association
		string m_code;								// database code of association
		long m_id;									// database id of association
		string m_inversename;						// name of inverse association
		bool m_displayable;							// is the inverse association gui-displayable, true=yes by default
		long m_namespaceId;							// id of the property's namespace, external

		static long mapAssocDefID;					// internal id, for search purposes
		static map<long, string> * pMapAssocDefID;	// to search by internal id
		static map<string, long> * pMapAssocDefName;	// to search by name

		static set<string> * pSetCode;				// to verify a code is not in use
		static set<long> * pSetID;					// to verify an id is not in use

		static long assocDefError;					// set if object not yet created and
													// static functions try to access maps

			// ********** Private Utility Functions 

		void initializeMaps(void);
		void addAssocDefToMap(void);

		bool codeInSet (const string &) const;
		bool idInSet (long) const;

	public:

			// ********** Constructor/Destructor

		// default constructor calls OntyxName::getNamespaceMapID(inSpace) 
		OntyxAssociationDef (	const string &	name = "", 
							const string &	code = "", 
							long			id = -1, 
							const string &	inverse = "",
							bool 			display = true,
							const string &	inSpace = "");

		OntyxAssociationDef(const OntyxAssociationDef & );

		virtual ~OntyxAssociationDef();

			// ********** Accessor Functions

		const string & getName (void) const;
		const string & getCode (void) const;
		const string & getInverse (void) const;
		bool isDisplayable (void) const;

		const string & getNamespace(void) const;
		long getNamespaceID (void) const;

		long getID (void) const;
		long getMapID (void) const;

			// ********** Mutator Functions

		OntyxAssociationDef & setID(long);
		OntyxAssociationDef & setCode(const string &);
		OntyxAssociationDef & setInverse(const string &);
		OntyxAssociationDef & setDisplayable(bool);

		void clearID (void);
		void clearCode (void);

			// ********** Comparison Functions

		bool isSame(const OntyxAssociationDef &) const;

			// ********** Relational Operators 

		bool operator== (const OntyxAssociationDef &) const;
		bool operator< (const OntyxAssociationDef &) const;
	
			// ********** Utility Functions 

		bool hasInverse(void);

		static void setError(long errorCode);
		static void clearError(long errorCode);
		static void clearError(void);
		static bool isError(void);
		static bool isError(long ErrorCode);
		static void printError (void);

		static long getAssocDefMapID(const string &);
		static const string & getAssocDefMapName(long id);
		static bool assocDefInMap(const string &);		// true if m_associationname is in map

};
#endif
