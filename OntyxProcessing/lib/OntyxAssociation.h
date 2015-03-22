//*******************************************************************
// Definition of the Ontyx association class OntyxAssociation
// 		Defines concept association objects in the Ontyx terminology kb
//		Initial class definition 5/14/2005
//		Modified based on "ontylog.dtd" from TDE 3
//
// Dependencies:	STL <string> class
//					<iostream>
//					OntyxAssociationDef - uses ID registry
//					OntyxTerminology - uses error codes
//
// Virtual:			~OntyxAssociation ()
//
//*******************************************************************

#ifndef _ONTYX_ASSOCIATION_
#define _ONTYX_ASSOCIATION_

#include "OntyxAssociationDef.h"
#include "OntyxQualifier.h"

#include <string>
#include <iostream>
#include <set>
#include <vector>

using namespace std;

class OntyxAssociation
{
	private:

		string	m_value;
		long	m_associationNameID;	// external OntyxAssociationDef ID of association Definition name
		vector<OntyxQualifier>	m_qualifiers;


		static long associationError;

	public:

			// ********** Constructor/Destructor

		// default constructor calls OntyxAssociationDef::getAssocDefMapID(assocName) 
		// to get association id
		OntyxAssociation(	const string &	inName = "", 
							const string &	inValue = "");

		OntyxAssociation(	const string &	inName, 
							const string &	inValue,
							vector<OntyxQualifier> inQual);


		OntyxAssociation(const OntyxAssociation &);

		virtual ~OntyxAssociation();

			// ********** Accessor Functions

		const string & getValue (void) const;
		const string & getName (void) const;
		long getNameID (void) const;			// returns m_associationNameID

		const vector<OntyxQualifier> & getQualifiers (void) const;
		bool hasQualifier (void) const;

			// ********** Mutator Functions 

		OntyxAssociation & setValue (const string & );


			// ********** Relational Operators 

		bool operator== (const OntyxAssociation &) const;		// applied to all member data
		bool operator< (const OntyxAssociation &) const;		// applied to all member data


			// ********** Output Friend Operators

		friend ostream & operator<< (ostream &, const OntyxAssociation &);

			// ********** Public Static Utility Functions 

		static void setError (long errorCode);
		static void clearError (long errorCode);
		static void clearError (void);
		static bool isError (long ErrorCode);
		static bool isError (void);
		static void printError (void);
};
#endif

