//*******************************************************************
// Definition of the Ontyx role OntyxRole class
// 		Defines role objects in the Ontyx terminology kb
//		Initial class definition 4/15/2001
//		Current based on "ontylog.dtd" from 10/19/01
//
// Dependencies:	STL <string> class
// Dependencies:	<iostream>
// Dependencies:	OntyxPropertyDef, OntyxRoleEnum classes
// Virtual:			~OntyxProperty ()
//*******************************************************************

#ifndef _ONTYX_ROLE_H_
#define _ONTYX_ROLE_H_

#include "OntyxRoleDef.h"
#include "OntyxRoleEnum.h"
#include "OntyxTerminology.h"

#include <string>
#include <iostream>

using namespace std;

class OntyxRole
{
	private:

		long			m_roleNameID;		// id of Role Definition object in external map
											// pMapRoleDefID or pMapRoleDefName, defined in OntyxRoleDef
		string			m_value;			// name of a concept, may use an id in the future
		OntyxRoleEnum	m_modifier;			// contained role modifier object

		long			m_rolegroup;		// not required by roles, this is a conceptDef issue but
											// it makes more sense to handle at the level of OntyxRole

		static long		roleError;

	public:

			// ********** Constructor/Destructor
		// default constructor calls OntyxRoleDef::getRoleDefMapID(namestring) 
		// to get role id
		OntyxRole (	const string &	inName = "", 
					const string &	inValue = "", 
					const string &	inMod = "");

		OntyxRole (	const string &	inName, 
					const string &	inValue, 
					const string &	inMod,
					long rolegroup);

		OntyxRole(const OntyxRole &);

		virtual ~OntyxRole();

			// ********** Mutator Functions

		OntyxRole & setValue (const string & );
		OntyxRole & setName (const string & newName); // generates error if newName not yet defined
		OntyxRole & setModififier (const string &);
		OntyxRole & setRoleGroup (long rolegroupID);
		
			// ********** Accessor Functions

		const string & getValue (void) const;
		const string & getModifier(void) const; // calls OntyxRoleEnum::getModifier('c') to get
												// role modifier string
		const string & getName (void) const;	// calls OntyxRoleDef::getRoleDefMapName(m_roleNameID)
												// to get name
		long getRolegroup (void) const;
		long getNameID (void) const;			// returns m_roleNameID

		bool inRolegroup (void) const;

			// ********** Relational Operators 

		bool operator== (const OntyxRole &) const;		// applied to all member data
		bool operator< (const OntyxRole &) const;		// applied to all member data


			// ********** Output Friend Operators

		friend ostream & operator<< (ostream &, const OntyxRole &);

			// ********** Public Static Utility Functions 

		static void setError (long errorCode);
		static void clearError (long errorCode);
		static void clearError (void);
		static bool isError (long ErrorCode);
		static bool isError (void);
		static void printError (void);

};
#endif


