//*******************************************************************
// Implementation of the Ontyx class OntyxConcept
//
//		Initial class definition 4/11/2001
//
// Dependencies:	STL <string>, <set> class
// Dependencies:	<iostream>, <iomanip> class
// Dependencies:	OntyxRole, OntyxProperty, OntyxName, OntyxKind
// Virtual:			~OntyxConcept()
//*******************************************************************

#include "OntyxConcept.h"
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
#include <cstdio>		// sprintf()

using namespace std;

			// ********** Static Data

long OntyxConcept::conceptError = 0;
long OntyxConcept::mapConceptID = 0;

typedef map<long, string> mapID;
mapID * OntyxConcept::pMapConceptID = 0;
typedef multimap<string, long> mapName;
mapName * OntyxConcept::pMapConceptName = 0;

set<string> * OntyxConcept::pSetCode = 0;
set<long> * OntyxConcept::pSetID = 0;

			// ********** Constructor/Destructor

OntyxConcept::OntyxConcept (	const string &	name , 
								const string &	code , 
								long			id ,
								const string &	inNamespace ,
								const string &	inKind ,
								bool			inState )
		: m_name(name), m_code(code), m_id(id), m_state(inState)
{
	if( mapConceptID == 0 )
		initializeMaps();

	m_namespaceID = OntyxName::getNamespaceMapID(inNamespace);
	if( OntyxName::isError(OntyxTerminology::NO_NAMESPACE) )
		setError(OntyxTerminology::NO_NAMESPACE);

	m_kindID = OntyxKind::getKindMapID(inKind);
	if( OntyxKind::isError(OntyxTerminology::NO_KIND) )
		setError(OntyxTerminology::NO_KIND);

	if( pSetCode->count(code) != 0 )
		setError(OntyxTerminology::CODE_EXISTS);
	if( pSetID->count(id) != 0 )
		setError(OntyxTerminology::ID_EXISTS);
	if( conceptInMap(name) )
		setError(OntyxTerminology::NAME_EXISTS);
	mapConceptID++;
	m_myID = mapConceptID;
	addConceptToMap();
	pSetID->insert(id);
	pSetCode->insert(code);
}
OntyxConcept::OntyxConcept (const OntyxConcept & inRight)		// copy constructor
{
	m_name = inRight.m_name;
	m_code = inRight.m_code;
	m_id = inRight.m_id;
	m_namespaceID = inRight.m_namespaceID;
	m_kindID = inRight.m_kindID;
	m_parent = inRight.m_parent;
	m_role = inRight.m_role;
	m_property = inRight.m_property;
	m_associations = inRight.m_associations;
	m_state = inRight.m_state;
	m_rolegroups = inRight.m_rolegroups;
	mapConceptID++;
	m_myID = mapConceptID;
}	
OntyxConcept::~OntyxConcept ()
{
	mapConceptID--;
	if( mapConceptID == 0 ) {
		delete pMapConceptID;
		delete pMapConceptName;
		delete pSetCode;
		delete pSetID;
		}
}

			// ********** Assignment Operator

OntyxConcept & OntyxConcept::operator= (const OntyxConcept & inRight)
{
	if( this != &inRight) {
		m_name = inRight.m_name;
		m_code = inRight.m_code;
		m_id = inRight.m_id;
		m_namespaceID = inRight.m_namespaceID;
		m_kindID = inRight.m_kindID;
		m_parent = inRight.m_parent;
		m_role = inRight.m_role;
		m_rolegroups = inRight.m_rolegroups;
		m_property = inRight.m_property;
		m_associations = inRight.m_associations;
		m_state = inRight.m_state;
		m_myID = inRight.m_myID;
		}
	return *this;
}

			// ********** Mutator Functions

void OntyxConcept::clear (void)
{
	m_name = "";
	m_code = "";
	m_id = -1;
	m_namespaceID = -1;
	m_kindID = -1;
	m_parent.clear();
	m_role.clear();
	m_property.clear();
	m_rolegroups.clear();
	m_associations.clear();
	m_state = false;
	conceptError = 0;
}
void OntyxConcept::clearID (void)
{
	m_id = -1;
}
void OntyxConcept::clearCode (void)
{
	m_code = "";
}
void OntyxConcept::clearKind (void)
{
	m_kindID = -1;
}


OntyxConcept & OntyxConcept::setName (const string & inName)
{// do not use setName, use reName() instead as it deals with registries
	if( conceptInMap(inName) )
		setError(OntyxTerminology::NAME_EXISTS);
	else
		m_name = inName;
	return *this;
}
OntyxConcept & OntyxConcept::reName (const string & newName)
{
	if( conceptInMap(newName) )
		setError(OntyxTerminology::NAME_EXISTS);
	pair< multimap<string, long>::iterator, multimap<string, long>::iterator > p;
	p = pMapConceptName->equal_range(m_name);
	mapName::iterator pName = p.first;
	for( ; pName != p.second; ++pName) {
		if( pName->second == m_myID )
			pMapConceptName->erase(pName);
		}
	pMapConceptID->erase(m_myID);
	m_name = newName;
	pMapConceptID->insert(mapID::value_type(m_myID, m_name));
	pMapConceptName->insert(mapName::value_type(m_name, m_myID));
	return *this;
}

OntyxConcept & OntyxConcept::setCode (const string & inCode)
{
	if( pSetCode->count(inCode) != 0 )
		setError(OntyxTerminology::CODE_EXISTS);
	m_code = inCode;
	return *this;
}
OntyxConcept & OntyxConcept::setID (long inID)
{
	if( pSetID->count(inID) != 0 )
		setError(OntyxTerminology::ID_EXISTS);
	m_id = inID;
	return *this;
}
OntyxConcept & OntyxConcept::setNamespace (const string & inNamespace)
{
	if( !isError(OntyxTerminology::NO_NAMESPACE) )
		m_namespaceID = OntyxName::getNamespaceMapID(inNamespace);
	return *this;
}
OntyxConcept & OntyxConcept::setKind (const string & inKind)
{
	if( !isError(OntyxTerminology::NO_KIND) )
		m_kindID = OntyxKind::getKindMapID(inKind);
	return *this;
}
OntyxConcept & OntyxConcept::addParent (const string & inParent)
// eventually will have to consider checking parents and maybe using ID's
{
	m_parent.insert(inParent);
	return *this;
}
OntyxConcept & OntyxConcept::addRole (const OntyxRole & inRole)
{
	if( OntyxRole::isError(OntyxTerminology::NO_ROLEDEF) )
		setError(OntyxTerminology::NO_ROLEDEF);
	m_role.insert(multimap<long, OntyxRole>::value_type(inRole.getNameID(), inRole) );
	m_rolegroups[inRole.getRolegroup()]++;
	return *this;
}
OntyxConcept & OntyxConcept::addProperty (const OntyxProperty & inProperty)
{
	if( OntyxProperty::isError(OntyxTerminology::NO_PROPDEF) )
		setError(OntyxTerminology::NO_PROPDEF);
	m_property.insert(multimap<long, OntyxProperty>::value_type(inProperty.getNameID(), inProperty) ); 
	return *this;
}
OntyxConcept & OntyxConcept::addParents (vector<string> & inPar)
{
	static vector<string>::iterator pPar;
	pPar = inPar.begin();
	for( ; pPar != inPar.end(); ++pPar)
		addParent(*pPar);
	return *this;
}
OntyxConcept & OntyxConcept::addRoles (vector<OntyxRole> & inRole)
{
	static vector<OntyxRole>::iterator pRole;
	pRole = inRole.begin();
	for( ; pRole != inRole.end(); ++pRole) {
		addRole(*pRole);
		}
	return *this;
}
OntyxConcept & OntyxConcept::addProperties (vector<OntyxProperty> & inProp)
{
	static vector<OntyxProperty>::iterator pProp;
	pProp = inProp.begin();
	for( ; pProp != inProp.end(); ++pProp)
		addProperty(*pProp);
	return *this;
}
OntyxConcept & OntyxConcept::setPropertyValue 
			(multimap<long, OntyxProperty>::iterator pProp, const string & value)
{
	if( pProp != m_property.end() )
		pProp->second.setValue(value);
	return *this;
}
OntyxConcept & OntyxConcept::setRoleValue 
			(multimap<long, OntyxRole>::iterator pRole, const string & value)
{
	if( pRole != m_role.end() )
		pRole->second.setValue(value);
	return *this;
}

OntyxConcept & OntyxConcept::setDefined (void)					// sets state to defined, true
{
	m_state = true;
	return *this;
}
OntyxConcept & OntyxConcept::setPrimitive (void)				// sets state to primitive, false
{
	m_state = false;
	return *this;
}

OntyxConcept & OntyxConcept::setState (bool state)				// true sets defined, false is primitive
{
	m_state = state;
	return *this;
}

OntyxConcept & OntyxConcept::addAssociations (vector<OntyxAssociation> & assocs)
{
	vector<OntyxAssociation>::iterator pAssocs = assocs.begin();
	for( ; pAssocs != assocs.end(); ++pAssocs )
		addAssociation(*pAssocs);
	return *this;
}
OntyxConcept & OntyxConcept::addAssociation (const OntyxAssociation & inAssociation)
{
	if( OntyxAssociation::isError(OntyxTerminology::NO_PROPDEF) )
		setError(OntyxTerminology::NO_PROPDEF);
	m_associations.insert(multimap<long, OntyxAssociation>::value_type(inAssociation.getNameID(), inAssociation) ); 
	return *this;
}

OntyxConcept & OntyxConcept::deleteParent (const string & name)
{
	m_parent.erase(name);
	return *this;
}
OntyxConcept & OntyxConcept::deleteRole (const string & name, const string & value)
{
	static multimap<long, OntyxRole>::iterator pRole;
	pRole = m_role.begin();
	while( pRole != m_role.end()) {
		if( pRole->second.getValue() == value && pRole->second.getName() == name ) {
			m_rolegroups[pRole->second.getRolegroup()]--;
			m_role.erase(pRole);
			pRole = m_role.begin();
			}
		else
			 ++pRole;
		}
	return *this;
}

OntyxConcept & OntyxConcept::deleteRole (const string & name)
{
	static multimap<long, OntyxRole>::iterator pRole;
	pRole = m_role.begin();
	while( pRole != m_role.end()) {
		if( pRole->second.getName() == name ) {
			m_rolegroups[pRole->second.getRolegroup()]--;
			m_role.erase(pRole);
			pRole = m_role.begin();
			}
		else
			 ++pRole;
		}
	return *this;
}

OntyxConcept & OntyxConcept::deleteRole (const OntyxRole & inRole)
{
	static multimap<long, OntyxRole>::iterator pRole;
	pRole = m_role.begin();
	while( pRole != m_role.end()) {
		if( pRole->second == inRole ) {
			m_rolegroups[pRole->second.getRolegroup()]--;
			m_role.erase(pRole);
			pRole = m_role.begin();
			}
		else
			 ++pRole;
		}
	return *this;
}

OntyxConcept & OntyxConcept::deleteAllProperties (void)
{
	m_property.clear();
	return *this;
}


OntyxConcept & OntyxConcept::deleteAllRoles (void)
{
	m_rolegroups.clear();
	m_role.clear();
	return *this;
}

OntyxConcept & OntyxConcept::deleteAssociation (const OntyxAssociation & compAssoc)
{
	static multimap<long, OntyxAssociation>::iterator pAssoc;
	pAssoc = m_associations.begin();
	while( pAssoc != m_associations.end() ) {
		if( pAssoc->second == compAssoc ) {
			m_associations.erase(pAssoc);
			pAssoc = m_associations.begin();
			}
		else
			++pAssoc;
		}
	return *this;
}

OntyxConcept & OntyxConcept::deleteAllAssociations (void)
{
	m_associations.clear();
	return *this;
}

void OntyxConcept::resetAssociationValueIfValue (const string & oldVal, const string & newVal)
{
	static multimap<long, OntyxAssociation>::iterator pAssoc;
	pAssoc = m_associations.begin();
	for( ; pAssoc != m_associations.end(); ++pAssoc) {
		if( pAssoc->second.getValue() == oldVal )
			pAssoc->second.setValue(newVal);
		}
}

void OntyxConcept::resetRoleValueIfValue (const string & oldVal, const string & newVal)
{
	static multimap<long, OntyxRole>::iterator pRole;
	pRole = m_role.begin();
	for( ; pRole != m_role.end(); ++pRole) {
		if( pRole->second.getValue() == oldVal )
			pRole->second.setValue(newVal);
		}
}
void OntyxConcept::resetRoleNameIfName (const string & ifName, const string & newName)
{							//	OntyxRole generates error if newName not yet defined
	static multimap<long, OntyxRole>::iterator pRole;
	pRole = m_role.begin();
	for( ; pRole != m_role.end(); ++pRole) {
		if( pRole->second.getName() == ifName )
			pRole->second.setName(newName);
		}
}

void OntyxConcept::addRolesTo (OntyxConcept & toConcept) const
{
	bool foundFlag;
	multimap<long, OntyxRole>::iterator pToRole = toConcept.m_role.begin();
	multimap<long, OntyxRole>::const_iterator pMyRole = m_role.begin();
	for( ; pMyRole != m_role.end(); ++pMyRole ) {
		foundFlag = false;
		for( ; pToRole != toConcept.m_role.end(); ++pToRole ) {
			if( pMyRole->second.getName() == pToRole->second.getName() 
				&& pMyRole->second.getValue() == pToRole->second.getValue()
				&& pMyRole->second.getModifier() == pToRole->second.getModifier() 
				&& pMyRole->second.getRolegroup() == pToRole->second.getRolegroup() )
				foundFlag = true;
			}
		if( !foundFlag ) {
			toConcept.addRole(pMyRole->second);
			cout << "role-add-from-tde\t" << toConcept.getName() << '\t' 
				<< pMyRole->second.getName() << '\t' << pMyRole->second.getValue() << endl;
			}
		}
}

void OntyxConcept::resetParentIfParent (const string & oldPar, const string & newPar)
{
	if( m_parent.count(oldPar) > 0 ) {
		m_parent.erase(oldPar);
		m_parent.insert(newPar);
		}
}
void OntyxConcept::convertAllParentToProperty (const string & propName)
{
	vector<OntyxProperty> vecProp;
	set<string>::iterator pPar = m_parent.begin();
	for( ; pPar != m_parent.end(); ++pPar) {
		OntyxProperty tmpProp(propName, *pPar, "");
		vecProp.push_back(tmpProp);
		}
	m_parent.clear();
	for(vector<OntyxProperty>::iterator pProp = vecProp.begin();
			pProp != vecProp.end(); ++pProp )
		addProperty(*pProp);
}

void OntyxConcept::convertToProperty (const string & value, const string & propName)
{
	OntyxProperty newProp(propName, value, "");
	addProperty(newProp);
}

bool OntyxConcept::convertAllRoleToProperty (void)
{
	multimap<long, OntyxRole>::iterator pRole = m_role.begin();
	for( ; pRole != m_role.end(); ++pRole) {
		if( OntyxPropertyDef::getPropDefMapID(pRole->second.getName()) == -1) {
			cerr << "OntyxConcept::convertAllRole ERROR: NO Property Definition in KB" << endl;
			return false;
			}
		cout << "role-convert-property\t" << getName() << '\t' << pRole->second.getName()
			<< '\t' << pRole->second.getValue() << endl;
		OntyxProperty newProp(pRole->second.getName(),pRole->second.getValue(), "");
		addProperty(newProp);
		}
	m_rolegroups.clear();
	m_role.clear();
	return true;
} 

void OntyxConcept::convertToSanctionedRoleOrProperty (const string & inKind, 
												map<string, string> & mapping,
												const string & oldRoleName,
												const string & newPropertyName)
{
	map<string, string>::iterator pMap;
	if( (pMap = mapping.find(inKind)) != mapping.end() ) { // remap role to sanctioned role
		string newRoleName = pMap->second;
		multimap<long, OntyxRole>::iterator pRole = m_role.begin();
		while( pRole != m_role.end() ) {
			if( pRole->second.getName() == oldRoleName ) {
				cout << "role-remap-tde\t" << getName() << '\t' << oldRoleName 
					<< '\t' << newRoleName << '\t' << pRole->second.getValue() << endl;
				OntyxRole tmpRole(newRoleName, pRole->second.getValue(), pRole->second.getModifier(), pRole->second.getRolegroup() );
				deleteRole(pRole->second);
				addRole(tmpRole);
				pRole = m_role.begin();
				}
			else
				++pRole;
			}
		}
	else {	// if a mapped role doesn't exist, convert role to property
		multimap<long, OntyxRole>::iterator pRole = m_role.begin();
		while( pRole != m_role.end() ) {
			if( pRole->second.getName() == oldRoleName ) {
				cout << "role-convert-property\t" << getName() << '\t' << oldRoleName << '\t' << pRole->second.getValue() << endl;
				OntyxProperty tmpProp(newPropertyName, pRole->second.getValue(), "");
				addProperty(tmpProp);
				deleteRole(pRole->second);
				pRole = m_role.begin();
				}
			else
				++pRole;
			}
		}
}
void OntyxConcept::mergePropertiesFrom (const OntyxConcept & inConcept)
{
	bool foundFlag;
	multimap<long, OntyxProperty>::iterator pMyProp;
	multimap<long, OntyxProperty>::const_iterator pInProp = inConcept.m_property.begin();
	for( ; pInProp != inConcept.m_property.end(); ++pInProp) {
		foundFlag = false;
		for( pMyProp = m_property.begin(); pMyProp != m_property.end(); ++pMyProp) {
			if( pMyProp->second.getName() == pInProp->second.getName()
				&& pMyProp->second.getValue() == pInProp->second.getValue() ) {
				foundFlag = true;
				break;
				}
			}
		if( !foundFlag ) {
			OntyxProperty tmpProp(pInProp->second.getName(), 
				pInProp->second.getValue(), pInProp->second.getLocale());
			addProperty(tmpProp);
			}
		}
}

void OntyxConcept::convertToNonredundantProperty (void)
{
	if( m_property.size() > 0 ) {
		bool found;
		vector<OntyxProperty> vecProp;
		vector<OntyxProperty>::iterator pVecProp;
		multimap<long, OntyxProperty>::iterator posProp = m_property.begin();
		vecProp.push_back(posProp->second);
		++posProp;
		for( ; posProp != m_property.end(); ++posProp) {
			found = false;
			for( pVecProp = vecProp.begin(); pVecProp != vecProp.end(); ++pVecProp) {
				if( posProp->second.getName() == pVecProp->getName()
						&& posProp->second.getValue() == pVecProp->getValue() ) {
					found = true;
					break;
					}
				}
			if( !found )
				vecProp.push_back(posProp->second);
			}
		m_property.clear();
		addProperties(vecProp);
		}
}
void OntyxConcept::convertToNonredundantRole (void)
{
	if( m_role.size() > 0 ) {
		bool found;
		vector<OntyxRole> vecRole;
		vector<OntyxRole>::iterator pVecRole;
		multimap<long, OntyxRole>::iterator posRole = m_role.begin();
		vecRole.push_back(posRole->second);
		++posRole;
		for( ; posRole != m_role.end(); ++posRole) {
			found = false;
			for( pVecRole = vecRole.begin(); pVecRole != vecRole.end(); ++pVecRole) {
				if( posRole->second.getName() == pVecRole->getName()
						&& posRole->second.getValue() == pVecRole->getValue() 
						&& posRole->second.getModifier() == pVecRole->getModifier() 
						&& posRole->second.getRolegroup() == pVecRole->getRolegroup() ) {
					found = true;
					break;
					}
				}
			if( !found )
				vecRole.push_back(posRole->second);
			}
		m_role.clear();
		m_rolegroups.clear();
		addRoles(vecRole);
		}
}

			// ********** Accessor Functions

const string & OntyxConcept::getName (void) const
	{ return m_name; }

const string & OntyxConcept::getCode (void) const
	{ return m_code; }

long OntyxConcept::getID (void) const
	{ return m_id; }

long OntyxConcept::getMapID (void) const
{
	return m_myID;
}

string OntyxConcept::getID (char c) const	// signature change
{
	static char array[20];					// enough to cover a long
	sprintf( array, "%i", static_cast<int> (m_id) );
	string tmp = array;
	return tmp;
}
const string & OntyxConcept::getNamespace (void) const
{
	if( !isError(OntyxTerminology::NO_NAMESPACE) )
		return OntyxName::getNamespaceMapName(m_namespaceID);
	else
		return OntyxTerminology::emptyString;
}
const string & OntyxConcept::getKind (void) const
{
	if( !isError(OntyxTerminology::NO_KIND) )
		return OntyxKind::getKindMapName(m_kindID);
	else
		return OntyxTerminology::emptyString;
}

const set<string> & OntyxConcept::getParents (void) const
	{ return m_parent; }

long OntyxConcept::getNumParents (void) const
	{ return m_parent.size(); }

set<string>::const_iterator OntyxConcept::getFirstParent (void) const
	{ return m_parent.begin(); }

set<string>::const_iterator OntyxConcept::getEndParent (void) const
	{ return m_parent.end(); }

const multimap<long, OntyxRole> & OntyxConcept::getRoles (void) const
	{ return m_role; }

long OntyxConcept::getNumRoles (void) const
	{ return m_role.size(); }

long OntyxConcept::getNumRoleGroups (void) const
{ 
	if( (m_rolegroups.begin())->first == 0 )
		return m_rolegroups.size() - 1;
	return m_rolegroups.size(); 				// i.e. any roles are in rolegroups
}


multimap<long, OntyxRole>::const_iterator OntyxConcept::getFirstRole (void) const
	{ return m_role.begin(); }

multimap<long, OntyxRole>::const_iterator OntyxConcept::getEndRole (void) const
	{ return m_role.end(); }

const multimap<long, OntyxProperty> & OntyxConcept::getProperties (void) const
	{ return m_property; }

const multimap<long, OntyxAssociation> & OntyxConcept::getAssociations (void) const
	{ return m_associations; }

long OntyxConcept::getNumAssociations (void) const
	{ return m_associations.size(); }

long OntyxConcept::getNumProperties (void) const
	{ return m_property.size(); }

multimap<long, OntyxProperty>::const_iterator OntyxConcept::getFirstProperty (void) const
	{ return m_property.begin(); }

multimap<long, OntyxProperty>::const_iterator OntyxConcept::getEndProperty (void) const
	{ return m_property.end(); }

bool OntyxConcept::getState (void) const						// defined or primitive, true is defined
	{ return m_state; }

multimap<long, OntyxProperty>::const_iterator OntyxConcept::findProperty (const string & name) const
{
	multimap<long, OntyxProperty>::const_iterator pProp = m_property.begin();
	for( ; pProp != m_property.end(); ++pProp ) {
		if( pProp->second.getName() == name )
			return pProp;
		}
	return m_property.end();
}

multimap<long, OntyxProperty>::iterator OntyxConcept::propertyIterator (const string & name)
{
	multimap<long, OntyxProperty>::iterator pProp = m_property.begin();
	for( ; pProp != m_property.end(); ++pProp ) {
		if( pProp->second.getName() == name )
			return pProp;
		}
	return m_property.end();
}


OntyxConcept & OntyxConcept::deleteProperty (const string & name, const string & value)
{
	multimap<long, OntyxProperty>::iterator pProp = m_property.begin();
	while( pProp != m_property.end() ) {
		if( pProp->second.getName() == name && pProp->second.getValue() == value ) {
			m_property.erase(pProp);
			pProp = m_property.begin();
			}
		else
			++pProp;
		}
	return *this;
}
OntyxConcept & OntyxConcept::deleteProperty (const OntyxProperty & inProp)
{
	multimap<long, OntyxProperty>::iterator pProp = m_property.begin();
	while( pProp != m_property.end() ) {
		if( pProp->second == inProp ) {
			m_property.erase(pProp);
			pProp = m_property.begin();
			}
		else
			++pProp;
		}
	return *this;

}

OntyxConcept & OntyxConcept::deleteProperty (const string & name)	// deletes all properties of type "name"
{
	multimap<long, OntyxProperty>::iterator pProp = m_property.begin();
	while( pProp != m_property.end() ) {
		if( pProp->second.getName() == name ) {
			m_property.erase(pProp);
			pProp = m_property.begin();
			}
		else
			++pProp;
		}
	return *this;
}
string OntyxConcept::getPropertyValue (const string & name) const
{
	multimap<long, OntyxProperty>::const_iterator pProp;
	if( (pProp = findProperty(name)) != m_property.end() )
		return pProp->second.getValue();
	return OntyxTerminology::emptyString;
}

long OntyxConcept::getPropertyNumber (const string & name) const
{
	int numProp = 0;
	multimap<long, OntyxProperty>::const_iterator pProp = m_property.begin();
	for( ; pProp != m_property.end(); ++pProp ) {
		if( pProp->second.getName() == name )
			numProp++;
		}
	return numProp;
}

			// ********** Private Utility Functions 

void OntyxConcept::initializeMaps(void)
{
	pMapConceptID = new mapID; 
	pMapConceptName = new mapName;
	pSetID = new set<long>;
	pSetCode = new set<string>;
	if( pMapConceptID == 0 || pMapConceptName == 0 || pSetID == 0 || pSetCode == 0 ) {
		cerr << "Could not allocate memory for a map or a set.  Will EXIT." << endl;
		exit(0);
		}
}

void OntyxConcept::addConceptToMap(void)
{
	pMapConceptID->insert(mapID::value_type(m_myID, m_name));
	pMapConceptName->insert(mapName::value_type(m_name, m_myID));
}

bool OntyxConcept::codeInSet (const string & inCode) const
{
	if( pSetCode->count(inCode) != 0 )
		return true;
	return false;
}
bool OntyxConcept::idInSet (long inID) const
{
	if( pSetID->count(inID) != 0 )
		return true;
	return false;
}


			// ********** Comparison Functions 

bool OntyxConcept::hasSameName (const OntyxConcept & inRight) const
	{ return (m_name == inRight.m_name ? true : false) ; }

bool OntyxConcept::hasSameCode (const OntyxConcept & inRight) const
	{ return (m_code == inRight.m_code ? true : false) ; }

bool OntyxConcept::hasSameID (const OntyxConcept & inRight) const
	{ return (m_id == inRight.m_id ? true : false) ; }

bool OntyxConcept::hasSameNamespace (const OntyxConcept & inRight) const
	{ return (m_namespaceID == inRight.m_namespaceID ? true : false) ; }

bool OntyxConcept::hasSameKind (const OntyxConcept & inRight) const
	{ return (m_kindID == inRight.m_kindID ? true : false) ; }

bool OntyxConcept::hasSameParents (const OntyxConcept & inRight) const
	{ return (m_parent == inRight.m_parent ? true : false) ; }

bool OntyxConcept::hasRole (const string & inRoleName) const
{
	multimap<long, OntyxRole>::const_iterator pRole = m_role.begin();
	for( ; pRole != m_role.end(); ++pRole ) {
		if( pRole->second.getName() == inRoleName )
			return true;
		}
	return false;
}
bool OntyxConcept::hasRole (const OntyxRole & inRole) const
{
	multimap<long, OntyxRole>::const_iterator pRole = m_role.begin();
	for( ; pRole != m_role.end(); ++pRole ) {
		if( pRole->second == inRole )
			return true;
		}
	return false;
}

bool OntyxConcept::hasRolegroups (void) const
{
	if( m_rolegroups.size() > 1 || (m_rolegroups.begin())->first == 1 ) {
		return true;
		}
	return false;
}

bool OntyxConcept::hasSameRoles (const OntyxConcept & inRight) const
{
	static multimap<long, OntyxRole>::const_iterator pm_Role;
	static multimap<long, OntyxRole>::const_iterator pRight_Role;
	
	if( m_role.size() != inRight.m_role.size() )
		return false;
	if( m_role.size() > 0 ) {
		pm_Role = m_role.begin();
		pRight_Role = inRight.m_role.begin();
		for( ; pm_Role != m_role.end(); ++pm_Role, ++pRight_Role) {
			if( !(pm_Role->second == pRight_Role->second) )
				return false;
			}
		}
	return true;
}

bool OntyxConcept::hasAllRolesInGroup (void) const
{
	if( (m_rolegroups.begin())->first == 1 )
		return true;
	return false;
}

bool OntyxConcept::hasLessThanRoles ( const OntyxConcept & inRight) const
// relies on sorted maps to compare equivalent roles
{
	static multimap<long, OntyxRole>::const_iterator pm_Role;
	static multimap<long, OntyxRole>::const_iterator pRight_Role;
	
	if( m_role.size() > inRight.m_role.size() )
		return false;
	if( m_role.size() < inRight.m_role.size() )
		return true;
	if( m_role.size() > 0 ) {
		pm_Role = m_role.begin();
		pRight_Role = inRight.m_role.begin();
		for( ; pm_Role != m_role.end(); ++pm_Role, ++pRight_Role) {
			if( !(pm_Role->second < pRight_Role->second) )
				return false;
			}
		}
	return true;
}

bool OntyxConcept::hasSameProperties (const OntyxConcept & inRight) const
{
	static multimap<long, OntyxProperty>::const_iterator pm_Property;
	static multimap<long, OntyxProperty>::const_iterator pRight_Property;
	
	if( m_property.size() != inRight.m_property.size() )
		return false;
	if( m_property.size() > 0 ) {
		pm_Property = m_property.begin();
		pRight_Property = inRight.m_property.begin();
		for( ; pm_Property != m_property.end(); ++pm_Property, ++pRight_Property) {
			if( !(pm_Property->second == pRight_Property->second) )
				return false;
			}
		}
	return true;
}
bool OntyxConcept::hasLessThanProperties (const OntyxConcept & inRight) const
// relies on sorted maps to compare equivalent roles
{
	static multimap<long, OntyxProperty>::const_iterator pm_Property;
	static multimap<long, OntyxProperty>::const_iterator pRight_Property;
	
	if( m_property.size() > inRight.m_property.size() )
		return false;
	if( m_property.size() < inRight.m_property.size() )
		return true;
	if( m_property.size() > 0 ) {
		pm_Property = m_property.begin();
		pRight_Property = inRight.m_property.begin();
		for( ; pm_Property != m_property.end(); ++pm_Property, ++pRight_Property) {
			if( !(pm_Property->second < pRight_Property->second) )
				return false;
			}
		}
	return true;
}
bool OntyxConcept::hasSameAssociations (const OntyxConcept & inRight) const
{
	static multimap<long, OntyxAssociation>::const_iterator pm_Association;
	static multimap<long, OntyxAssociation>::const_iterator pRight_Association;
	
	if( m_associations.size() != inRight.m_associations.size() )
		return false;
	if( m_associations.size() > 0 ) {
		pm_Association = m_associations.begin();
		pRight_Association = inRight.m_associations.begin();
		for( ; pm_Association != m_associations.end(); ++pm_Association, ++pRight_Association) {
			if( !(pm_Association->second == pRight_Association->second) )
				return false;
			}
		}
	return true;
}

bool OntyxConcept::hasLessThanAssociations (const OntyxConcept & inRight) const
{
	static multimap<long, OntyxAssociation>::const_iterator pm_Association;
	static multimap<long, OntyxAssociation>::const_iterator pRight_Association;
	
	if( m_associations.size() > inRight.m_associations.size() )
		return false;
	if( m_associations.size() < inRight.m_associations.size() )
		return true;
	if( m_associations.size() > 0 ) {
		pm_Association = m_associations.begin();
		pRight_Association = inRight.m_associations.begin();
		for( ; pm_Association != m_associations.end(); ++pm_Association, ++pRight_Association) {
			if( !(pm_Association->second < pRight_Association->second) )
				return false;
			}
		}
	return true;
}

bool OntyxConcept::hasSameState (const OntyxConcept & inRight) const
	{ return (m_state == inRight.m_state ? true : false) ; }

bool OntyxConcept::isSame (const OntyxConcept & inRight) const		// applied to all members
{
	return ( *this == inRight ? true : false);
}
	

			// ********** Relational Operators 

bool OntyxConcept::operator== (const OntyxConcept & inRight) const		// applied to all members
{
	if( m_id == inRight.m_id
		&& m_code == inRight.m_code
		&& m_namespaceID == inRight.m_namespaceID
		&& m_kindID == inRight.m_kindID
		&& m_state == inRight.m_state
		&& m_name == inRight.m_name
		&& m_parent == inRight.m_parent
		&& hasSameRoles(inRight)
		&& hasSameAssociations(inRight)
		&& hasSameProperties(inRight) )
		return true;

	return false;
}

bool OntyxConcept::operator< (const OntyxConcept & inRight) const		// applied to all members
{
	if( m_id < inRight.m_id
		&& m_code < inRight.m_code
		&& m_namespaceID < inRight.m_namespaceID
		&& m_kindID < inRight.m_kindID
		&& m_state < inRight.m_state
		&& m_name < inRight.m_name
		&& m_parent < inRight.m_parent
		&& hasLessThanRoles(inRight)
		&& hasLessThanAssociations(inRight)
		&& hasLessThanProperties(inRight) 
		)
		return true;

	return false;
}

			// ********** Output Friend Operator

ostream & operator<< (ostream & outStream, const OntyxConcept & inRight)
{
	static set<string>::const_iterator pParent;
	static multimap<long, OntyxRole>::const_iterator pRole;
	static multimap<long, OntyxProperty>::const_iterator pProperty;
	static multimap<long, OntyxAssociation>::const_iterator pAssoc;

	outStream << setiosflags(ios::right)
			<< setw(20) << "Name:\t" << inRight.getName() << endl;

	if( inRight.getState() )
		outStream << setw(20) << "\t" << "Defined" << endl;
	else
		outStream << setw(20) << "\t" << "Primitive" << endl;

		// empty string gets printed if values don't exist
	outStream << setw(20) << "Code:\t" << inRight.getCode() << endl
			<< setw(20) << "ID:\t" << inRight.getID() << endl
			<< setw(20) << "Namespace:\t" << inRight.getNamespace() << endl
			<< setw(20) << "Kind:\t" << inRight.getKind() << endl;

	if( inRight.m_parent.size() != 0) {
		pParent = inRight.m_parent.begin();
		for( ; pParent != inRight.m_parent.end(); ++pParent)
			outStream << setw(20) << "Defining Concept:\t" << *pParent << endl;
		}

	if( inRight.m_role.size() != 0 ) {
		pRole = inRight.m_role.begin();
		for( ; pRole != inRight.m_role.end(); ++pRole)
			outStream << setw(20) << "Defining Role:\t" << pRole->second << endl;
		}

	if( inRight.m_property.size() != 0) {
		pProperty = inRight.m_property.begin();
		for( ; pProperty != inRight.m_property.end(); ++pProperty)
			outStream << setw(20) << "Property:\t" << pProperty->second << endl;
		}

	if( inRight.m_associations.size() != 0) {
		pAssoc = inRight.m_associations.begin();
		for( ; pAssoc != inRight.m_associations.end(); ++pAssoc)
			outStream << setw(20) << "Association:\t" << pAssoc->second << endl;
		}

	outStream << resetiosflags(ios::right);
	return outStream;
}

			// ********** Public Static Utility Functions 

long OntyxConcept::getConceptMapID(const string & inName)
{
	if( pMapConceptName == 0 ) {
		setError(OntyxTerminology::NO_CONCEPTDEF);
		return -1;
		}
	static mapName::iterator iterMap;

	iterMap = pMapConceptName->find(inName);
	if( iterMap == pMapConceptName->end() )
		return -1;
	return iterMap->second;
}
const string & OntyxConcept::getConceptMapName(long id)
{
	if( pMapConceptID == 0 ) {
		setError(OntyxTerminology::NO_CONCEPTDEF);
		return OntyxTerminology::emptyString;
		}
	static mapID::iterator iterID;

	iterID = pMapConceptID->find(id);
	if( iterID == pMapConceptID->end() )
		return OntyxTerminology::emptyString;
	return iterID->second;
}
bool OntyxConcept::conceptInMap(const string & inName)
{
	if( pMapConceptName == 0 ) {
		setError(OntyxTerminology::NO_CONCEPTDEF);
		return false;
		}
	if( pMapConceptName->find(inName) == pMapConceptName->end() )
		return false;
	return true;
}

void OntyxConcept::setError(long errorCode)
{
	conceptError |= errorCode;
}
void OntyxConcept::clearError(long errorCode)
{
	conceptError &= ~errorCode;
}
bool OntyxConcept::isError(long errorCode)
{
	return ( (conceptError & errorCode) ? true : false );
}
bool OntyxConcept::isError (void)
{
	return ( conceptError ? true : false );
}
void OntyxConcept::clearError (void)
	{ conceptError = 0; }

void OntyxConcept::printError (void)
{
	for( long i = 0; i < OntyxTerminology::numErrors; i++) {
		if( isError( OntyxTerminology::errorArray[i] ) )
			cerr << "Error class OntyxConcept " << OntyxTerminology::errorNames[i] << endl;
		}
}

