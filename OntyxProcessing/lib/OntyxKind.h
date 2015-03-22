//*******************************************************************
// Definition of Ontyx class OntyxKind
// 		Defines Kind objects in the Ontylog KB terminology
//		Based on xml dtd for ontylog declared in "ontylog.dtd"
//
//		Initial class definition 4/10/2001
//		12/14/01	Added static boolean error variable and error methods
//		12/14/01	Made getKindMapID and getKindMapName static, both set error
//					flag true if called when objects haven't yet been created
//		12/14/01	Added static sets to keep track of codes and ids
//		12/14/01	Modified relational operators to compare name instead of id.
//		12/18/01	Modified addKindToMap to take void
//		12/18/01	Renamed kindNotInMap to kindInMap & recoded
//
// Dependencies:	STL <string>, <map>, <set> classes
// Dependencies:	<iostream> class
// Dependencies		C <stdlib> for exit()
// Dependencies:	OntyxName class:  needs to call OntyxName::getNamespaceMapID(string kindName)
//					to get the internal id stored for m_namespaceID prior to construction
// Dependencies:	OntyxTerminology class for constants
//
// Virtual:			~OntyxKind()
//
// Misc:		1	Constructor sets error flag true if id, code, or name already exists
//				2	Constructs static maps of <string, long> and <long, string>
//					for clients to search for registered kind names/ids.
//				3	The kind code and id are not used for searching, searches
//					of the maps by id are done with internal ids generated and supplied 
//					by the class.  A client needs to call 
//					OntyxKind::getKindMapID(string kindname) to get the internal 
//					id stored for the kind prior to constructing, or in the 
//					constructor of, the client object
//				4	Relational comparisons of OntyxKind (kind_name) objects take into account
//					only the name.  An isSame() method compares all attributes.
//				5	Exits if memory allocation for map or set fails
//
//*******************************************************************

#ifndef _ONTYX_KIND_DEF_
#define _ONTYX_KIND_DEF_

#include "OntyxName.h"
#include "OntyxTerminology.h"

#include <cstdlib>
#include <map>
#include <set>
#include <string>
using namespace std;

class OntyxKind 
{

	private:

		string m_kindname;							// string name of Kind
		string m_code;								// database code, defined externally
		long m_id;									// database id number, defined externally
		long m_namespaceID;							// ID of namespace, defined in external OntyxName::map 
		bool m_reference;							// required by kind definition

		static long mapKindID;						// internal id for kind objects in map
		static map<long, string> * pMapKindID;		// to search by internal id
		static map<string, long> * pMapKindName;	// to search by kind name

		static long kindError;						// set if objects not yet created and 
													// static functions try to access maps

		static set<string> * pSetCode;				// to verify a code is not in use
		static set<long> * pSetID;					// to verify an id is not in use


			// ********** Private Utility Functions 

		void initializeMaps(void);
		void addKindToMap(void);

		bool codeInSet (const string &) const;
		bool idInSet (long) const;

	public:

			// ********** Constructor/Destructor

		// have to call OntyxName::getNamespaceMapID(Namespace) 
		// prior to construction
		OntyxKind (	const string & 	NameKind = "",	// default constructor
					long 			id = -1,
					const string & 	code = "", 
					const string & 	Namespace = "",
					bool 			reference = false);

		OntyxKind (const OntyxKind &);
		virtual ~OntyxKind ();

			// ********** Accessor Functions

		const string & getKindname (void) const;
		const string & getCode(void) const;
		long getID (void) const;
		long getNamespaceID (void) const;
		const string & getNamespace (void) const;
		bool getReference (void) const;
		long getMapID (void) const;
		
			// ********** Mutator Functions

		OntyxKind & setID(long);
		OntyxKind & setCode(const string &);

		void clearID (void);
		void clearCode (void);

			// ********** Comparison Functions

		bool isSame(const OntyxKind &) const;

			// ********** Relational Operators 

		bool operator== (const OntyxKind &) const;
		bool operator< (const OntyxKind &) const;

			// ********** Utility Functions 

		static void setError(long errorCode);
		static void clearError(long errorCode);
		static void clearError(void);
		static bool isError(void);
		static bool isError(long errorCode);
		static void printError (void);
	
		static long getKindMapID(const string &);
		static const string & getKindMapName(long id);
		static bool kindInMap(const string &);			// true if m_kindname is in map

};
#endif
