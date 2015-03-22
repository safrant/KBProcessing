//*******************************************************************
// Definition of Ontyx role modifiers and class OntyxRoleEnum
//		Defines permissible role modifiers
//		Initial class definition 4/13/2001
//		Conforms to "ontylog.dtd" from 10/19/01
//
// Dependencies:	STL <string> class
// Virtual:			~OntyxRoleEnum ()
//
//*******************************************************************


#ifndef _ROLE_ENUMS_H
#define _ROLE_ENUMS_H

#include <string>
using namespace std;


// "atleast", "atmost", and "the" role modifiers are not used by ontylog
enum rolemodifier {modifier_low_bound = -1, 
					some, all, poss,
					modifier_high_bound, 
					num_of_modifiers = 3};

class OntyxRoleEnum
{
	private:

		rolemodifier modifier;

		static string tmpStr;
		static const char * modifierNames[];
		// order of enum strings above is identical to enumeration order


			// ********** Private Utility Functions 

				// ********** Validation 
		bool isValid (const string &) const;		// to check string validity prior to input

				// ********** String -> enum Conversion
		rolemodifier convertToModifier(const string &);


	public:

			// ********** Constructor/Destructor

		OntyxRoleEnum (const string & inRole = "some");
		OntyxRoleEnum (const OntyxRoleEnum &);

		virtual ~OntyxRoleEnum ();

			// ********** Accessor Functions

		rolemodifier getModifier (void) const;
		const string & getModifier (char) const;	// takes any char to provide a distinct signature

			// ********** Relational Operators 

		bool operator== (const OntyxRoleEnum &) const;
		bool operator< (const OntyxRoleEnum &) const;

};
#endif


