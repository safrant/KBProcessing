//*******************************************************************
// Definition of the Ontyx property class OntyxProperty
// 		Defines concept property objects in the Ontyx terminology kb
//		Initial class definition 4/13/2001
//		Modified based on "ontylog.dtd" from 10/19/01
//			(locale property -> accessor, comparison, << operators)
//
// Dependencies:	STL <string> class
//					<iostream>
//					OntyxPropertyDef - uses ID registry
//					OntyxTerminology - uses error codes
//
// Virtual:			~OntyxProperty ()
//
//*******************************************************************

#ifndef _ONTYX_PROPERTY_
#define _ONTYX_PROPERTY_

#include "OntyxPropertyDef.h"
#include "OntyxTerminology.h"
#include "OntyxQualifier.h"

#include <string>
#include <iostream>
#include <vector>

using namespace std;

class OntyxProperty
{
	private:

		string	m_value;
		long	m_propertyNameID;		// external OntyxPropertyDef ID of Property Definition name
		vector<OntyxQualifier> m_qualifiers;

		string	m_locale;				// defined in ontylog.dtd from 10/19/01, unimplemented
										// as of 12/28/01

		static long propertyError;

	public:

			// ********** Constructor/Destructor

		// default constructor calls OntyxPropertyDef::getPropDefMapID(propName) 
		// to get property id
		OntyxProperty(	const string &	inName = "", 
						const string &	inValue = "",
						const string &	inLocale = "");

		OntyxProperty(const OntyxProperty &);

		virtual ~OntyxProperty();

			// ********** Mutator Functions

		OntyxProperty & setValue (const string & );
		OntyxProperty & setName (const string & inName);	// generates error if inName not yet defined
		OntyxProperty & setQualifier (vector<OntyxQualifier> & inQual);
		OntyxProperty & addQualifier (OntyxQualifier & inQual);

			// ********** Accessor Functions

		const string & getValue (void) const;
		const string & getLocale (void) const;
		const string & getName (void) const;
		long getNameID (void) const;			// returns m_propertyNameID
		const vector<OntyxQualifier> & getQualifiers (void) const;

		bool hasQualifier (void) const;

			// ********** Relational Operators 

		bool operator== (const OntyxProperty &) const;		// applied to all member data
		bool operator< (const OntyxProperty &) const;		// applied to all member data


			// ********** Output Friend Operators

		friend ostream & operator<< (ostream &, const OntyxProperty &);

			// ********** Public Static Utility Functions 

		static void setError (long errorCode);
		static void clearError (long errorCode);
		static void clearError (void);
		static bool isError (long ErrorCode);
		static bool isError (void);
		static void printError (void);
};
#endif

