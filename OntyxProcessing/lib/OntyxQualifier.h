//*******************************************************************
// Definition of the Ontyx qualifier class OntyxQualifier
// 		Defines association and property qualifier objects in the Ontyx terminology kb
//		Initial class definition 5/14/2005
//		Based on "ontylog.dtd" from TDE 3
//
// Dependencies:	STL <string> class
//					<iostream>
//					OntyxQualifierDef - uses ID registry
//					OntyxTerminology - uses error codes
//
// Virtual:			~OntyxQualifier ()
//
//*******************************************************************

#ifndef _ONTYX_QUALIFIER_
#define _ONTYX_QUALIFIER_

#include "OntyxQualifierDef.h"

#include <string>
#include <iostream>

using namespace std;

class OntyxQualifier
{
	private:

		string	m_value;
		long	m_qualifierNameID;	// external OntyxQualifierDef ID of qualifier Definition name


		static long qualifierError;

	public:

			// ********** Constructor/Destructor

		// default constructor calls OntyxQualifierDef::getAssocDefMapID(assocName) 
		// to get qualifier id
		OntyxQualifier(	const string &	inName = "", 
						const string &	inValue = "");

		OntyxQualifier(const OntyxQualifier &);

		virtual ~OntyxQualifier();

			// ********** Accessor Functions

		const string & getValue (void) const;
		const string & getName (void) const;
		long getNameID (void) const;			// returns m_qualifierNameID

		bool isValidQualifier(void) const;

			// ********** Relational Operators 

		bool operator== (const OntyxQualifier &) const;		// applied to all member data
		bool operator< (const OntyxQualifier &) const;		// applied to all member data

			// ********** Output Friend Operators

		friend ostream & operator<< (ostream &, const OntyxQualifier &);

			// ********** Public Static Utility Functions 

		static void setError (long errorCode);
		static void clearError (long errorCode);
		static void clearError (void);
		static bool isError (long ErrorCode);
		static bool isError (void);
		static void printError (void);
};
#endif

