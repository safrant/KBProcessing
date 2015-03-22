//*******************************************************************
// Implementation of Ontyx class OntyxKind
//
//		Initial class definition 4/10/2001
//		See header file for additional information
//
//*******************************************************************

#include "OntyxKind.h"
#include "OntyxName.h"

#include <string>
#include <set>
#include <map>
using namespace std;


			// ********** Static Data

long OntyxKind::kindError = 0;
long OntyxKind::mapKindID = 0;

typedef map<long, string> mapID;
mapID * OntyxKind::pMapKindID = 0;				// to search by internal id "mapKindID"

typedef map<string, long> mapName;
mapName * OntyxKind::pMapKindName = 0;			// to search by kind name

set<string> * OntyxKind::pSetCode = 0;			// for verification that a code is not in use
set<long> * OntyxKind::pSetID = 0;				// for verification that an id is not in use


			// ********** Constructor/Destructor

OntyxKind::OntyxKind (	const string & 	NameKind, 			// default constructor
						long 			id, 
						const string & 	code, 
						const string & 	Namespace, 
						bool 			reference)
		: m_kindname(NameKind), m_code(code), m_id(id), m_reference(reference)
 {
	m_namespaceID = OntyxName::getNamespaceMapID(Namespace);
	if( OntyxName::isError(OntyxTerminology::NO_NAMESPACE))
		setError(OntyxTerminology::NO_NAMESPACE);
	if( mapKindID == 0 )
		initializeMaps();
	if( pSetCode->count(code) != 0 )
		setError(OntyxTerminology::CODE_EXISTS);
	if( pSetID->count(id) != 0 )
		setError(OntyxTerminology::ID_EXISTS);
	if( kindInMap(NameKind) )
		setError(OntyxTerminology::NAME_EXISTS);
	addKindToMap();
	pSetID->insert(id);
	pSetCode->insert(code);
	mapKindID++;
}

OntyxKind::OntyxKind (const OntyxKind & right)
{
	m_kindname = right.m_kindname;
	m_code = right.m_code;
	m_id = right.m_id;
	m_namespaceID = right.m_namespaceID;
	m_reference = right.m_reference;
	mapKindID++;
}
	
OntyxKind::~OntyxKind ()
{
	mapKindID--;
	if( mapKindID == 0) {
		delete pMapKindID;
		delete pMapKindName;
		delete pSetID;
		delete pSetCode;
		}
}

			// ********** Accessor Functions

const string & OntyxKind::getKindname (void) const
	{ return m_kindname; }
	
const string & OntyxKind::getCode(void) const
	{ return m_code; }

long OntyxKind::getID (void) const
	{ return m_id; }

long OntyxKind::getNamespaceID (void) const
	{ return m_namespaceID; }

const string & OntyxKind::getNamespace (void) const
{
	if( !isError(OntyxTerminology::NO_NAMESPACE) )
		return OntyxName::getNamespaceMapName(m_namespaceID);
	else
		return OntyxTerminology::emptyString;
}
bool OntyxKind::getReference (void) const
	{ return m_reference; }

long OntyxKind::getMapID (void) const
{
	return (pMapKindName->find(m_kindname))->second;
}


			// ********** Mutator Functions

OntyxKind & OntyxKind::setCode (const string & inCode)
{
	if( pSetCode->count(inCode) != 0 )
		setError(OntyxTerminology::CODE_EXISTS);
	else
		m_code = inCode;
	return *this;
}
OntyxKind & OntyxKind::setID (long inID)
{
	if( pSetID->count(inID) != 0 )
		setError(OntyxTerminology::ID_EXISTS);
	else
		m_id = inID;
	return *this;
}
void OntyxKind::clearID (void)
{
	m_id = -1;
}
void OntyxKind::clearCode (void)
{
	m_code = "";
}

			// ********** Comparison Functions

bool OntyxKind::isSame(const OntyxKind & right) const
{
	return ( m_kindname == right.m_kindname ? 
				( m_id == right.m_id ? 
					( m_code == right.m_code ?
						( m_namespaceID == right.m_namespaceID ?
							( m_reference == right.m_reference ? true : false )
						: false)
					: false)
				: false)
			: false);
}

			// ********** Relational Operators 

bool OntyxKind::operator== (const OntyxKind & right) const
{ 
	return ( m_kindname == right.m_kindname ? true : false );
}

bool OntyxKind::operator< (const OntyxKind & right) const
{
	return ( m_kindname < right.m_kindname ? true : false );
}


			// ********** Private Utility Functions 

void OntyxKind::initializeMaps(void)
{
	pMapKindID = new mapID; 
	pMapKindName = new mapName;
	pSetID = new set<long>;
	pSetCode = new set<string>;
	if( pMapKindID == 0 || pMapKindName == 0 || pSetID == 0 || pSetCode == 0 ) {
		cerr << "Could not allocate memory for a map or a set.  Will EXIT" << endl;
		exit(0);
		}
}
void OntyxKind::addKindToMap(void)
{
	pMapKindID->insert(mapID::value_type(mapKindID, m_kindname));
	pMapKindName->insert(mapName::value_type(m_kindname, mapKindID));
}
bool OntyxKind::codeInSet (const string & inCode) const
{
	if( pSetCode->count(inCode) != 0 )
		return true;
	return false;
}
bool OntyxKind::idInSet (long inID) const
{
	if( pSetID->count(inID) != 0 )
		return true;
	return false;
}
			// ********** Public Static Functions 

long OntyxKind::getKindMapID(const string & kindName)
{
	if( pMapKindName == 0 ) {
		setError(OntyxTerminology::NO_KIND);
		return -1;
		}
	static mapName::iterator iterMap;
	iterMap = pMapKindName->find(kindName);
	if( iterMap == pMapKindName->end() )
		return -1;
	return iterMap->second;
}
const string & OntyxKind::getKindMapName(long id)
{
	if( pMapKindID == 0 ) {
		setError(OntyxTerminology::NO_KIND);
		return OntyxTerminology::emptyString;
		}
	static mapID::iterator iterID;
	iterID = pMapKindID->find(id);
	if( iterID == pMapKindID->end() )
		return OntyxTerminology::emptyString;
	return iterID->second;
}
bool OntyxKind::kindInMap(const string & kindName)
{
	if( pMapKindName == 0 ) {
		setError(OntyxTerminology::NO_KIND);
		return false;
		}
	if( pMapKindName->find(kindName) == pMapKindName->end() )
		return false;
	return true;
}
void OntyxKind::setError(long errorCode)
{
	kindError |= errorCode;
}
void OntyxKind::clearError(long errorCode)
{
	kindError &= ~errorCode;
}
void OntyxKind::clearError(void)
{
	kindError = false;
}
bool OntyxKind::isError(void)
{
	return ( kindError ? true : false ) ;
}
bool OntyxKind::isError(long errorCode)
{
	return ( (kindError & errorCode) ? true : false );
}
void OntyxKind::printError (void)
{
	for( long i = 0; i < OntyxTerminology::numErrors; i++) {
		if( isError( OntyxTerminology::errorArray[i] ) )
			cerr << "Error class OntyxKind " << OntyxTerminology::errorNames[i] << endl;
		}
}





