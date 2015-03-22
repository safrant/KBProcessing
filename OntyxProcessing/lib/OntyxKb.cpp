//*******************************************************************
// Implementation of the Ontyx class OntyxKb
//
//		Initial class definition 4/10/2001
//
// Dependencies:	STL <string>, <set>, <algorithm>, <map>, <vector>
// Dependencies:	iostream, fstream
// Dependencies:	stdlib
// Dependencies:	OntyxName,OntyxKind,OntyxRoleDef,OntyxPropertyDef,OntyxConcept
// Virtual:			~OntyxKb()
//*******************************************************************

#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <map>
#include <vector>
#include <algorithm>
#include <string>
#include <utility>		// pair
#include <cstdlib>		// exit()

#include "OntyxKb.h"
#include "OntyxConcept.h"
#include "OntyxRoleDef.h"
#include "OntyxPropertyDef.h"
#include "OntyxAssociationDef.h"
#include "OntyxName.h"
#include "OntyxKind.h"

using namespace std;

long OntyxKb::s_conMapID = 0;


			// ********** Constructor/Destructor

OntyxKb::OntyxKb ()
{
	m_kbname = "";
	m_association.clear();
	m_qualifier.clear();

	// not output of picklists in writeOntyx, no treatment of role groups

/*	cerr << "track: 050316" << endl;		unqualified associations */

/*	cerr << "track: 050430" << endl;  		qualifierdef, parentrole in def, picklist items read
											qualified properties, qualified associations */
/*	cerr << "track: 050513" << endl;		owl output modified so that properties/roles renamed if same name concepts
											are found */
/*	cerr << "track: 050603" << endl;		annotation properties made range = xsd:string
											role hierarchies exported to owl */

	cerr << "track: 060828" << endl;	/*	property picklists exported in owl
											owl by code
											owl by name
											roles/properties/associations, concepts renamed on clash with self-defining concepts in owl export
												rather than the other way around (050513)
											role groups in by code
											role groups in by name
											all documentation copied to property/role/assoc declarations from self-defining concepts
											exports to owl using owl:deprecatedclass when appropriate
											retired classes exported to owl as owl:DeprecatedClass
											*/
}
OntyxKb::OntyxKb (const OntyxKb & inRight)
{
		m_kbname = "";
		m_namespace = inRight.m_namespace;
		m_association = inRight.m_association;
		m_qualifier = inRight.m_qualifier;
		m_kind = inRight.m_kind;
		m_property = inRight.m_property;
		m_role = inRight.m_role;
		m_concept = inRight.m_concept;
		m_parents = inRight.m_parents;
		m_conName = inRight.m_conName;
		m_terms = inRight.m_terms;
		m_cuis = inRight.m_cuis;
		m_alphastem = inRight.m_alphastem;
		m_alphastemterms = inRight.m_alphastemterms;
		m_codes = inRight.m_codes;
}

OntyxKb::~OntyxKb()
{
	;
}
			// ********** Mutator Functions
OntyxKb & OntyxKb::setKbName(string & name)
{
	m_kbname = name;
	return *this;
}

OntyxKb & OntyxKb::addNamespace(const OntyxName & inName)
{
	m_namespace.insert(map<long, OntyxName>::value_type(inName.getMapID(), inName) );
	return *this;
}
OntyxKb & OntyxKb::addKind (const OntyxKind & inKind)
{
	m_kind.insert(map<long, OntyxKind>::value_type(inKind.getMapID(), inKind) );
	return *this;
}
OntyxKb & OntyxKb::addPropertyDef (const OntyxPropertyDef & inPropDef)
{
	m_property.insert(map<long, OntyxPropertyDef>::value_type(inPropDef.getMapID(), inPropDef) );
	return *this;
}
OntyxKb & OntyxKb::removePropertyDef (const string & inPropDefName)
{
	m_property.erase( OntyxPropertyDef::getPropDefMapID(inPropDefName) );
	return *this;
}

OntyxKb & OntyxKb::removeQualifierDef (const string & inQualDefName)
{
	m_qualifier.erase( OntyxQualifierDef::getQualifierDefMapID(inQualDefName));
	return *this;
}

OntyxKb & OntyxKb::addRoleDef (const OntyxRoleDef & inRoleDef)
{
	m_role.insert(map<long, OntyxRoleDef>::value_type(inRoleDef.getMapID(), inRoleDef) );
	return *this;
}

OntyxKb & OntyxKb::addAssociationDef (const OntyxAssociationDef & inAssocDef)
{
	m_association.insert(map<long, OntyxAssociationDef>::value_type(inAssocDef.getMapID(), inAssocDef) );
	return *this;
}

OntyxKb & OntyxKb::addQualifierDef (const OntyxQualifierDef & inQualDef)
{
	m_qualifier.insert(map<long, OntyxQualifierDef>::value_type(inQualDef.getMapID(), inQualDef) );
	return *this;
}

OntyxKb & OntyxKb::createPropertiesFromRoles (void)
{
	long maxPropID = getMaxPropID();
	char c_maxPropCode[22];
	string s_maxPropCode;

	map<long, OntyxRoleDef>::iterator pRole = m_role.begin();
	for( ; pRole != m_role.end(); ++pRole) {
		if( OntyxPropertyDef::getPropDefMapID(pRole->second.getName()) == -1) {
			if( maxPropID != -1 ) {
				maxPropID++;
				sprintf(c_maxPropCode, "%ld", maxPropID);
				s_maxPropCode = c_maxPropCode;
				s_maxPropCode = "P" + s_maxPropCode;
				}
			else {
				maxPropID = -1;
				s_maxPropCode = "";
				}
			OntyxPropertyDef tmpPropDef(pRole->second.getName(),s_maxPropCode,maxPropID,"string",false,true,"NCI");
			addPropertyDef(tmpPropDef);
			}
		}
	return *this;
}

OntyxKb & OntyxKb::addConcept (const OntyxConcept & inConcept)
{ // 060616, had to make name registries case sensitive for HL7 processing...  [comment deleted]

//	if( m_conName.find(toLower(inConcept.getName())) != m_conName.end() ) {
	if( m_conName.find(inConcept.getName()) != m_conName.end() ) {
		cout << "ERROR::addConcept:DupConceptName\t" << inConcept.getName()<< endl;
	//	return *this;  // need to rem out  to process hl7
		}
	s_conMapID++;
	m_concept.insert(map<long, OntyxConcept>::value_type( s_conMapID, inConcept) );
//	m_conName.insert(map<string, long>::value_type(toLower(inConcept.getName()), s_conMapID) );	// registry w low-case name
	m_conName.insert(map<string, long>::value_type(inConcept.getName(), s_conMapID) );	// registry w exact name

	// stemmed alpha name token registry
	tString tName = toLower(inConcept.getName());
	m_alphastem.insert(multimap<string,long>::value_type(tName.getAlphaStemSequence(), s_conMapID));

	// cui registry
//	if( inConcept.getPropertyNumber("CUI") == 0 )
//		cerr << "WARNING::addConcept:NoCUI\t" << inConcept.getName() << endl;
/*	else */ if( inConcept.getPropertyNumber("CUI") > 1 )
		cerr << "ERROR::addConcept:MultiCUI\t" << inConcept.getName() << endl;
	else {
		m_cuis.insert(map<string,long>::value_type(inConcept.getPropertyValue("CUI"), s_conMapID));
		}
	// end cui registry

	// code registry
	m_codes.insert(map<string, long>::value_type(inConcept.getCode(), s_conMapID));
	// end code registry

	// term registry hard coded property names AND alpha-ordered & stemmed term registry
	multimap<long, OntyxProperty> l_props = inConcept.getProperties();
	multimap<long, OntyxProperty>::const_iterator pProps = l_props.begin();
	tString tTerm;
	for( ; pProps != l_props.end(); ++pProps) {
		if( pProps->second.getName() == "Preferred_Name" || pProps->second.getName() == "FULL_SYN" ) {
			m_terms.insert(multimap<string, long>::value_type(toLower(pProps->second.getValue()), s_conMapID) );
			tTerm = toLower(pProps->second.getValue());
			m_alphastemterms.insert(multimap<string, long>::value_type(tTerm.getAlphaStemSequence(), s_conMapID) );
			}
		}
	// term registry

	// register parents
	if( inConcept.getNumParents() > 0 ) {
		set<string> l_parents;
		l_parents = inConcept.getParents();
		set<string>::iterator posParent = l_parents.begin();
		for( ; posParent != l_parents.end(); ++posParent)
//			m_parents.insert(multimap<string, long>::value_type(toLower(*posParent), s_conMapID));
			m_parents.insert(multimap<string, long>::value_type(*posParent, s_conMapID));
		}

	return *this;
}

OntyxKb & OntyxKb::codeToXname (void)
{
	//m_codes.insert(map<string, long>::value_type(inConcept.getCode(), s_conMapID));
	//m_codes.erase(tmpConcept.getCode());
	//map<string, long>			m_codes;			// concept code registry

	// getIDFromName(x)

	m_codes.clear();
	string s_code;
	set<string> l_code;
	map<long, OntyxConcept>::iterator pCon = m_concept.begin();
	for( ; pCon != m_concept.end(); ++pCon) {
		s_code = xName(pCon->second.getCode());
		if( l_code.find(s_code) != l_code.end() )  	// found a duplicate xnamed-code, fix manually
			cerr << "ERROR, a duplicate xnamed code has been found, in concept " << pCon->second.getName() << ", continue processing." << endl;
		else {
			l_code.insert(s_code);
			m_codes.insert(map<string, long>::value_type(s_code, getIDFromName(pCon->second.getName())));
			}
		pCon->second.setCode(s_code);
		}	

	return *this;
}

OntyxKb & OntyxKb::nameToXname (void)
{
	map<string, string> mapRename;
	mapRename.clear();
	set<string> uniqueNames;
	string l_name;
	map<string, long>::iterator pName = m_conName.begin();
	for( ; pName != m_conName.end(); ++pName ) {
		l_name = xName(pName->first);
		if( uniqueNames.find(l_name) != uniqueNames.end() || m_conName.find(l_name) != m_conName.end() ) {
			while( uniqueNames.find(l_name) != uniqueNames.end() || m_conName.find(l_name) != m_conName.end() )
				l_name += "_";
			}
		uniqueNames.insert(l_name);
		mapRename.insert( make_pair( pName->first, l_name ) );
	}
	renameConcept( mapRename );
	return *this;
}

void OntyxKb::copyDefsTo (OntyxKb & outKB) const
{
	map<long, OntyxName>::const_iterator pName = m_namespace.begin();
	for( ; pName != m_namespace.end(); ++pName)
		outKB.addNamespace(pName->second);
	map<long, OntyxKind>::const_iterator pKind = m_kind.begin();
	for( ; pKind != m_kind.end(); ++pKind)
		outKB.addKind(pKind->second);
	map<long, OntyxPropertyDef >::const_iterator pProp = m_property.begin();
	for( ; pProp != m_property.end(); ++pProp)
		outKB.addPropertyDef(pProp->second);
	map<long, OntyxRoleDef>::const_iterator pRole = m_role.begin();
	for( ; pRole != m_role.end(); ++pRole)
		outKB.addRoleDef(pRole->second);
	map<long, OntyxAssociationDef>::const_iterator pAssoc = m_association.begin();
	for( ; pAssoc != m_association.end(); ++pAssoc)
		outKB.addAssociationDef(pAssoc->second);
	map<long, OntyxQualifierDef>::const_iterator pQual = m_qualifier.begin();
	for( ; pQual != m_qualifier.end(); ++pQual)
		outKB.addQualifierDef(pQual->second);
}

map<long, OntyxPropertyDef>	OntyxKb::getPropDefs (void)
{
	return m_property;
}

bool OntyxKb::copyConceptTo (long conceptID, OntyxKb & outKB)
{
	if( outKB.conceptNameInKB(getConceptName(conceptID)) == false ) {
		outKB.addConcept(getConcept(conceptID));
		return true;
		}
	return false;
}
bool OntyxKb::copySetConceptsTo (const set<long> & conceptIDs, OntyxKb & outKB)	// returns false if target KB has copy 
{
	bool allFlag = true;
	set<long>::const_iterator pConID = conceptIDs.begin();
	for( ; pConID != conceptIDs.end(); ++pConID) {
		if( !copyConceptTo(*pConID, outKB) )
			allFlag = false;
		}
	return allFlag;
}

bool OntyxKb::moveConceptTo (long conceptID, OntyxKb & outKB)
{
	if( outKB.conceptNameInKB(getConceptName(conceptID)) == false ) {				// had to change this, can only copy concept to if the Defs are from this KB
		if( m_concept.find(conceptID) != m_concept.end() ) {
			outKB.addConcept(getConcept(conceptID));
			removeConcept(conceptID);
			return true;
			}
		}
	return false;
}
bool OntyxKb::moveSetConceptsTo (const set<long> & conceptIDs, OntyxKb & outKB)
{
	bool allFlag = true;
	set<long>::const_iterator pConID = conceptIDs.begin();
	for( ; pConID != conceptIDs.end(); ++pConID) {
		if( !moveConceptTo(*pConID, outKB) )
			allFlag = false;
		}
	return allFlag;
}
bool OntyxKb::moveAllConceptsTo (OntyxKb & outKB)
{
	set<long> allSet;
	map<long, OntyxConcept>::iterator posCon = m_concept.begin();
	for( ; posCon != m_concept.end(); ++posCon )
		allSet.insert(posCon->first);
	return moveSetConceptsTo(allSet, outKB);
}


bool OntyxKb::removeConcept (long conceptID)
{
	// see if conceptID is valid
	if( m_concept.count(conceptID) == 0 ) {
		cerr << "WARNING::removeConcept:  Requested removal of non-existent concept ID = " << conceptID << endl;
		return false;
		}
	
	// term registry hard coded property names
	OntyxConcept tmpConcept = getConcept(conceptID);
	multimap<long, OntyxProperty> l_props = tmpConcept.getProperties();
	multimap<long, OntyxProperty>::iterator pProps = l_props.begin();
	pair< multimap<string, long>::iterator, multimap<string, long>::iterator > p;
	multimap<string, long>::iterator posTerm;
	tString tTerm;
	for( ; pProps != l_props.end(); ++pProps) {
		if( pProps->second.getName() == "Preferred_Name" || pProps->second.getName() == "FULL_SYN" ) {
			p = m_terms.equal_range(toLower(pProps->second.getValue()));
			posTerm = p.first;
			while( posTerm != p.second ) {
				if( posTerm->second == conceptID) {
					m_terms.erase(posTerm);
					p = m_terms.equal_range(toLower(pProps->second.getValue()));
					posTerm = p.first;
					}
				else
					++posTerm;
				}

			tTerm = toLower(pProps->second.getValue());
			p = m_alphastemterms.equal_range(tTerm.getAlphaStemSequence());
			posTerm = p.first;
			while( posTerm != p.second ) {
				if( posTerm->second == conceptID) {
					m_alphastemterms.erase(posTerm);
					p = m_alphastemterms.equal_range(tTerm.getAlphaStemSequence());
					posTerm = p.first;
					}
				else
					++posTerm;
				}

			}
		}
	// term registry

	// stemmed alpha name token registry
	tString tName = toLower(getConcept(conceptID).getName());
	p = m_alphastem.equal_range(tName.getAlphaStemSequence());
	posTerm = p.first;
	while( posTerm != p.second ) {
		if( posTerm->second == conceptID) {
			m_alphastem.erase(posTerm);
			p = m_alphastem.equal_range(tName.getAlphaStemSequence());
			posTerm = p.first;
			}
		else
			++posTerm;
		}
	// end stemmed alpha name token registry


	// deregister parents, 060616 change to case-sensitive to deal with HL7
	if( tmpConcept.getNumParents() > 0 ) {
		set<string> l_parents = tmpConcept.getParents();
		set<string>::iterator posParent = l_parents.begin();
		multimap<string, long>::iterator posPar;
		for( ; posParent != l_parents.end(); ++posParent) {
//			p = m_parents.equal_range(toLower(*posParent));
			p = m_parents.equal_range(*posParent);
			posPar = p.first;
			while( posPar != p.second ) {
				if( posPar->second == conceptID ) {
					m_parents.erase(posPar);
//					p = m_parents.equal_range(toLower(*posParent));
					p = m_parents.equal_range(*posParent);
					posPar = p.first;
					}
				else
					++posPar;
				}
			}
		}

	m_cuis.erase(tmpConcept.getPropertyValue("CUI"));	// doesn't check for 0/dups, errors/warnings are
														// reported during concept addition.
	m_codes.erase(tmpConcept.getCode());
//	m_conName.erase(toLower(getConceptName(conceptID)));
	m_conName.erase(getConceptName(conceptID));
	m_concept.erase(conceptID);

	return true;
}
void OntyxKb::renameConcept(map<string, string> & renamePairs)	// {old name, new name} pairs
{
	map<string, string>::iterator posMap = renamePairs.begin();
	for( ; posMap != renamePairs.end(); ++posMap )
		renameConcept( getIDFromName (posMap->first), posMap->second);
}

void OntyxKb::renameConcept(long conceptID, const string & inName)
{// should be a bool, but will fix later

	string newName = inName, fixName;
	// rename concept
	map<long, OntyxConcept>::iterator pCon = m_concept.find(conceptID);
	if( pCon == m_concept.end() ) {
		cerr << "FATAL ERROR::renameConcept:  Requested concept rename to '" << inName << "' with non-existent ID.  Exiting" << endl;
		exit(0);
		}
	string oldName = pCon->second.getName();

	if( toLower(oldName) != toLower(newName) ) { // it's not a simple upper-lower case change, 060616 disallow renaming to different case
		if( m_conName.find(newName) != m_conName.end() ) { // see if the new name already exists
			cout << "ERROR::renameConcept-exists:\t" << pCon->second.getName()<< endl;
			fixName = newName + "_ren"; // 12/1/11 changed " (ren)" to "_ren"
			if( m_conName.find(fixName) != m_conName.end())
				cerr << "FATAL: \"" << newName << "\" concept has been renamed already" << endl;
			else
				renameConcept(getIDFromName(inName), fixName );
			}
		else {
			cout << "rename-concept\t" << oldName << '\t' << newName << endl;

			pCon->second.reName(newName);
	
			// fix kb registry
			if( m_conName.count(oldName) > 1 )
				cerr << "WARNING::renameConcept: Deleting a repeated name" << endl;
			m_conName.erase(oldName);
			m_conName.insert(map<string, long>::value_type(newName, conceptID));

			// fix parent registry
			pair< multimap<string, long>::iterator, multimap<string, long>::iterator > p;
			if( m_parents.count(oldName) > 0 ) {
				p = m_parents.equal_range(oldName);
				multimap<string, long>::iterator posPar = p.first;
				vector<long> tempVec;
				for( ; posPar != p.second; ++posPar)
					tempVec.push_back(posPar->second);
				m_parents.erase(p.first, p.second);
				vector<long>::iterator i = tempVec.begin();
				for( ; i != tempVec.end(); ++i)
					m_parents.insert(multimap<string, long>::value_type(newName, *i));
				}

			// fix roles, associations, & parent references in other concepts
			for( pCon = m_concept.begin(); pCon != m_concept.end(); ++pCon) {
				pCon->second.resetRoleValueIfValue (oldName, newName);
				pCon->second.resetAssociationValueIfValue (oldName, newName);
				pCon->second.resetParentIfParent (oldName, newName);
				}

			// stemmed alpha name token registry
			multimap<string, long>::iterator posTerm;
			tString tName = toLower(getConcept(conceptID).getName());
			p = m_alphastem.equal_range(tName.getAlphaStemSequence());
			posTerm = p.first;
			while( posTerm != p.second ) {
				if( posTerm->second == conceptID) {
					m_alphastem.erase(posTerm);
					break;
					}
				else
					++posTerm;
				}
			tName = toLower(newName);
			m_alphastem.insert(multimap<string,long>::value_type(tName.getAlphaStemSequence(), conceptID));
			// end stemmed alpha name token registry
			}
		}
}
bool OntyxKb::resolveCui (OntyxConcept & toCon, OntyxConcept & fromCon)
{
	bool setFlag = false;
	if( toCon.findProperty("CUI") == toCon.getEndProperty() ) {
		cout << "no-cui\t" << toCon.getName() << endl;
		OntyxProperty tmpProp("CUI", fromCon.getPropertyValue("CUI"), "");
		toCon.addProperty(tmpProp);
		setFlag = true;
		}
	else if( fromCon.getPropertyValue("CUI") != toCon.getPropertyValue("CUI") ) {
		toCon.setPropertyValue(toCon.propertyIterator("CUI"), fromCon.getPropertyValue("CUI") );
		setFlag = true;
		}
	return setFlag;
}

void OntyxKb::clearKb (void)
{
	m_kbname = "";
	m_namespace.clear();
	m_kind.clear();
	m_property.clear();
	m_role.clear();
	m_concept.clear();
	m_conName.clear();
	m_terms.clear();
	m_parents.clear();
	m_cuis.clear();
	m_alphastem.clear();
	m_alphastemterms.clear();
	m_codes.clear();
	m_association.clear();
	m_qualifier.clear();

}


			// ********** Accessor Functions
string OntyxKb::getKbName (void) const
	{ return m_kbname; }

bool OntyxKb::hasKbName(void) const
{
	if( m_kbname == "" )
		return false;
	return true;
}

bool OntyxKb::hasAssociations (void) const
{
	if( m_association.size() == 0 )
		return false;
	return true;
}

bool OntyxKb::hasQualifiers (void) const
{
	if( m_qualifier.size() == 0 )
		return false;
	return true;
}

bool OntyxKb::hasAssociationDef (const string & assoc) const
{
	map<long, OntyxAssociationDef>::const_iterator pAssoc = m_association.begin();
	for( ; pAssoc != m_association.end(); ++pAssoc) {
		if( pAssoc->second.getName() == assoc )
			return true;
		}
	return false;
}

long OntyxKb::getMaxPropID (void) const
{
	map<long, OntyxPropertyDef>::const_iterator pProperty = m_property.begin();
	long maxID = 0, currentID;
	for( ; pProperty != m_property.end(); ++pProperty ) {
		currentID = pProperty->second.getID();
		if( currentID > maxID )
			maxID = currentID;
		}
	return maxID;
}
long OntyxKb::getMaxConID (void) const
{
	map<long, OntyxConcept>::const_iterator pConcept = m_concept.begin();
	long maxID = 0, currentID;
	for( ; pConcept != m_concept.end(); ++pConcept ) {
		currentID = pConcept->second.getID();
		if( currentID > maxID )
			maxID = currentID;
		}
	return maxID;
}
long OntyxKb::getMaxRoleID (void) const
{
	map<long, OntyxRoleDef>::const_iterator pRole = m_role.begin();
	long maxID = 0, currentID;
	for( ; pRole != m_role.end(); ++pRole ) {
		currentID = pRole->second.getID();
		if( currentID > maxID )
			maxID = currentID;
		}
	return maxID;
}

long OntyxKb::getMaxKindID (void) const
{
	map<long, OntyxKind>::const_iterator pKind = m_kind.begin();
	long maxID = 0, currentID;
	for( ; pKind != m_kind.end(); ++pKind ) {
		currentID = pKind->second.getID();
		if( currentID > maxID )
			maxID = currentID;
		}
	return maxID;
}

const string & OntyxKb::getRoleDefRangeKind (const string & testRoleDefName )
{
	map<long, OntyxRoleDef>::iterator pRoleDef = m_role.begin();
	for( ; pRoleDef != m_role.end(); ++pRoleDef ) {
		if( pRoleDef->second.getName() == testRoleDefName )
			return pRoleDef->second.getRange();
		}
	return OntyxTerminology::emptyString;
}
const string & OntyxKb::getRoleDefDomainKind (const string & testRoleDefName )
{
	map<long, OntyxRoleDef>::iterator pRoleDef = m_role.begin();
	for( ; pRoleDef != m_role.end(); ++pRoleDef ) {
		if( pRoleDef->second.getName() == testRoleDefName )
			return pRoleDef->second.getDomain();
		}
	return OntyxTerminology::emptyString;
}

const string & OntyxKb::getPropertyDefRange (const string & propName )
{
	map<long, OntyxPropertyDef>::iterator pPropDef = m_property.begin();
	for( ; pPropDef != m_property.end(); ++pPropDef ) {
		if( pPropDef->second.getName() == propName )
			return pPropDef->second.getRange();
		}
	return OntyxTerminology::emptyString;
}

map<long, OntyxConcept>::iterator OntyxKb::getFirstConceptIterator (void) 
{
	return m_concept.begin();
}
map<long, OntyxConcept>::iterator OntyxKb::getEndConceptIterator (void) 
{
	return m_concept.end();
}

bool OntyxKb::hasPropertyDef (string & propertyname) const
{
	map<long, OntyxPropertyDef>::const_iterator pProp = m_property.begin();
	for( ; pProp != m_property.end(); ++pProp) {
		if( pProp->second.getName() == propertyname )
			return true;
		}
	return false;
}
bool OntyxKb::hasRoleDef (string & rolename) const
{
	map<long, OntyxRoleDef>::const_iterator pRole = m_role.begin();
	for( ; pRole != m_role.end(); ++pRole) {
		if( pRole->second.getName() == rolename )
			return true;
		}
	return false;
}

bool OntyxKb::hasKind (string & kindname) const
{
	map<long, OntyxKind>::const_iterator pKind = m_kind.begin();
	for( ; pKind != m_kind.end(); ++pKind ) {
		if( pKind->second.getKindname() == kindname )
			return true;
		}
	return false;
}


long OntyxKb::getNumConcepts (void) const
	{	return m_concept.size(); }

long OntyxKb::getNumKinds (void) const
	{	return m_kind.size(); }

long OntyxKb::getNumRoles (void) const
	{	return m_role.size(); }

long OntyxKb::getNumProps (void) const
	{	return m_property.size(); }

long OntyxKb::getNumNames (void) const
	{	return m_namespace.size(); }

long OntyxKb::getNumAssocs (void) const
	{	return m_association.size(); }

long OntyxKb::getNumQuals (void) const
	{	return m_qualifier.size(); }

vector<string> OntyxKb::getKbQuals (void) const
{
	vector<string> tmpQual;
	map<long, OntyxQualifierDef>::const_iterator pQual = m_qualifier.begin();
	for( ; pQual != m_qualifier.end(); ++pQual)
		tmpQual.push_back(pQual->second.getName());
	return tmpQual;
}
vector<string> OntyxKb::getKbKinds (void) const
{
	vector<string> tmpKind;
	map<long, OntyxKind>::const_iterator pKind = m_kind.begin();
	for( ; pKind != m_kind.end(); ++pKind)
		tmpKind.push_back(pKind->second.getKindname());
	return tmpKind;
}
OntyxKind OntyxKb::getKindDef (string & name) const
{
	map<long, OntyxKind>::const_iterator pKind = m_kind.begin();
	for( ; pKind != m_kind.end(); ++pKind ) {
		if( pKind->second.getKindname() == name ) {
			OntyxKind tmpKind(pKind->second);
			return tmpKind;
			}
		}
	OntyxKind nullKind("fake",0,"K0","NCI",false);
	return nullKind;
}

OntyxRoleDef OntyxKb::getRoleDef (string & name) const
{
	map<long, OntyxRoleDef>::const_iterator pRole = m_role.begin();
	for( ; pRole != m_role.end(); ++pRole ) {
		if( pRole->second.getName() == name ) {
			OntyxRoleDef tmpRole(pRole->second);
			return tmpRole;
			}
		}
	OntyxRoleDef nullRole("fake", "R0",0,"NCI","NCI_Kind","NCI_Kind","","");
	return nullRole;
}

OntyxPropertyDef OntyxKb::getPropertyDef (string & name) const
{
	map<long, OntyxPropertyDef>::const_iterator pProp = m_property.begin();
	for( ; pProp != m_property.end(); ++pProp ) {
		if( pProp->second.getName() == name ) {
			OntyxPropertyDef tmpProp(pProp->second);
			return tmpProp;
			}
		}
	OntyxPropertyDef nullProp("fake","P0",0,"string",false,false,"NCI");
	return nullProp;
}

OntyxAssociationDef OntyxKb::getAssociationDef (string & name) const
{
	map<long, OntyxAssociationDef>::const_iterator pAssoc = m_association.begin();
	for( ; pAssoc != m_association.end(); ++pAssoc ) {
		if( pAssoc->second.getName() == name ) {
			OntyxAssociationDef tmpAssoc(pAssoc->second);
			return tmpAssoc;
			}
		}
	OntyxAssociationDef nullAssoc("fake","A0",0,"",true,"NCI");
	return nullAssoc;

}

OntyxQualifierDef OntyxKb::getQualifierDef (string & name) const
{
	map<long, OntyxQualifierDef>::const_iterator pQual = m_qualifier.begin();
	for( ; pQual != m_qualifier.end(); ++pQual ) {
		if( pQual->second.getName() == name ) {
			OntyxQualifierDef tmpQual(pQual->second);
			return tmpQual;
			}
		}
	vector<string> emptyVec;
	OntyxQualifierDef nullQual("fake","Q0",0,"NCI","CP",emptyVec);
	return nullQual;

}


vector<string> OntyxKb::getKbRoles (void) const
{
	vector<string> tmpRole;
	map<long, OntyxRoleDef>::const_iterator pRole = m_role.begin();
	for( ; pRole != m_role.end(); ++pRole)
		tmpRole.push_back(pRole->second.getName());
	return tmpRole;
}
vector<string> OntyxKb::getKbNames (void) const
{
	vector<string> tmpName;
	map<long, OntyxName>::const_iterator pName = m_namespace.begin();
	for( ; pName != m_namespace.end(); ++pName)
		tmpName.push_back(pName->second.getNamespace());
	return tmpName;
}
vector<string> OntyxKb::getKbProps (void) const
{
	vector<string> tmpProp;
	map<long, OntyxPropertyDef >::const_iterator pProp = m_property.begin();
	for( ; pProp != m_property.end(); ++pProp)
		tmpProp.push_back(pProp->second.getName());
	return tmpProp;
}
vector<string> OntyxKb::getKbAssocs (void) const
{
	vector<string> tmpAssoc;
	map<long, OntyxAssociationDef>::const_iterator pAssoc = m_association.begin();
	for( ; pAssoc != m_association.end(); ++pAssoc)
		tmpAssoc.push_back(pAssoc->second.getName());
	return tmpAssoc;
}

const string & OntyxKb::getConceptKind (const OntyxConcept & inConcept) const	// only returns one kind if multikind
{
	if( inConcept.getKind() != OntyxTerminology::emptyString )
		return inConcept.getKind();
	else {
		set<string> outSet;
		findKinds (inConcept, outSet);
		set<string>::iterator posOutSet = outSet.begin();
		return *posOutSet;
		}
}
const string & OntyxKb::getConceptKind (long conceptID) //const
{
	if( getConcept(conceptID).getKind() != OntyxTerminology::emptyString )
		return getConcept(conceptID).getKind();
	return getConceptKind(getConcept(conceptID));
}

OntyxConcept & OntyxKb::getConcept (const string & conName) 
{
//	map<string, long>::const_iterator id = m_conName.find(toLower(conName));
	map<string, long>::const_iterator id = m_conName.find(conName);
	if( id == m_conName.end() ) {
		cerr << "FATAL ERROR. Requested concept with non-existent name '" << conName << "'. Exiting." << endl;
		exit(0);
		}
	map<long, OntyxConcept>::iterator pCon = m_concept.find(id->second);
	if( pCon == m_concept.end() ) {
		cerr << "FATAL ERROR. Requested concept with non-existent ID from name. Exiting." << endl;
		exit(0);
		}
	return pCon->second;
}
OntyxConcept & OntyxKb::getConcept (long conceptID) 
{
	map<long, OntyxConcept>::iterator pCon = m_concept.find(conceptID);
	if( pCon == m_concept.end() ) {
		cerr << "FATAL ERROR. Requested concept with non-existent ID = " << conceptID << ". Exiting." << endl;
		exit(0);
		}
	return pCon->second;
}

const string & OntyxKb::getConceptName (long conceptID) const
{
	map<long, OntyxConcept>::const_iterator pCon = m_concept.find(conceptID);
	if( pCon == m_concept.end() )
		return OntyxTerminology::emptyString;
	return pCon->second.getName();
}


void OntyxKb::findChildren (const OntyxConcept & inConcept, set<string> & outChildren)
{
//	findChildren(toLower(inConcept.getName()), outChildren);
	findChildren(inConcept.getName(), outChildren);
}

void OntyxKb::findChildren (const string & conceptName, set<string> & outChildren)
{
	multimap<string, long>::iterator posParent;
	pair< multimap<string, long>::iterator, multimap<string, long>::iterator > p;
//	p = m_parents.equal_range(toLower(conceptName));
	p = m_parents.equal_range(conceptName);
	for( posParent = p.first; posParent != p.second; ++posParent)
//		outChildren.insert( toLower(getConceptName(posParent->second)) );
		outChildren.insert( getConceptName(posParent->second) );
}

void OntyxKb::clearKindsOfDescendants(const string & name)
{	// includes the parent concept

	// get all descendants in a set first
	set<string> descSet;
	findDescendants (name, descSet);
	// get all concepts now and clear the kind
	set<string>::iterator posDesc = descSet.begin();
	for( ; posDesc != descSet.end(); ++posDesc) {
		getConcept(*posDesc).clearKind();
		}
}

void OntyxKb::assignKindsToDescendants(const string & name, const string & kind)
{
	// get all descendants in a set first
	set<string> descSet;
	findDescendants (name, descSet);
	// cerr << "there are " << descSet.size() << " descendants of " << name << endl;
	// now reset the kind
	set<string>::iterator posDesc = descSet.begin();
	for(posDesc = descSet.begin(); posDesc != descSet.end(); ++posDesc) {
		if( getConcept(*posDesc).getKind() != OntyxTerminology::emptyString 
				&& getConcept(*posDesc).getKind() != kind) {
			cout << "multi-kind\t" << *posDesc << '\t' << kind << '\t' << getConcept(*posDesc).getKind() << endl;
			}
		getConcept(*posDesc).setKind(kind);
		if( getConcept(*posDesc).getKind() != kind )
			cout << "set kind failed for " << *posDesc << endl;
		}
}

void OntyxKb::findDescendants ( const string & conceptName, set<string> & outDescendants) // const
{
	/* this works for untangled hierarchies, doesn't work in multi-hierarchies, need to find another way
	if( outDescendants.find(conceptName) != outDescendants.end() ) { // check for cycles
		cerr << "WARNING:findDescendants::Detected potential cycle in hierarchy, concept name = " << conceptName << endl;
		return;
		}
	*/
	multimap<string, long>::iterator posParent;
	pair< multimap<string, long>::iterator, multimap<string, long>::const_iterator > p;
//	p = m_parents.equal_range(toLower(conceptName));
	p = m_parents.equal_range(conceptName);
	if( p.first != p.second ) {
		for( posParent = p.first; posParent != p.second; ++posParent) {
			if( conceptName == getConceptName(posParent->second) ) {
				cout << "PARENT-RELATION\t" << conceptName << endl;
				cerr << "PARENT-RELATION\t" << conceptName << endl;
				getConcept(conceptName).convertToProperty (conceptName, "PARENT");
				getConcept(conceptName).deleteParent(conceptName);
				continue;
				}
			else
				findDescendants( getConceptName(posParent->second), outDescendants );
			}
		}
//	outDescendants.insert(toLower(conceptName) );
	outDescendants.insert(conceptName);
		
}

void OntyxKb::findDescendants (long conceptID, set<long> & outDescendantIDs) //const
{
	set<string> outDescendants;
	findDescendants(getConceptName(conceptID), outDescendants);
	set<string>::const_iterator posDesc = outDescendants.begin();
	outDescendantIDs.insert(conceptID);
	for( ; posDesc != outDescendants.end(); ++posDesc)
		outDescendantIDs.insert(getIDFromName(*posDesc));
}

void OntyxKb::findRootConcepts (set<string> & outRoots) const
{
	map<long, OntyxConcept>::const_iterator posConcept;
	posConcept = m_concept.begin();
	for( ; posConcept != m_concept.end(); ++posConcept ) {
		if( posConcept->second.getNumParents() == 0 )
//			outRoots.insert(toLower(posConcept->second.getName()));
			outRoots.insert(posConcept->second.getName());
		}
}

void OntyxKb::findRootConceptsInKb (set<string> & outRoots) const
{
	map<long, OntyxConcept>::const_iterator posConcept;
	posConcept = m_concept.begin();
	for( ; posConcept != m_concept.end(); ++posConcept ) {
		if( posConcept->second.getNumParents() == 0 
				|| !parentNameInKb(posConcept->second) )
//			outRoots.insert(toLower(posConcept->second.getName()));
			outRoots.insert(posConcept->second.getName());
		}
}

void OntyxKb::findMultiKind (set<string> & outSet)		// modifies set of concept names
{
	set<string> kindSet;
	map<long, OntyxConcept>::iterator posConcept;
	posConcept = m_concept.begin();
	for( ; posConcept != m_concept.end(); ++posConcept ) {
		kindSet.clear();
		if( posConcept->second.getNumParents() > 1 
//				&& posConcept->second.getKind() == OntyxTerminology::emptyString ) {
				&& posConcept->second.getKind() != OntyxTerminology::emptyString ) {
			findKinds(posConcept->second, kindSet);
			if( kindSet.size() > 1 ) {
				outSet.insert(posConcept->second.getName());				
				}
			}
		}
}

void OntyxKb::findKinds (const OntyxConcept & inConcept, set<string> & outSet) const
{
	set<string> newParents = inConcept.getParents();
	if( newParents.size() != 0 ) {
		for( set<string>::iterator posParent = newParents.begin(); posParent != newParents.end(); ++posParent) {
//			map<string, long>::const_iterator id = m_conName.find(toLower(*posParent));
			map<string, long>::const_iterator id = m_conName.find(*posParent);
			map<long, OntyxConcept>::const_iterator pCon = m_concept.find(id->second);
			findKinds(pCon->second, outSet);
			}
		}
	else {
		outSet.insert(inConcept.getKind());
		}
}

vector<string> OntyxKb::findConceptNamesByProperty (const string & propertyName, const string & propertyValue) const
{	// properties are not indexed, consider doing that in the future
	vector<string> tmp;
	map<long, OntyxConcept>::const_iterator posConcept;
	multimap<long, OntyxProperty> l_props;
	multimap<long, OntyxProperty>::const_iterator pProps;
	posConcept = m_concept.begin();
	for( ; posConcept != m_concept.end(); ++posConcept ) {
		l_props = posConcept->second.getProperties();
		pProps = l_props.begin();
		for( ; pProps != l_props.end(); ++pProps) {
			if( pProps->second.getName() == propertyName && pProps->second.getValue() == propertyValue )
				tmp.push_back(posConcept->second.getName());
			}
		}
	return tmp;	
}

vector<string> OntyxKb::findConceptNamesByProperty (const string & propertyName) const
{	// properties are not indexed, consider doing that in the future
	vector<string> tmp;
	map<long, OntyxConcept>::const_iterator posConcept;
	multimap<long, OntyxProperty> l_props;
	multimap<long, OntyxProperty>::const_iterator pProps;
	posConcept = m_concept.begin();
	for( ; posConcept != m_concept.end(); ++posConcept ) {
		l_props = posConcept->second.getProperties();
		pProps = l_props.begin();
		for( ; pProps != l_props.end(); ++pProps) {
			if( pProps->second.getName() == propertyName)
				tmp.push_back(posConcept->second.getName());
			}
		}
	return tmp;	
}


void OntyxKb::reportOrphans (void) const
{
	map<long, OntyxConcept>::const_iterator posConcept;
	posConcept = m_concept.begin();
	cerr << "Searching for orphans" << endl;
	for( ; posConcept != m_concept.end(); ++posConcept ) {
		if( posConcept->second.getNumParents() == 0 ) {
			cout << "orphan\t" << posConcept->second.getName() << endl;				
			}
		}
	cerr << "Orphan search complete" << endl;
}

void OntyxKb::assignParentToOrphans (const string & name)
{
	map<long, OntyxConcept>::iterator posConcept;
	posConcept = m_concept.begin();
	for( ; posConcept != m_concept.end(); ++posConcept ) {
		if( posConcept->second.getNumParents() == 0 ) {
			posConcept->second.addParent(name);				
			}
		}
}

bool OntyxKb::isParentReferencedInRole (const OntyxConcept & refCon) 
{
	if( refCon.getNumParents() > 0 ) {
		if( refCon.getNumRoles() > 0 ) {
			set<string> locParents;
			getParentSet(refCon, locParents);
			multimap<long, OntyxRole> locRoles = refCon.getRoles();
			multimap<long, OntyxRole>::const_iterator pRole = locRoles.begin();
			for( ; pRole != locRoles.end(); ++pRole) {
				if( locParents.count(pRole->second.getValue()) != 0 )
					return true;
				}
			}
		}
	return false;
}

void OntyxKb::reportIfParentReferencedInRole (void) 
{
	map<long, OntyxConcept>::iterator posConcept = m_concept.begin();
	for( ; posConcept != m_concept.end(); ++posConcept) {
		if( isParentReferencedInRole(posConcept->second) )
			cout << "role-refers-parent\t" << posConcept->second.getKind() + "\t" + posConcept->second.getName() << endl;
		}
}

bool OntyxKb::parentNameInKb (const OntyxConcept & refCon) const // returns true if at least one parent is in Kb
{
	bool found = false;
	set<string>::const_iterator posParent = refCon.getFirstParent();
	for( ; posParent != refCon.getEndParent(); ++posParent) {
		if( conceptNameInKB(*posParent) )
			found = true;
		}
	return found;
}

void OntyxKb::getParentSet (const OntyxConcept & refCon, set<string> & outParentSet) 
{
	set<string> locParents;
	if( refCon.getNumParents() > 0 ) {
		locParents = refCon.getParents();
		set<string>::iterator posParent = locParents.begin();
		for( ; posParent != locParents.end(); ++posParent) {
			outParentSet.insert(*posParent);
			OntyxConcept parConcept = getConcept(*posParent);
			getParentSet(parConcept, outParentSet);
			}
		}
}

void OntyxKb::getKbIDSet (set<long> & idSet)
{
	for(map<long, OntyxConcept>::const_iterator pCon = m_concept.begin();
			pCon != m_concept.end(); ++pCon)
		idSet.insert(pCon->first);
}
		
bool OntyxKb::conceptNameInKB (const string & inName) const
{
//	if( m_conName.find(toLower(inName)) != m_conName.end() )
	if( m_conName.find(inName) != m_conName.end() )
		return true;
	return false;
	
}

bool OntyxKb::termInKB (const string & term) const
{
	if( m_terms.find(toLower(term)) != m_terms.end() )
		return true;
	return false;
}

bool OntyxKb::alphastemtermInKB (const string & term) const
{
	tString tTerm = toLower(term);
	if( m_alphastemterms.find(tTerm.getAlphaStemSequence()) != m_alphastemterms.end() )
		return true;
	return false;
}


long OntyxKb::getIDFromName (const string & conceptName) const		// case-insensitive search, 060616 made case-sensitive for HL7
{
//	map<string, long>::const_iterator pName = m_conName.find(toLower(conceptName));
	map<string, long>::const_iterator pName = m_conName.find(conceptName);
	if( pName != m_conName.end() )
		return pName->second;
	return -1;
}

long OntyxKb::getIDFromCui (const string & cui ) const			// returns ID or -1 if not found
{
	map<string, long>::const_iterator pCui;
	if( (pCui = m_cuis.find(cui)) != m_cuis.end() )
		return pCui->second;
	return -1;
}

long OntyxKb::getIDFromCode (const string & code ) const			// returns ID or -1 if not found
{
	map<string, long>::const_iterator pCode;
	if( (pCode = m_codes.find(code)) != m_codes.end() )
		return pCode->second;
	return -1;
}

void OntyxKb::getIDFromTerm (const string & term, set<long> & outSet) const	// case-insensitive search
{
	multimap<string, long>::const_iterator posTerm;
	pair< multimap<string, long>::const_iterator, multimap<string, long>::const_iterator > p;
	p = m_terms.equal_range(toLower(term));
	for( posTerm = p.first; posTerm != p.second; ++posTerm)
		outSet.insert(posTerm->second);
}
void OntyxKb::getIDFromAlphaStemTerm (const string & term, set<long> & outSet) const
	// case-insensitive search, checks concept name terms
{
	multimap<string, long>::const_iterator posTerm;
	pair< multimap<string, long>::const_iterator, multimap<string, long>::const_iterator > p;
	tString tName = toLower(term);
	p = m_alphastem.equal_range( tName.getAlphaStemSequence() );
	for( posTerm = p.first; posTerm != p.second; ++posTerm)
		outSet.insert(posTerm->second);
}

void OntyxKb::getIDFromAlphaStemTermsTerm (const string & term, set<long> & outSet) const
	// case-insensitive search, checks pref_name & FULL_SYN terms
{
	multimap<string, long>::const_iterator posTerm;
	pair< multimap<string, long>::const_iterator, multimap<string, long>::const_iterator > p;
	tString tName = toLower(term);
	p = m_alphastemterms.equal_range(tName.getAlphaStemSequence() );
	for( posTerm = p.first; posTerm != p.second; ++posTerm)
		outSet.insert(posTerm->second);
}

void OntyxKb::getConceptTerms (const OntyxConcept & inConcept, set<string> & outSet) const
{
	multimap<long, OntyxProperty> l_props = inConcept.getProperties();
	multimap<long, OntyxProperty>::const_iterator pProps = l_props.begin();
	for( ; pProps != l_props.end(); ++pProps) {
		if( pProps->second.getName() == "Preferred_Name" || pProps->second.getName() == "FULL_SYN" )
			outSet.insert(toLower(pProps->second.getValue()) );
		}
}

void OntyxKb::getAlphaStemConceptTerms (const OntyxConcept & inConcept, set<string> & outSet) const
{
	multimap<long, OntyxProperty> l_props = inConcept.getProperties();
	multimap<long, OntyxProperty>::const_iterator pProps = l_props.begin();
	for( ; pProps != l_props.end(); ++pProps) {
		if( pProps->second.getName() == "Preferred_Name" || pProps->second.getName() == "FULL_SYN" ) {
			tString tName = toLower(pProps->second.getValue());
			outSet.insert(tName.getAlphaStemSequence());
			}
		}
}

bool OntyxKb::verifyReferences(const long refnumber) const
{
	bool referenceOK = true;
	set<string> conPar;
	set<string>::const_iterator pConPar;
	multimap<long, OntyxRole> conRole;
	multimap<long, OntyxRole>::const_iterator pConRole;
	map<long, OntyxConcept>::const_iterator pCon = m_concept.begin();
	for( ; pCon != m_concept.end(); ++pCon ) {
		// check parent references
		conPar = pCon->second.getParents();
		pConPar = conPar.begin();
		for( ; pConPar != conPar.end(); ++pConPar ) {
			if( !conceptNameInKB(*pConPar) ) {
				referenceOK = false;
				cout << "unres-parent\t" << refnumber << '\t' << pCon->second.getName() << '\t' << *pConPar << endl;
				}
			}
		// check role references
		conRole = pCon->second.getRoles();
		pConRole = conRole.begin();
		for( ; pConRole != conRole.end(); ++pConRole ) {
			if( !conceptNameInKB(pConRole->second.getValue()) ) {
				referenceOK = false;
				cout << "unres-role\t" << refnumber << '\t' << pCon->second.getName() << '\t' << pConRole->second.getValue() << endl;
				}
			}
		}
	return referenceOK;
}
void OntyxKb::fixReferences (void)
{
	long l_id;
	set<string> conPar;
	set<string>::const_iterator pConPar;
	multimap<long, OntyxRole> conRole;
	multimap<long, OntyxRole>::iterator pConRole;
	multimap<long, OntyxAssociation> conAssoc;
	multimap<long, OntyxAssociation>::iterator pConAssoc;

	long maxPropID = getMaxPropID();
	char c_maxPropCode[22];
	string s_maxPropCode = "";
	if( OntyxPropertyDef::getPropDefMapID("PARENT") == -1) {
		maxPropID++;
		sprintf(c_maxPropCode, "%ld", maxPropID);
		s_maxPropCode = c_maxPropCode;
		s_maxPropCode = "P" + s_maxPropCode;
		OntyxPropertyDef tmpPropDef("PARENT",s_maxPropCode,maxPropID,"string",false,true,"NCI");
		addPropertyDef(tmpPropDef);
		}
	
	map<long, OntyxConcept>::iterator pCon = m_concept.begin();
	for( ; pCon != m_concept.end(); ++pCon ) {
		// check parent references
		conPar = pCon->second.getParents();
		pConPar = conPar.begin();
		while( pConPar != conPar.end() ) {
			if( (l_id = getIDFromName(*pConPar) ) == -1 ) { 
				cout << "convert-parent-no-name-resolution\t" << '\t' << pCon->second.getName() << '\t' << *pConPar << endl;
				pCon->second.convertToProperty ( *pConPar, "PARENT");
				pCon->second.deleteParent (*pConPar);
				conPar = pCon->second.getParents();
				pConPar = conPar.begin();
				}
			else if( getConcept(l_id).getName() != *pConPar ) { // case differences
				cout << "convert-parent-rename\t" << '\t' << pCon->second.getName() << '\t' << *pConPar << endl;
				pCon->second.deleteParent (*pConPar);
				pCon->second.addParent( getConcept(l_id).getName() );
				conPar = pCon->second.getParents();
				pConPar = conPar.begin();
				}
			else
				++pConPar;
			}
		// check role references
		conRole = pCon->second.getRoles();
		pConRole = conRole.begin();
		while( pConRole != conRole.end()) {
			if( (l_id = getIDFromName(pConRole->second.getValue()) ) == -1) {

				if( OntyxPropertyDef::getPropDefMapID(pConRole->second.getName()) == -1) {
					maxPropID++;
					sprintf(c_maxPropCode, "%ld", maxPropID);
					s_maxPropCode = c_maxPropCode;
					s_maxPropCode = "P" + s_maxPropCode;
					OntyxPropertyDef tmpPropDef1(pConRole->second.getName(),s_maxPropCode,maxPropID,"string",false,false,"NCI");
					addPropertyDef(tmpPropDef1);
					cerr << "Created property " << pConRole->second.getName() << endl;
					if( OntyxPropertyDef::getPropDefMapID(pConRole->second.getName()) == -1)
						cerr << "ERROR: property '" << pConRole->second.getName() << "' not created" << endl;
					}

				cout << "convert-role-no-name-resolution\t" << '\t' << pCon->second.getName() << '\t' 
					<< pConRole->second.getName() << '\t' << pConRole->second.getValue() << endl;

				pCon->second.convertToProperty (pConRole->second.getValue(), pConRole->second.getName());
				pCon->second.deleteRole (pConRole->second.getName(), pConRole->second.getValue());
				conRole = pCon->second.getRoles();
				pConRole = conRole.begin();
				}
			else if( getConcept(l_id).getName() != pConRole->second.getValue() ) {  // case differences
				cout << "convert-role-rename\t" << '\t' << pCon->second.getName() << '\t' 
					<< pConRole->second.getValue() << '\t' << getConcept(l_id).getName() << endl;
			//	pCon->second.convertToProperty (pConRole->second.getValue(), pConRole->second.getName());
				OntyxRole tmpRole ( pConRole->second.getName(),
										getConcept(l_id).getName(), pConRole->second.getModifier() );
				pCon->second.deleteRole (pConRole->second.getName(), pConRole->second.getValue());
				pCon->second.addRole (tmpRole);
				conRole = pCon->second.getRoles();
				pConRole = conRole.begin();
				}
			else
				++pConRole;
			}
		// check association references
		conAssoc = pCon->second.getAssociations();
		pConAssoc = conAssoc.begin(); 
		while( pConAssoc != conAssoc.end() ) {
			if( (l_id = getIDFromName(pConAssoc->second.getValue()) ) == -1) {

				if( OntyxPropertyDef::getPropDefMapID(pConAssoc->second.getName()) == -1) {
					maxPropID++;
					sprintf(c_maxPropCode, "%ld", maxPropID);
					s_maxPropCode = c_maxPropCode;
					s_maxPropCode = "P" + s_maxPropCode;
					OntyxPropertyDef tmpPropDef1(pConAssoc->second.getName(),s_maxPropCode,maxPropID,"string",false,false,"NCI");
					addPropertyDef(tmpPropDef1);
					cerr << "Created property " << pConAssoc->second.getName() << endl;
					if( OntyxPropertyDef::getPropDefMapID(pConAssoc->second.getName()) == -1)
						cerr << "ERROR: property '" << pConAssoc->second.getName() << "' not created" << endl;
					}

				cout << "convert-association-no-name-resolution\t" << '\t' << pCon->second.getName() << '\t' 
					<< pConAssoc->second.getName() << '\t' << pConAssoc->second.getValue() << endl;
				OntyxProperty tmpProp(pConAssoc->second.getName(), pConAssoc->second.getValue());
				pCon->second.addProperty(tmpProp);
				OntyxAssociation tmpAssoc(pConAssoc->second.getName(), pConAssoc->second.getValue());
				pCon->second.deleteAssociation(tmpAssoc);
				conAssoc = pCon->second.getAssociations();
				pConAssoc = conAssoc.begin();
cerr << "fixing association" << endl;
				}
			else
				++pConAssoc;
			}
		}
}	
void OntyxKb::remapRoleReferences (map<string, string> & inMap)
{
	map<string, string>::iterator posMap;
	multimap<long, OntyxRole> conRole;
	multimap<long, OntyxRole>::iterator pConRole;
	map<long, OntyxConcept>::iterator pCon = m_concept.begin();
	for( ; pCon != m_concept.end(); ++pCon ) {
		conRole = pCon->second.getRoles();
		pConRole = conRole.begin();
		while( pConRole != conRole.end() ) {
			posMap = inMap.find(pConRole->second.getValue());
			if( posMap != inMap.end() ) {
				if( getIDFromName(posMap->second) == -1)
					cerr << "remapped role target not found in kb: " << pConRole->second.getValue() << endl;
				pCon->second.resetRoleValueIfValue (posMap->first, posMap->second);
				cout << "role-ref-fixmap\t" << pCon->second.getName() << '\t'
					<< pConRole->second.getName() << '\t' << posMap->second << endl;
				conRole = pCon->second.getRoles();
				pConRole = conRole.begin();
				}
			else
				++pConRole;
			}
		}
}

void OntyxKb::remapRoleReferences (map<string, string> & inMap, OntyxKb & otherKb)
{
	map<string, string>::iterator posMap;
	multimap<long, OntyxRole> conRole;
	multimap<long, OntyxRole>::iterator pConRole;
	map<long, OntyxConcept>::iterator pCon = otherKb.m_concept.begin();
	for( ; pCon != otherKb.m_concept.end(); ++pCon ) {
		conRole = pCon->second.getRoles();
		pConRole = conRole.begin();
		while( pConRole != conRole.end() ) {
			posMap = inMap.find(pConRole->second.getValue());
			if( posMap != inMap.end() ) {
				if( getIDFromName(posMap->second) == -1)
					cerr << "remapped role target not found in kb: " << pConRole->second.getValue() << endl;
				pCon->second.resetRoleValueIfValue (posMap->first, posMap->second);
				cout << "role-ref-fixmap\t" << pCon->second.getName() << '\t'
					<< pConRole->second.getName() << '\t' << posMap->second << endl;
				conRole = pCon->second.getRoles();
				pConRole = conRole.begin();
				}
			else
				++pConRole;
			}
		}
}



			// ********** Comparison Functions 

void OntyxKb::diff_Concepts ( OntyxKb & inCompare, OntyxKb & outKb) const
{
	// uses code to match concepts
	map<long, OntyxConcept>::const_iterator pm_concept = m_concept.begin();
	long l_ID;
	for( ; pm_concept != m_concept.end(); ++pm_concept) {
		l_ID = inCompare.getIDFromCode(pm_concept->second.getCode());	// new
		if( l_ID != -1 ) { 												// new
			outKb.addConcept( pm_concept->second );
			outKb.addConcept( inCompare.getConcept(l_ID) );			// new
			}
		}
	copyDefsTo(outKb);
}

/* I did something & broke this function, don't remember now what I did, will rewrite
void OntyxKb::unique_Concepts (const OntyxKb & inCompare, OntyxKb & outKb) const
{
	map<long, OntyxConcept>::iterator pIn_concept = outKb.m_concept.begin();

	set_symmetric_difference(m_concept.begin(), m_concept.end(), inCompare.m_concept.begin(),
			inCompare.m_concept.end(), inserter(outKb.m_concept, pIn_concept) );
	outKb.m_namespace = m_namespace;
	outKb.m_kind = m_kind;
	outKb.m_property = m_property;
	outKb.m_role = m_role;
}
*/
	// re-written from above
void OntyxKb::unique_Concepts (const OntyxKb & inCompare, OntyxKb & outKb) const
{
	map<long, OntyxConcept>::const_iterator pm_concept = m_concept.begin();
	long l_ID;
		// this KB first
	OntyxConcept tmpConcept1("deleted_concepts", "C0", 0, "NCI", "NCI_Kind", true);
	outKb.addConcept(tmpConcept1);
	for( ; pm_concept != m_concept.end(); ++pm_concept) {
		l_ID = inCompare.getIDFromCode(pm_concept->second.getCode());
		if( l_ID == -1 ) { 
			outKb.addConcept( pm_concept->second );
			}
		}
		// comparison KB second
	OntyxConcept tmpConcept2("new_concepts", "C00", 1, "NCI", "NCI_Kind", true);
	outKb.addConcept(tmpConcept2);
	pm_concept = inCompare.m_concept.begin();
	for( ; pm_concept != inCompare.m_concept.end(); ++pm_concept) {
		l_ID = getIDFromCode(pm_concept->second.getCode());
		if( l_ID == -1 ) { 
			outKb.addConcept( pm_concept->second );
			}
		}
	copyDefsTo(outKb);
}


			// ********** Print Functions
void OntyxKb::printKbConcepts (void) const
// Prints all concepts in the KB
{
	map<long, OntyxConcept>::const_iterator pConcept;

	if( m_concept.size() != 0) {
		pConcept = m_concept.begin();
		for( ; pConcept != m_concept.end(); ++pConcept) 
			cout << endl << pConcept->second << endl;
		}
}

			// ********** File I/O Functions

void OntyxKb::readOntyxXMLFile (const string & inFile)
{
	try {
		XMLPlatformUtils::Initialize();
		}
	catch (const XMLException& toCatch) {
		char* message = XMLString::transcode(toCatch.getMessage());
		cout << "Initialization Error: " << message << endl;
		XMLString::release(&message);
		exit(-1);
		}

	SAX2XMLReader * parser = XMLReaderFactory::createXMLReader();
	parser->setFeature(XMLUni::fgSAX2CoreValidation, false);  // optional
	parser->setFeature(XMLUni::fgSAX2CoreNameSpaces, false);  // optional
	parser->setFeature(XMLUni::fgXercesLoadExternalDTD, false);  // optional

	OntylogHandler * handler = new OntylogHandler(*this);
	parser->setContentHandler(handler);
	parser->setErrorHandler(handler);

	try {
		parser->parse(inFile.c_str());
		}
	catch (const XMLException& toCatch) {
		char * message = XMLString::transcode(toCatch.getMessage());
		cout << "Exception message is: \n"
		 << message << "\n";
		XMLString::release(&message);
		exit(-1);
		}
	catch (const SAXParseException& toCatch) {
		char* message = XMLString::transcode(toCatch.getMessage());
		cout << "Exception message is: \n"
		 << message << "\n";
		XMLString::release(&message);
		exit(-1);
		}
	catch (...) {
		cout << "Unexpected Exception \n" ;
		exit(-1);
		}

	delete parser;
	delete handler;
	XMLPlatformUtils::Terminate();

	return;
}

void OntyxKb::readFlatFile (const string & inFile, const string & inNamespace, const string & kind,
				const string & sem_type, const string & parent, const string & source)
{
	ifstream kbFile;
	kbFile.open(inFile.c_str(), ifstream::in);
	if( !kbFile.good() ) {
		cerr << "Error opening file \"" << inFile << "\", will EXIT." << endl;
		exit(0);
		}

	OntyxName tmpName (inNamespace, -1,"");
	addNamespace(tmpName);

	OntyxKind tmpKind (kind, -1, "", inNamespace, false);
	addKind(tmpKind);

	OntyxPropertyDef tmpSem("Semantic_Type", "", -1, "string", false, true, inNamespace);
	addPropertyDef(tmpSem);
	OntyxPropertyDef tmpPN("Preferred_Name", "", -1, "string", false, true, inNamespace);
	addPropertyDef(tmpPN);
	OntyxPropertyDef tmpFS("FULL_SYN", "", -1, "long_string", false, true, inNamespace);
	addPropertyDef(tmpFS);

//      Modified 081204: Create FlatKb using qualified values
//
//	string full_syn;
//	string full_syn_pref = "<![CDATA[<term-name>";
//	string full_syn_value;
//	string full_syn_suf = "</term-name><term-group>PT</term-group><term-source>" + source + "</term-source>]]>";
	string inputLine;
	string conname;

	while( getline(kbFile, inputLine), !kbFile.eof() ) {
		if( inputLine.size() != 0 ) {
			conname = inputLine;
			OntyxConcept tmpConcept(conname, "", -1, inNamespace, kind, false);
			tmpConcept.addParent(parent);
			OntyxProperty tmpProperty("Preferred_Name", conname, "");
			OntyxProperty tmp2Property("Semantic_Type", sem_type, "");
//			full_syn = full_syn_pref + conname + full_syn_suf;

			OntyxProperty tmp3Property("FULL_SYN", conname, "");
			OntyxQualifier tmpQual("Syn_Term_Type", "PT" );
			OntyxQualifier tmp2Qual("Syn_Source", "NCI" );
			tmp3Property.addQualifier(tmpQual).addQualifier(tmp2Qual);


			tmpConcept.addProperty(tmpProperty).addProperty(tmp2Property).addProperty(tmp3Property);
			addConcept(tmpConcept);
			}
		}
}
void OntyxKb::writeOntyxXMLFile (const string & filename, bool flag, bool noProps) // flag = true:renumber
// if noProps is true, properties with names derived from roles are not exported
// 060813 since we have been deleting properties from a file listing, will make the "noProps" argument
// irrelevant and will change the signature in the future, the bool noProps doesn't make a difference
// as of 060813.
{
	string h1 = "<?xml version=\"1.0\" encoding=\"UTF-8\"?><!DOCTYPE terminology SYSTEM \"ontylog.dtd\">\n\n",
			h2 = "<terminology ref_by=\"name\" if_exists_action=\"replace\">",
			header = h1 + h2;
	string trailer = "</terminology>";
	ofstream outFile;
	outFile.open(filename.c_str());
	long counter = 1;

	outFile << header << endl << endl;

	map<long, OntyxName>::iterator pName = m_namespace.begin();
	for( ; pName != m_namespace.end(); ++pName ) {
		outFile << "<namespaceDef>" << endl
			<< "<name>" << pName->second.getNamespace() << "</name>" << endl
			<< "<code>" << ( flag ? "N" : "") ;
		if( flag ) 
			outFile << counter;
		else
			outFile << pName->second.getCode();
		outFile << "</code>" << endl << "<id>";
		if ( flag )
			outFile << counter++;
		else 
			outFile << pName->second.getID();
		outFile << "</id>" << endl
			<< "</namespaceDef>" << endl << endl;
		}

	map<long, OntyxKind>::iterator pKind = m_kind.begin();
	for( ; pKind != m_kind.end(); ++pKind) {
		outFile  << "<kindDef reference=\"" << (pKind->second.getReference()? "true" : "false") << "\">" << endl
			<< "<name>" << pKind->second.getKindname() << "</name>" << endl
			<< "<code>" << ( flag ? "K" : "") ;
		if( flag ) 
			outFile << counter;
		else
			outFile << pKind->second.getCode();
		outFile << "</code>" << endl << "<id>";
		if ( flag )
			outFile << counter++;
		else 
			outFile << pKind->second.getID();
		outFile << "</id>" << endl
			<< "<namespace>" << pKind->second.getNamespace() << "</namespace>" << endl
			<< "</kindDef>" << endl << endl;
		}

	map<long, OntyxRoleDef>::iterator pRole = m_role.begin();
	for( ; pRole != m_role.end(); ++pRole ) {
		outFile << "<roleDef>" << endl
			<< "<name>" << pRole->second.getName () << "</name>" << endl
			<< "<code>" << ( flag ? "R" : "") ;
		if( flag ) 
			outFile << counter;
		else
			outFile << pRole->second.getCode();
		outFile << "</code>" << endl << "<id>";
		if ( flag )
			outFile << counter++;
		else 
			outFile << pRole->second.getID();
		outFile << "</id>" << endl
			<< "<namespace>" << pRole->second.getNamespace() << "</namespace>" << endl
			<< "<domain>" << pRole->second.getDomain() << "</domain>" << endl
			<< "<range>" << pRole->second.getRange() << "</range>" << endl;
		if( pRole->second.hasParent() )
			outFile << "<parentRole>" << pRole->second.getParentRole() << "</parentRole>" << endl;
		outFile << "</roleDef>" << endl << endl;
		}

	map<long, OntyxPropertyDef>::iterator pProp = m_property.begin();
	for( ; pProp != m_property.end(); ++pProp ) {
			outFile << "<propertyDef>" << endl
				<< "<name>" << pProp->second.getName () << "</name>" << endl
				<< "<code>" << ( flag ? "P" : "") ;
			if( flag ) 
				outFile << counter;
			else
				outFile << pProp->second.getCode();
			outFile << "</code>" << endl << "<id>";
			if ( flag )
				outFile << counter++;
			else 
				outFile << pProp->second.getID();
			outFile << "</id>" << endl
				<< "<namespace>" << pProp->second.getNamespace() << "</namespace>" << endl
				<< "<range>" << pProp->second.getRange() << "</range>" << endl
				<< (pProp->second.isContainsIndex() ? "<containsIndex/>\n" : "" )
				<< (pProp->second.isLocalized() ? "<localized/>\n" : "" );
			if( pProp->second.hasPickList() ) {
				outFile << "<pickList>" << endl;
				vector<string> pickItem = pProp->second.getPickList();
				vector<string>::iterator pPick = pickItem.begin();
				for( ; pPick != pickItem.end(); ++pPick )
					outFile << "<pickListItem>" << *pPick << "</pickListItem>" << endl;
				outFile << "</pickList>" << endl;
				}
			outFile << "</propertyDef>" << endl << endl;
//			}
		}

	// association def section
	map<long, OntyxAssociationDef>::iterator pAssoc = m_association.begin();
	for( ; pAssoc != m_association.end(); ++pAssoc ) {
		outFile << "<associationDef>" << endl
			<< "<name>" << pAssoc->second.getName () << "</name>" << endl
			<< "<code>" << ( flag ? "A" : "") ;
		if( flag ) 
			outFile << counter;
		else
			outFile << pAssoc->second.getCode();
		outFile << "</code>" << endl << "<id>";
		if ( flag )
			outFile << counter++;
		else 
			outFile << pAssoc->second.getID();
		outFile << "</id>" << endl
			<< "<namespace>" << pAssoc->second.getNamespace() << "</namespace>" << endl;
		if( pAssoc->second.hasInverse() )
			outFile << "<inverseName>" << pAssoc->second.getInverse() << "</inverseName>" << endl;
		if( pAssoc->second.isDisplayable() )
			outFile << "<displayable/>" << endl;
		outFile
			<< "</associationDef>" << endl << endl;
		}

	// qualifier def section, doesn't output the pick list items
	map<long, OntyxQualifierDef>::iterator pQual = m_qualifier.begin();
	for( ; pQual != m_qualifier.end(); ++pQual ) {
		outFile << "<qualifierDef type=\"" << pQual->second.getQualifierType() << "\">" << endl
			<< "<name>" << pQual->second.getName() << "</name>" << endl
			<< "<code>" << ( flag ? "Q" : "") ;
		if( flag )
			outFile << counter;
		else
			outFile << pQual->second.getCode();
		outFile << "</code>" << endl << "<id>";
		if( flag )
			outFile << counter++;
		else
			outFile << pQual->second.getID();
		outFile << "</id>" << endl
			<< "<namespace>" << pQual->second.getNamespace() << "</namespace>" << endl;
		if( pQual->second.hasPickList() ) {
			outFile << "<pickList>" << endl;
			vector<string> pickItem = pQual->second.getPickList();
			vector<string>::iterator pPick = pickItem.begin();
			for( ; pPick != pickItem.end(); ++pPick )
				outFile << "<pickListItem>" << *pPick << "</pickListItem>" << endl;
			outFile << "</pickList>" << endl;
			}
		outFile << "</qualifierDef>" << endl << endl;
		}

	// concept def section
	map<long, OntyxConcept>::iterator pCon = m_concept.begin();
	for( ; pCon != m_concept.end(); ++pCon ) {
		outFile << "<conceptDef>" << endl
			<< "<name>" << verifyOutString(pCon->second.getName()) << "</name>" << endl
			<< "<code>" << ( flag ? "C" : "") ;
		if( flag ) 
			outFile << counter;
		else
			outFile << pCon->second.getCode();
		outFile << "</code>" << endl << "<id>";
		if ( flag )
			outFile << counter++;
		else 
			outFile << pCon->second.getID();
		outFile << "</id>" << endl
			<< "<namespace>" << pCon->second.getNamespace() << "</namespace>" << endl
			<< (pCon->second.getState() ? "" : "<primitive/>\n" );
		if( pCon->second.getKind() != "" )
			outFile << "<kind>" + pCon->second.getKind() + "</kind>" << endl;

		outFile << "<definingConcepts>" << endl;
		set<string> conPar = pCon->second.getParents();
		set<string>::const_iterator pConPar = conPar.begin();
		for( ; pConPar != conPar.end(); ++pConPar ) {
			outFile << "<concept>" << verifyOutString(*pConPar) << "</concept>" << endl;
			}
		outFile << "</definingConcepts>" << endl;

		outFile	<< "<definingRoles>" << endl;
		multimap<long, OntyxRole> conRoles = pCon->second.getRoles();
		multimap<long, OntyxRole>::const_iterator pConRoles = conRoles.begin();
		// had to be split into no_role_groups and some/all_role_groups
		for( ; pConRoles != conRoles.end(); ++pConRoles ) {
			if( pConRoles->second.getRolegroup() == 0 ) {
				outFile << "<role>" << endl
					<< "<" + pConRoles->second.getModifier() + "/>" << endl
					<< "<name>" << pConRoles->second.getName() << "</name>" << endl
					<< "<value>" << verifyOutString(pConRoles->second.getValue()) << "</value>" << endl
					<< "</role>" << endl;
				}
			}
		if( pCon->second.hasRolegroups() ) { 
			for(long groupCounter = 1; groupCounter <= pCon->second.getNumRoleGroups(); ++groupCounter) {
				outFile << "<roleGroup>" << endl;
				for( pConRoles = conRoles.begin(); pConRoles != conRoles.end(); ++pConRoles ) {
					if( pConRoles->second.getRolegroup() == groupCounter ) {
						outFile << "<role>" << endl
							<< "<" + pConRoles->second.getModifier() + "/>" << endl
							<< "<name>" << pConRoles->second.getName() << "</name>" << endl
							<< "<value>" << verifyOutString(pConRoles->second.getValue()) << "</value>" << endl
							<< "</role>" << endl;
						}
					}
				outFile << "</roleGroup>" << endl;
				}
			}
		outFile << "</definingRoles>" << endl;


		outFile << "<properties>" << endl;
		multimap<long, OntyxProperty> conProp = pCon->second.getProperties();
		multimap<long, OntyxProperty>::const_iterator pConProp = conProp.begin();
		string propString;
		for( ; pConProp != conProp.end(); ++pConProp ) {
				propString = verifyOutString(pConProp->second.getValue());
				outFile << "<property>" << endl
					<< "<name>" << pConProp->second.getName() << "</name>" << endl
					<< "<value>" << propString << "</value>" << endl;
					if( pConProp->second.hasQualifier() ) {
						outFile << "<qualifiers>" << endl;
						vector<OntyxQualifier> vqual = pConProp->second.getQualifiers();
						vector<OntyxQualifier>::iterator pvqual = vqual.begin();
						for( ; pvqual != vqual.end(); ++pvqual )
							outFile << "<qualifier>" << endl
								<< "<name>" << pvqual->getName() << "</name>" << endl
								<< "<value>" << verifyOutString(pvqual->getValue()) << "</value>" << endl 
								<< "</qualifier>" << endl;
						outFile << "</qualifiers>" << endl;
						}
					// locale not being taken care of
				outFile << "</property>" << endl;
//				}
			}
		outFile << "</properties>" << endl;

		if( pCon->second.getNumAssociations() > 0 ) {
			outFile << "<associations>" << endl;
			multimap<long, OntyxAssociation> conAssoc = pCon->second.getAssociations();
			multimap<long, OntyxAssociation>::const_iterator pConAssoc = conAssoc.begin();
			for( ; pConAssoc != conAssoc.end(); ++pConAssoc ) {
				outFile << "<association>" << endl
					<< "<name>" << pConAssoc->second.getName() << "</name>" << endl
					<< "<value>" << verifyOutString(pConAssoc->second.getValue()) << "</value>" << endl;
				if( pConAssoc->second.hasQualifier() ) {
						outFile << "<qualifiers>" << endl;
						vector<OntyxQualifier> vqual = pConAssoc->second.getQualifiers();
						vector<OntyxQualifier>::iterator pvqual = vqual.begin();
						for( ; pvqual != vqual.end(); ++pvqual )
							outFile << "<qualifier>" << endl
								<< "<name>" << pvqual->getName() << "</name>" << endl
								<< "<value>" << verifyOutString(pvqual->getValue()) << "</value>" << endl 
								<< "</qualifier>" << endl;
						outFile << "</qualifiers>" << endl;
					}
				outFile << "</association>" << endl;
				}
			outFile << "</associations>" << endl;
			}
		outFile << "</conceptDef>" << endl << endl;
		}
	outFile << trailer << endl;
	outFile.close();
}

string OntyxKb::verifyOutString (const string & inString)
{
	if( inString.find_first_of("<&",0) != string::npos )
		if( inString.substr(0,9) != "<![CDATA[" )
			return "<![CDATA[" + inString + "]]>";
	return inString;
}
void OntyxKb::printKbDiffConcepts (void) const

// Routine ALWAYS prints paired concepts, the main search search routine
// diff_Concepts() discards unpaired (new/deleted) concepts.
// Routine is long because it looks in more detail the roles/attributes 
// in order to mark the changed roles/atts (*) in the printout.
{
	string sName = "               Name:\t",
		sDefined = "                    \tDefined",
		sPrimiti = "                    \tPrimitive",
	  sNamespace = "          Namespace:\t",
		   sCode = "               Code:\t",
			 sID = "                 ID:\t",
		   sKind = "               Kind:\t",
		 sParent = "   Defining Concept:\t",
		   sRole = "      Defining Role:\t",
	   sProperty = "           Property:\t",
	sAssociation = "        Association:\t";

	bool foundMatch;
	string tmpString;

	set<string>::const_iterator pParent;
	multimap<long, OntyxRole>::const_iterator pRole;
	multimap<long, OntyxProperty>::const_iterator pProperty;
	map<long, OntyxConcept>::const_iterator pConcept;
	map<long, OntyxConcept>::const_iterator pDupCon;

	vector<string> vecOutCon1, vecOutCon2, vecTmp1, vecTmp2;
	vector<string>::iterator pVecOut1, pVecOut2;

	if( m_concept.size() != 0) {
		pConcept = m_concept.begin();
		pDupCon = pConcept;
		pDupCon++;
		for( ; pDupCon != m_concept.end() && pConcept != m_concept.end(); ++pConcept, ++pDupCon) {

			vecOutCon1.clear();
			vecOutCon2.clear();

			vecOutCon1.push_back( sName + pConcept->second.getName() );

			if( pConcept->second.getState() )
				vecOutCon1.push_back( sDefined );
			else
				vecOutCon1.push_back( sPrimiti );

			vecOutCon1.push_back( sCode + pConcept->second.getCode() );
			tmpString = pConcept->second.getID('c');
			vecOutCon1.push_back( sID + tmpString );
			vecOutCon1.push_back( sNamespace + pConcept->second.getNamespace() );

			if( pConcept->second.getKind() != "")
				vecOutCon1.push_back( sKind + pConcept->second.getKind() );

			if( pConcept->second.getNumParents() != 0) {
				pParent = pConcept->second.getFirstParent();
				for( ; pParent != pConcept->second.getEndParent(); ++pParent)
					vecOutCon1.push_back( sParent + *pParent );
				}
			if( pConcept->second.getNumRoles() != 0 ) {
				pRole = pConcept->second.getFirstRole();
				for( ; pRole != pConcept->second.getEndRole(); ++pRole) {
					vecOutCon1.push_back( sRole + pRole->second.getModifier() + " "
						+ pRole->second.getName() + " " + pRole->second.getValue() );
					}
				}
			if( pConcept->second.getNumProperties() != 0) {
				pProperty = pConcept->second.getFirstProperty();
				for( ; pProperty != pConcept->second.getEndProperty(); ++pProperty) {
					if( pProperty->second.hasQualifier() ) {
						vector<OntyxQualifier> qual = pProperty->second.getQualifiers();
						vector<OntyxQualifier>::const_iterator pQual = qual.begin();
						string cum = sProperty + " " + pProperty->second.getName() + " "
							+ pProperty->second.getValue();
						for( ; pQual != qual.end(); ++pQual ) {
 							cum = cum + "|" + pQual->getName() + " " + pQual->getValue();
							}
						vecOutCon1.push_back(cum);
						}
					else {
						vecOutCon1.push_back( sProperty + pProperty->second.getName() + " " 
							+ pProperty->second.getValue() );
						}
					}
				}
			if( pConcept->second.getNumAssociations() > 0) {
				multimap<long, OntyxAssociation> assoc = pConcept->second.getAssociations();
				multimap<long, OntyxAssociation>::const_iterator pAssoc = assoc.begin();
				for( ; pAssoc != assoc.end(); ++pAssoc) {
					if( pAssoc->second.hasQualifier() ) {
						vector<OntyxQualifier> qual = pAssoc->second.getQualifiers();
						vector<OntyxQualifier>::const_iterator pQual = qual.begin();
						string cum = sAssociation + " " + pAssoc->second.getName() + " "
							+ pAssoc->second.getValue();
						for( ; pQual != qual.end(); ++pQual ) {
 							cum = cum + "|" + pQual->getName() + " " + pQual->getValue();
							}
						vecOutCon1.push_back(cum);
						}
					else {
						vecOutCon1.push_back( sAssociation + pAssoc->second.getName() + " " 
							+ pAssoc->second.getValue() );
						}
					}
				}



			vecOutCon2.push_back( sName + pDupCon->second.getName() );

			if( pDupCon->second.getState() )
				vecOutCon2.push_back( sDefined );
			else
				vecOutCon2.push_back( sPrimiti );

			vecOutCon2.push_back( sCode + pDupCon->second.getCode() );
			tmpString = pDupCon->second.getID('c');
			vecOutCon2.push_back( sID + tmpString );
			vecOutCon2.push_back( sNamespace + pDupCon->second.getNamespace() );

			if( pDupCon->second.getKind() != "")
				vecOutCon2.push_back( sKind + pDupCon->second.getKind() );

			if( pDupCon->second.getNumParents() != 0) {
				pParent = pDupCon->second.getFirstParent();
				for( ; pParent != pDupCon->second.getEndParent(); ++pParent)
					vecOutCon2.push_back( sParent + *pParent );
				}
			if( pDupCon->second.getNumRoles() != 0 ) {
				pRole = pDupCon->second.getFirstRole();
				for( ; pRole != pDupCon->second.getEndRole(); ++pRole) {
					vecOutCon2.push_back( sRole + pRole->second.getModifier() + " "
						+ pRole->second.getName() + " " + pRole->second.getValue() );
					}
				}
			if( pDupCon->second.getNumProperties() != 0) {
				pProperty = pDupCon->second.getFirstProperty();
				for( ; pProperty != pDupCon->second.getEndProperty(); ++pProperty) {
					if( pProperty->second.hasQualifier() ) {
						vector<OntyxQualifier> qual = pProperty->second.getQualifiers();
						vector<OntyxQualifier>::const_iterator pQual = qual.begin();
						string cum = sProperty + " " + pProperty->second.getName() + " "
							+ pProperty->second.getValue();
						for( ; pQual != qual.end(); ++pQual ) {
							cum = cum + "|" + pQual->getName() + " " + pQual->getValue();
							}
						vecOutCon2.push_back(cum);
						}
					else {
						vecOutCon2.push_back( sProperty + pProperty->second.getName() + " " 
							+ pProperty->second.getValue() );
						}
					}
				}

			if( pDupCon->second.getNumAssociations() > 0) {
				multimap<long, OntyxAssociation> assoc = pDupCon->second.getAssociations();
				multimap<long, OntyxAssociation>::const_iterator pAssoc = assoc.begin();
				for( ; pAssoc != assoc.end(); ++pAssoc) {
					if( pAssoc->second.hasQualifier() ) {
						vector<OntyxQualifier> qual = pAssoc->second.getQualifiers();
						vector<OntyxQualifier>::const_iterator pQual = qual.begin();
						string cum = sAssociation + " " + pAssoc->second.getName() + " "
							+ pAssoc->second.getValue();
						for( ; pQual != qual.end(); ++pQual ) {
							cum = cum + "|" + pQual->getName() + " " + pQual->getValue();
							}
						vecOutCon2.push_back(cum);
						}
					else {
						vecOutCon2.push_back( sAssociation + pAssoc->second.getName() + " " 
							+ pAssoc->second.getValue() );
						}
					}
				}

			vecTmp1 = vecOutCon1;
			vecTmp2 = vecOutCon2;
			for( pVecOut1 = vecOutCon1.begin(); pVecOut1 != vecOutCon1.end(); ++pVecOut1) {
				foundMatch = false;
				for( pVecOut2 = vecOutCon2.begin(); pVecOut2 != vecOutCon2.end(); ++pVecOut2)
					if( *pVecOut1 == *pVecOut2 ) {
						foundMatch = true;
						break;
						}
				if( !foundMatch ) {
					pVecOut1[0][0] = '<';
					pVecOut1[0][1] = '<';
					}
				}
			for( pVecOut2 = vecOutCon2.begin(); pVecOut2 != vecOutCon2.end(); ++pVecOut2) {
				foundMatch = false;
				for( pVecOut1 = vecTmp1.begin(); pVecOut1 != vecTmp1.end(); ++pVecOut1) 
					if( *pVecOut1 == *pVecOut2 ) {
						foundMatch = true;
						break;
						}
				if( !foundMatch ) {
					pVecOut2[0][0] = '>';
					pVecOut2[0][1] = '>';
					}
				}
			if( vecTmp1 != vecOutCon1 || vecTmp2 != vecOutCon2 ) {
				cout << endl << endl << endl 
					<< "----------------------------------" << endl << endl;
				for( pVecOut1 = vecOutCon1.begin(); pVecOut1 != vecOutCon1.end(); ++pVecOut1)
					cout << *pVecOut1 << endl;
				cout << endl << endl;
				for( pVecOut2 = vecOutCon2.begin(); pVecOut2 != vecOutCon2.end(); ++pVecOut2)
					cout << *pVecOut2 << endl;
				}

			++pDupCon;
			++pConcept;

			} // end main "for loop"
		} // end "if" check for concepts in kb

}


//*******************************************************************
string OntyxKb::toLower (const string & inString) const
{
	unsigned i = 0;
	string tmpString = inString;
	while( i < tmpString.size() ) {
		tmpString[i] = tolower(tmpString[i]);
		i++;
	}
	return tmpString;
}




bool OntyxKb::verifyRegistries(long ID) const
{
	bool badRegistry = false;
	map<long, OntyxConcept>::const_iterator posCon = m_concept.begin();
	for( ; posCon != m_concept.end(); ++posCon ) {
//		if( m_conName.find(toLower(posCon->second.getName())) == m_conName.end() ) {
		if( m_conName.find(posCon->second.getName()) == m_conName.end() ) {
			cout << "REGISTRY ERROR (" << ID << "): Concept " << posCon->second.getName() << " not in conName registry" << endl;
			badRegistry = true;
			}
		}
	map<string, long>::const_iterator posName = m_conName.begin();
	for( ; posName != m_conName.end(); ++posName) {
		if( m_concept.find(posName->second) == m_concept.end() ) {
			cout << "REGISTRY ERROR (" << ID << "): conName ID " << posName->second << " not in concept registry" << endl;
			badRegistry = true;
			}
		}
	multimap<string, long>::const_iterator posTerm = m_terms.begin();
	for( ; posTerm != m_terms.end(); ++posTerm) {
		if( m_concept.find(posTerm->second) == m_concept.end() ) {
			cout << "REGISTRY ERROR (" << ID << "): term ID " << posTerm->second << " not in concept registry" << endl;
			badRegistry = true;
			}
		}
/*	multimap<string, long>::const_iterator posPar = m_parents.begin();
	for( ; posPar != m_parents.end(); ++posPar) {
		if( m_concept.find(posPar->second) == m_concept.end() ) {
			cout << "REGISTRY ERROR (" << ID << "): Parent ID " << posPar->second << " not in concept registry" << endl;
			badRegistry = true;
			}
		}
*/	return badRegistry;
}






long OntyxKb::simpleMatchKB (OntyxKb & inKB, const string & conceptName, set<long> & termID)
{ 	// 

	set<string> termSet;
	set<string>::iterator pTerms;
	set<long>::iterator pID; // iterator for termID
	long outConID;
	string conName;
	string outConName;
	set<string> matchTerms;

	conName = conceptName;
	OntyxConcept tmpConcept = getConcept(conceptName);

	outConID = inKB.getIDFromName(conName);		// case-insensitive search
	termID.clear();
	if( outConID != -1)							// concept name has a match
		termID.insert(outConID);

	termSet.clear();
	getConceptTerms(tmpConcept, termSet);	// search now with all terms (case-insensitive)

	set<long> matchTermIDs;
	if( termSet.size() != 0 ) {
		pTerms = termSet.begin();
		for( ; pTerms != termSet.end(); ++pTerms)					// do a lookup of all terms -> ID in outside KB
			inKB.getIDFromTerm(*pTerms, termID);
		matchTermIDs.clear();
		for( pID = termID.begin(); pID != termID.end(); ++pID) {	// do a reverse lookup ...
			matchTerms.clear();
			getConceptTerms(inKB.getConcept(*pID), matchTerms);
			set<string>::iterator pMatchTerms = matchTerms.begin();
			for( ; pMatchTerms != matchTerms.end(); ++pMatchTerms)	// from outside KB terms -> ID to *this KB
				getIDFromTerm(*pMatchTerms, matchTermIDs);
			}
		}
	if( termID.size() == 1 && matchTermIDs.size() == 1 )	// match
		return 0; 
	else if( termID.size() > 1 && matchTermIDs.size() == 1) // split in outside kb
		return -1;
	else if( termID.size() == 1 && matchTermIDs.size() > 1) // merge in outside kb
		return 1;
	else if( termID.size() == 0 )							// no match (deleted?) in outside KB
		return 5;
	return 4;												// unknown
}

long OntyxKb::alphaStemMatchKB (OntyxKb & inKB, const string & conceptName, set<long> & termID)
{ 	// 

	set<string> termSet;
	set<string>::iterator pTerms;
	set<long>::iterator pID; // iterator for termID
	long outConID;
	string conName;
	string outConName;
	set<string> matchTerms;

	conName = conceptName;
	OntyxConcept tmpConcept = getConcept(conceptName);

	outConID = inKB.getIDFromName(conName);		// case-insensitive search
	termID.clear();
	if( outConID != -1)							// concept name has a match
		termID.insert(outConID);

	termSet.clear();
	getAlphaStemConceptTerms(tmpConcept, termSet);	// search now with all terms (case-insensitive)

	set<long> matchTermIDs;
	if( termSet.size() > 0 ) {
		pTerms = termSet.begin();
		for( ; pTerms != termSet.end(); ++pTerms) {					// do a lookup of all terms -> ID in outside KB
			inKB.getIDFromAlphaStemTermsTerm(*pTerms, termID);
			}
		matchTermIDs.clear();
		for( pID = termID.begin(); pID != termID.end(); ++pID) {	// do a reverse lookup ...
			matchTerms.clear();
			getAlphaStemConceptTerms(inKB.getConcept(*pID), matchTerms);
			set<string>::iterator pMatchTerms = matchTerms.begin();
			for( ; pMatchTerms != matchTerms.end(); ++pMatchTerms)	// from outside KB terms -> ID to *this KB
				getIDFromAlphaStemTermsTerm(*pMatchTerms, matchTermIDs);
			}
		}

	if( termID.size() == 1 && matchTermIDs.size() == 1 )	// match
		return 0; 
	else if( termID.size() > 1 && matchTermIDs.size() == 1) // split in outside kb
		return -1;
	else if( termID.size() == 1 && matchTermIDs.size() > 1) // merge in outside kb
		return 1;
	else if( termID.size() == 0 )							// no match (deleted?) in outside KB
		return 5;
	return 4;												// unknown
} 

 

bool OntyxKb::isValidOWL (const string & inString )
{
 
	unsigned i;
	string tmpString = inString;
	bool isValid = true;

	// check if first character is illegal
	if( !isalpha(inString[0]) && inString[0] != '_' ) {
		isValid = false;
		cerr << "ERROR:  invalid OWL name; first char \"" << inString << "\"" << endl;
		}
	// check rest of characters
	for( i = 1; i < inString.size(); i++) {
		if( !isalnum(inString[i]) && inString[i] != '_' && inString[i] != '-' && inString[i] != '.' ) {
			isValid = false;
			cerr << "ERROR:  invalid OWL name; internal char at " << i+1 << ", \"" << inString << "\"" << endl;
			}
		}
	return isValid;
}

void OntyxKb::writeOWLFile (const string & filename, const string headerFile, bool noProps) // flag = true:renumber
// if noProps is true, properties with names derived from roles are not exported

// 060813 since we have been deleting properties from a file listing, will make the "noProps" argument
// irrelevant and will change the signature in the future, the bool noProps doesn't make a difference
// as of 060813.
{
	set<string> kind_names;
/*	string roleprefix = "o"; // for object property
	string propprefix = "d"; // for datatype property

	map<string, string> validrolenames;
	map<string, string> validpropnames;
	string validname = "";
*/

	set<string> hasValueDatatypes;
	hasValueDatatypes.insert("Gene_In_Chromosomal_Location");
	hasValueDatatypes.insert("Allele_In_Chromosomal_Location");
	hasValueDatatypes.insert("Allele_Absent_From_Wild-type_Chromosomal_Location");
	hasValueDatatypes.insert("Gene_Has_Physical_Location");

	ifstream inFile;
	inFile.open(headerFile.c_str());
	if( !inFile.good() ) {
		cerr << "Can't find/open file '" << headerFile.c_str() << "'.  Will EXIT." << endl;
		exit(0);
		}
	string inputLine;
	
	string trailer = "</rdf:RDF>";
	ofstream outFile;
	outFile.open(filename.c_str());

	while( getline(inFile, inputLine), !inFile.eof() )
		outFile << inputLine << endl;

	string conceptnamefordoc = "";
	OntyxConcept docConcept;
	multimap<long, OntyxProperty> docProperties;
	multimap<long, OntyxProperty>::iterator pTConProp;

	map<long, OntyxRoleDef>::iterator pRole = m_role.begin();
	for( ; pRole != m_role.end(); ++pRole ) {
		isValidOWL(pRole->second.getCode());
		outFile << "<owl:ObjectProperty rdf:ID=\"" << pRole->second.getCode() << "\">" << endl
				<< "<rdfs:label>" << pRole->second.getName() << "</rdfs:label>" << endl;
		if( pRole->second.hasParent() ) {
			string rname = pRole->second.getParentRole();
			OntyxRoleDef tmpRole = getRoleDef(rname);
			string mcode = tmpRole.getCode();
			outFile << '\t' << "<rdfs:subPropertyOf rdf:resource=\"#" << mcode << "\"/>" << endl;
			}
		else {
			string dname = pRole->second.getDomain();
			OntyxKind dkind = getKindDef(dname);
			string dcode = dkind.getCode();
			string rname = pRole->second.getRange();
			OntyxKind rkind = getKindDef(rname);
			string rcode = rkind.getCode();
			outFile << '\t' << "<rdfs:domain rdf:resource=\"#" << dcode << "\"/>" << endl;		// kinds are not renamed, at least yet
			outFile << '\t' << "<rdfs:range rdf:resource=\"#" << rcode << "\"/>" << endl;
			}
		conceptnamefordoc = pRole->second.getName() + "_Role";
		if( conceptNameInKB(conceptnamefordoc) ) {
			docConcept = getConcept(conceptnamefordoc);
			docProperties = docConcept.getProperties();
			}
		else
			docProperties.clear();
		if( docProperties.size() > 0 ) {
			for( pTConProp = docProperties.begin(); pTConProp !=docProperties.end(); ++pTConProp ) {
				string mname = pTConProp->second.getName();
				OntyxPropertyDef tmpProp = getPropertyDef(mname);
				string mcode = tmpProp.getCode();
				outFile << "<" << mcode << ">" << verifyOutString(pTConProp->second.getValue())
						<< "</" << mcode << ">" << endl;
				}
			}
		outFile << "</owl:ObjectProperty>" << endl << endl;
		}

	map<long, OntyxPropertyDef>::iterator pProp = m_property.begin();
	for( ; pProp != m_property.end(); ++pProp ) {
		outFile << "<owl:DatatypeProperty rdf:ID=\"" << pProp->second.getCode() << "\">" << endl
				<< "<rdfs:label>" << pProp->second.getName() << "</rdfs:label>" << endl
				<< "<rdf:type rdf:resource=\"http://www.w3.org/2002/07/owl#AnnotationProperty\"/>" << endl;
		if( !pProp->second.hasPickList() )
			outFile << "<rdfs:range rdf:resource=\"http://www.w3.org/2001/XMLSchema#string\"/>" << endl;
		else {
			vector<string> picklist = pProp->second.getPickList();
			vector<string>::iterator pPicklist = picklist.begin();
			outFile << "<rdfs:range>" << endl
					<< "\t" << "<owl:DataRange>" << endl
					<< "\t" << "<owl:oneOf>" << endl;
			while( pPicklist != picklist.end() ) {
				outFile << "\t\t" << "<rdf:List>" << endl
						<< "\t\t" << "<rdf:first rdf:datatype=\"http://www.w3.org/2001/XMLSchema#string\">" << *pPicklist << "</rdf:first>" << endl;
				if( ++pPicklist != picklist.end() )
					outFile << "\t\t" << "<rdf:rest>" << endl;
				else
					outFile << "\t\t" << "<rdf:rest rdf:resource=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#nil\"/>" << endl
							<< "\t\t" << "</rdf:List>" << endl;
				}
			pPicklist = picklist.begin();
			++pPicklist;				// need to advance it 'cause the last element in the listing above gets an </rdf:List>
			for( ; pPicklist != picklist.end(); ++pPicklist ) {
				outFile << "\t\t" << "</rdf:rest>"  << endl
						<< "\t\t" << "</rdf:List>" << endl;
				}
			outFile << "\t" << "</owl:oneOf>" << endl
					<< "\t" << "</owl:DataRange>" << endl
					<< "</rdfs:range>" << endl;
			}
		conceptnamefordoc = pProp->second.getName() + "_Property";
		if( conceptNameInKB(conceptnamefordoc) ) {
			docConcept = getConcept(conceptnamefordoc);
			docProperties = docConcept.getProperties();
			}
		else
			docProperties.clear();
		if( docProperties.size() > 0 ) {
			for( pTConProp = docProperties.begin(); pTConProp !=docProperties.end(); ++pTConProp ) {
				string mname = pTConProp->second.getName();
				OntyxPropertyDef tmpProp = getPropertyDef(mname);
				string mcode = tmpProp.getCode();
				outFile << "<" << mcode << ">" << verifyOutString(pTConProp->second.getValue())
						<< "</" << mcode << ">" << endl;
				}
			}
		outFile << "</owl:DatatypeProperty>" << endl << endl;
		}

	map<long, OntyxAssociationDef>::iterator pAssoc = m_association.begin();
	for( ; pAssoc != m_association.end(); ++pAssoc ) {
		outFile << "<owl:ObjectProperty rdf:ID=\"" << pAssoc->second.getCode() << "\">" << endl
				<< "<rdfs:label>" << pAssoc->second.getName() << "</rdfs:label>" << endl
				<< "<rdf:type rdf:resource=\"http://www.w3.org/2002/07/owl#AnnotationProperty\"/>" << endl;
		conceptnamefordoc = pAssoc->second.getName() + "_Association";
		if( conceptNameInKB(conceptnamefordoc) ) {
			docConcept = getConcept(conceptnamefordoc);
			docProperties = docConcept.getProperties();
			}
		else
			docProperties.clear();
		if( docProperties.size() > 0 ) {
			for( pTConProp = docProperties.begin(); pTConProp !=docProperties.end(); ++pTConProp ) {
				string mname = pTConProp->second.getName();
				OntyxPropertyDef tmpProp = getPropertyDef(mname);
				string mcode = tmpProp.getCode();
				outFile << "<" << mcode << ">" << verifyOutString(pTConProp->second.getValue())
						<< "</" << mcode << ">" << endl;
				}
			}
		outFile << "</owl:ObjectProperty>" << endl << endl;
		}

	map<long, OntyxKind>::iterator pKind0;
	map<long, OntyxKind>::iterator pKind = m_kind.begin();
	for( ; pKind != m_kind.end(); ++pKind) {
		isValidOWL(pKind->second.getKindname());
		kind_names.insert(pKind->second.getCode());
		string conname, condef = "", consemtype = "", lcode;
		OntyxConcept tmpConcept;
		lcode = pKind->second.getCode();
		conname = pKind->second.getKindname();
		outFile << "<owl:Class rdf:ID=\"" << pKind->second.getCode() << "\">" << endl
				<< '\t' << "<rdfs:label>" << conname << "</rdfs:label>" << endl;
		if( conceptNameInKB(pKind->second.getKindname())) {		// is there a concept with the same name
			tmpConcept = getConcept(pKind->second.getKindname());

			multimap<long, OntyxProperty> conProp = tmpConcept.getProperties();
			multimap<long, OntyxProperty>::const_iterator pConProp = conProp.begin();
			string propString, propName = "";
			for( ; pConProp != conProp.end(); ++pConProp ) {
				propString = verifyOutString(pConProp->second.getValue());
				string mname = pConProp->second.getName();
				OntyxPropertyDef tmpProp = getPropertyDef(mname);
				string mcode = tmpProp.getCode();
				if( pConProp->second.hasQualifier() ) {         // 080623 if qualifiers exist, print CDATA tag
				    outFile << "<" << mcode << ">" << "<![CDATA[<value>" << propString << "</value>";
				    vector<OntyxQualifier> vqual = pConProp->second.getQualifiers();
				    vector<OntyxQualifier>::iterator pvqual = vqual.begin();
				    for( ; pvqual != vqual.end(); ++pvqual ) {
                                      outFile << "<qual>";
                                      outFile << "<qual-name>" << pvqual->getName() << "</qual-name>";
                                      outFile << "<qual-value>" << pvqual->getValue() << "</qual-value>";
                                      outFile << "</qual>";
                                    }
                                    outFile << "]]></" << mcode << ">" << endl;
                                }
                                else
                                    outFile << "<" << mcode << ">" << propString << "</" << mcode << ">" << endl;
				}
			}
		for( pKind0 = m_kind.begin(); pKind0 != m_kind.end(); ++pKind0 ) {
			if( pKind0->second.getKindname() != pKind->second.getKindname() )
				outFile << '\t' << "<owl:disjointWith rdf:resource=\"#" 
					<< pKind0->second.getCode() << "\"/>" << endl;
			} 
		outFile << "</owl:Class>" << endl << endl;
		}

	map<long, OntyxConcept>::iterator pCon = m_concept.begin();
	string label;
	for( ; pCon != m_concept.end(); ++pCon ) {
		if( kind_names.count(pCon->second.getName()) > 0 )	// kind concept
			continue;
		isValidOWL( pCon->second.getCode() );	// just do a check, let routine output warning
		if (pCon->second.getKind() == "Retired_Kind")
			outFile << "<owl:DeprecatedClass rdf:ID=\"" << pCon->second.getCode() << "\">" << endl;
		else if (pCon->second.getNumParents() == 1){
			set<string>::const_iterator pConPar = pCon->second.getFirstParent();
            string sConPar = verifyOutString(*pConPar);
            if (sConPar == "ObsoleteClass")
    			outFile << "<owl:DeprecatedClass rdf:ID=\"" << pCon->second.getCode() << "\">" << endl;
            else 
    			outFile << "<owl:Class rdf:ID=\"" << pCon->second.getCode() << "\">" << endl;
		}
		else
			outFile << "<owl:Class rdf:ID=\"" << pCon->second.getCode() << "\">" << endl;
		if( pCon->second.getPropertyNumber("Preferred_Name") > 0 )
			label =  verifyOutString(pCon->second.getPropertyValue("Preferred_Name"));
		else
			label = verifyOutString(pCon->second.getName());
		outFile << "<rdfs:label>" << label << "</rdfs:label>" << endl;

		if( !pCon->second.getState() ) {  // if not defined, i.e. primitive, deal with it here
			if( pCon->second.getNumParents() > 0 ) {
				set<string> conPar = pCon->second.getParents();
				set<string>::const_iterator pConPar = conPar.begin();
				for( ; pConPar != conPar.end(); ++pConPar ) {
					OntyxConcept tmpConcept = getConcept(*pConPar);
					string mcon = tmpConcept.getCode();
					outFile << "<rdfs:subClassOf rdf:resource=\"#" << mcon << "\"/>" << endl;
					}
				}
			else {		// root node, make the kind its parent
				string tmpKindname = pCon->second.getKind();
				outFile << "<rdfs:subClassOf rdf:resource=\"#" << getKindDef(tmpKindname).getCode() << "\"/>" << endl;
				}
			if( pCon->second.getNumRoles() > 0 ) {
				multimap<long, OntyxRole> conRoles = pCon->second.getRoles();
				multimap<long, OntyxRole>::const_iterator pConRoles = conRoles.begin();
				for( ; pConRoles != conRoles.end(); ++pConRoles ) {
					if( pConRoles->second.getRolegroup() == 0 ) {			// take care of ontylog role groups, do ungrouped roles first
						outFile << "<rdfs:subClassOf>" << endl
								<< "\t" << "<owl:Restriction>" << endl;
						string rname = pConRoles->second.getName();
						OntyxRoleDef rrole = getRoleDef(rname);
						string rcode = rrole.getCode();	
						outFile << "\t\t" << "<owl:onProperty rdf:resource=\"#" << rcode << "\"/>" << endl;
						if( pConRoles->second.getModifier() == "all" ) 
							outFile << "\t\t" << "<owl:allValuesFrom rdf:resource=\"#" << getConcept(pConRoles->second.getValue()).getCode() << "\"/>" << endl;
						else 					// default, poss will also get converted to some
							outFile << "\t\t" << "<owl:someValuesFrom rdf:resource=\"#" << getConcept(pConRoles->second.getValue()).getCode() << "\"/>" << endl;
						outFile << "\t" << "</owl:Restriction>" << endl
								<< "</rdfs:subClassOf>" << endl;
						}
					}
				if( pCon->second.hasRolegroups() ) {  // ontylog role groups, do grouped roles now
					outFile << "<rdfs:subClassOf>" << endl
							<< "\t" << "<owl:Class>" << endl;
					if( pCon->second.getNumRoleGroups() > 1 )
						outFile << "\t\t" << "<owl:unionOf rdf:parseType=\"Collection\">" << endl;
					for(long groupCounter = 1; groupCounter <= pCon->second.getNumRoleGroups(); ++groupCounter) {
						if( pCon->second.getNumRoleGroups() > 1 ) {
							outFile << "\t\t\t" << "<owl:Class>" << endl;
							}
						outFile << "\t\t\t\t" << "<owl:intersectionOf rdf:parseType=\"Collection\">" << endl;
						for( pConRoles = conRoles.begin(); pConRoles != conRoles.end(); ++pConRoles ) {
							if( pConRoles->second.getRolegroup() == groupCounter ) {
								outFile << "\t\t\t\t\t" << "<owl:Restriction>" << endl;
								string rname = pConRoles->second.getName();
								OntyxRoleDef rrole = getRoleDef(rname);
								string rcode = rrole.getCode();	
								outFile << "\t\t\t\t\t\t" << "<owl:onProperty rdf:resource=\"#" << rcode << "\"/>" << endl;
								if( pConRoles->second.getModifier() == "all" ) 
									outFile << "\t\t\t\t\t\t" << "<owl:allValuesFrom rdf:resource=\"#" << getConcept(pConRoles->second.getValue()).getCode() << "\"/>" << endl;
								else 			// default is some
									outFile << "\t\t\t\t\t\t" << "<owl:someValuesFrom rdf:resource=\"#" << getConcept(pConRoles->second.getValue()).getCode() << "\"/>" << endl;
								outFile << "\t\t\t\t\t" << "</owl:Restriction>" << endl;
								}
							}
						outFile << "\t\t\t\t" << "</owl:intersectionOf>" << endl;
						if( pCon->second.getNumRoleGroups() > 1 ) {
							outFile << "\t\t\t" << "</owl:Class>"  << endl;
							}
						}
					if( pCon->second.getNumRoleGroups() > 1 )
						outFile << "\t\t" << "</owl:unionOf>" << endl;
					outFile << "\t" << "</owl:Class>" << endl
							<< "</rdfs:subClassOf>" << endl;
					}
				}
			}
		else {  // concept is defined, assume there are >= 2 conditions, and the "parent" is not a kind, i.e. it's deep in tree 
			outFile << "<owl:equivalentClass>" << endl
					<< "\t" 	<< "<owl:Class>" << endl
					<< "\t\t" 	<< "<owl:intersectionOf rdf:parseType=\"Collection\">" << endl;

			set<string> conPar = pCon->second.getParents();
			set<string>::const_iterator pConPar = conPar.begin();
			for( ; pConPar != conPar.end(); ++pConPar )  {
				OntyxConcept tmpConcept = getConcept(*pConPar);
				string mcon = tmpConcept.getCode();
				outFile << "\t\t\t" << "<owl:Class rdf:about=\"#" << mcon << "\"/>" << endl;
				}
			if( pCon->second.getNumRoles() > 0 ) {
				multimap<long, OntyxRole> conRoles = pCon->second.getRoles();
				multimap<long, OntyxRole>::const_iterator pConRoles = conRoles.begin();
				for( ; pConRoles != conRoles.end(); ++pConRoles ) {
					if( pConRoles->second.getRolegroup() == 0 ) {			// take care of ontylog role groups, do ungrouped roles first
						string rname = pConRoles->second.getName();
						OntyxRoleDef rrole = getRoleDef(rname);
						string rcode = rrole.getCode();	
						outFile << "\t\t\t" << "<owl:Restriction>" << endl
								<< "\t\t\t\t" << "<owl:onProperty rdf:resource=\"#" << rcode << "\"/>" << endl;
						if( pConRoles->second.getModifier() == "all" ) 		// use "some" by default, dismiss "poss"
							outFile << "\t\t\t\t" << "<owl:allValuesFrom rdf:resource=\"#" << getConcept(pConRoles->second.getValue()).getCode() << "\"/>" << endl;
						else 
							outFile << "\t\t\t\t" << "<owl:someValuesFrom rdf:resource=\"#" << getConcept(pConRoles->second.getValue()).getCode() << "\"/>" << endl;
						outFile	<< "\t\t\t" << "</owl:Restriction>" << endl;
						}
					}
				if( pCon->second.hasRolegroups() ) {  // ontylog role groups, do grouped roles now
					if( pCon->second.getNumRoleGroups() > 1 )
						outFile << "\t\t\t" << "<owl:Class>" << endl
								<< "\t\t\t" << "<owl:unionOf rdf:parseType=\"Collection\">" << endl;
					multimap<long, OntyxRole> conRoles = pCon->second.getRoles();
					multimap<long, OntyxRole>::const_iterator pConRoles;
					for(long groupCounter = 1; groupCounter <= pCon->second.getNumRoleGroups(); ++groupCounter) {
						outFile << "\t\t\t" << "<owl:Class>" << endl
								<< "\t\t\t\t" << "<owl:intersectionOf rdf:parseType=\"Collection\">" << endl;
						for( pConRoles = conRoles.begin(); pConRoles != conRoles.end(); ++pConRoles ) {
							if( pConRoles->second.getRolegroup() == groupCounter ) {
								string rname = pConRoles->second.getName();
								OntyxRoleDef rrole = getRoleDef(rname);
								string rcode = rrole.getCode();	
								outFile << "\t\t\t\t\t" << "<owl:Restriction>" << endl
										<< "\t\t\t\t\t\t" << "<owl:onProperty rdf:resource=\"#" << rcode << "\"/>" << endl;
								if( pConRoles->second.getModifier() == "all" ) 
									outFile << "\t\t\t\t\t\t" << "<owl:allValuesFrom rdf:resource=\"#" << getConcept(pConRoles->second.getValue()).getCode() << "\"/>" << endl;
								else 
									outFile << "\t\t\t\t\t\t" << "<owl:someValuesFrom rdf:resource=\"#" << getConcept(pConRoles->second.getValue()).getCode() << "\"/>" << endl;
								outFile << "\t\t\t\t\t" << "</owl:Restriction>" << endl;
								}
							}
						outFile << "\t\t\t\t" << "</owl:intersectionOf>" << endl
								<< "\t\t\t" << "</owl:Class>" << endl;
						}
					if( pCon->second.getNumRoleGroups() > 1 )
						outFile << "\t\t\t" << "</owl:unionOf>" << endl
								<< "\t\t\t" << "</owl:Class>" << endl;
					}
				}
			outFile << "\t\t"	<< "</owl:intersectionOf>" << endl
					<< "\t"		<< "</owl:Class>" << endl
					<< "</owl:equivalentClass>" << endl;
			}

		multimap<long, OntyxProperty> conProp = pCon->second.getProperties();
		multimap<long, OntyxProperty>::const_iterator pConProp = conProp.begin();
		string propString, propName = "";
		for( ; pConProp != conProp.end(); ++pConProp ) {
//			propString = verifyOutString(pConProp->second.getValue());
//			propName = validpropnames[pConProp->second.getName()];
			string mname = pConProp->second.getName();
			OntyxPropertyDef tmpProp = getPropertyDef(mname);
			string mcode = tmpProp.getCode();
			if( pConProp->second.hasQualifier() ) {         // 080623 if qualifiers exist, print CDATA tag
			    outFile << "<" << mcode << ">" << "<![CDATA[<value>" << pConProp->second.getValue() << "</value>";
			    vector<OntyxQualifier> vqual = pConProp->second.getQualifiers();
			    vector<OntyxQualifier>::iterator pvqual = vqual.begin();
			    for( ; pvqual != vqual.end(); ++pvqual ) {
                              outFile << "<qual>";
                              outFile << "<qual-name>" << pvqual->getName() << "</qual-name>";
                              outFile << "<qual-value>" << pvqual->getValue() << "</qual-value>";
                              outFile << "</qual>";
                            }
                            outFile << "]]></" << mcode << ">" << endl;
                        }
                        else
			    outFile << "<" << mcode << ">" << verifyOutString(pConProp->second.getValue()) << "</" << mcode << ">" << endl;
				// locale not being taken care of
			}

		// need to output associations as object properties
		multimap<long, OntyxAssociation> conAssoc = pCon->second.getAssociations();
		multimap<long, OntyxAssociation>::const_iterator pConAssoc = conAssoc.begin();
		string assocName = "", assocValue = "";
		for( ; pConAssoc != conAssoc.end(); ++pConAssoc ) {
			string aname = pConAssoc->second.getName();
			OntyxAssociationDef tmpAssoc = getAssociationDef(aname);
			string acode = tmpAssoc.getCode();
			outFile << "<" << acode << " rdf:resource=\"#" << getConcept(pConAssoc->second.getValue()).getCode() << "\"/>" << endl;
			}

		if (pCon->second.getKind() == "Retired_Kind")
			outFile << "</owl:DeprecatedClass>" << endl << endl;
		else if (pCon->second.getNumParents() == 1){
			set<string>::const_iterator pConPar = pCon->second.getFirstParent();
           string sConPar = verifyOutString(*pConPar);
            if (sConPar == "ObsoleteClass")
    			outFile << "</owl:DeprecatedClass>" << endl << endl;
            else 
    			outFile << "</owl:Class>" << endl << endl;
		}
		else
			outFile << "</owl:Class>" << endl << endl;
		}

	outFile << trailer << endl;
	outFile.close();
}


void OntyxKb::writeFlatFile (const string & filename, bool flag, bool noProps) // flag = true:renumber
// if noProps is true, properties with names derived from roles are not exported
{
	string h1 = "<?xml version=\"1.0\" ?><!DOCTYPE terminology SYSTEM \"http://apelon.com/dtd/tde/ontylog.dtd\">\n\n",
			h2 = "<terminology ref_by=\"name\" if_exists_action=\"replace\">",
			header = h1 + h2;
	string trailer = "</terminology>";
	ofstream outFile;
	outFile.open(filename.c_str());
	long counter = 1;

	outFile << header << endl << endl;

	map<long, OntyxName>::iterator pName = m_namespace.begin();
	for( ; pName != m_namespace.end(); ++pName ) {
		outFile << "<namespaceDef>" << endl
			<< "<name>" << pName->second.getNamespace() << "</name>" << endl
			<< "<code>" << ( flag ? "C" : "") ;
		if( flag ) 
			outFile << counter;
		else
			outFile << pName->second.getCode();
		outFile << "</code>" << endl << "<id>";
		if ( flag )
			outFile << counter++;
		else 
			outFile << pName->second.getID();
		outFile << "</id>" << endl
			<< "</namespaceDef>" << endl << endl;
		}

	map<long, OntyxKind>::iterator pKind = m_kind.begin();
	for( ; pKind != m_kind.end(); ++pKind) {
		outFile  << "<kindDef reference=\"" << (pKind->second.getReference()? "true" : "false") << "\">" << endl
			<< "<name>" << pKind->second.getKindname() << "</name>" << endl
			<< "<code>" << ( flag ? "C" : "") ;
		if( flag ) 
			outFile << counter;
		else
			outFile << pKind->second.getCode();
		outFile << "</code>" << endl << "<id>";
		if ( flag )
			outFile << counter++;
		else 
			outFile << pKind->second.getID();
		outFile << "</id>" << endl
			<< "<namespace>" << pKind->second.getNamespace() << "</namespace>" << endl
			<< "</kindDef>" << endl << endl;
		}

	map<long, OntyxRoleDef>::iterator pRole = m_role.begin();
	for( ; pRole != m_role.end(); ++pRole ) {
		outFile << "<roleDef>" << endl
			<< "<name>" << pRole->second.getName () << "</name>" << endl
			<< "<code>" << ( flag ? "C" : "") ;
		if( flag ) 
			outFile << counter;
		else
			outFile << pRole->second.getCode();
		outFile << "</code>" << endl << "<id>";
		if ( flag )
			outFile << counter++;
		else 
			outFile << pRole->second.getID();
		outFile << "</id>" << endl
			<< "<namespace>" << pRole->second.getNamespace() << "</namespace>" << endl
			<< "<domain>" << pRole->second.getDomain() << "</domain>" << endl
			<< "<range>" << pRole->second.getRange() << "</range>" << endl
			<< "</roleDef>" << endl << endl;
		}

	map<long, OntyxPropertyDef>::iterator pProp = m_property.begin();
	for( ; pProp != m_property.end(); ++pProp ) {
		if( noProps ) {
			if( !OntyxRoleDef::roleDefInMap(pProp->second.getName()) ) { // if the property doesn't exist as role
					outFile << "<propertyDef>" << endl
						<< "<name>" << pProp->second.getName () << "</name>" << endl
						<< "<code>" << ( flag ? "C" : "") ;
					if( flag ) 
						outFile << counter;
					else
						outFile << pProp->second.getCode();
					outFile << "</code>" << endl << "<id>";
					if ( flag )
						outFile << counter++;
					else 
						outFile << pProp->second.getID();
					outFile << "</id>" << endl
						<< "<namespace>" << pProp->second.getNamespace() << "</namespace>" << endl
						<< "<range>" << pProp->second.getRange() << "</range>" << endl
						<< (pProp->second.isContainsIndex() ? "<containsIndex/>\n" : "" )
						<< (pProp->second.isLocalized() ? "<localized/>\n" : "" )
						<< "</propertyDef>" << endl << endl;
				}
			}
		else {
			outFile << "<propertyDef>" << endl
				<< "<name>" << pProp->second.getName () << "</name>" << endl
				<< "<code>" << ( flag ? "C" : "") ;
			if( flag ) 
				outFile << counter;
			else
				outFile << pProp->second.getCode();
			outFile << "</code>" << endl << "<id>";
			if ( flag )
				outFile << counter++;
			else 
				outFile << pProp->second.getID();
			outFile << "</id>" << endl
				<< "<namespace>" << pProp->second.getNamespace() << "</namespace>" << endl
				<< "<range>" << pProp->second.getRange() << "</range>" << endl
				<< (pProp->second.isContainsIndex() ? "<containsIndex/>\n" : "" )
				<< (pProp->second.isLocalized() ? "<localized/>\n" : "" )
				<< "</propertyDef>" << endl << endl;
			}
		}

	map<long, OntyxConcept>::iterator pCon = m_concept.begin();
	for( ; pCon != m_concept.end(); ++pCon ) {
		outFile << "<conceptDef>" << endl
			<< "<name>" << verifyOutString(pCon->second.getName()) << "</name>" << endl
			<< "<code>" << ( flag ? "C" : "") ;
		if( flag ) 
			outFile << counter;
		else
			outFile << pCon->second.getCode();
		outFile << "</code>" << endl << "<id>";
		if ( flag )
			outFile << counter++;
		else 
			outFile << pCon->second.getID();
		outFile << "</id>" << endl
			<< "<namespace>" << pCon->second.getNamespace() << "</namespace>" << endl
			<< (pCon->second.getState() ? "" : "<primitive/>\n" );
		if( pCon->second.getKind() != "" )
			outFile << "<kind>" + pCon->second.getKind() + "</kind>" << endl;
		outFile << "<definingConcepts>" << endl;
		set<string> conPar = pCon->second.getParents();
		set<string>::const_iterator pConPar = conPar.begin();
		for( ; pConPar != conPar.end(); ++pConPar ) {
			outFile << "<concept>" << verifyOutString(*pConPar) << "</concept>" << endl;
			}
		outFile << "</definingConcepts>" << endl
				<< "<definingRoles>" << endl;
		multimap<long, OntyxRole> conRoles = pCon->second.getRoles();
		multimap<long, OntyxRole>::const_iterator pConRoles = conRoles.begin();
		for( ; pConRoles != conRoles.end(); ++pConRoles ) {
			outFile << "<role>" << endl
				<< "<" + pConRoles->second.getModifier() + "/>" << endl
				<< "<name>" << pConRoles->second.getName() << "</name>" << endl
				<< "<value>" << verifyOutString(pConRoles->second.getValue()) << "</value>" << endl
				<< "</role>" << endl;
			}
		outFile << "</definingRoles>" << endl
				<< "<properties>" << endl;

		multimap<long, OntyxProperty> conProp = pCon->second.getProperties();
		multimap<long, OntyxProperty>::const_iterator pConProp = conProp.begin();
		string propString;
		for( ; pConProp != conProp.end(); ++pConProp ) {
			propString = verifyOutString(pConProp->second.getValue());
			outFile << "<property>" << endl
				<< "<name>" << pConProp->second.getName() << "</name>" << endl
				<< "<value>" << propString << "</value>" << endl
				// locale not being taken care of
				<< "</property>" << endl;
			}
		outFile << "</properties>" << endl
				<< "</conceptDef>" << endl << endl;
		}
	outFile << trailer << endl;
	outFile.close();
}




void OntyxKb::writeOWLFileByName (const string & filename, const string headerFile, bool noProps) // flag = true:renumber
// if noProps is true, properties with names derived from roles are not exported
// 060813 since we have been deleting properties from a file listing, will make the "noProps" argument
// irrelevant and will change the signature in the future, the bool noProps doesn't make a difference
// as of 060813.

{
	set<string> kind_names;

	set<string> hasValueDatatypes;
/*	hasValueDatatypes.insert("Gene_In_Chromosomal_Location");		// uncomment when ready to do datatypes
	hasValueDatatypes.insert("Allele_In_Chromosomal_Location");
	hasValueDatatypes.insert("Allele_Absent_From_Wild-type_Chromosomal_Location");
	hasValueDatatypes.insert("Gene_Has_Physical_Location");
*/
	string roleTarget; // the preferred_name of the filler value, used for hasValue

	ifstream inFile;
	inFile.open(headerFile.c_str());
	if( !inFile.good() ) {
		cerr << "Can't find/open file '" << headerFile.c_str() << "'.  Will EXIT." << endl;
		exit(0);
		}
	string inputLine;
	
	string trailer = "</rdf:RDF>";
	ofstream outFile;
	outFile.open(filename.c_str());

	while( getline(inFile, inputLine), !inFile.eof() )
		outFile << inputLine << endl;


	string conceptnamefordoc = "";
	OntyxConcept docConcept;
	multimap<long, OntyxProperty> docProperties;
	multimap<long, OntyxProperty>::iterator pTConProp;

	// used for property renaming, on clashes with assocs & roles on a property conversion
	set<string> nameReg;
	map<string, string> nameClash;
	string modName, validName;

	map<long, OntyxRoleDef>::iterator pRole = m_role.begin();
	for( ; pRole != m_role.end(); ++pRole ) {
		isValidOWL(pRole->second.getName());
		nameReg.insert(pRole->second.getName());
		if( conceptNameInKB(pRole->second.getName()) ) {
			string name = pRole->second.getName();
			string newname = "c_" + name;
			renameConcept(getIDFromName(name), newname);
			} 
		conceptnamefordoc = pRole->second.getName() + "_Role";
		if( conceptNameInKB(conceptnamefordoc) ) {
			docConcept = getConcept(conceptnamefordoc);
			docProperties = docConcept.getProperties();
			}
		else
			docProperties.clear();

		if( hasValueDatatypes.find(pRole->second.getName()) == hasValueDatatypes.end() )
			outFile << "<owl:ObjectProperty rdf:ID=\"" << pRole->second.getName() << "\">" << endl;
		else
			outFile << "<owl:DatatypeProperty rdf:ID=\"" << pRole->second.getName() << "\">" << endl;
		outFile << "<code>" << pRole->second.getCode() << "</code>" << endl
					<< "<rdfs:label>" << pRole->second.getName() << "</rdfs:label>" << endl;
		if( pRole->second.hasParent() )
			outFile << '\t' << "<rdfs:subPropertyOf rdf:resource=\"#" << pRole->second.getParentRole() << "\"/>" << endl;
		else {
			if( hasValueDatatypes.find(pRole->second.getName()) == hasValueDatatypes.end() ) {
				outFile << '\t' << "<rdfs:domain rdf:resource=\"#" << pRole->second.getDomain() << "\"/>" << endl;		// kinds are not renamed, at least yet
				outFile << '\t' << "<rdfs:range rdf:resource=\"#" << pRole->second.getRange() << "\"/>" << endl;
				}
			else
				outFile << "\t" << "<rdfs:range rdf:resource=\"http://www.w3.org/2001/XMLSchema#string\"/>" << endl;
			}
		if( docProperties.size() > 0 ) {
			for( pTConProp = docProperties.begin(); pTConProp !=docProperties.end(); ++pTConProp ) {
				outFile << "<" << pTConProp->second.getName() << ">" << verifyOutString(pTConProp->second.getValue())
						<< "</" << pTConProp->second.getName() << ">" << endl;
				}
			}
		if( hasValueDatatypes.find(pRole->second.getName()) == hasValueDatatypes.end() ) 
			outFile << "</owl:ObjectProperty>" << endl << endl;
		else
			outFile << "</owl:DatatypeProperty>" << endl << endl;
		}


	map<long, OntyxAssociationDef>::iterator pAssoc = m_association.begin();
	for( ; pAssoc != m_association.end(); ++pAssoc ) {
		isValidOWL(pAssoc->second.getName());	// let routine issue warnings to console
		nameReg.insert(pAssoc->second.getName());
		if( conceptNameInKB(pAssoc->second.getName()) ) {
			string name = pAssoc->second.getName();
			string newname = "c_" + name;
			renameConcept(getIDFromName(name), newname);
			} 
		conceptnamefordoc = pAssoc->second.getName() + "_Association";
		if( conceptNameInKB(conceptnamefordoc) ) {
			docConcept = getConcept(conceptnamefordoc);
			docProperties = docConcept.getProperties();
			}
		outFile << "<owl:ObjectProperty rdf:ID=\"" << pAssoc->second.getName() << "\">" << endl
				<< "<rdf:type rdf:resource=\"http://www.w3.org/2002/07/owl#AnnotationProperty\"/>" << endl
				<< "<code>" << pAssoc->second.getCode() << "</code>" << endl
				<< "<rdfs:label>" << pAssoc->second.getName() << "</rdfs:label>" << endl;
		if( docProperties.size() > 0 ) {
			for( pTConProp = docProperties.begin(); pTConProp !=docProperties.end(); ++pTConProp ) {
				outFile << "<" << pTConProp->second.getName() << ">" << verifyOutString(pTConProp->second.getValue())
						<< "</" << pTConProp->second.getName() << ">" << endl;
				}
			}
		outFile << "</owl:ObjectProperty>" << endl << endl;
		}


	map<long, OntyxPropertyDef>::iterator pProp = m_property.begin();
	for( ; pProp != m_property.end(); ++pProp ) {
		isValidOWL(pProp->second.getName());  // let routine issue warnings to console
		if( conceptNameInKB(pProp->second.getName()) ) {
			string name = pProp->second.getName();
			string newname = "c_" + name;
			renameConcept(getIDFromName(name), newname);
			} 
		conceptnamefordoc = pProp->second.getName() + "_Property";
		if( conceptNameInKB(conceptnamefordoc) ) {
			docConcept = getConcept(conceptnamefordoc);
			docProperties = docConcept.getProperties();
			}
		else
			docProperties.clear();
		if( nameReg.count(pProp->second.getName()) > 0 ) 
			modName = "ro" + pProp->second.getName();
		else
			modName = pProp->second.getName();
		nameClash.insert(map<string,string>::value_type(pProp->second.getName(), modName));
		outFile << "<owl:DatatypeProperty rdf:ID=\"" << nameClash[pProp->second.getName()] << "\">" << endl
				<< "<code>" << pProp->second.getCode() << "</code>" << endl 
				<< "<rdfs:label>" << pProp->second.getName() << "</rdfs:label>" << endl
				<< "<rdf:type rdf:resource=\"http://www.w3.org/2002/07/owl#AnnotationProperty\"/>" << endl;
		if( docProperties.size() > 0 ) {
			for( pTConProp = docProperties.begin(); pTConProp !=docProperties.end(); ++pTConProp ) {
				outFile << "<" << pTConProp->second.getName() << ">" << verifyOutString(pTConProp->second.getValue())
						<< "</" << pTConProp->second.getName() << ">" << endl;
				}
			}
		if( !pProp->second.hasPickList() )
			outFile << "<rdfs:range rdf:resource=\"http://www.w3.org/2001/XMLSchema#string\"/>" << endl;
		else {
			vector<string> picklist = pProp->second.getPickList();
			vector<string>::iterator pPicklist = picklist.begin();
			outFile << "<rdfs:range>" << endl
					<< "\t" << "<owl:DataRange>" << endl
					<< "\t" << "<owl:oneOf>" << endl;
			while( pPicklist != picklist.end() ) {
				outFile << "\t\t" << "<rdf:List>" << endl
						<< "\t\t" << "<rdf:first rdf:datatype=\"http://www.w3.org/2001/XMLSchema#string\">" << *pPicklist << "</rdf:first>" << endl;
				if( ++pPicklist != picklist.end() )
					outFile << "\t\t" << "<rdf:rest>" << endl;
				else
					outFile << "\t\t" << "<rdf:rest rdf:resource=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#nil\"/>" << endl
							<< "\t\t" << "</rdf:List>" << endl;
				}
			pPicklist = picklist.begin();
			++pPicklist;				// need to advance it 'cause the last element in the listing above gets an </rdf:List>
			for( ; pPicklist != picklist.end(); ++pPicklist ) {
				outFile << "\t\t" << "</rdf:rest>"  << endl
						<< "\t\t" << "</rdf:List>" << endl;
				}
			outFile << "\t" << "</owl:oneOf>" << endl
					<< "\t" << "</owl:DataRange>" << endl
					<< "</rdfs:range>" << endl;
			}
		outFile << "</owl:DatatypeProperty>" << endl << endl;
		}


	map<long, OntyxKind>::iterator pKind0;
	map<long, OntyxKind>::iterator pKind = m_kind.begin();
	for( ; pKind != m_kind.end(); ++pKind) {
		isValidOWL(pKind->second.getKindname());
		kind_names.insert(pKind->second.getKindname());
		string conname, lcode;
		OntyxConcept tmpConcept;
		lcode = pKind->second.getCode();
		conname = pKind->second.getKindname();
		outFile  << "<owl:Class rdf:ID=\"" << pKind->second.getKindname() << "\">" << endl
			<< '\t' << "<code>" << lcode << "</code>" << endl
			<< '\t' << "<rdfs:label>" << conname << "</rdfs:label>" << endl;
		if( conceptNameInKB(pKind->second.getKindname())) {		// is there a concept with the same name
			tmpConcept = getConcept(pKind->second.getKindname());
			multimap<long, OntyxProperty> conProp = tmpConcept.getProperties();
			multimap<long, OntyxProperty>::const_iterator pConProp = conProp.begin();
			string propString, propName = "";
			for( ; pConProp != conProp.end(); ++pConProp ) {
				propString = verifyOutString(pConProp->second.getValue());
				string mname = nameClash[pConProp->second.getName()];
				outFile << "<" << mname << ">" << propString << "</" << mname << ">" << endl;
				}
			}
		for( pKind0 = m_kind.begin(); pKind0 != m_kind.end(); ++pKind0 ) {
			if( pKind0->second.getKindname() != pKind->second.getKindname() )
				outFile << '\t' << "<owl:disjointWith rdf:resource=\"#" 
					<< pKind0->second.getKindname() << "\"/>" << endl;
			} 
		outFile << "</owl:Class>" << endl << endl;
		}

	map<long, OntyxConcept>::iterator pCon = m_concept.begin();
	string label;
	for( ; pCon != m_concept.end(); ++pCon ) {
		if( kind_names.count(pCon->second.getName()) > 0 )	// kind concept
			continue;
		isValidOWL( pCon->second.getName() );	// just do a check, let routine output warning
		if (pCon->second.getKind() == "Retired_Kind")
			outFile << "<owl:DeprecatedClass rdf:ID=\"" << pCon->second.getName() << "\">" << endl; // changed getCode() to getName()
		else if (pCon->second.getNumParents() == 1) {
			set<string>::const_iterator pConPar = pCon->second.getFirstParent();
			string sConPar = verifyOutString(*pConPar);
 			if (sConPar == "ObsoleteClass")
    				outFile << "<owl:DeprecatedClass rdf:ID=\"" << pCon->second.getName() << "\">" << endl; // changed getCode() to getName()
          		else 
    				outFile << "<owl:Class rdf:ID=\"" << pCon->second.getName() << "\">" << endl; // changed getCode() to getName()
			}
		else
			outFile << "<owl:Class rdf:ID=\"" << verifyOutString(pCon->second.getName()) << "\">" << endl;
		if( pCon->second.getPropertyNumber("Preferred_Name") > 0 )
			label =  verifyOutString(pCon->second.getPropertyValue("Preferred_Name"));
		else
			label = verifyOutString(pCon->second.getName());
		outFile << "<rdfs:label>" << label << "</rdfs:label>" << endl;
		outFile << "<code>" << pCon->second.getCode() << "</code>" << endl;
		outFile << "<ID>" << pCon->second.getID() << "</ID>" << endl; 	// temporary for 05.03d processing

		if( !pCon->second.getState() ) {  // if not defined, i.e. primitive, deal with it here
			if( pCon->second.getNumParents() > 0 ) {
				set<string> conPar = pCon->second.getParents();
				set<string>::const_iterator pConPar = conPar.begin();
					outFile << "<hasType>primitive</hasType>" << endl;	// temporary for 05.03d processing
				for( ; pConPar != conPar.end(); ++pConPar ) {
					outFile << "<rdfs:subClassOf rdf:resource=\"#" << verifyOutString(*pConPar) << "\"/>" << endl;
					}
				}
			else {		// root node, make the kind its parent
				outFile << "<rdfs:subClassOf rdf:resource=\"#" << pCon->second.getKind() << "\"/>" << endl;
				}

			if( pCon->second.getNumRoles() > 0 ) {
				multimap<long, OntyxRole> conRoles = pCon->second.getRoles();
				multimap<long, OntyxRole>::const_iterator pConRoles = conRoles.begin();
				for( ; pConRoles != conRoles.end(); ++pConRoles ) {
					if( pConRoles->second.getRolegroup() == 0 ) {			// take care of ontylog role groups, do ungrouped roles first
						outFile << "<rdfs:subClassOf>" << endl
								<< "\t" << "<owl:Restriction>" << endl;
						outFile << "\t\t" << "<owl:onProperty rdf:resource=\"#" << pConRoles->second.getName() << "\"/>" << endl;
						if( hasValueDatatypes.find(pConRoles->second.getName()) != hasValueDatatypes.end() ) {
							roleTarget = getConcept(pConRoles->second.getValue()).getPropertyValue("Preferred_Name");
							if( roleTarget == OntyxTerminology::emptyString )
								roleTarget = pConRoles->second.getValue();
							outFile << "\t\t" << "<owl:hasValue rdf:datatype=\"http://www.w3.org/2001/XMLSchema#string\">" 
								<< verifyOutString(roleTarget) << "</owl:hasValue>" << endl;
							}
						else if( pConRoles->second.getModifier() == "all" ) 
							outFile << "\t\t" << "<owl:allValuesFrom rdf:resource=\"#" << verifyOutString(pConRoles->second.getValue())  << "\"/>" << endl;
						else 					// default, poss will also get converted to some
							outFile << "\t\t" << "<owl:someValuesFrom rdf:resource=\"#" << verifyOutString(pConRoles->second.getValue())  << "\"/>" << endl;
						outFile << "\t" << "</owl:Restriction>" << endl
								<< "</rdfs:subClassOf>" << endl;
						}
					}
				if( pCon->second.hasRolegroups() ) {  // ontylog role groups, do grouped roles now
					outFile << "<rdfs:subClassOf>" << endl
							<< "\t" << "<owl:Class>" << endl;
					if( pCon->second.getNumRoleGroups() > 1 )
						outFile << "\t\t" << "<owl:unionOf rdf:parseType=\"Collection\">" << endl;
					for(long groupCounter = 1; groupCounter <= pCon->second.getNumRoleGroups(); ++groupCounter) {
						if( pCon->second.getNumRoleGroups() > 1 ) {
							outFile << "\t\t\t" << "<owl:Class>" << endl;
							}
						outFile << "\t\t\t\t" << "<owl:intersectionOf rdf:parseType=\"Collection\">" << endl;
						for( pConRoles = conRoles.begin(); pConRoles != conRoles.end(); ++pConRoles ) {
							if( pConRoles->second.getRolegroup() == groupCounter ) {
								outFile << "\t\t\t\t\t" << "<owl:Restriction>" << endl;
								outFile << "\t\t\t\t\t\t" << "<owl:onProperty rdf:resource=\"#" << pConRoles->second.getName() << "\"/>" << endl;
								if( hasValueDatatypes.find(pConRoles->second.getName()) != hasValueDatatypes.end() ) {
									roleTarget = getConcept(pConRoles->second.getValue()).getPropertyValue("Preferred_Name");
									if( roleTarget == OntyxTerminology::emptyString )
										roleTarget = pConRoles->second.getValue();
									outFile << "\t\t\t\t\t\t" << "<owl:hasValue rdf:datatype=\"http://www.w3.org/2001/XMLSchema#string\">" 
										<< verifyOutString(roleTarget) << "</owl:hasValue>" << endl;
									}
								else if( pConRoles->second.getModifier() == "all" ) 
									outFile << "\t\t\t\t\t\t" << "<owl:allValuesFrom rdf:resource=\"#" << verifyOutString(pConRoles->second.getValue()) << "\"/>" << endl;
								else 			// default is some
									outFile << "\t\t\t\t\t\t" << "<owl:someValuesFrom rdf:resource=\"#" << verifyOutString(pConRoles->second.getValue()) << "\"/>" << endl;
								outFile << "\t\t\t\t\t" << "</owl:Restriction>" << endl;
								}
							}
						outFile << "\t\t\t\t" << "</owl:intersectionOf>" << endl;
						if( pCon->second.getNumRoleGroups() > 1 ) {
							outFile << "\t\t\t" << "</owl:Class>" << endl;
							}
						}
					if( pCon->second.getNumRoleGroups() > 1 )
						outFile << "\t\t" << "</owl:unionOf>" << endl;
					outFile << "\t" << "</owl:Class>" << endl
							<< "</rdfs:subClassOf>" << endl;
					}
				}

			}
		else {  // concept is defined, assume there are >= 2 conditions, and the "parent" is not a kind, i.e. it's deep in tree 
			outFile << "<owl:equivalentClass>" << endl
					<< "\t" 	<< "<owl:Class>" << endl
					<< "\t\t" 	<< "<owl:intersectionOf rdf:parseType=\"Collection\">" << endl;

			set<string> conPar = pCon->second.getParents();
			set<string>::const_iterator pConPar = conPar.begin();
			for( ; pConPar != conPar.end(); ++pConPar )  {
				outFile << "\t\t\t" << "<owl:Class rdf:about=\"#" << verifyOutString(*pConPar) << "\"/>" << endl;
				}
			if( pCon->second.getNumRoles() > 0 ) {
				multimap<long, OntyxRole> conRoles = pCon->second.getRoles();
				multimap<long, OntyxRole>::const_iterator pConRoles = conRoles.begin();
				for( ; pConRoles != conRoles.end(); ++pConRoles ) {
					if( pConRoles->second.getRolegroup() == 0 ) {			// take care of ontylog role groups, do ungrouped roles first
						outFile << "\t\t\t" << "<owl:Restriction>" << endl
								<< "\t\t\t\t" << "<owl:onProperty rdf:resource=\"#" << pConRoles->second.getName() << "\"/>" << endl;
						if( hasValueDatatypes.find(pConRoles->second.getName()) != hasValueDatatypes.end() ) {
							roleTarget = getConcept(pConRoles->second.getValue()).getPropertyValue("Preferred_Name");
							if( roleTarget == OntyxTerminology::emptyString )
								roleTarget = pConRoles->second.getValue();
							outFile << "\t\t\t\t" << "<owl:hasValue rdf:datatype=\"http://www.w3.org/2001/XMLSchema#string\">"
								<< verifyOutString(roleTarget) << "</owl:hasValue>" << endl;
							}
						else if( pConRoles->second.getModifier() == "all" ) 		// use "some" by default, dismiss "poss"
							outFile << "\t\t\t\t" << "<owl:allValuesFrom rdf:resource=\"#" << verifyOutString(pConRoles->second.getValue()) << "\"/>" << endl;
						else 
							outFile << "\t\t\t\t" << "<owl:someValuesFrom rdf:resource=\"#" << verifyOutString(pConRoles->second.getValue()) << "\"/>" << endl;
						outFile	<< "\t\t\t" << "</owl:Restriction>" << endl;
						}
					}
				if( pCon->second.hasRolegroups() ) {  // ontylog role groups, do grouped roles now
					if( pCon->second.getNumRoleGroups() > 1 )
						outFile << "\t\t\t" << "<owl:Class>" << endl
								<< "\t\t\t" << "<owl:unionOf rdf:parseType=\"Collection\">" << endl;
					multimap<long, OntyxRole> conRoles = pCon->second.getRoles();
					multimap<long, OntyxRole>::const_iterator pConRoles;
					for(long groupCounter = 1; groupCounter <= pCon->second.getNumRoleGroups(); ++groupCounter) {
						outFile << "\t\t\t" << "<owl:Class>" << endl
								<< "\t\t\t\t" << "<owl:intersectionOf rdf:parseType=\"Collection\">" << endl;
						for( pConRoles = conRoles.begin(); pConRoles != conRoles.end(); ++pConRoles ) {
							if( pConRoles->second.getRolegroup() == groupCounter ) {
								outFile << "\t\t\t\t\t" << "<owl:Restriction>" << endl;
								outFile << "\t\t\t\t\t\t" << "<owl:onProperty rdf:resource=\"#" << pConRoles->second.getName() << "\"/>" << endl;
								if( hasValueDatatypes.find(pConRoles->second.getName()) != hasValueDatatypes.end() ) {
									roleTarget = getConcept(pConRoles->second.getValue()).getPropertyValue("Preferred_Name");
									if( roleTarget == OntyxTerminology::emptyString )
										roleTarget = pConRoles->second.getValue();
									outFile << "\t\t\t\t" << "<owl:hasValue rdf:datatype=\"http://www.w3.org/2001/XMLSchema#string\">" 
										<< verifyOutString(roleTarget) << "</owl:hasValue>" << endl;
									}
								else if( pConRoles->second.getModifier() == "all" ) 
									outFile << "\t\t\t\t\t\t" << "<owl:allValuesFrom rdf:resource=\"#" << verifyOutString(pConRoles->second.getValue()) << "\"/>" << endl;
								else 
									outFile << "\t\t\t\t\t\t" << "<owl:someValuesFrom rdf:resource=\"#" << verifyOutString(pConRoles->second.getValue()) << "\"/>" << endl;
								outFile << "\t\t\t\t\t" << "</owl:Restriction>" << endl;
								}
							}
						outFile << "\t\t\t\t" << "</owl:intersectionOf>" << endl
								<< "\t\t\t" << "</owl:Class>" << endl;
						}
					if( pCon->second.getNumRoleGroups() > 1 )
						outFile << "\t\t\t" << "</owl:unionOf>" << endl
								<< "\t\t\t" << "</owl:Class>" << endl;
					}
				}
			outFile << "\t\t"	<< "</owl:intersectionOf>" << endl
					<< "\t"		<< "</owl:Class>" << endl
					<< "</owl:equivalentClass>" << endl;
			}

		multimap<long, OntyxProperty> conProp = pCon->second.getProperties();
		multimap<long, OntyxProperty>::const_iterator pConProp = conProp.begin();
		string propString, propName = "";
		for( ; pConProp != conProp.end(); ++pConProp ) {
			propString = verifyOutString(pConProp->second.getValue());
			propName = nameClash[pConProp->second.getName()];
			if( pConProp->second.hasQualifier() ) {         // 080623 if qualifiers exist, print CDATA tag
			    outFile << "<" << propName << ">" << "<![CDATA[<value>" << propString << "</value>";
			    vector<OntyxQualifier> vqual = pConProp->second.getQualifiers();
			    vector<OntyxQualifier>::iterator pvqual = vqual.begin();
			    for( ; pvqual != vqual.end(); ++pvqual ) {
                                     outFile << "<qual>";
                                     outFile << "<qual-name>" << pvqual->getName() << "</qual-name>";
                                     outFile << "<qual-value>" << pvqual->getValue() << "</qual-value>";
                                     outFile << "</qual>";
                                   }
                                   outFile << "]]></" << propName << ">" << endl;
                               }
                         else
                             outFile << "<" << propName << ">" << propString << "</" << propName << ">" << endl;
					// locale not being taken care of
			}

		// associations
		multimap<long, OntyxAssociation> conAssoc = pCon->second.getAssociations();
		multimap<long, OntyxAssociation>::const_iterator pConAssoc = conAssoc.begin();
		string assocName = "";
		for( ; pConAssoc != conAssoc.end(); ++pConAssoc ) {
			assocName = pConAssoc->second.getName();
			isValidOWL(pConAssoc->second.getValue());  // redundant, filler will also be verified at conceptDef stage
			outFile << "<" << assocName << " rdf:resource=\"#" << pConAssoc->second.getValue() << "\"/>" << endl;
			}

		if (pCon->second.getKind() == "Retired_Kind")
			outFile << "</owl:DeprecatedClass>" << endl << endl;
		else if (pCon->second.getNumParents() == 1){
			set<string>::const_iterator pConPar = pCon->second.getFirstParent();
            string sConPar = verifyOutString(*pConPar);
            if (sConPar == "ObsoleteClass")
    			outFile << "</owl:DeprecatedClass>" << endl << endl;
            else 
    			outFile << "</owl:Class>" << endl << endl;
		}
		else
			outFile << "</owl:Class>" << endl << endl;
		}

	outFile << trailer << endl;
	outFile.close();
}



string OntyxKb::xName( const string & inString )
{
 
	unsigned i;
	string tmpString, 
			tmpInString = inString, 
			outString = "";
	string plus = "+", 
			plus_string = "_plus_";
	outString.reserve(256);

	// replace '+' character with "plus" string
	unsigned startpos;
	if( tmpInString.find("+") != string::npos ) {
		while( (startpos = tmpInString.find(plus, 0)) != string::npos )
			tmpInString.replace(startpos, plus.size(), plus_string);
		}
	// prefix string with an underscore if first character is illegal
	if( !isalpha(tmpInString[0]) && tmpInString[0] != '_' )
		tmpString = "_";
	// eliminate illegal characters, string will become a xName
	for( i = 0; i < tmpInString.size(); ++i ) {
		if( isalnum(tmpInString[i]) || tmpInString[i] == '_' || tmpInString[i] == '-')
			tmpString.push_back(tmpInString[i]);
		else
			tmpString.push_back('_');
		}
	// "beautify" xName now
	outString.push_back(tmpString[0]);
	for( i = 1; i < tmpString.size(); ++i ) {
		if( tmpString[i] != '_' && tmpString[i] != '-' )
			outString.push_back(tmpString[i]);
		else if( tmpString[i] == '_' && tmpString[i-1] != '_' && tmpString[i-1] != '-')
			outString.push_back(tmpString[i]);
		else if( tmpString[i] == '-' && tmpString[i-1] != '_' && tmpString[i-1] != '-')
			outString.push_back(tmpString[i]);
		}
	return outString;
}

string indent(string change)
{
	string indentstr = "\t";

	static unsigned level = 0;
	string slevel = "";
	if( change == "+" ) {
		level++;
		}
	else if( change == "-" && level > 0 ) {
		level--;
		}
	else if( change == "s" ) {
		; // keep current level
		}
	else if( change == "c" ) {
		level = 0;
		}
	unsigned i = 0;
	while( i < level ) {
		slevel += indentstr;
		i++;
		}
	return slevel;
}

struct nameValue {
	string name;
	string value;
	};
struct owlaxioms {
	vector<struct nameValue> nameVal;
	string propertyValue;
	string classID;
	string propertyName;
	};

void OntyxKb::writeNDRFT2OWLFile (string & filename, vector<string> & header, string & locnamespace )
{
// keep it simple for ndfrt:
// - assume no "self-defining" terminology, i.e. roles, properties, associations are not documented or qualified
// - assume one root node per kind, alert when number of roots differs from number of kinds
// - don't output kinds, remap disjointness & domain/range axioms to roots


	string trailer = "</rdf:RDF>";
	ofstream outFile;
	outFile.open(filename.c_str());


	//
	// RENAME CONCEPTS USING THE NUI, THE RENAME WILL BE DONE ON OUTPUT USING A name:NUI map CREATED BELOW
	//
	//	nameNuiMap
	//
	map<string,string> nameNuiMap;
	map<long, OntyxConcept>::iterator pCon = m_concept.begin();
	for( ; pCon != m_concept.end(); ++pCon ) {
		multimap<long, OntyxProperty> conProp = pCon->second.getProperties();
		multimap<long, OntyxProperty>::const_iterator pConProp = conProp.begin();
		string propName = "NUI";
		for( ; pConProp != conProp.end(); ++pConProp ) {
			if( propName == pConProp->second.getName() ) 
				nameNuiMap[pCon->second.getName()] = pConProp->second.getValue();
			}
		}

	//
	// RENAME ROLES USING THE code, THE RENAME WILL BE DONE ON OUTPUT USING A name:Code map
	//
	//	nameCodeMap
	//
	map<string,string> nameCodeMap;
	map<long, OntyxRoleDef>::iterator pRole = m_role.begin();
	for( ; pRole != m_role.end(); ++pRole ) {
		nameCodeMap[pRole->second.getName()] = pRole->second.getCode();
		}

	//
	// GENERATE A MAP OF kind:root-node SO ROLE DOMAINS AND RANGES CAN BE REMAPPED
	//
	// 	kindRootMap
	//
	set<string> roots;
	findRootConcepts(roots);
	if( static_cast<unsigned long>(getNumKinds()) != static_cast<unsigned long>(roots.size()) )
		cerr << "Number of Kinds is not the same as the number of root Concepts."  << endl;
	map<string,string> kindRootMap;
	set<string>::iterator pRoot = roots.begin();
	for( ; pRoot != roots.end(); ++pRoot ) {
		long conID = getIDFromName(*pRoot);
		string kindName = getConceptKind(conID);
		kindRootMap[kindName] = *pRoot;
		}

	//
	// OUTPUT HEADER
	//
	vector<string>::iterator pHead = header.begin();
	for( ; pHead != header.end(); ++pHead ) 
		outFile << *pHead << endl;
	outFile << endl;

	//
	// OUTPUT ROLE DEFINITIONS
	//
	pRole = m_role.begin();
	for( ; pRole != m_role.end(); ++pRole ) {
		isValidOWL(nameCodeMap[pRole->second.getName()]);  // warn of invalid names
		outFile << "<owl:ObjectProperty rdf:ID=\"" << nameCodeMap[pRole->second.getName()] << "\">" << endl
				<< "<code>" << pRole->second.getCode() << "</code>" << endl
				<< "<rdfs:label>" << pRole->second.getName() << "</rdfs:label>" << endl;
		if( pRole->second.hasParent() ) {
			outFile << '\t' << "<rdfs:subPropertyOf rdf:resource=\"#" << nameCodeMap[pRole->second.getParentRole()] << "\"/>" << endl;
			}
		else {
			string dname = nameNuiMap[ kindRootMap[pRole->second.getDomain()] ];
			string rname = nameNuiMap[ kindRootMap[pRole->second.getRange()] ];
			outFile << '\t' << "<rdfs:domain rdf:resource=\"#" << dname << "\"/>" << endl;		
			outFile << '\t' << "<rdfs:range rdf:resource=\"#" << rname  << "\"/>" << endl;
			}

		outFile << "</owl:ObjectProperty>" << endl << endl;
		}

	//
	// OUTPUT ANNOTATION PROPERTY DEFINITIONS
	//
	map<long, OntyxPropertyDef>::iterator pProp = m_property.begin();
	for( ; pProp != m_property.end(); ++pProp ) {
		isValidOWL(pProp->second.getName());  // warn of invalid names, deal with them later if they come up
		outFile << "<owl:AnnotationProperty rdf:ID=\"" << pProp->second.getName() << "\">" << endl
				<< "<code>" << pProp->second.getCode() << "</code>" << endl
				<< "<rdfs:label>" << pProp->second.getName() << "</rdfs:label>" << endl;
		if( !pProp->second.hasPickList() )
			outFile << "<rdfs:range rdf:resource=\"http://www.w3.org/2001/XMLSchema#string\"/>" << endl
				<< "<rdf:type rdf:resource=\"http://www.w3.org/2002/07/owl#DatatypeProperty\"/>" << endl;
		else {
			indent("c");
			vector<string> picklist = pProp->second.getPickList();
			vector<string>::iterator pPicklist = picklist.begin();
			outFile << "<rdfs:range>" << endl;
			outFile << indent("+") << "<owl:DataRange>" << endl;
			outFile << indent("s") << "<owl:oneOf>" << endl;
			while( pPicklist != picklist.end() ) {
				outFile << indent("+") << "<rdf:List>" << endl;
				outFile << indent("+") << "<rdf:first rdf:datatype=\"http://www.w3.org/2001/XMLSchema#string\">" << *pPicklist << "</rdf:first>" << endl;
				if( ++pPicklist != picklist.end() )
					outFile << indent("s") << "<rdf:rest>" << endl;
				else {
					outFile << indent("s") << "<rdf:rest rdf:resource=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#nil\"/>" << endl;
					outFile << indent("-") << "</rdf:List>" << endl;
					}
				}
			pPicklist = picklist.begin();
			++pPicklist;				// need to advance it 'cause the last element in the listing above gets an </rdf:List>
			for( ; pPicklist != picklist.end(); ++pPicklist ) {
				outFile << indent("-") << "</rdf:rest>"  << endl;
				outFile << indent("-") << "</rdf:List>" << endl;
				}
			outFile << indent("-") << "</owl:oneOf>" << endl;
			outFile << indent("s") << "</owl:DataRange>" << endl;
			outFile << indent("-") << "</rdfs:range>" << endl;
			}
		outFile << "</owl:AnnotationProperty>" << endl << endl;
		}

	// 
	// OUTPUT ONTYLOG QUALIFIERS AS ANNOTATION PROPERTIES
	//
	map<long, OntyxQualifierDef>::iterator pQualDef = m_qualifier.begin();
	for( ; pQualDef != m_qualifier.end(); ++pQualDef ) {
		isValidOWL(pQualDef->second.getName());  // warn of invalid names, deal with them later if they come up
		outFile << "<owl:AnnotationProperty rdf:ID=\"" << pQualDef->second.getName() << "\">" << endl
				<< "<code>" << pQualDef->second.getCode() << "</code>" << endl
				<< "<rdfs:label>" << pQualDef->second.getName() << "</rdfs:label>" << endl;
		if( !pQualDef->second.hasPickList() )
			outFile << "<rdfs:range rdf:resource=\"http://www.w3.org/2001/XMLSchema#string\"/>" << endl
				<< "<rdf:type rdf:resource=\"http://www.w3.org/2002/07/owl#DatatypeProperty\"/>" << endl;
		else {
			vector<string> picklist = pQualDef->second.getPickList();
			vector<string>::iterator pPicklist = picklist.begin();
			outFile << "<rdfs:range>" << endl
					<< "\t" << "<owl:DataRange>" << endl
					<< "\t" << "<owl:oneOf>" << endl;
			while( pPicklist != picklist.end() ) {
				outFile << "\t\t" << "<rdf:List>" << endl
						<< "\t\t" << "<rdf:first rdf:datatype=\"http://www.w3.org/2001/XMLSchema#string\">" << *pPicklist << "</rdf:first>" << endl;
				if( ++pPicklist != picklist.end() )
					outFile << "\t\t" << "<rdf:rest>" << endl;
				else
					outFile << "\t\t" << "<rdf:rest rdf:resource=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#nil\"/>" << endl
							<< "\t\t" << "</rdf:List>" << endl;
				}
			pPicklist = picklist.begin();
			++pPicklist;				// need to advance it 'cause the last element in the listing above gets an </rdf:List>
			for( ; pPicklist != picklist.end(); ++pPicklist ) {
				outFile << "\t\t" << "</rdf:rest>"  << endl
						<< "\t\t" << "</rdf:List>" << endl;
				}
			outFile << "\t" << "</owl:oneOf>" << endl
					<< "\t" << "</owl:DataRange>" << endl
					<< "</rdfs:range>" << endl;
			}
		outFile << "</owl:AnnotationProperty>" << endl << endl;
		}

	//
	// ASSOCIATIONS WILL NEED TO BE TWEAKED AS THEIR OWL2 REPRESENTATION GETS WORKED OUT
	//
	map<long, OntyxAssociationDef>::iterator pAssoc = m_association.begin();
	for( ; pAssoc != m_association.end(); ++pAssoc ) {
		isValidOWL(pAssoc->second.getName());  // warn of invalid names, deal with them later if they come up
		outFile << "<owl:ObjectProperty rdf:ID=\"" << pAssoc->second.getName() << "\">" << endl
				<< "<rdfs:label>" << pAssoc->second.getName() << "</rdfs:label>" << endl
				<< "<rdf:type rdf:resource=\"http://www.w3.org/2002/07/owl#AnnotationProperty\"/>" << endl;

		outFile << "</owl:ObjectProperty>" << endl << endl;
		}

	// 
	// ADD DISJOINTNESS AXIOMS TO ROOT CONCEPTS
	//
	pRoot = roots.begin();
	set<string>::iterator pRoot2;
	pRoot = roots.begin();
	for( ; pRoot != roots.end(); ++pRoot ) {
		isValidOWL(nameNuiMap[*pRoot]);
		outFile << "<owl:Class rdf:about=\"#" << nameNuiMap[*pRoot] << "\">" << endl;
		for( pRoot2 = roots.begin(); pRoot2 != roots.end(); ++pRoot2 ) {
			isValidOWL(nameNuiMap[*pRoot2]);
			if( *pRoot != *pRoot2 )
				outFile << "\t" << "<owl:disjointWith rdf:resource=\"#" << nameNuiMap[*pRoot2] << "\"/>" << endl;
			}
		outFile << "</owl:Class>" << endl << endl;
		}


	//
	// OUTPUT CONCEPT DECLARATIONS, ROLES OUTPUT DIFFERENTLY DEPENDING ON PRIMITVE/DEFINED STATE
	//
	pCon = m_concept.begin();
	string label;
	for( ; pCon != m_concept.end(); ++pCon ) {

		outFile << "<owl:Class rdf:about=\"#" << nameNuiMap[pCon->second.getName()] << "\">" << endl;
		if( pCon->second.getPropertyNumber("Preferred_Name") > 0 )
			label =  verifyOutString(pCon->second.getPropertyValue("Preferred_Name"));
		else
			label = verifyOutString(pCon->second.getName());
		outFile << "<rdfs:label>" << label << "</rdfs:label>" << endl;
		outFile << "<code>" << pCon->second.getCode() << "</code>" << endl;


		if( !pCon->second.getState() ) {  // PRIMITIVE CONCEPT, I.E NOT DEFINED
			if( pCon->second.getNumParents() > 0 ) {
				set<string> conPar = pCon->second.getParents();
				set<string>::const_iterator pConPar = conPar.begin();
				for( ; pConPar != conPar.end(); ++pConPar ) {
					outFile << "<rdfs:subClassOf rdf:resource=\"#" << nameNuiMap[*pConPar] << "\"/>" << endl;
					}
				}
			if( pCon->second.getNumRoles() > 0 ) {
				multimap<long, OntyxRole> conRoles = pCon->second.getRoles();
				multimap<long, OntyxRole>::const_iterator pConRoles = conRoles.begin();
				for( ; pConRoles != conRoles.end(); ++pConRoles ) {
					if( pConRoles->second.getRolegroup() == 0 ) {			// take care of ontylog role groups, do ungrouped roles first
						outFile << "<rdfs:subClassOf>" << endl
								<< "\t" << "<owl:Restriction>" << endl;
						outFile << "\t\t" << "<owl:onProperty rdf:resource=\"#" << nameCodeMap[pConRoles->second.getName()] << "\"/>" << endl;
						if( pConRoles->second.getModifier() == "all" ) 
							outFile << "\t\t" << "<owl:allValuesFrom rdf:resource=\"#" << nameNuiMap[pConRoles->second.getValue()]  << "\"/>" << endl;
						else 					// default, poss will also get converted to some
							outFile << "\t\t" << "<owl:someValuesFrom rdf:resource=\"#" << nameNuiMap[pConRoles->second.getValue()]  << "\"/>" << endl;
						outFile << "\t" << "</owl:Restriction>" << endl
								<< "</rdfs:subClassOf>" << endl;
						}
					}
				if( pCon->second.hasRolegroups() ) {  // ontylog role groups, do grouped roles now
					outFile << "<rdfs:subClassOf>" << endl
							<< "\t" << "<owl:Class>" << endl;
					if( pCon->second.getNumRoleGroups() > 1 )
						outFile << "\t\t" << "<owl:unionOf rdf:parseType=\"Collection\">" << endl;
					for(long groupCounter = 1; groupCounter <= pCon->second.getNumRoleGroups(); ++groupCounter) {
						if( pCon->second.getNumRoleGroups() > 1 ) {
							outFile << "\t\t\t" << "<owl:Class>" << endl;
							}
						outFile << "\t\t\t\t" << "<owl:intersectionOf rdf:parseType=\"Collection\">" << endl;
						for( pConRoles = conRoles.begin(); pConRoles != conRoles.end(); ++pConRoles ) {
							if( pConRoles->second.getRolegroup() == groupCounter ) {
								outFile << "\t\t\t\t\t" << "<owl:Restriction>" << endl;
								outFile << "\t\t\t\t\t\t" << "<owl:onProperty rdf:resource=\"#" << nameCodeMap[pConRoles->second.getName()] << "\"/>" << endl;
								if( pConRoles->second.getModifier() == "all" ) 
									outFile << "\t\t\t\t\t\t" << "<owl:allValuesFrom rdf:resource=\"#" << nameNuiMap[pConRoles->second.getValue()] << "\"/>" << endl;
								else 			// default is some
									outFile << "\t\t\t\t\t\t" << "<owl:someValuesFrom rdf:resource=\"#" << nameNuiMap[pConRoles->second.getValue()] << "\"/>" << endl;
								outFile << "\t\t\t\t\t" << "</owl:Restriction>" << endl;
								}
							}
						outFile << "\t\t\t\t" << "</owl:intersectionOf>" << endl;
						if( pCon->second.getNumRoleGroups() > 1 ) {
							outFile << "\t\t\t" << "</owl:Class>" << endl;
							}
						}
					if( pCon->second.getNumRoleGroups() > 1 )
						outFile << "\t\t" << "</owl:unionOf>" << endl;
					outFile << "\t" << "</owl:Class>" << endl
							<< "</rdfs:subClassOf>" << endl;
					}
				}
			}
		else {  // DEFINED CONCEPT, assume there are >= 2 conditions, and the "parent" is not a kind, i.e. it's deep in tree 
			outFile << "<owl:equivalentClass>" << endl
					<< "\t" 	<< "<owl:Class>" << endl
					<< "\t\t" 	<< "<owl:intersectionOf rdf:parseType=\"Collection\">" << endl;

			set<string> conPar = pCon->second.getParents();
			set<string>::const_iterator pConPar = conPar.begin();
			for( ; pConPar != conPar.end(); ++pConPar )  {
				outFile << "\t\t\t" << "<owl:Class rdf:about=\"#" << nameNuiMap[*pConPar] << "\"/>" << endl;
				}
			if( pCon->second.getNumRoles() > 0 ) {
				multimap<long, OntyxRole> conRoles = pCon->second.getRoles();
				multimap<long, OntyxRole>::const_iterator pConRoles = conRoles.begin();
				for( ; pConRoles != conRoles.end(); ++pConRoles ) {
					if( pConRoles->second.getRolegroup() == 0 ) {			// simple roles, take care of role groups below
						outFile << "\t\t\t" << "<owl:Restriction>" << endl
								<< "\t\t\t\t" << "<owl:onProperty rdf:resource=\"#" << nameCodeMap[pConRoles->second.getName()] << "\"/>" << endl;
						if( pConRoles->second.getModifier() == "all" ) 		// use "some" by default, dismiss "poss"
							outFile << "\t\t\t\t" << "<owl:allValuesFrom rdf:resource=\"#" << nameNuiMap[pConRoles->second.getValue()] << "\"/>" << endl;
						else 
							outFile << "\t\t\t\t" << "<owl:someValuesFrom rdf:resource=\"#" << nameNuiMap[pConRoles->second.getValue()] << "\"/>" << endl;
						outFile	<< "\t\t\t" << "</owl:Restriction>" << endl;
						}
					}
				if( pCon->second.hasRolegroups() ) {  // ontylog role groups
					if( pCon->second.getNumRoleGroups() > 1 )
						outFile << "\t\t\t" << "<owl:Class>" << endl
								<< "\t\t\t" << "<owl:unionOf rdf:parseType=\"Collection\">" << endl;
					multimap<long, OntyxRole> conRoles = pCon->second.getRoles();
					multimap<long, OntyxRole>::const_iterator pConRoles;
					for(long groupCounter = 1; groupCounter <= pCon->second.getNumRoleGroups(); ++groupCounter) {
						outFile << "\t\t\t" << "<owl:Class>" << endl
								<< "\t\t\t\t" << "<owl:intersectionOf rdf:parseType=\"Collection\">" << endl;
						for( pConRoles = conRoles.begin(); pConRoles != conRoles.end(); ++pConRoles ) {
							if( pConRoles->second.getRolegroup() == groupCounter ) {
								outFile << "\t\t\t\t\t" << "<owl:Restriction>" << endl;
								outFile << "\t\t\t\t\t\t" << "<owl:onProperty rdf:resource=\"#" << nameCodeMap[pConRoles->second.getName()] << "\"/>" << endl;
								if( pConRoles->second.getModifier() == "all" ) 
									outFile << "\t\t\t\t\t\t" << "<owl:allValuesFrom rdf:resource=\"#" << nameNuiMap[pConRoles->second.getValue()]  << "\"/>" << endl;
								else 
									outFile << "\t\t\t\t\t\t" << "<owl:someValuesFrom rdf:resource=\"#" << nameNuiMap[pConRoles->second.getValue()]  << "\"/>" << endl;
								outFile << "\t\t\t\t\t" << "</owl:Restriction>" << endl;
								}
							}
						outFile << "\t\t\t\t" << "</owl:intersectionOf>" << endl
								<< "\t\t\t" << "</owl:Class>" << endl;
						}
					if( pCon->second.getNumRoleGroups() > 1 )
						outFile << "\t\t\t" << "</owl:unionOf>" << endl
								<< "\t\t\t" << "</owl:Class>" << endl;
					}
				}
			outFile << "\t\t" << "</owl:intersectionOf>" << endl
					<< "\t" << "</owl:Class>" << endl
					<< "</owl:equivalentClass>" << endl;
			}

		//
		// OUTPUT CONCEPT PROPERTIES AND CAPTURE THEIR QUALIFIERS
		//
		vector<owlaxioms> annotations;	// a vector of structures reflecting owl annotation axioms

		multimap<long, OntyxProperty> conProp = pCon->second.getProperties();
		multimap<long, OntyxProperty>::const_iterator pConProp = conProp.begin();
		for( ; pConProp != conProp.end(); ++pConProp ) {
			string pname = pConProp->second.getName();
			if( pConProp->second.hasQualifier() ) { 
				owlaxioms tmpAx;
				vector<OntyxQualifier> vqual = pConProp->second.getQualifiers();
				vector<OntyxQualifier>::iterator pvqual = vqual.begin();
				for( ; pvqual != vqual.end(); ++pvqual ) {
					nameValue tmpNV;
					tmpNV.name = pvqual->getName();
					tmpNV.value = verifyOutString(pvqual->getValue());
					tmpAx.nameVal.push_back(tmpNV);
					}
				tmpAx.propertyValue = verifyOutString(pConProp->second.getValue());
				tmpAx.classID = nameNuiMap[pCon->second.getName()];
				tmpAx.propertyName = pname;
				annotations.push_back(tmpAx);
				}
			outFile << "<" << pname << ">" << verifyOutString(pConProp->second.getValue()) << "</" << pname << ">" << endl;
			}

		// need to output associations as object properties
		//
		// OUTPUT CONCEPT ASSOCIATIONS AND CAPTURE THEIR QUALIFIERS
		//
		multimap<long, OntyxAssociation> conAssoc = pCon->second.getAssociations();
		multimap<long, OntyxAssociation>::const_iterator pConAssoc = conAssoc.begin();
		for( ; pConAssoc != conAssoc.end(); ++pConAssoc ) {
			string aname = pConAssoc->second.getName();
			if( pConAssoc->second.hasQualifier() ) {
				owlaxioms tmpAx;
				vector<OntyxQualifier> vqual = pConAssoc->second.getQualifiers();
				vector<OntyxQualifier>::iterator pvqual = vqual.begin();
				for( ; pvqual != vqual.end(); ++pvqual ) {
					nameValue tmpNV;
					tmpNV.name = pvqual->getName();
					tmpNV.value = verifyOutString(pvqual->getValue());
					tmpAx.nameVal.push_back(tmpNV);
					}
				tmpAx.propertyValue = locnamespace + nameNuiMap[pConAssoc->second.getValue()];
				tmpAx.classID = nameNuiMap[pCon->second.getName()];
				tmpAx.propertyName = aname;
				annotations.push_back(tmpAx);
				}
			outFile << "<" << aname << " rdf:resource=\"#" << nameNuiMap[pConAssoc->second.getValue()] << "\"/>" << endl;
			}
		outFile << "</owl:Class>" << endl;
	
		//
		// OUTPUT THE QUALIFIERS
		//
		if( annotations.size() > 0 ) {
			vector<owlaxioms>::iterator pAnnotations = annotations.begin();
			for( ; pAnnotations != annotations.end(); ++pAnnotations ) {
				outFile << "<owl:Axiom>" << endl;
				vector<nameValue>::iterator pNV = pAnnotations->nameVal.begin();
				for( ; pNV != pAnnotations->nameVal.end(); ++pNV ) 
					outFile << "\t" << "<" << pNV->name << ">" << pNV->value << "</" << pNV->name << ">" << endl;
				outFile << "\t" << "<owl:annotatedTarget>" << pAnnotations->propertyValue << "</owl:annotatedTarget>" << endl;
				outFile << "\t" << "<owl:annotatedSource rdf:resource=\"#" << pAnnotations->classID << "\"/>" << endl;
				outFile << "\t" << "<owl:annotatedProperty rdf:resource=\"#" << pAnnotations->propertyName << "\"/>" << endl;
				outFile << "</owl:Axiom>" << endl;
				}
			}
		outFile << endl;
		}

	outFile << trailer << endl;
	outFile.close();
}


