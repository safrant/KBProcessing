//*******************************************************************
// Definition of Ontyx class OntyxTerminology
// 		Defines Terminology constants of the Ontylog KB terminology
//
//		Initial class definition 12/17/2001
//
// Dependencies		<iostream>
// Dependencies		STL <string> class
// Dependencies		C <stdlib> for exit()
//
// Virtual:			~OntyxTerminology()
//
// Misc:		1	Provides for terminology-wide reference types:  by name, id, or code
//				2	Provides terminology-wide error codes
//				3	Exits if xml terminology file lacks the ref_by value
//
//*******************************************************************

#ifndef _ONTYX_TERMINOLOGY_DEF_
#define _ONTYX_TERMINOLOGY_DEF_

#include <string>
#include <cstdlib>
#include <iostream>

using namespace std;

enum ref_by {name = 0, id, code };

class OntyxTerminology {

	private:

		static ref_by reference;
		static const string refStrings[];

	public:

		static const long	CODE_EXISTS,					// error codes
							ID_EXISTS,
							NAME_EXISTS,
							NO_NAMESPACE,
							NO_KIND,
							NO_PROPDEF,
							NO_ROLEDEF,
							NO_CONCEPTDEF;

		static const long errorArray[];
		static const long numErrors;
		static const string errorNames[];

		static const string emptyString;
		static void setReference(string inRef);

		static const string & getReference(void);
		static const ref_by getReference(char c);		// takes any char for distinct signature

		OntyxTerminology(void);
		virtual ~OntyxTerminology();
};
#endif

