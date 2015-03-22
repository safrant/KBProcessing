//*******************************************************************
// Definition of an Ontyx role definition class OntyxRoleDef
// 		Defines the concept roles in the Ontyx terminology kb
//		Initial class definition 4/15/2001
//		Based on xml dtd for ontylog declared in "ontylog.dtd"
//		Modified:  introduced changes similar to those documented in "OntyxName.h"
//		Current as of "ontylog.dtd" from 10/19/01
//
// Dependencies:	C <stdlib> - exit
//					STL <string>, <map>, <set> class
//					<iostream>
//					OntyxName - uses NamespaceID registry
//					OntyxKind - uses KindID registry
//					OntyxTerminology - uses error codes
//
// Virtual:			~OntyxRoleDef()
//
// Misc:		1	Client uses this class along with OntyxRoleEnum to construct a
//					role object.
//				2	A client needs to call 
//					OntyxRoleDef::getRoleDefMapID(string role_def_name) to get the internal 
//					id stored for the role definition prior to constructing, or in the 
//					constructor of, the client object.  This is the registered ID of the
//					role definition.
//				3	Constructor calls OntyxName::getNamespaceMapID(string Namespace)
//					and OntyxKind::getKindMapID(string kindName) to get the registered
//					id's of the namespace and the kind.
//				4	Constructor sets error flag if name, code, or id already exist;
//					error constants defined in OntyxTerminology.
//				5	Constructs static maps of <string, long> and <long, string>
//					for clients to search for registered Role Definitions names/ids.
//				6	The database code and id are not used for searching, searches
//					of the maps by id are done with internal ids generated and supplied 
//					by the class.  
//				7	Relational comparisons of OntyxRoleDef (role_def_name) objects take 
//					into account only the role definition name.
//				8	Comparison method isSame() takes into account all attributes.
//				9	Exits if memory allocation for maps or sets fails
//
//*******************************************************************

#ifndef _ONTYX_ROLE_DEF_
#define _ONTYX_ROLE_DEF_

#include "OntyxName.h"
#include "OntyxKind.h"
#include "OntyxTerminology.h"

#include <map>
#include <set>
#include <string>
#include <iostream>
#include <cstdlib>
using namespace std;

class OntyxRoleDef 
{
	private:

		string m_rolename;								// string name of role
		string m_code;									// database code of role
		long m_id;										// database id of role
		long m_namespaceId;								// external OntyxName id of namespace  
		long m_domainId;								// external OntyxKind id of domain kind 
		long m_rangeId;									// external OntyxKind id of range kind
		string m_right;									// rolename of right identity, might use
														//   an id in the future
		string m_parentrole;							// parent role in hierarchy

		static long mapRoleDefID;						// internal id, for search purposes
		static map<long, string> * pMapRoleDefID;		// to search by internal id
		static map<string, long> * pMapRoleDefName;		// to search by role name

		static set<long> * pSetID;						// verify an external id is not in use
		static set<string> * pSetCode;					// verify a code is not in use
		static long roleDefError;						// error codes defined in OntyxTerminology

			// ********** Private Utility Functions 

		void initializeMaps (void);
		void addRoleDefToMap (void);

		bool codeInSet (const string &) const;
		bool idInSet (long) const;

	public:

			// ********** Constructor/Destructor

		OntyxRoleDef (	const string &	name = "", 
						const string &	code = "",
						long			id = -1,
						const string &	inSpace = "", 
						const string &	inDomain = "",
						const string &	inRange = "", 
						const string &	right = "",
						const string & 	parent = "");

		OntyxRoleDef (const OntyxRoleDef & );			// copy constructor

		virtual ~OntyxRoleDef ();

			// ********** Accessor Functions

		long getID (void) const;
		long getNamespaceID (void) const;
		long getDomainID (void) const;
		long getRangeID (void) const;
		long getMapID (void) const;

		const string & getName (void) const;
		const string & getCode (void) const;
		const string & getRight (void) const;
		const string & getNamespace (void) const;
		const string & getDomain (void) const;
		const string & getRange (void) const;
		const string & getParentRole (void) const;

			// ********** Mutator Functions

		OntyxRoleDef & setID(long);
		OntyxRoleDef & setCode(const string &);

		void clearID (void);
		void clearCode (void);

			// ********** Comparison Functions 

		bool isSame (const OntyxRoleDef &) const;
		bool hasParent (void) const;

		
			// ********** Relational Operators 

		bool operator== (const OntyxRoleDef &) const;	// Applied to m_rolename
		bool operator< (const OntyxRoleDef &) const;	// Applied to m_rolename
	
			// ********** Utility Functions 

		static void setError(long errorCode);
		static void clearError(long errorCode);
		static void clearError(void);
		static bool isError(void);
		static bool isError(long ErrorCode);
		static void printError (void);

		static bool roleDefInMap (const string &);		// true if m_rolename is not in map
		static long getRoleDefMapID (const string &);
		static const string & getRoleDefMapName (long id);

};
#endif
