//*******************************************************************
// Implementation of the Ontyx property definition class OntyxPropertyDef
//
//		Initial class definition 4/8/2001
//		See header file for additional information
//
//*******************************************************************

#include "OntyxPropertyDef.h"
#include "OntyxName.h"
#include "OntyxTerminology.h"

#include <map>
#include <set>
#include <string>
#include <iostream>
#include <vector>

using namespace std;


			// ********** Static Data

long OntyxPropertyDef::mapPropDefID = 0;
long OntyxPropertyDef::propDefError = 0;

typedef map<long, string> mapID;
mapID * OntyxPropertyDef::pMapPropDefID = 0;				// to search by id

typedef map<string, long> mapName;
mapName * OntyxPropertyDef::pMapPropDefName = 0;			// to search by name

set<string> * OntyxPropertyDef::pSetCode = 0;				// to verify a code is not in use
set<long> * OntyxPropertyDef::pSetID = 0;					// to verify an id is not in use


			// ********** Constructor/Destructor

		// default constructor calls OntyxName::getNamespaceMapID(Namespace) 
OntyxPropertyDef::OntyxPropertyDef (	const string &	name , 
										const string &	code , 
										long			id , 
										const string &	inRange ,
										bool			localized , 
										bool			indexed , 
										const string &	Namespace )
			: m_propertyname(name), m_code(code), m_id(id), m_range(inRange),
				m_localized(localized), m_containsIndex(indexed)
{
	m_namespaceId = OntyxName::getNamespaceMapID(Namespace);
	if( OntyxName::isError(OntyxTerminology::NO_NAMESPACE) )
		setError(OntyxTerminology::NO_NAMESPACE);
	if( mapPropDefID == 0 )
		initializeMaps();
	if( pSetCode->count(code) != 0 )
		setError(OntyxTerminology::CODE_EXISTS);
	if( pSetID->count(id) != 0 )
		setError(OntyxTerminology::ID_EXISTS);
	if( propDefInMap(name) )
		setError(OntyxTerminology::NAME_EXISTS);
	addPropDefToMap();
	pSetID->insert(id);
	pSetCode->insert(code);
	m_picklist.clear();
	mapPropDefID++;
}

OntyxPropertyDef::OntyxPropertyDef(const OntyxPropertyDef & right)
{
	m_propertyname = right.m_propertyname;
	m_code = right.m_code;
	m_range = right.m_range;
	m_id = right.m_id;
	m_namespaceId = right.m_namespaceId;
	m_containsIndex = right.m_containsIndex;
	m_localized = right.m_localized;
	m_picklist = right.m_picklist;
	mapPropDefID++;
}

OntyxPropertyDef::~OntyxPropertyDef()
{
	mapPropDefID--;
	if( mapPropDefID == 0) {
		delete pMapPropDefID;
		delete pMapPropDefName;
		delete pSetID;
		delete pSetCode;
		}
}

			// ********** Accessor Functions

const string & OntyxPropertyDef::getName (void) const
	{ return m_propertyname; }

const string & OntyxPropertyDef::getCode (void) const
	{ return m_code; }

long OntyxPropertyDef::getID (void) const
	{ return m_id; }

long OntyxPropertyDef::getMapID (void) const
{
	return (pMapPropDefName->find(m_propertyname))->second;
}
long OntyxPropertyDef::getNamespaceID (void) const
	{ return m_namespaceId; }

const string & OntyxPropertyDef::getNamespace(void) const
{
	if( isError(OntyxTerminology::NO_NAMESPACE) )
		return OntyxTerminology::emptyString;
	else
		return OntyxName::getNamespaceMapName(m_namespaceId);
}

const string & OntyxPropertyDef::getRange (void) const
	{ return m_range; }

vector<string> OntyxPropertyDef::getPickList(void) const
	{ return m_picklist; }

bool OntyxPropertyDef::isContainsIndex(void) const
	{ return m_containsIndex; }

bool OntyxPropertyDef::isLocalized(void) const
	{ return m_localized; }

bool OntyxPropertyDef::hasPickList(void) const
{
	if( m_picklist.empty() )
		return false;
	return true;
}

			// ********** Mutator Functions

OntyxPropertyDef & OntyxPropertyDef::setCode (const string & inCode)
{
	if( pSetCode->count(inCode) != 0 )
		setError(OntyxTerminology::CODE_EXISTS);
	else
		m_code = inCode;
	return *this;
}
OntyxPropertyDef & OntyxPropertyDef::setID (long inID)
{
	if( pSetID->count(inID) != 0 )
		setError(OntyxTerminology::ID_EXISTS);
	else
		m_id = inID;
	return *this;
}
OntyxPropertyDef & OntyxPropertyDef::setPickList(vector<string> & inList)
{
	m_picklist = inList;
	return *this;
}

void OntyxPropertyDef::clearID (void)
{
	m_id = -1;
}
void OntyxPropertyDef::clearCode (void)
{
	m_code = "";
}

			// ********** Comparison Functions

bool OntyxPropertyDef::isSame(const OntyxPropertyDef & right) const
{
	return ( m_propertyname == right.m_propertyname ?
				( m_code == right.m_code ?
					( m_id == right.m_id ?
						( m_range == right.m_range ?
							( m_namespaceId == right.m_namespaceId ?
								( m_containsIndex == right.m_containsIndex ?
									( m_localized == right.m_localized ? true : false)
								: false)
							: false)
						: false)
					: false)
				: false)
			: false);
}


			// ********** Relational Operators 

bool OntyxPropertyDef::operator== (const OntyxPropertyDef & right) const
	{ return ( m_propertyname == right.m_propertyname ? true : false) ; }

bool OntyxPropertyDef::operator< (const OntyxPropertyDef & right) const
	{ return ( m_propertyname < right.m_propertyname ? true : false) ; }
	

			// ********** Private Utility Functions 

void OntyxPropertyDef::initializeMaps(void)
{
	pMapPropDefID = new mapID; 
	pMapPropDefName = new mapName;
	pSetID = new set<long>;
	pSetCode = new set<string>;
	if( pMapPropDefID == 0 || pMapPropDefName == 0 || pSetID == 0 || pSetCode == 0 ) {
		cerr << "Could not allocate memory for a map or a set.  Will EXIT." << endl;
		exit(0);
		}
}
void OntyxPropertyDef::addPropDefToMap(void)
{
	pMapPropDefID->insert(mapID::value_type(mapPropDefID, m_propertyname));
	pMapPropDefName->insert(mapName::value_type(m_propertyname, mapPropDefID));
}

bool OntyxPropertyDef::codeInSet (const string & inCode) const
{
	if( pSetCode->count(inCode) != 0 )
		return true;
	return false;
}
bool OntyxPropertyDef::idInSet (long inID) const
{
	if( pSetID->count(inID) != 0 )
		return true;
	return false;
}
			// ********** Public Static Functions 

long OntyxPropertyDef::getPropDefMapID(const string & propName)
{
	if( pMapPropDefName == 0 ) {
		setError(OntyxTerminology::NO_PROPDEF);
		return -1;
		}
	static mapName::iterator iterMap;
	iterMap = pMapPropDefName->find(propName);
	if( iterMap == pMapPropDefName->end() )
		return -1;
	return iterMap->second;
}
const string & OntyxPropertyDef::getPropDefMapName(long id)
{
	if( pMapPropDefID == 0 ) {
		setError(OntyxTerminology::NO_PROPDEF);
		return OntyxTerminology::emptyString;
		}
	static mapID::iterator iterID;
	iterID = pMapPropDefID->find(id);
	if( iterID == pMapPropDefID->end() )
		return OntyxTerminology::emptyString;
	return iterID->second;
}
bool OntyxPropertyDef::propDefInMap(const string & propName)		// true if m_propertyname is in map
{
	if( pMapPropDefName == 0 ) {
		setError(OntyxTerminology::NO_PROPDEF);
		return false;
		}
	if( pMapPropDefName->find(propName) == pMapPropDefName->end() )
		return false;
	return true;
}
void OntyxPropertyDef::clearError(void)
{
	propDefError = 0;
}
bool OntyxPropertyDef::isError(void)
{
	return ( propDefError ? true : false ) ;
}

void OntyxPropertyDef::setError(long errorCode)
{
	propDefError |= errorCode;
}
void OntyxPropertyDef::clearError(long errorCode)
{
	propDefError &= ~errorCode;
}
bool OntyxPropertyDef::isError(long errorCode)
{
	return ( (propDefError & errorCode) ? true : false );
}

void OntyxPropertyDef::printError (void)
{
	for( long i = 0; i < OntyxTerminology::numErrors; i++) {
		if( isError( OntyxTerminology::errorArray[i] ) )
			cerr << "Error class OntyxPropertyDef " << OntyxTerminology::errorNames[i] << endl;
		}
}



