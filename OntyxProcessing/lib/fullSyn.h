/*
	use read full_syn pipe-delimited object, validate, output log if not, convert to xml
*/


#ifndef _FULL_SYN_
#define _FULL_SYN_


#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <set>
#include <algorithm>



using namespace std;

class fullSyn {
private:

	string m_term;
	string m_type;
	string m_source;
	string m_code;
	string m_normalized;

	// CLASS-WIDE STATIC STUFF, SETS OF VALID SOURCES AND TERM TYPES OBTAINED FROM fullSynConst CLASS
	static set<string> s_typeSet;
	static set<string> s_sourceSet;
	static string s_delim;
	static unsigned s_maxField;

	static void initialize(void);

	vector<string> extractFromDelim (const string & inString);
	string normalizeFullSyn(const string & inString);
	unsigned countFields(void) const;						// acts on m_normalized
	unsigned countFields(const string & inString) const; 	// acts on input string


public:
	fullSyn( const string & inString);
	// reads the string, parses it if it has delimited fields, validates as fullSyn

	virtual ~fullSyn();

	void setTerm( const string & term);
	void setType( const string & type);
	void setSource( const string & source);
	void setCode( const string & code);

	const string & getTerm(void) const;
	const string & getType(void) const;
	const string & getSource(void) const;
	const string & getCode(void) const;
	bool hasCode() const;

	bool isValid(void) const;				// returns true if sources and term types are valid
	string convertXML(void) const;

	static bool setDelimiter(const string & delim);


};  // end class definition
#endif



