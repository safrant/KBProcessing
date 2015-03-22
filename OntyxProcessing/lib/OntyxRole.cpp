//*******************************************************************
// Implementation of the Ontyx role OntyxRole class
//
//		Initial class definition 4/15/2001
//		See header file for additional information
//
//*******************************************************************

#include "OntyxRole.h"
#include "OntyxRoleDef.h"
#include "OntyxRoleEnum.h"
#include "OntyxTerminology.h"

#include <string>
#include <iostream>

using namespace std;

long OntyxRole::roleError = 0;


			// ********** Constructor/Destructor

OntyxRole::OntyxRole (	const string &	inName, 
						const string &	inValue,
						const string &	inMod )
					: m_value(inValue), m_modifier(inMod)
{
	m_roleNameID = OntyxRoleDef::getRoleDefMapID(inName) ;
	if( OntyxRoleDef::isError(OntyxTerminology::NO_ROLEDEF) )
		setError(OntyxTerminology::NO_ROLEDEF);
	m_rolegroup = 0;										// default value, not in a role group
}
	
OntyxRole::OntyxRole (	const string &	inName, 
						const string &	inValue,
						const string &	inMod,
						long rolegroup )
					: m_value(inValue), m_modifier(inMod), m_rolegroup(rolegroup)
{
	m_roleNameID = OntyxRoleDef::getRoleDefMapID(inName) ;
	if( OntyxRoleDef::isError(OntyxTerminology::NO_ROLEDEF) )
		setError(OntyxTerminology::NO_ROLEDEF);
}
	
OntyxRole::OntyxRole(const OntyxRole & inRight)
{
	m_roleNameID = inRight.m_roleNameID;
	m_value = inRight.m_value;
	m_modifier = inRight.m_modifier;
	m_rolegroup = inRight.m_rolegroup;
}

OntyxRole::~OntyxRole()
{
	;
}

			// ********** Mutator Functions

OntyxRole & OntyxRole::setValue (const string & newVal)
{
	m_value = newVal;
	return *this;
}
OntyxRole & OntyxRole::setName (const string & newName)
{
	m_roleNameID = OntyxRoleDef::getRoleDefMapID(newName) ;
	if( OntyxRoleDef::isError(OntyxTerminology::NO_ROLEDEF) ) {
		setError(OntyxTerminology::NO_ROLEDEF);
		cerr << "ERROR:  OntyxRoleDef for '" << newName << "' not yet defined." << endl;
		}
	return *this;
}

OntyxRole & OntyxRole::setModififier (const string & modifier)
{
	m_modifier = OntyxRoleEnum(modifier);
	return *this;
}
OntyxRole & OntyxRole::setRoleGroup (long rolegroupID)
{
	m_rolegroup = rolegroupID;
	return *this;
}

			// ********** Accessor Functions

const string & OntyxRole::getValue (void) const
	{ return m_value; }

const string & OntyxRole::getName (void) const
{
	if( isError(OntyxTerminology::NO_ROLEDEF) )
		return OntyxTerminology::emptyString;
	else
		return OntyxRoleDef::getRoleDefMapName(m_roleNameID);
}

const string & OntyxRole::getModifier(void) const
	{ return m_modifier.getModifier('c'); }

long OntyxRole::getNameID (void) const
	{ return m_roleNameID; }

long OntyxRole::getRolegroup (void) const
	{ return m_rolegroup; }

bool OntyxRole::inRolegroup (void) const
{
	return (m_rolegroup == 0 ? true : false );
}


			// ********** Relational Operators 

bool OntyxRole::operator== (const OntyxRole & inRight) const	// applied to all member data 
{
	return ( m_roleNameID == inRight.m_roleNameID ?
				(m_value == inRight.m_value ? 
					( m_modifier == inRight.m_modifier ? 
						( m_rolegroup == inRight.m_rolegroup ? true : false )
					: false)
				: false)
			: false);
}
bool OntyxRole::operator< (const OntyxRole & inRight) const		// applied to all member data 
{
	return ( m_roleNameID < inRight.m_roleNameID ?
				(m_value < inRight.m_value ? 
					( m_modifier < inRight.m_modifier ? 
						( m_rolegroup < inRight.m_rolegroup ? true : false )
					: false)
				: false)
			: false);
}

			// ********** Output Friend Operators

ostream & operator<< (ostream & outStream, const OntyxRole & inRight)
{
	outStream << inRight.getModifier() << ' ' << inRight.getName() << ' ' << inRight.getValue();
	return outStream;
}

			// ********** Public Static Utility Functions 

void OntyxRole::setError(long errorCode)
{
	roleError |= errorCode;
}
void OntyxRole::clearError(long errorCode)
{
	roleError &= ~errorCode;
}
bool OntyxRole::isError(long errorCode)
{
	return ( (roleError & errorCode) ? true : false );
}
bool OntyxRole::isError (void)
{
	return ( roleError ? true : false );
}
void OntyxRole::clearError (void)
	{ roleError = 0; }

void OntyxRole::printError (void)
{
	for( long i = 0; i < OntyxTerminology::numErrors; i++) {
		if( isError( OntyxTerminology::errorArray[i] ) )
			cerr << "Error class OntyxRole " << OntyxTerminology::errorNames[i] << endl;
		}
}

