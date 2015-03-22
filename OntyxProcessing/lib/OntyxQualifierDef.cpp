//*******************************************************************
// Implementation of the Ontyx qualifier definition class OntyxQualifierDef
//
//		Initial class definition 3/5/2005 
//		See header file for additional information
//
//*******************************************************************

#include "OntyxQualifierDef.h"
#include "OntyxName.h"
#include "OntyxTerminology.h"

#include <map>
#include <set>
#include <string>
#include <iostream>
#include <vector>

using namespace std;


			// ********** Static Data

long OntyxQualifierDef::mapQualifierDefID = 0;
long OntyxQualifierDef::qualifierDefError = 0;

typedef map<long, string> mapID;
mapID * OntyxQualifierDef::pMapQualifierDefID = 0;				// to search by id

typedef map<string, long> mapName;
mapName * OntyxQualifierDef::pMapQualifierDefName = 0;			// to search by name

set<string> * OntyxQualifierDef::pSetCode = 0;				// to verify a code is not in use
set<long> * OntyxQualifierDef::pSetID = 0;					// to verify an id is not in use

typedef map<long, OntyxQualifierDef *> mapObj;
mapObj * OntyxQualifierDef::pQualifierDef = 0;						// to aid in pick list validation


			// ********** Constructor/Destructor

		// default constructor calls OntyxName::getNamespaceMapID(Namespace) 
OntyxQualifierDef::OntyxQualifierDef (	const string &	name , 
										const string &	code , 
										long			id , 
										const string &	Namespace,
										const string &  inType,
										vector<string> inPickList)
			: m_qualifiername(name), m_code(code), m_id(id), pickList(inPickList) 
{
	m_namespaceId = OntyxName::getNamespaceMapID(Namespace);
	if( OntyxName::isError(OntyxTerminology::NO_NAMESPACE) )
		setError(OntyxTerminology::NO_NAMESPACE);
	if( mapQualifierDefID == 0 )
		initializeMaps();
	if( pSetCode->count(code) != 0 )
		setError(OntyxTerminology::CODE_EXISTS);
	if( pSetID->count(id) != 0 )
		setError(OntyxTerminology::ID_EXISTS);
	if( qualifierDefInMap(name) )
		setError(OntyxTerminology::NAME_EXISTS);
	if( inType == "CA" || inType == "CR" || inType == "CP" )
		m_qualifiertype = inType; 
	else setError(OntyxTerminology::NO_PROPDEF);		// recycling error codes, maybe fix in future

	addQualifierDefToMap();
	pSetID->insert(id);
	pSetCode->insert(code);
	mapQualifierDefID++;
}

OntyxQualifierDef::OntyxQualifierDef(const OntyxQualifierDef & right)
{
	m_qualifiername = right.m_qualifiername;
	m_code = right.m_code;
	m_id = right.m_id;
	m_namespaceId = right.m_namespaceId;
	m_qualifiertype = right.m_qualifiertype;
	pickList = right.pickList;
	mapQualifierDefID++;
}

OntyxQualifierDef::~OntyxQualifierDef()
{
	mapQualifierDefID--;
	if( mapQualifierDefID == 0) {
		delete pMapQualifierDefID;
		delete pMapQualifierDefName;
		delete pSetID;
		delete pSetCode;
		delete pQualifierDef;
		}
}

			// ********** Accessor Functions

const string & OntyxQualifierDef::getName (void) const
	{ return m_qualifiername; }

const string & OntyxQualifierDef::getCode (void) const
	{ return m_code; }

const string & OntyxQualifierDef::getQualifierType(void) const
	{ return m_qualifiertype; }

const vector<string> & OntyxQualifierDef::getPickList(void) const
	{ return pickList; }

long OntyxQualifierDef::getID (void) const
	{ return m_id; }

long OntyxQualifierDef::getMapID (void) const
{
	return (pMapQualifierDefName->find(m_qualifiername))->second;
}
long OntyxQualifierDef::getNamespaceID (void) const
	{ return m_namespaceId; }

const string & OntyxQualifierDef::getNamespace(void) const
{
	if( isError(OntyxTerminology::NO_NAMESPACE) )
		return OntyxTerminology::emptyString;
	else
		return OntyxName::getNamespaceMapName(m_namespaceId);
}


			// ********** Mutator Functions

OntyxQualifierDef & OntyxQualifierDef::setCode (const string & inCode)
{
	if( pSetCode->count(inCode) != 0 )
		setError(OntyxTerminology::CODE_EXISTS);
	else
		m_code = inCode;
	return *this;
}
OntyxQualifierDef & OntyxQualifierDef::setID (long inID)
{
	if( pSetID->count(inID) != 0 )
		setError(OntyxTerminology::ID_EXISTS);
	else
		m_id = inID;
	return *this;
}


void OntyxQualifierDef::clearID (void)
{
	m_id = -1;
}
void OntyxQualifierDef::clearCode (void)
{
	m_code = "";
}

			// ********** Comparison Functions

bool OntyxQualifierDef::isSame(const OntyxQualifierDef & right) const
{
	return ( m_qualifiername == right.m_qualifiername ?
				( m_code == right.m_code ?
					( m_id == right.m_id ?
						( m_qualifiertype == right.m_qualifiertype ?
							( pickList == right.pickList ?
								( m_namespaceId == right.m_namespaceId ? true : false)
							: false)
						: false)
					: false)
				: false)
			: false);
}

bool OntyxQualifierDef::hasPickList(const string & QualName) 
{
	const OntyxQualifierDef * tmpQualDef = getQualifierDef(QualName);
	if( tmpQualDef == NULL )
		return false;
	if( tmpQualDef->hasPickList() )
		return true;
	return false;
}
bool OntyxQualifierDef::hasPickList(void) const
{
	if( pickList.empty() )
		return false;
	return true;
}

bool OntyxQualifierDef::stringInPickList(const string & QualName, const string & item) 
{
	const OntyxQualifierDef * tmpQualDef = getQualifierDef(QualName);
	if( tmpQualDef == NULL )
		return false;
	if( tmpQualDef->stringInPickList(item) )
		return true;
	return false;
}
bool OntyxQualifierDef::stringInPickList(const string & item) const
{
	vector<string>::const_iterator pPick = pickList.begin();
	for( ; pPick != pickList.end(); ++pPick ) {
		if( *pPick == item )
			return true;
		}
	return false;
}

			// ********** Relational Operators 

bool OntyxQualifierDef::operator== (const OntyxQualifierDef & right) const
	{ return ( m_qualifiername == right.m_qualifiername ? true : false) ; }

bool OntyxQualifierDef::operator< (const OntyxQualifierDef & right) const
	{ return ( m_qualifiername < right.m_qualifiername ? true : false) ; }
	

			// ********** Private Utility Functions 

void OntyxQualifierDef::initializeMaps(void)
{
	pMapQualifierDefID = new mapID; 
	pMapQualifierDefName = new mapName;
	pSetID = new set<long>;
	pSetCode = new set<string>;
	pQualifierDef = new map<long, OntyxQualifierDef *>;
	if( pMapQualifierDefID == 0 || pMapQualifierDefName == 0 || pSetID == 0 
			|| pSetCode == 0 || pQualifierDef == 0 ) {
		cerr << "Could not allocate memory for a map or a set.  Will EXIT." << endl;
		exit(0);
		}
}
void OntyxQualifierDef::addQualifierDefToMap(void)
{
	pMapQualifierDefID->insert(mapID::value_type(mapQualifierDefID, m_qualifiername));
	pMapQualifierDefName->insert(mapName::value_type(m_qualifiername, mapQualifierDefID));
	pQualifierDef->insert(mapObj::value_type(mapQualifierDefID, this));
}

bool OntyxQualifierDef::codeInSet (const string & inCode) const
{
	if( pSetCode->count(inCode) != 0 )
		return true;
	return false;
}
bool OntyxQualifierDef::idInSet (long inID) const
{
	if( pSetID->count(inID) != 0 )
		return true;
	return false;
}
			// ********** Public Static Functions 

long OntyxQualifierDef::getQualifierDefMapID(const string & propName)
{
	if( pMapQualifierDefName == 0 ) {
		setError(OntyxTerminology::NO_PROPDEF);
		return -1;
		}
	static mapName::iterator iterMap;
	iterMap = pMapQualifierDefName->find(propName);
	if( iterMap == pMapQualifierDefName->end() )
		return -1;
	return iterMap->second;
}
const OntyxQualifierDef * OntyxQualifierDef::getQualifierDef(const string & name) 
{
	mapObj::iterator iterMap;
	long mapid = getQualifierDefMapID(name);
	iterMap = pQualifierDef->find(mapid);
	if( iterMap == pQualifierDef->end() )
		return NULL;
	return iterMap->second;
}

const string & OntyxQualifierDef::getQualifierDefMapName(long id)
{
	if( pMapQualifierDefID == 0 ) {
		setError(OntyxTerminology::NO_PROPDEF);
		return OntyxTerminology::emptyString;
		}
	static mapID::iterator iterID;
	iterID = pMapQualifierDefID->find(id);
	if( iterID == pMapQualifierDefID->end() )
		return OntyxTerminology::emptyString;
	return iterID->second;
}
bool OntyxQualifierDef::qualifierDefInMap(const string & propName)		// true if m_qualifiername is in map
{
	if( pMapQualifierDefName == 0 ) {
		setError(OntyxTerminology::NO_PROPDEF);
		return false;
		}
	if( pMapQualifierDefName->find(propName) == pMapQualifierDefName->end() )
		return false;
	return true;
}
void OntyxQualifierDef::clearError(void)
{
	qualifierDefError = 0;
}
bool OntyxQualifierDef::isError(void)
{
	return ( qualifierDefError ? true : false ) ;
}

void OntyxQualifierDef::setError(long errorCode)
{
	qualifierDefError |= errorCode;
}
void OntyxQualifierDef::clearError(long errorCode)
{
	qualifierDefError &= ~errorCode;
}
bool OntyxQualifierDef::isError(long errorCode)
{
	return ( (qualifierDefError & errorCode) ? true : false );
}

void OntyxQualifierDef::printError (void) 				// using property error codes
{
	for( long i = 0; i < OntyxTerminology::numErrors; i++) {
		if( isError( OntyxTerminology::errorArray[i] ) )
			cerr << "Error class OntyxQualifierDef " << OntyxTerminology::errorNames[i] << endl;
		}
}



