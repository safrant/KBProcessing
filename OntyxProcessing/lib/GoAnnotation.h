/*
	use read go_annotation pipe-delimited object, validate, output log if not, convert to xml
*/


#ifndef _GO_ANNOT_
#define _GO_ANNOT_


#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <set>
#include <algorithm>



using namespace std;

class GoAnnotation {
private:

	string m_id;
	string m_term;
	string m_evidence;
	string m_source;
	string m_date;
	string m_normalized;

	// CLASS-WIDE STATIC STUFF, SETS OF VALID SOURCES AND TERM TYPES OBTAINED FROM GoAnnotationConst CLASS
	static set<string> s_evidenceSet;
	static set<string> s_sourceSet;
	static string s_delim;
	static unsigned s_maxField;

	static void initialize(void);

	vector<string> extractFromDelim (const string & inString);
	string normalizeGoAnnotation(const string & inString);
	unsigned countFields(void) const;


public:
	GoAnnotation( const string & inString);
	// reads the string, parses it if it has delimited fields, validates as GoAnnotation

	virtual ~GoAnnotation();

	void setID( const string & id);
	void setTerm( const string & term);
	void setEvidence( const string & evi);
	void setSource( const string & source);
	void setDate( const string & date);

	const string & getID(void) const;
	const string & getTerm(void) const;
	const string & getEvidence(void) const;
	const string & getSource(void) const;
	const string & getDate(void) const;

	bool isValid(void) const;				// returns true if sources and term types are valid
	string convertXML(void) const;

	static bool setDelimiter(const string & delim);


};  // end class definition
#endif



