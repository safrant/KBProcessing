//*******************************************************************
// Definition of Ontyx class OntyxName
// 		Defines namespace objects of the Ontylog KB terminology
//		Based on xml dtd for ontylog declared in "ontylog.dtd"
//
//		Initial class definition 4/8/2001
//		12/11/01	Added static boolean error variable and error methods
//		12/11/01	Made getNamespaceMapID and getNamespaceMapName static, both set error
//					flag true if called when objects haven't yet been created
//		12/11/01	Added static sets to keep track of codes and ids
//		12/14/01	Modified relational operators to compare name instead of id.
//		12/18/01	Modified addNamespaceToMap to take void
//		12/18/01	Renamed namespaceNotInMap to namespaceInMap & recoded
//
// Dependencies:	STL <string>, <map>, <set> class
// Dependencies:	<iostream> class
// Dependencies		C <stdlib> for exit()
// Dependencies:	OntyxTerminology class for constants
//
// Virtual:			~OntyxName()
//
// Misc:		1	Constructor sets error flag true if name, code, or id already exist;
//					error constants defined in OntyxTerminology.
//				2	Constructs static maps of <string, long> and <long, string>
//					for clients to search for registered namespace names/ids.
//				3	The namespace code and id are not used for searching, searches
//					of the maps by id are done with internal ids generated and supplied 
//					by the class.  A client needs to call 
//					OntyxName::getNamespaceMapID(string namespace) to get the internal 
//					id stored for the namespace prior to constructing, or in the 
//					constructor of, the client object
//				4	Relational comparisons of OntyxName (namespace) objects take into account
//					only the namespace name.
//				5	Comparison method isSame() takes into account namespace name, code, & id.
//				6	Exits if memory allocation for maps or sets fails
//
//*******************************************************************

#ifndef _ONTYX_NAMESPACE_DEF_
#define _ONTYX_NAMESPACE_DEF_

#include <cstdlib>
#include <map>
#include <set>
#include <string>

#include "OntyxTerminology.h"

using namespace std;

class OntyxName {

	private:

		string m_namespace;							// string name of namespace
		string m_code;								// external database code
		long m_id;									// external database id number

		static long mapNamespaceID;					// internal id for namespace objects in map
		static map<long, string> * pMapNameID;		// map to search by internal id
		static map<string, long> * pMapNameName;	// map to search by "name"

		static long namespaceError;					// set if objects not yet created and 
													// static functions try to access maps

		static set<string> * pSetCode;				// for verifying a code is not in use
		static set<long> * pSetID;					// for verifying an id is not in use


			// ********** Private Utility Functions 

		void initializeMaps (void);
		void addNamespaceToMap (void);
		bool codeInSet (const string &) const;
		bool idInSet (long) const;

	public:

			// ********** Constructor/Destructor

		OntyxName (	const string & 	Namespace = "", 					// default constructor
					long 			id = -1, 
					const string & 	code = "");

		OntyxName (const OntyxName &);
		virtual ~OntyxName ();

			// ********** Accessor Functions

		const string & getNamespace (void) const;
		const string & getCode(void) const;
		long getID (void) const;
		long getMapID (void) const;

			// ********** Mutator Functions

		OntyxName & setID(long);
		OntyxName & setCode(const string &);

		void clearID (void);
		void clearCode (void);

			// ********** Comparison Functions

		bool isSame(const OntyxName &) const;

			// ********** Relational Operators 

		bool operator== (const OntyxName &) const;
		bool operator< (const OntyxName &) const;

			// ********** Public Static Utility Functions 

		static void setError(long errorCode);
		static void clearError(long errorCode);
		static void clearError(void);
		static bool isError(void);
		static bool isError(long ErrorCode);
		static void printError (void);

		static long getNamespaceMapID(const string &);
		static const string & getNamespaceMapName(long id);
		static bool namespaceInMap(const string &);				// true if m_namespace is in map
};
#endif
