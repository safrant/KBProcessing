//*******************************************************************
// Implementation of the Ontyx association class OntyxAssociation
//
//		See header file for additional information
//
//*******************************************************************

#include "OntyxAssociation.h"
#include "OntyxAssociationDef.h"

#include <string>
#include <iostream>

using namespace std;


long OntyxAssociation::associationError = 0;

			// ********** Constructor/Destructor

OntyxAssociation::OntyxAssociation (	const string &	inName, 
										const string &	inValue)  // assume the qualifier is valid
		: m_value(inValue)
{
	m_associationNameID = OntyxAssociationDef::getAssocDefMapID(inName);
	if( OntyxAssociationDef::isError(OntyxTerminology::NO_PROPDEF) )
		setError(OntyxTerminology::NO_PROPDEF);
}

OntyxAssociation::OntyxAssociation (	const string &	inName, 
										const string &	inValue,
										vector<OntyxQualifier> inQual)  // assume the qualifier is valid
		: m_value(inValue), m_qualifiers(inQual)
{
	m_associationNameID = OntyxAssociationDef::getAssocDefMapID(inName);
	if( OntyxAssociationDef::isError(OntyxTerminology::NO_PROPDEF) )
		setError(OntyxTerminology::NO_PROPDEF);
}
OntyxAssociation::OntyxAssociation(const OntyxAssociation & inRight)
{
	m_value = inRight.m_value;
	m_associationNameID = inRight.m_associationNameID;
	m_qualifiers = inRight.m_qualifiers;
}
OntyxAssociation::~OntyxAssociation()
{
	;
}

			// ********** Mutator Functions 

OntyxAssociation & OntyxAssociation::setValue (const string & inVal)
{
	m_value = inVal;
	return *this;
}

			// ********** Accessor Functions

const string & OntyxAssociation::getValue (void) const
	{ return m_value; }

const string & OntyxAssociation::getName (void) const
{
	if( isError(OntyxTerminology::NO_PROPDEF) )
		return OntyxTerminology::emptyString;
	else
		return OntyxAssociationDef::getAssocDefMapName(m_associationNameID);
}

long OntyxAssociation::getNameID (void) const
	{ return m_associationNameID; }


const vector<OntyxQualifier> & OntyxAssociation::getQualifiers (void) const
	{ return m_qualifiers; }

bool OntyxAssociation::hasQualifier (void) const
{
	if( m_qualifiers.size() == 0 )
		return false;
	return true;
}

			// ********** Relational Operators 

bool OntyxAssociation::operator== (const OntyxAssociation & inRight) const	// applied to all member data
{
	if( hasQualifier() ) {
		return ( m_associationNameID == inRight.m_associationNameID ?
				( m_value == inRight.m_value ?
					( m_qualifiers == inRight.m_qualifiers ? true : false)
				: false)
			: false);
		}
	else {
		return ( m_associationNameID == inRight.m_associationNameID ?
				( m_value == inRight.m_value ? true : false)
			: false);
		}

}
bool OntyxAssociation::operator< (const OntyxAssociation & inRight) const		// applied to all member data
{
	if( hasQualifier() ) {
		return ( m_associationNameID < inRight.m_associationNameID ?
				( m_value < inRight.m_value ?
					( m_qualifiers < inRight.m_qualifiers ? true : false)
				: false)
			: false);
		}
	else {
		return ( m_associationNameID < inRight.m_associationNameID ?
				( m_value < inRight.m_value ? true : false)
			: false);
		}
}


			// ********** Output Friend Operators

ostream & operator<< (ostream & outStream, const OntyxAssociation & inRight)
{
	outStream << inRight.getName() << ' ' << inRight.getValue() ;
	if( inRight.hasQualifier() ) {
		vector<OntyxQualifier> qual = inRight.getQualifiers();
		vector<OntyxQualifier>::const_iterator pQual = qual.begin();
		for( ; pQual != qual.end() ; ++pQual )
			outStream << "|" << *pQual;
		}
	return outStream;
}
			// ********** Public Static Utility Functions 

void OntyxAssociation::setError(long errorCode)
{
	associationError |= errorCode;
}
void OntyxAssociation::clearError(long errorCode)
{
	associationError &= ~errorCode;
}
bool OntyxAssociation::isError(long errorCode)
{
	return ( (associationError & errorCode) ? true : false );
}
bool OntyxAssociation::isError (void)
{
	return ( associationError ? true : false );
}
void OntyxAssociation::clearError (void)
	{ associationError = 0; }

void OntyxAssociation::printError (void)
{
	for( long i = 0; i < OntyxTerminology::numErrors; i++) {
		if( isError( OntyxTerminology::errorArray[i] ) )
			cerr << "Error class OntyxAssociation " << OntyxTerminology::errorNames[i] << endl;
		}
}

