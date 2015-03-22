//*******************************************************************
// Definition of the Ontyx Qualifier definition class OntyxQualifierDef
// 		Defines the qualifier attributes of properties and associations
//		in the Ontyx terminology kb.
//		Initial class definition 3/14/2005
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
// Virtual:			~OntyxQualifierDef()
//
// Misc:		1	Constructor sets error flag true if name, code, or id already exist;
//					error constants defined in OntyxTerminology.
//				2	Constructs static maps of <string, long> and <long, string>
//					for clients to search for registered Association Definitions names/ids.
//				3	The code and id are not used for searching, searches
//					of the maps by id are done with internal ids generated and supplied 
//					by the class.  
//				4	A client needs to call 
//					OntyxQualifierDef::getQualifierDefMapID(string qualifier_def_name) to get the internal 
//					id stored for the qualifier definition prior to constructing, or in the 
//					constructor of, the client object
//				5	Relational comparisons of OntyxQualifierDef (qual_def_name) objects take 
//					into account only the qualifier definition name.
//				6	Comparison method isSame() takes into account all attributes.
//				7	Exits if memory allocation for maps or sets fails
//
//*******************************************************************

#ifndef _ONTYX_QUALIFIER_DEF_
#define _ONTYX_QUALIFIER_DEF_

#include "OntyxName.h"
#include "OntyxTerminology.h"

#include <cstdlib>
#include <iostream>
#include <set>
#include <map>
#include <string>
#include <vector>

using namespace std;


class OntyxQualifierDef 
{
	private:

		string m_qualifiername;						// string name of qualifier
		string m_code;								// database code of qualifier
		long m_id;									// database id of qualifier
		long m_namespaceId;							// id of the property's namespace, external
		string m_qualifiertype	;					/* Qualifier type can be CA, CP or CR.
													  CA - qualifies an association between two concepts
													  CP - qualifies a property attached to a concept
													  CR - qualifies a role attached to a concept
													*/
		vector<string> pickList;					// optional, restricts choices to enumerations



		static long mapQualifierDefID;					// internal id, for search purposes
		static map<long, string> * pMapQualifierDefID;	// to search by internal id
		static map<string, long> * pMapQualifierDefName;	// to search by name

		static map<long, OntyxQualifierDef *> * pQualifierDef;// intended for pick list validation

		static set<string> * pSetCode;				// to verify a code is not in use
		static set<long> * pSetID;					// to verify an id is not in use

		static long qualifierDefError;				// set if object not yet created and
													// static functions try to access maps

			// ********** Private Utility Functions 

		void initializeMaps(void);
		void addQualifierDefToMap(void);

		bool codeInSet (const string &) const;
		bool idInSet (long) const;

		static const OntyxQualifierDef * getQualifierDef(const string & name);

	public:

			// ********** Constructor/Destructor

		// default constructor calls OntyxName::getNamespaceMapID(inSpace) 
		OntyxQualifierDef (	const string &	name = "", 
							const string &	code = "", 
							long			id = -1, 
							const string &	inSpace = "",
							const string &  inType = "",
							vector<string> inPickList = vector<string>() );

		OntyxQualifierDef(const OntyxQualifierDef & );

		virtual ~OntyxQualifierDef();

			// ********** Accessor Functions

		const string & getName (void) const;
		const string & getCode (void) const;
		const string & getNamespace(void) const;
		long getNamespaceID (void) const;

		const string & getQualifierType(void) const;
		const vector<string> & getPickList(void) const;

		long getID (void) const;
		long getMapID (void) const;


			// ********** Mutator Functions

		OntyxQualifierDef & setID(long);
		OntyxQualifierDef & setCode(const string &);

		void clearID (void);
		void clearCode (void);

			// ********** Comparison Functions

		bool isSame(const OntyxQualifierDef &) const;

		bool hasPickList(void) const;
		static bool hasPickList(const string & QualName) ;

		bool stringInPickList(const string & item) const;
		static bool stringInPickList(const string & QualName, const string & item);

			// ********** Relational Operators 

		bool operator== (const OntyxQualifierDef &) const;
		bool operator< (const OntyxQualifierDef &) const;
	
			// ********** Utility Functions 

		static void setError(long errorCode);
		static void clearError(long errorCode);
		static void clearError(void);
		static bool isError(void);
		static bool isError(long ErrorCode);
		static void printError (void);

		static long getQualifierDefMapID(const string &);
		static const string & getQualifierDefMapName(long id);
		static bool qualifierDefInMap(const string &);		// true if m_qualifiername is in map

};
#endif
