
#include "GoAnnotation.h"
#include "tString.h"
#include <set>
#include <string>
#include <vector>
#include "GoAnnotationConst.h"

using namespace std;

set<string> GoAnnotation::s_evidenceSet;
set<string> GoAnnotation::s_sourceSet;
string GoAnnotation::s_delim = "|";
unsigned GoAnnotation::s_maxField = 5;				// number of fields in goAnnotation

GoAnnotation::GoAnnotation( const string & inString)
{
	if( s_evidenceSet.size() == 0 )
		initialize();

	m_normalized = normalizeGoAnnotation(inString);
	
	vector<string> lTerms = extractFromDelim(m_normalized);	// make it explicit that input is a normalized full_syn
	m_term = lTerms[0];
	m_type = tString::toUpper(lTerms[1]);
	m_source = tString::toUpper(lTerms[2]);
	m_code = lTerms[3];
}

GoAnnotation::~GoAnnotation() {}

void GoAnnotation::setTerm( const string & term) { m_term = term; }
void GoAnnotation::setType( const string & type) { m_type = type; }
void GoAnnotation::setSource( const string & source) { m_source = source; }
void GoAnnotation::setCode( const string & code) { m_code = code; }

const string & GoAnnotation::getTerm(void) const { return m_term; }
const string & GoAnnotation::getType(void) const { return m_type; }
const string & GoAnnotation::getSource(void) const { return m_source; }
const string & GoAnnotation::getCode(void) const { return m_code; }

void GoAnnotation::initialize(void)
{
	unsigned ssize = (sizeof GoAnnotationConst::evidenceCode)/sizeof(string);
	for( unsigned i = 0; i < ssize ; ++i ) 
		s_evidenceSet.insert(tString::toUpper(GoAnnotationConst::evidenceCode[i]));
	ssize = (sizeof GoAnnotationConst::sources)/sizeof(string);
	for( unsigned i = 0; i < ssize ; ++i )
		s_sourceSet.insert(tString::toUpper(GoAnnotationConst::sources[i]));

}

bool GoAnnotation::setDelimiter(const string & delim)
{
	if( delim.size() > 0 ) {
		cerr << "Warning:  delimiter not set, remains \"" << s_delim << "\"." << endl;
		return false;
		}
	else
		s_delim = delim;
	return true;
}
bool GoAnnotation::isValid(void) const 
{
	static bool fsState = false;
	if( countFields() < s_maxField )
		fsState = true;
	else
		fsState = false;

	if( s_evidenceSet.count( tString::toUpper(getType()) ) != 0 
		&& s_sourceSet.count( tString::toUpper(getSource()) ) != 0 )
		fsState = true;
	else 
		fsState = false;

	return fsState;
}

unsigned GoAnnotation::countFields(void) const
{
	unsigned count = 0, i = 0;
	for( ; i < m_normalized.size(); ++i ) {
		if( m_normalized[i] == s_delim[0] )
			count++;
		}
	return count;
}

vector<string> GoAnnotation::extractFromDelim (const string & inString)	// 
{
	unsigned i = 0;
	vector<string> fSynVec(s_maxField, "");
	tString tTmpString = inString;
	tTmpString.tokenize(s_delim);
	while( tTmpString.nextToken()&& i < s_maxField ) {
		fSynVec[i] = tTmpString.getToken();
		i++;
		}
	return fSynVec;
}

string GoAnnotation::convertXML(void) const
{
	string tmpString;

	tmpString = "<" + GoAnnotationConst::termTag + ">" + m_term + "</" + GoAnnotationConst::termTag + ">";
	if( m_type != "" )
		tmpString += "<" + GoAnnotationConst::typeTag + ">" + m_type + "</" + GoAnnotationConst::typeTag + ">";
	if( m_source != "" )
		tmpString += "<" + GoAnnotationConst::sourceTag + ">" + m_source + "</" + GoAnnotationConst::sourceTag + ">";
	if( m_code != "" )
		tmpString += "<" + GoAnnotationConst::codeTag + ">" + m_code + "</" + GoAnnotationConst::codeTag + ">";

	return tmpString;
}

string GoAnnotation::normalizeGoAnnotation(const string & inString)
{
	string tmpString = inString, tmpS;

	// clean up pipe-terminated strings
	while( tmpString[tmpString.size()-1] == s_delim[0] ) {
		tmpString.erase(tmpString.size() - 1);
		}

	// check for empty fields	
	unsigned pos = 0, dpos = 0, insertType = 0, insertSource = 0, firstDelim = 0, secondDelim = 0 ;
	if( tmpString.find(s_delim) != string::npos ) {
		string emptyField = s_delim + s_delim;
		if( tmpString.find(emptyField) != string::npos )	{	// pipe-delim'd empty fields found
			firstDelim = tmpString.find(s_delim, firstDelim); 
			secondDelim = tmpString.find(s_delim, firstDelim+1); 
			pos = tmpString.find(emptyField, pos);
			dpos = tmpString.find(emptyField, pos+1);

			if( firstDelim == pos )
				insertType = firstDelim+1;
			if( secondDelim == pos || secondDelim == dpos )
				insertSource = secondDelim+1;

			if( insertSource != 0 )
				tmpString.insert(insertSource, GoAnnotationConst::defaultSource);
			if( insertType != 0 )
				tmpString.insert(insertType, GoAnnotationConst::defaultType);
			}
		}
	// count number of fields & set default to implicit fields:  goID|goTerm|evi|source|date
	// uses tmpS
	// uses tmpString as modified
	int count = countFields();
	switch(count) {
		case 0:			// has only term, add type and source by fall-through
			tmpS = s_delim + GoAnnotationConst::defaultType;
			tmpString.append(tmpS);
		case 1:			// has term and type, add source
			tmpS = s_delim + GoAnnotationConst::defaultSource;
			tmpString.append(tmpS);
		case 2:			// has term, type, & source (code is unspecified), add nothing
			break;
		case 3:			// all fields detailed
			break;
		default:		// number of pipes exceeds allowed
			cerr << "Invalid full_syn found during normalization:  \n\t" << inString << endl;
		}

	return tmpString;
}

