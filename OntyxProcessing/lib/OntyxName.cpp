//*******************************************************************
// Implementation of Ontyx class OntyxName, defines Ontylog namespaces
//
//		Initial class definition 4/8/2001
//		See header file for additional information
//
//*******************************************************************

#include "OntyxTerminology.h"
#include "OntyxName.h"
#include <map>
#include <set>
#include <string>
#include <iostream>
#include <cstdlib>
using namespace std;


			// ********** Static Data

long OntyxName::namespaceError = 0;
long OntyxName::mapNamespaceID = 0;

typedef map<long, string> mapID;
mapID * OntyxName::pMapNameID = 0; 			// to search by internal id "mapNamespaceID"

typedef map<string, long> mapName;
mapName * OntyxName::pMapNameName = 0;		// to search by name

set<string> * OntyxName::pSetCode = 0;		// for verification that a code is not in use
set<long> * OntyxName::pSetID = 0;			// for verification that an id is not in use


			// ********** Constructor/Destructor

OntyxName::OntyxName (	const string & 	Namespace, 			// default constructor
						long 			id, 
						const string & 	code)
		: m_namespace(Namespace), m_code(code), m_id(id)
{
	if( mapNamespaceID == 0 )
		initializeMaps();
	if( pSetCode->count(code) != 0 )
		setError(OntyxTerminology::CODE_EXISTS);
	if( pSetID->count(id) != 0 )
		setError(OntyxTerminology::ID_EXISTS);
	if( namespaceInMap(Namespace) )
		setError(OntyxTerminology::NAME_EXISTS);
	addNamespaceToMap();
	pSetID->insert(id);
	pSetCode->insert(code);
	mapNamespaceID++;
}
OntyxName::OntyxName (const OntyxName & right)
{
	m_namespace = right.m_namespace;
	m_code = right.m_code;
	m_id = right.m_id;
	mapNamespaceID++;
}
OntyxName::~OntyxName ()
{
	mapNamespaceID--;
	if( mapNamespaceID == 0) {
		delete pMapNameID;
		delete pMapNameName;
		delete pSetID;
		delete pSetCode;
		}
}

			// ********** Accessor Functions

const string & OntyxName::getNamespace (void) const
	{ return m_namespace; }

const string & OntyxName::getCode(void) const
	{ return m_code; }

long OntyxName::getID (void) const
	{ return m_id; }

long OntyxName::getMapID (void) const
{
	return (pMapNameName->find(m_namespace))->second;
}

			// ********** Mutator Functions

OntyxName & OntyxName::setCode (const string & inCode)
{
	if( pSetCode->count(inCode) != 0 )
		setError(OntyxTerminology::CODE_EXISTS);
	else
		m_code = inCode;
	return *this;
}
OntyxName & OntyxName::setID (long inID)
{
	if( pSetID->count(inID) != 0 )
		setError(OntyxTerminology::ID_EXISTS);
	else
		m_id = inID;
	return *this;
}
void OntyxName::clearID (void)
{
	m_id = -1;
}
void OntyxName::clearCode (void)
{
	m_code = "";
}

			// ********** Comparison Functions

bool OntyxName::isSame(const OntyxName & right) const
{
	return ( m_namespace == right.m_namespace ?
				(m_id == right.m_id ?
					(m_code == right.m_code ? true : false)
				: false)
			: false);
}

			// ********** Relational Operators 

bool OntyxName::operator== (const OntyxName & right) const
	{ return ( m_namespace == right.m_namespace ? true : false); }

bool OntyxName::operator< (const OntyxName & right) const
	{ return ( m_namespace < right.m_namespace ? true : false); }


			// ********** Private Utility Functions 

void OntyxName::addNamespaceToMap(void)
{
	pMapNameID->insert(mapID::value_type(mapNamespaceID, m_namespace));
	pMapNameName->insert(mapName::value_type(m_namespace, mapNamespaceID));
}
void OntyxName::initializeMaps(void)
{
	pMapNameID = new mapID; 
	pMapNameName = new mapName;
	pSetID = new set<long>;
	pSetCode = new set<string>;
	if( pMapNameID == 0 || pMapNameName == 0 || pSetID == 0 || pSetCode == 0 ) {
		cerr << "Could not allocate memory for a map or a set.  Will EXIT." << endl;
		exit(0);
		}
}
bool OntyxName::codeInSet (const string & inCode) const
{
	if( pSetCode->count(inCode) != 0 )
		return true;
	return false;
}
bool OntyxName::idInSet (long inID) const
{
	if( pSetID->count(inID) != 0 )
		return true;
	return false;
}

			// ********** Public Static Functions 

long OntyxName::getNamespaceMapID(const string & inName)
{
	if( pMapNameName == 0 ) {
		setError(OntyxTerminology::NO_NAMESPACE);
		return -1;
		}
	static mapName::iterator iterMap;

	iterMap = pMapNameName->find(inName);
	if( iterMap == pMapNameName->end() )
		return -1;
	return iterMap->second;
}
const string & OntyxName::getNamespaceMapName(long id)
{
	if( pMapNameID == 0 ) {
		setError(OntyxTerminology::NO_NAMESPACE);
		return OntyxTerminology::emptyString;
		}
	static mapID::iterator iterID;

	iterID = pMapNameID->find(id);
	if( iterID == pMapNameID->end() )
		return OntyxTerminology::emptyString;
	return iterID->second;
}
bool OntyxName::namespaceInMap(const string & inName)
{
	if( pMapNameName == 0 ) {
		setError(OntyxTerminology::NO_NAMESPACE);
		return false;
		}
	if( pMapNameName->find(inName) == pMapNameName->end() )
		return false;
	return true;
}
void OntyxName::setError(long errorCode)
{
	namespaceError |= errorCode;
}
void OntyxName::clearError(long errorCode)
{
	namespaceError &= ~errorCode;
}
void OntyxName::clearError(void)
{
	namespaceError = 0;
}
bool OntyxName::isError(void)
{
	return ( namespaceError ? true : false);
}
bool OntyxName::isError(long errorCode)
{
	return ( (namespaceError & errorCode) ? true : false );
}
void OntyxName::printError (void)
{
	for( long i = 0; i < OntyxTerminology::numErrors; i++) {
		if( isError( OntyxTerminology::errorArray[i] ) )
			cerr << "Error class OntyxName " << OntyxTerminology::errorNames[i] << endl;
		}
}

