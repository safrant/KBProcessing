//*******************************************************************
// Implementation of the Ontyx property class OntyxProperty
//
//		Initial class definition 4/13/2001
//		See header file for additional information
//
//*******************************************************************

#include "OntyxProperty.h"
#include "OntyxPropertyDef.h"

#include <string>
#include <iostream>

using namespace std;


long OntyxProperty::propertyError = 0;

			// ********** Constructor/Destructor

OntyxProperty::OntyxProperty (	const string &	inName, 
								const string &	inValue,
								const string &	inLocale)
		: m_value(inValue), m_locale(inLocale)
{
	m_propertyNameID = OntyxPropertyDef::getPropDefMapID(inName);
	if( OntyxPropertyDef::isError(OntyxTerminology::NO_PROPDEF) )
		setError(OntyxTerminology::NO_PROPDEF);
}
OntyxProperty::OntyxProperty(const OntyxProperty & inRight)
{
	m_value = inRight.m_value;
	m_propertyNameID = inRight.m_propertyNameID;
	m_locale = inRight.m_locale;
	m_qualifiers = inRight.m_qualifiers;
}
OntyxProperty::~OntyxProperty()
{
	;
}
			// ********** Mutator Functions

OntyxProperty & OntyxProperty::setValue (const string & newVal)
{
	m_value = newVal;
	return *this;
}

OntyxProperty & OntyxProperty::setName (const string & inName)
{
	m_propertyNameID = OntyxPropertyDef::getPropDefMapID(inName);
	if( OntyxPropertyDef::isError(OntyxTerminology::NO_PROPDEF) ) {
		setError(OntyxTerminology::NO_PROPDEF);
		cerr << "ERROR:  PropertyDef for '" << inName << "' not yet defined." << endl;
		}
	return *this;
}

OntyxProperty & OntyxProperty::setQualifier (vector<OntyxQualifier> & inQual)
{
	m_qualifiers = inQual; 
	return *this;
}

OntyxProperty & OntyxProperty::addQualifier (OntyxQualifier & inQual)
{
	m_qualifiers.push_back(inQual);
	return *this;
}

			// ********** Accessor Functions

const string & OntyxProperty::getValue (void) const
	{ return m_value; }

const string & OntyxProperty::getLocale (void) const
	{ return m_locale; }

const string & OntyxProperty::getName (void) const
{
	if( isError(OntyxTerminology::NO_PROPDEF) )
		return OntyxTerminology::emptyString;
	else
		return OntyxPropertyDef::getPropDefMapName(m_propertyNameID);
}

long OntyxProperty::getNameID (void) const
	{ return m_propertyNameID; }

const vector<OntyxQualifier> & OntyxProperty::getQualifiers (void) const
	{ return m_qualifiers;	}

bool OntyxProperty::hasQualifier (void) const
{
	if( m_qualifiers.size() == 0 )
		return false;
	return true;
}

			// ********** Relational Operators 

bool OntyxProperty::operator== (const OntyxProperty & inRight) const	// applied to all member data
{
	if( hasQualifier() ) {
		return ( m_propertyNameID == inRight.m_propertyNameID ?
			( m_value == inRight.m_value ?
				( m_locale == inRight.m_locale ? 
					( m_qualifiers == inRight.m_qualifiers ? true : false)
					: false)
				: false)
			: false);
		}
	else {
		return ( m_propertyNameID == inRight.m_propertyNameID ?
			( m_value == inRight.m_value ?
				( m_locale == inRight.m_locale ? true : false)
			: false)
		: false);
		}
}

bool OntyxProperty::operator< (const OntyxProperty & inRight) const		// applied to all member data
{
	return ( m_propertyNameID < inRight.m_propertyNameID ?
				( m_value < inRight.m_value ?
					( m_locale < inRight.m_locale ? 
						( m_qualifiers < inRight.m_qualifiers ? true : false)
					: false)
				: false)
			: false);
}

			// ********** Output Friend Operators

ostream & operator<< (ostream & outStream, const OntyxProperty & inRight)
{
	outStream << inRight.getName() << ' ' << inRight.getValue() ;  // add this back in if we ever implement locale << ' ' << inRight.getLocale() ;
	if( inRight.hasQualifier() ) {
		vector<OntyxQualifier> qual = inRight.getQualifiers();
		vector<OntyxQualifier>::const_iterator pQual = qual.begin();
		for( ; pQual != qual.end() ; ++pQual )
			outStream << "|" << *pQual;
		}
	return outStream;
}

			// ********** Public Static Utility Functions 

void OntyxProperty::setError(long errorCode)
{
	propertyError |= errorCode;
}
void OntyxProperty::clearError(long errorCode)
{
	propertyError &= ~errorCode;
}
bool OntyxProperty::isError(long errorCode)
{
	return ( (propertyError & errorCode) ? true : false );
}
bool OntyxProperty::isError (void)
{
	return ( propertyError ? true : false );
}
void OntyxProperty::clearError (void)
	{ propertyError = 0; }

void OntyxProperty::printError (void)
{
	for( long i = 0; i < OntyxTerminology::numErrors; i++) {
		if( isError( OntyxTerminology::errorArray[i] ) )
			cerr << "Error class OntyxProperty " << OntyxTerminology::errorNames[i] << endl;
		}
}

