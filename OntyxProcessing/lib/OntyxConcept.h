//*******************************************************************
// Definition of the Ontyx class OntyxConcept
// 		Defines concepts in the Ontyx terminology kb
//		Initial class definition 4/11/2001
//
// Dependencies:	STL <string>, <set> class
// Dependencies:	<iostream>, <iomanip> class
// Dependencies:	OntyxRole, OntyxProperty, OntyxName, OntyxKind
// Virtual:			~OntyxConcept()
//*******************************************************************

#ifndef _ONTYX_CONCEPT_H
#define _ONTYX_CONCEPT_H

#include "OntyxKind.h"
#include "OntyxName.h"
#include "OntyxRole.h"
#include "OntyxProperty.h"
#include "OntyxAssociation.h"

#include <iostream>
#include <iomanip>
#include <string>
#include <map>
#include <set>
#include <vector>
#include <utility>
#include <cstdlib>

using namespace std;

class OntyxKb;		// forward declaration

class OntyxConcept 
{
	private:

		string m_name;						// database name of concept
		string m_code;						// database code of concept
		long m_id;							// database id of concept
		long m_namespaceID;					// id of namespace in OntyxName
		long m_kindID;						// id of kind in OntyxKind, -1 if none
		bool m_state;						// defined or primitive, true is defined
		set<string> m_parent;				// database name(s) of parent(s) (defining concepts),
											// 		(concept names of parents are unique)
		long m_myID;						// used for internal ID of concept, used in maps
		multimap<long, OntyxRole> m_role;		// OntyxRole(s), uses ID from definition map for key
		multimap<long, OntyxProperty> m_property;	// OntyxProperty(ies), uses ID from definition map for key
		multimap<long, OntyxAssociation> m_associations;  // OntyxAssociation, ID from definition map for key
		map<long, long>	m_rolegroups;		//  {rolegroupID, numberInRoleGroup}, size()-1 = number of role groups

		static long conceptError;

		static long mapConceptID;
		static map<long, string> * pMapConceptID;
		static multimap<string, long> * pMapConceptName;
		static set<string> * pSetCode;
		static set<long> * pSetID;

			// ********** Private Utility Functions 

		void initializeMaps (void);
		void addConceptToMap (void);

		bool codeInSet (const string &) const;
		bool idInSet (long) const;

	public:

			// ********** Constructor/Destructor

		OntyxConcept (	const string &	name = "", 
						const string &	code = "", 
						long			id = -1,
						const string &	inNamespace = "",
						const string &	inKind = "",
						bool			inState = false);

		OntyxConcept (const OntyxConcept & );		// copy constructor
	
		virtual ~OntyxConcept ();

			// ********** Assignment Operator

		OntyxConcept & operator= (const OntyxConcept &);

			// ********** Mutator Functions

		OntyxConcept & setName (const string &);
		OntyxConcept & reName (const string &);
		OntyxConcept & setCode (const string &);
		OntyxConcept & setID (long);
		OntyxConcept & setNamespace (const string &);
		OntyxConcept & setKind (const string &);
		OntyxConcept & addParent (const string &);
		OntyxConcept & addRole (const OntyxRole & );
		OntyxConcept & addProperty (const OntyxProperty & );
		OntyxConcept & addParents (vector<string> &);
		OntyxConcept & addRoles (vector<OntyxRole> & );
		OntyxConcept & addProperties (vector<OntyxProperty> & );
		OntyxConcept & setPropertyValue 
			(multimap<long, OntyxProperty>::iterator pProp, const string & value);
		OntyxConcept & setRoleValue 
			(multimap<long, OntyxRole>::iterator pRole, const string & value);
		OntyxConcept & addAssociations (vector<OntyxAssociation> & );
		OntyxConcept & addAssociation (const OntyxAssociation & inAssociation);

		OntyxConcept & deleteRole (const string & name, const string & value);
		OntyxConcept & deleteRole (const string & name);
		OntyxConcept & deleteRole (const OntyxRole & inRole);
		OntyxConcept & deleteAllRoles (void) ;
		OntyxConcept & deleteProperty (const string & name, const string & value);
		OntyxConcept & deleteProperty (const OntyxProperty & inProp);
		OntyxConcept & deleteProperty (const string & name);	// deletes all properties of type "name"
		OntyxConcept & deleteAllProperties (void);
		OntyxConcept & deleteParent (const string & name);
		OntyxConcept & deleteAssociation (const OntyxAssociation & compAssoc);
		OntyxConcept & deleteAllAssociations (void);

		OntyxConcept & setDefined (void);					// sets state to defined or primitive
		OntyxConcept & setPrimitive (void);					// sets state to primitive
		OntyxConcept & setState (bool state);				// true sets defined, false is primitive

		void clear (void);									// re-initializes the concept
		void clearID (void);
		void clearCode (void);
		void clearKind (void);								// sets to -1

		void resetRoleValueIfValue (const string & ifVal, const string & newVal);
		void resetRoleNameIfName (const string & ifName, const string & newName); // OntyxRole generates error if newName not yet defined
		void addRolesTo (OntyxConcept & toConcept) const;

		void resetAssociationValueIfValue (const string & oldVal, const string & newVal);

		void resetParentIfParent (const string & oldPar, const string & newPar);
		void convertAllParentToProperty (const string & propName);
		void convertToProperty (const string & value, const string & propName);  
		bool convertAllRoleToProperty (void);  

		void mergePropertiesFrom (const OntyxConcept & );
		void convertToSanctionedRoleOrProperty (const string & inKind, 
												map<string, string> & mapping,
												const string & oldRoleName,
												const string & newPropertyName);
		void convertToNonredundantProperty (void) ;
		void convertToNonredundantRole (void);


			// ********** Accessor Functions

		const string & getName (void) const;
		const string & getCode (void) const;
		long getID (void) const;
		string getID (char) const;							// signature change, returns ID as string
		const string & getNamespace (void) const;
		const string & getKind (void) const;
		long getMapID (void) const;							// returns the internal ID of concept

		const set<string> & getParents (void) const;
		long getNumParents (void) const;
		set<string>::const_iterator getFirstParent (void) const;
		set<string>::const_iterator getEndParent (void) const;

		long getNumRoleGroups (void) const;

		const multimap<long, OntyxRole> & getRoles (void) const;
		long getNumRoles (void) const;
		multimap<long, OntyxRole>::const_iterator getFirstRole (void) const;
		multimap<long, OntyxRole>::const_iterator getEndRole (void) const;

		const multimap<long, OntyxProperty> & getProperties (void) const;
		long getNumProperties (void) const;
		multimap<long, OntyxProperty>::const_iterator getFirstProperty (void) const;
		multimap<long, OntyxProperty>::const_iterator getEndProperty (void) const;

		bool getState (void) const;							// defined or primitive, true is defined

		multimap<long, OntyxProperty>::const_iterator findProperty (const string & name) const;
		multimap<long, OntyxProperty>::iterator propertyIterator (const string & name);

		string getPropertyValue (const string & name) const;	// returns value for 1st "name" found
		long getPropertyNumber (const string & name) const;		// returns number of props of type "name"

		long getNumAssociations (void) const;
		const multimap<long, OntyxAssociation> & getAssociations (void) const;

			// ********** Comparison Functions // appropriate comparisons for associations not yet included

		bool hasSameName (const OntyxConcept &) const;
		bool hasSameCode (const OntyxConcept &) const;
		bool hasSameID (const OntyxConcept &) const;
		bool hasSameNamespace (const OntyxConcept &) const;
		bool hasSameKind (const OntyxConcept &) const;
		bool hasSameParents (const OntyxConcept &) const;
		bool hasSameRoles (const OntyxConcept &) const;
		bool hasLessThanRoles ( const OntyxConcept &) const;
		bool hasSameProperties (const OntyxConcept &) const;
		bool hasLessThanProperties (const OntyxConcept &) const;
		bool hasSameState (const OntyxConcept &) const;
		bool isSame (const OntyxConcept & ) const;

		bool hasSameAssociations (const OntyxConcept &) const;
		bool hasLessThanAssociations (const OntyxConcept &) const;

		bool hasRole (const string & roleName) const;
		bool hasRole (const OntyxRole & inRole) const;
		bool hasRolegroups (void) const;
		bool hasAllRolesInGroup (void) const;

			// ********** Relational Operators 

		bool operator== (const OntyxConcept &) const;		// applied to all members
		bool operator< (const OntyxConcept &) const;		// applied to all members

			// ********** Output Friend Operator

		friend ostream & operator<< (ostream &, const OntyxConcept &);

			// ********** Public Static Utility Functions 

		static void setError (long errorCode);
		static void clearError (long errorCode);
		static void clearError (void);
		static bool isError (long ErrorCode);
		static bool isError (void);
		static void printError (void);

		static long getConceptMapID (const string &);
		static const string & getConceptMapName (long id);
		static bool conceptInMap (const string &);

};
#endif

