//*******************************************************************
// Implementation of the Ontyx qualifier class OntyxQualifier
//
//		See header file for additional information
//
//*******************************************************************

#include "OntyxQualifier.h"
#include "OntyxQualifierDef.h"

#include <string>
#include <iostream>

using namespace std;


long OntyxQualifier::qualifierError = 0;

			// ********** Constructor/Destructor

OntyxQualifier::OntyxQualifier (	const string &	inName, 
									const string &	inValue)
		: m_value(inValue)
{
	m_qualifierNameID = OntyxQualifierDef::getQualifierDefMapID(inName);
	if( OntyxQualifierDef::isError(OntyxTerminology::NO_PROPDEF) )
		setError(OntyxTerminology::NO_PROPDEF);
}
OntyxQualifier::OntyxQualifier(const OntyxQualifier & inRight)
{
	m_value = inRight.m_value;
	m_qualifierNameID = inRight.m_qualifierNameID;
}
OntyxQualifier::~OntyxQualifier()
{
	;
}

			// ********** Accessor Functions

const string & OntyxQualifier::getValue (void) const
	{ return m_value; }

const string & OntyxQualifier::getName (void) const
{
	if( isError(OntyxTerminology::NO_PROPDEF) )
		return OntyxTerminology::emptyString;
	else
		return OntyxQualifierDef::getQualifierDefMapName(m_qualifierNameID);
}

long OntyxQualifier::getNameID (void) const
	{ return m_qualifierNameID; }

bool OntyxQualifier::isValidQualifier(void) const
{
	if( OntyxQualifierDef::hasPickList(getName()) )
		if( OntyxQualifierDef::stringInPickList(getName(), getValue()) )
			return true;
	return false;
}

			// ********** Relational Operators 

bool OntyxQualifier::operator== (const OntyxQualifier & inRight) const	// applied to all member data
{
	return ( m_qualifierNameID == inRight.m_qualifierNameID ?
				( m_value == inRight.m_value ? true : false)
			: false);
}
bool OntyxQualifier::operator< (const OntyxQualifier & inRight) const		// applied to all member data
{
	return ( m_qualifierNameID < inRight.m_qualifierNameID ?
				( m_value < inRight.m_value ? true	: false)
			: false);
}


			// ********** Output Friend Operators

ostream & operator<< (ostream & outStream, const OntyxQualifier & inRight)
{
	outStream << inRight.getName() << ' ' << inRight.getValue() ;
	return outStream;
}
			// ********** Public Static Utility Functions 

void OntyxQualifier::setError(long errorCode)
{
	qualifierError |= errorCode;
}
void OntyxQualifier::clearError(long errorCode)
{
	qualifierError &= ~errorCode;
}
bool OntyxQualifier::isError(long errorCode)
{
	return ( (qualifierError & errorCode) ? true : false );
}
bool OntyxQualifier::isError (void)
{
	return ( qualifierError ? true : false );
}
void OntyxQualifier::clearError (void)
	{ qualifierError = 0; }

void OntyxQualifier::printError (void)
{
	for( long i = 0; i < OntyxTerminology::numErrors; i++) {
		if( isError( OntyxTerminology::errorArray[i] ) )
			cerr << "Error class OntyxQualifier " << OntyxTerminology::errorNames[i] << endl;
		}
}

