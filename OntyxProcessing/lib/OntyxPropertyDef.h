//*******************************************************************
// Definition of the Ontyx property definition class OntyxPropertyDef
// 		Defines the property attributes of concepts in the Ontyx terminology kb
//		Based on xml dtd for ontylog declared in "ontylog.dtd"
//		Initial class definition 4/8/2001
//		Modified based on "ontylog.dtd" from 10/19/01, and various other changes
//		similar to those documented in "OntyxName.h"
//		
//
// Dependencies:	<iostream>
// 					STL <string>, <map>, <set>
//					<cstdlib> - exit()
//					OntyxName - uses NamespaceID registry
//					OntyxTerminology - uses error codes
//
// Virtual:			~OntyxPropertyDef()
//
// Misc:		1	Constructor sets error flag true if name, code, or id already exist;
//					error constants defined in OntyxTerminology.
//				2	Constructs static maps of <string, long> and <long, string>
//					for clients to search for registered Property Definitions names/ids.
//				3	The code and id are not used for searching, searches
//					of the maps by id are done with internal ids generated and supplied 
//					by the class.  
//				4	A client needs to call 
//					OntyxPropertyDef::getPropDefMapID(string prop_def_name) to get the internal 
//					id stored for the property definition prior to constructing, or in the 
//					constructor of, the client object
//				5	Relational comparisons of OntyxPropertyDef (prop_def_name) objects take 
//					into account only the property definition name.
//				6	Comparison method isSame() takes into account all attributes.
//				7	Exits if memory allocation for maps or sets fails
//
//*******************************************************************

#ifndef _ONTYX_PROPERTY_DEF_
#define _ONTYX_PROPERTY_DEF_

#include "OntyxName.h"
#include "OntyxTerminology.h"

#include <cstdlib>
#include <iostream>
#include <set>
#include <map>
#include <string>
#include <vector>

using namespace std;


class OntyxPropertyDef 
{
	private:

		string m_propertyname;						// string name of property
		string m_code;								// database code of property
		long m_id;									// database id of property
		string m_range;								// data type of property value in database
													//   takes string, long_string, and
													//	 real_long_string
		bool m_localized;							// NOT CURRENTLY USED IN TDE, WATCH OUT FOR
													// WHEN IT DOES
		bool m_containsIndex;						// indexed in database if true 
		long m_namespaceId;							// id of the property's namespace, external
		vector<string> m_picklist;

		static long mapPropDefID;					// internal id, for search purposes
		static map<long, string> * pMapPropDefID;	// to search by internal id
		static map<string, long> * pMapPropDefName;	// to search by name

		static set<string> * pSetCode;				// to verify a code is not in use
		static set<long> * pSetID;					// to verify an id is not in use

		static long propDefError;					// set if object not yet created and
													// static functions try to access maps

			// ********** Private Utility Functions 

		void initializeMaps(void);
		void addPropDefToMap(void);

		bool codeInSet (const string &) const;
		bool idInSet (long) const;

	public:

			// ********** Constructor/Destructor

		// default constructor calls OntyxName::getNamespaceMapID(inSpace) 
		OntyxPropertyDef (	const string &	name = "", 
							const string &	code = "", 
							long			id = -1, 
							const string &	inRange = "", 
							bool			localized = false, 
							bool			indexed = false, 
							const string &	inSpace = "");

		OntyxPropertyDef(const OntyxPropertyDef & );

		virtual ~OntyxPropertyDef();

			// ********** Accessor Functions

		const string & getName (void) const;
		const string & getCode (void) const;
		const string & getRange (void) const; 

		const string & getNamespace(void) const;
		long getNamespaceID (void) const;
		vector<string> getPickList(void) const;

		long getID (void) const;
		long getMapID (void) const;

		bool isContainsIndex(void) const;
		bool isLocalized(void) const;
		bool hasPickList(void) const;

			// ********** Mutator Functions

		OntyxPropertyDef & setID(long);
		OntyxPropertyDef & setCode(const string &);
		OntyxPropertyDef & setPickList(vector<string> &);

		void clearID (void);
		void clearCode (void);

			// ********** Comparison Functions

		bool isSame(const OntyxPropertyDef &) const;

			// ********** Relational Operators 

		bool operator== (const OntyxPropertyDef &) const;
		bool operator< (const OntyxPropertyDef &) const;
	
			// ********** Utility Functions 

		static void setError(long errorCode);
		static void clearError(long errorCode);
		static void clearError(void);
		static bool isError(void);
		static bool isError(long ErrorCode);
		static void printError (void);

		static long getPropDefMapID(const string &);
		static const string & getPropDefMapName(long id);
		static bool propDefInMap(const string &);		// true if m_propertyname is in map

};
#endif
