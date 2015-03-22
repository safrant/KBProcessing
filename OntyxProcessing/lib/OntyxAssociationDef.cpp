//*******************************************************************
// Implementation of the Ontyx associationn definition class OntyxAssociationDef
//
//		Initial class definition 3/5/2005 
//		See header file for additional information
//
//*******************************************************************

#include "OntyxAssociationDef.h"
#include "OntyxName.h"
#include "OntyxTerminology.h"

#include <map>
#include <set>
#include <string>
#include <iostream>

using namespace std;


			// ********** Static Data

long OntyxAssociationDef::mapAssocDefID = 0;
long OntyxAssociationDef::assocDefError = 0;

typedef map<long, string> mapID;
mapID * OntyxAssociationDef::pMapAssocDefID = 0;				// to search by id

typedef map<string, long> mapName;
mapName * OntyxAssociationDef::pMapAssocDefName = 0;			// to search by name

set<string> * OntyxAssociationDef::pSetCode = 0;				// to verify a code is not in use
set<long> * OntyxAssociationDef::pSetID = 0;					// to verify an id is not in use


			// ********** Constructor/Destructor

		// default constructor calls OntyxName::getNamespaceMapID(Namespace) 
OntyxAssociationDef::OntyxAssociationDef (	const string &	name , 
										const string &	code , 
										long			id , 
										const string & inverse,
										bool display,
										const string &	Namespace )
			: m_associationname(name), m_code(code), m_id(id), 
				m_inversename(inverse), m_displayable(display)
{
	m_namespaceId = OntyxName::getNamespaceMapID(Namespace);
	if( OntyxName::isError(OntyxTerminology::NO_NAMESPACE) )
		setError(OntyxTerminology::NO_NAMESPACE);
	if( mapAssocDefID == 0 )
		initializeMaps();
	if( pSetCode->count(code) != 0 )
		setError(OntyxTerminology::CODE_EXISTS);
	if( pSetID->count(id) != 0 )
		setError(OntyxTerminology::ID_EXISTS);
	if( assocDefInMap(name) )
		setError(OntyxTerminology::NAME_EXISTS);
	addAssocDefToMap();
	pSetID->insert(id);
	pSetCode->insert(code);
	mapAssocDefID++;
}

OntyxAssociationDef::OntyxAssociationDef(const OntyxAssociationDef & right)
{
	m_associationname = right.m_associationname;
	m_code = right.m_code;
	m_id = right.m_id;
	m_namespaceId = right.m_namespaceId;
	m_inversename = right.m_inversename;
	m_displayable = right.m_displayable;
	mapAssocDefID++;
}

OntyxAssociationDef::~OntyxAssociationDef()
{
	mapAssocDefID--;
	if( mapAssocDefID == 0) {
		delete pMapAssocDefID;
		delete pMapAssocDefName;
		delete pSetID;
		delete pSetCode;
		}
}

			// ********** Accessor Functions

const string & OntyxAssociationDef::getName (void) const
	{ return m_associationname; }

const string & OntyxAssociationDef::getCode (void) const
	{ return m_code; }

const string & OntyxAssociationDef::getInverse (void) const
	{ return m_inversename; }

bool OntyxAssociationDef::isDisplayable (void) const
	{ return m_displayable; }

long OntyxAssociationDef::getID (void) const
	{ return m_id; }

long OntyxAssociationDef::getMapID (void) const
{
	return (pMapAssocDefName->find(m_associationname))->second;
}
long OntyxAssociationDef::getNamespaceID (void) const
	{ return m_namespaceId; }

const string & OntyxAssociationDef::getNamespace(void) const
{
	if( isError(OntyxTerminology::NO_NAMESPACE) )
		return OntyxTerminology::emptyString;
	else
		return OntyxName::getNamespaceMapName(m_namespaceId);
}

bool OntyxAssociationDef::hasInverse(void)
{
	if( m_inversename == "" )
		return false;
	return true;
}

			// ********** Mutator Functions

OntyxAssociationDef & OntyxAssociationDef::setCode (const string & inCode)
{
	if( pSetCode->count(inCode) != 0 )
		setError(OntyxTerminology::CODE_EXISTS);
	else
		m_code = inCode;
	return *this;
}
OntyxAssociationDef & OntyxAssociationDef::setID (long inID)
{
	if( pSetID->count(inID) != 0 )
		setError(OntyxTerminology::ID_EXISTS);
	else
		m_id = inID;
	return *this;
}
OntyxAssociationDef & OntyxAssociationDef::setInverse(const string & name)
{
	m_inversename = name;
	return *this;
}

OntyxAssociationDef & OntyxAssociationDef::setDisplayable(bool value)
{
	m_displayable = value;
	return *this;
}

void OntyxAssociationDef::clearID (void)
{
	m_id = -1;
}
void OntyxAssociationDef::clearCode (void)
{
	m_code = "";
}

			// ********** Comparison Functions

bool OntyxAssociationDef::isSame(const OntyxAssociationDef & right) const
{
	return ( m_associationname == right.m_associationname ?
				( m_code == right.m_code ?
					( m_id == right.m_id ?
						( m_displayable == right.m_displayable ?
							( m_inversename == right.m_inversename ?
								( m_namespaceId == right.m_namespaceId ? true : false)
							: false)
						: false)
					: false)
				: false)
			: false);
}


			// ********** Relational Operators 

bool OntyxAssociationDef::operator== (const OntyxAssociationDef & right) const
	{ return ( m_associationname == right.m_associationname ? true : false) ; }

bool OntyxAssociationDef::operator< (const OntyxAssociationDef & right) const
	{ return ( m_associationname < right.m_associationname ? true : false) ; }
	

			// ********** Private Utility Functions 

void OntyxAssociationDef::initializeMaps(void)
{
	pMapAssocDefID = new mapID; 
	pMapAssocDefName = new mapName;
	pSetID = new set<long>;
	pSetCode = new set<string>;
	if( pMapAssocDefID == 0 || pMapAssocDefName == 0 || pSetID == 0 || pSetCode == 0 ) {
		cerr << "Could not allocate memory for a map or a set.  Will EXIT." << endl;
		exit(0);
		}
}
void OntyxAssociationDef::addAssocDefToMap(void)
{
	pMapAssocDefID->insert(mapID::value_type(mapAssocDefID, m_associationname));
	pMapAssocDefName->insert(mapName::value_type(m_associationname, mapAssocDefID));
}

bool OntyxAssociationDef::codeInSet (const string & inCode) const
{
	if( pSetCode->count(inCode) != 0 )
		return true;
	return false;
}
bool OntyxAssociationDef::idInSet (long inID) const
{
	if( pSetID->count(inID) != 0 )
		return true;
	return false;
}
			// ********** Public Static Functions 

long OntyxAssociationDef::getAssocDefMapID(const string & propName)
{
	if( pMapAssocDefName == 0 ) {
		setError(OntyxTerminology::NO_PROPDEF);
		return -1;
		}
	static mapName::iterator iterMap;
	iterMap = pMapAssocDefName->find(propName);
	if( iterMap == pMapAssocDefName->end() )
		return -1;
	return iterMap->second;
}
const string & OntyxAssociationDef::getAssocDefMapName(long id)
{
	if( pMapAssocDefID == 0 ) {
		setError(OntyxTerminology::NO_PROPDEF);
		return OntyxTerminology::emptyString;
		}
	static mapID::iterator iterID;
	iterID = pMapAssocDefID->find(id);
	if( iterID == pMapAssocDefID->end() )
		return OntyxTerminology::emptyString;
	return iterID->second;
}
bool OntyxAssociationDef::assocDefInMap(const string & propName)		// true if m_associationname is in map
{
	if( pMapAssocDefName == 0 ) {
		setError(OntyxTerminology::NO_PROPDEF);
		return false;
		}
	if( pMapAssocDefName->find(propName) == pMapAssocDefName->end() )
		return false;
	return true;
}
void OntyxAssociationDef::clearError(void)
{
	assocDefError = 0;
}
bool OntyxAssociationDef::isError(void)
{
	return ( assocDefError ? true : false ) ;
}

void OntyxAssociationDef::setError(long errorCode)
{
	assocDefError |= errorCode;
}
void OntyxAssociationDef::clearError(long errorCode)
{
	assocDefError &= ~errorCode;
}
bool OntyxAssociationDef::isError(long errorCode)
{
	return ( (assocDefError & errorCode) ? true : false );
}

void OntyxAssociationDef::printError (void) 				// using property error codes
{
	for( long i = 0; i < OntyxTerminology::numErrors; i++) {
		if( isError( OntyxTerminology::errorArray[i] ) )
			cerr << "Error class OntyxAssociationDef " << OntyxTerminology::errorNames[i] << endl;
		}
}



