//*******************************************************************
// Implementation of Ontyx role modifier enumeration class OntyxRoleEnum
//
//		Initial class definition 4/13/2001
//
//*******************************************************************

#include "OntyxRoleEnum.h"

#include <string>
using namespace std;


// order of enum strings is identical to enumeration order
const char * OntyxRoleEnum::modifierNames[] = { "some", "all", "poss" };

string OntyxRoleEnum::tmpStr = "";


			// ********** Constructor/Destructor
OntyxRoleEnum::OntyxRoleEnum (const string & inRole)
{
	if( isValid(inRole) )
		modifier = convertToModifier(inRole);
	else
		modifier = some;
}
		
OntyxRoleEnum::OntyxRoleEnum (const OntyxRoleEnum & inRight)
{
	modifier = inRight.modifier;
}
OntyxRoleEnum::~OntyxRoleEnum ()
{
	;
}

			// ********** Accessor Functions

rolemodifier OntyxRoleEnum::getModifier (void) const
	{ return modifier; }

const string & OntyxRoleEnum::getModifier (char c) const	// takes any char to provide a distinct signature
{
	static long tmpNum;
	tmpNum = static_cast<long> (modifier);
	tmpStr = modifierNames[tmpNum];
	return tmpStr;
}

			// ********** Validation & Conversion Functions 

bool OntyxRoleEnum::isValid (const string & tstStr) const	// to check string validity prior to input
{
	static unsigned i;
	for(i = 0; i < sizeof modifierNames / sizeof(char *); ++i)
		if( tstStr == modifierNames[i])
			return true;
	return false;
}
rolemodifier OntyxRoleEnum::convertToModifier(const string & inMod)
{
	static unsigned i;
	if( isValid(inMod) ) {
		for( i = 0; i < sizeof modifierNames / sizeof(char *); ++i)
			if( inMod == modifierNames[i] )
				return ( static_cast<rolemodifier> (i) );
		}
	return modifier_low_bound;
}

			// ********** Relational Operators 
bool OntyxRoleEnum::operator== (const OntyxRoleEnum & inRight) const
	{ return (modifier == inRight.modifier ? true : false) ; }

bool OntyxRoleEnum::operator< (const OntyxRoleEnum & inRight) const
	{ return (modifier < inRight.modifier ? true : false) ; }





