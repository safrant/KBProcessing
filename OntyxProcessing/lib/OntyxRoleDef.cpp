//*******************************************************************
// Implementation of the Ontyx role definition class OntyxRoleDef
//
//		Initial class definition 4/15/2001
//		See header file for additional information
//
//*******************************************************************

#include "OntyxRoleDef.h"
#include "OntyxName.h"
#include "OntyxTerminology.h"

#include <iostream>
#include <map>
#include <set>
#include <string>
#include <cstdlib>

using namespace std;


			// ********** Static Data

long OntyxRoleDef::roleDefError = 0;
long OntyxRoleDef::mapRoleDefID = 0;

typedef map<long, string> mapID;
mapID * OntyxRoleDef::pMapRoleDefID = 0;					// to search by id

typedef map<string, long> mapName;
mapName * OntyxRoleDef::pMapRoleDefName = 0;				// to search by role name

set<long> * OntyxRoleDef::pSetID = 0;						// verify an external id is not in use
set<string> * OntyxRoleDef::pSetCode = 0;					// verify a code is not in use


			// ********** Constructor/Destructor

OntyxRoleDef::OntyxRoleDef (	const string &	name, 
								const string &	code, 
								long			id,
								const string &	inSpace, 
								const string &	inDomain, 
								const string &	inRange,
								const string &	right,
								const string & 	parent)
		: m_rolename(name), m_code(code), m_id(id), m_right(right), m_parentrole(parent)
{
	m_namespaceId = OntyxName::getNamespaceMapID(inSpace);
	if( OntyxName::isError(OntyxTerminology::NO_NAMESPACE) )
		setError(OntyxTerminology::NO_NAMESPACE);

	m_domainId = OntyxKind::getKindMapID(inDomain);
	m_rangeId = OntyxKind::getKindMapID(inRange);
	if( OntyxKind::isError(OntyxTerminology::NO_KIND) )
		setError(OntyxTerminology::NO_KIND);

	if( mapRoleDefID == 0 )
		initializeMaps();
	if( pSetCode->count(code) != 0 )
		setError(OntyxTerminology::CODE_EXISTS);
	if( pSetID->count(id) != 0 )
		setError(OntyxTerminology::ID_EXISTS);
	if( roleDefInMap(name) )
		setError(OntyxTerminology::NAME_EXISTS);
	addRoleDefToMap();
	pSetID->insert(id);
	pSetCode->insert(code);
	mapRoleDefID++;
}
OntyxRoleDef::OntyxRoleDef(const OntyxRoleDef & inRight)		// copy constructor
{
	m_rolename = inRight.m_rolename;
	m_code = inRight.m_code;
	m_id = inRight.m_id;
	m_namespaceId = inRight.m_namespaceId;
	m_domainId = inRight.m_domainId;
	m_rangeId = inRight.m_rangeId;
	m_right = inRight.m_right;
	m_parentrole = inRight.m_parentrole;
	mapRoleDefID++;
}
OntyxRoleDef::~OntyxRoleDef()
{
	mapRoleDefID--;
	if( mapRoleDefID == 0) {
		delete pMapRoleDefID;
		delete pMapRoleDefName;
		delete pSetID;
		delete pSetCode;
		}
}

			// ********** Accessor Functions

const string & OntyxRoleDef::getName (void) const
	{ return m_rolename; }

const string & OntyxRoleDef::getCode (void) const
	{ return m_code; }

const string & OntyxRoleDef::getRight (void) const
	{ return m_right; }

long OntyxRoleDef::getID (void) const
	{ return m_id; }

long OntyxRoleDef::getMapID (void) const
{
	return (pMapRoleDefName->find(m_rolename))->second;
}
long OntyxRoleDef::getNamespaceID (void) const
	{ return m_namespaceId; }

const string & OntyxRoleDef::getNamespace (void) const
{
	if( isError(OntyxTerminology::NO_NAMESPACE) )
		return OntyxTerminology::emptyString;
	else
		return OntyxName::getNamespaceMapName(m_namespaceId);
}
long OntyxRoleDef::getDomainID (void) const
	{ return m_domainId; }

const string & OntyxRoleDef::getDomain (void) const
{
	if( isError(OntyxTerminology::NO_KIND) )
		return OntyxTerminology::emptyString;
	else
		return OntyxKind::getKindMapName(m_domainId);
}
long OntyxRoleDef::getRangeID (void) const
	{ return m_rangeId; }

const string & OntyxRoleDef::getRange (void) const
{
	if( isError(OntyxTerminology::NO_KIND) )
		return OntyxTerminology::emptyString;
	else
		return OntyxKind::getKindMapName(m_rangeId);
}
const string & OntyxRoleDef::getParentRole (void) const
	{ return m_parentrole; }
	

			// ********** Mutator Functions

OntyxRoleDef & OntyxRoleDef::setCode (const string & inCode)
{
	if( pSetCode->count(inCode) != 0 )
		setError(OntyxTerminology::CODE_EXISTS);
	else
		m_code = inCode;
	return *this;
}
OntyxRoleDef & OntyxRoleDef::setID (long inID)
{
	if( pSetID->count(inID) != 0 )
		setError(OntyxTerminology::ID_EXISTS);
	else
		m_id = inID;
	return *this;
}
void OntyxRoleDef::clearID (void)
{
	m_id = -1;
}
void OntyxRoleDef::clearCode (void)
{
	m_code = "";
}

			// ********** Comparison Functions

bool OntyxRoleDef::isSame (const OntyxRoleDef & right) const
{
	return ( m_rolename == right.m_rolename ?
				( m_code == right.m_code ?
					( m_id == right.m_id ?
						( m_namespaceId == right.m_namespaceId ?
							( m_domainId == right.m_domainId ?
								( m_rangeId == right.m_rangeId ?
									( m_parentrole == right.m_parentrole ?
										( m_right == right.m_right ? true : false)
									: false)
								: false)
							: false)
						: false)
					: false)
				: false)
			: false);
}

bool OntyxRoleDef::hasParent (void) const
{
	if( m_parentrole == "" )
		return false;
	return true;
}
			// ********** Relational Operators 

bool OntyxRoleDef::operator== (const OntyxRoleDef & inRight) const
	{ return ( m_rolename == inRight.m_rolename ? true : false ); }

bool OntyxRoleDef::operator< (const OntyxRoleDef & inRight) const
	{ return ( m_rolename < inRight.m_rolename ? true : false ); }
	

			// ********** Private Utility Functions 

void OntyxRoleDef::initializeMaps(void)
{
	pMapRoleDefID = new mapID; 
	pMapRoleDefName = new mapName;
	pSetID = new set<long>;
	pSetCode = new set<string>;
	if( pMapRoleDefID == 0 || pMapRoleDefName == 0 || pSetID == 0 || pSetCode == 0 ) {
		cerr << "Could not allocate memory for a map or a set.  Will EXIT." << endl;
		exit(0);
		}
}
void OntyxRoleDef::addRoleDefToMap(void)
{
	pMapRoleDefID->insert(mapID::value_type(mapRoleDefID, m_rolename));
	pMapRoleDefName->insert(mapName::value_type(m_rolename, mapRoleDefID));
}

bool OntyxRoleDef::codeInSet (const string & inCode) const
{
	if( pSetCode->count(inCode) != 0 )
		return true;
	return false;
}
bool OntyxRoleDef::idInSet (long inID) const
{
	if( pSetID->count(inID) != 0 )
		return true;
	return false;
}

			// ********** Public Utility Functions 

bool OntyxRoleDef::roleDefInMap(const string & roleName)			// true if m_rolename is in map
{
	if( pMapRoleDefName == 0 ) {
		setError(OntyxTerminology::NO_ROLEDEF);
		return false;
		}
	if( pMapRoleDefName->find(roleName) == pMapRoleDefName->end() )
		return false;
	return true;
}
long OntyxRoleDef::getRoleDefMapID(const string & roleName)
{
	if( pMapRoleDefName == 0 ) {
		setError(OntyxTerminology::NO_ROLEDEF);
		return -1;
		}
	static mapName::iterator iterMap;
	iterMap = pMapRoleDefName->find(roleName);
	if( iterMap == pMapRoleDefName->end() )
		return -1;
	return iterMap->second;
}
const string & OntyxRoleDef::getRoleDefMapName(long id)
{
	if( pMapRoleDefID == 0 ) {
		setError(OntyxTerminology::NO_ROLEDEF);
		return OntyxTerminology::emptyString;
		}
	static mapID::iterator iterID;
	iterID = pMapRoleDefID->find(id);
	if( iterID == pMapRoleDefID->end() )
		return OntyxTerminology::emptyString;
	return iterID->second;
}

void OntyxRoleDef::setError(long errorCode)
{
	roleDefError |= errorCode;
}
void OntyxRoleDef::clearError(long errorCode)
{
	roleDefError &= ~errorCode;
}
void OntyxRoleDef::clearError(void)
{
	roleDefError = 0;
}
bool OntyxRoleDef::isError(void)
{
	return ( roleDefError ? true : false);
}
bool OntyxRoleDef::isError(long errorCode)
{
	return ( (roleDefError & errorCode) ? true : false );
}

void OntyxRoleDef::printError (void)
{
	for( long i = 0; i < OntyxTerminology::numErrors; i++) {
		if( isError( OntyxTerminology::errorArray[i] ) )
			cerr << "Error class OntyxRoleDef " << OntyxTerminology::errorNames[i] << endl;
		}
}

