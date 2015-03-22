
#include "fullSyn.h"
#include "tString.h"
#include <set>
#include <string>
#include <vector>
#include "fullSynConst.h"

using namespace std;

set<string> fullSyn::s_typeSet;
set<string> fullSyn::s_sourceSet;
string fullSyn::s_delim = "|";
unsigned fullSyn::s_maxField = 4;				// number of fields in full_syn

fullSyn::fullSyn( const string & inString)
{
	if( s_typeSet.size() == 0 )
		initialize();

	m_normalized = normalizeFullSyn(inString);

	vector<string> lTerms = extractFromDelim(m_normalized);	// make it explicit that input is a normalized full_syn
	m_term = lTerms[0];
	m_type = tString::toUpper(lTerms[1]);
	m_source = tString::toUpper(lTerms[2]);
	m_code = lTerms[3];
}

fullSyn::~fullSyn() {}

void fullSyn::setTerm( const string & term) { m_term = term; }
void fullSyn::setType( const string & type) { m_type = type; }
void fullSyn::setSource( const string & source) { m_source = source; }
void fullSyn::setCode( const string & code) { m_code = code; }

const string & fullSyn::getTerm(void) const { return m_term; }
const string & fullSyn::getType(void) const { return m_type; }
const string & fullSyn::getSource(void) const { return m_source; }
const string & fullSyn::getCode(void) const { return m_code; }

void fullSyn::initialize(void)
{
	unsigned ssize = (sizeof fullSynConst::types)/sizeof(string);
	for( unsigned i = 0; i < ssize ; ++i ) 
		s_typeSet.insert(tString::toUpper(fullSynConst::types[i]));
	ssize = (sizeof fullSynConst::sources)/sizeof(string);
	for( unsigned i = 0; i < ssize ; ++i )
		s_sourceSet.insert(tString::toUpper(fullSynConst::sources[i]));

}

bool fullSyn::setDelimiter(const string & delim)
{
	if( delim.size() > 0 ) {
		cerr << "Warning:  delimiter not set, remains \"" << s_delim << "\"." << endl;
		return false;
		}
	else
		s_delim = delim;
	return true;
}
bool fullSyn::isValid(void) const 
{
	static bool fsState = false;
	if( countFields() < s_maxField )
		fsState = true;
	else
		fsState = false;

	if( s_typeSet.count( tString::toUpper(getType()) ) != 0 
		&& s_sourceSet.count( tString::toUpper(getSource()) ) != 0 )
		fsState = true;
	else 
		fsState = false;

	return fsState;
}

unsigned fullSyn::countFields(void) const
{
	unsigned count = 0, i = 0;
	for( ; i < m_normalized.size(); ++i ) {
		if( m_normalized[i] == s_delim[0] )
			count++;
		}
	return count;
}

unsigned fullSyn::countFields(const string & inString) const
{
	unsigned count = 0, i = 0;
	for( ; i < inString.size(); ++i ) {
		if( inString[i] == s_delim[0] )
			count++;
		}
	return count;
}

vector<string> fullSyn::extractFromDelim (const string & inString)	// 
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

bool fullSyn::hasCode() const
{
	if( m_code != "")
		return true;
	return false;
}

string fullSyn::convertXML(void) const
{
	string tmpString;

	tmpString = "<" + fullSynConst::termTag + ">" + m_term + "</" + fullSynConst::termTag + ">";
	if( m_type != "" )
		tmpString += "<" + fullSynConst::typeTag + ">" + m_type + "</" + fullSynConst::typeTag + ">";
	if( m_source != "" )
		tmpString += "<" + fullSynConst::sourceTag + ">" + m_source + "</" + fullSynConst::sourceTag + ">";
	if( m_code != "" )
		tmpString += "<" + fullSynConst::codeTag + ">" + m_code + "</" + fullSynConst::codeTag + ">";

	return tmpString;
}

string fullSyn::normalizeFullSyn(const string & inString)
{
	string tmpString = inString, tmpS;

	// clean up pipe-terminated full_syns
	while( tmpString[tmpString.size()-1] == s_delim[0] ) {
		tmpString.erase(tmpString.size() - 1);
		}

	// check for empty inner fields
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
				tmpString.insert(insertSource, fullSynConst::defaultSource);
			if( insertType != 0 )
				tmpString.insert(insertType, fullSynConst::defaultType);
			}
		}
	// check for trailing empty fields
	// count number of fields & set default to implicit fields:  term|type|source|code
	// uses tmpS
	// uses tmpString as modified
	int count = countFields(tmpString);
	switch(count) {
		case 0:			// has only term, add type and source by fall-through
			tmpS = s_delim + fullSynConst::defaultType;
			tmpString.append(tmpS);
		case 1:			// has term and type, add source
			tmpS = s_delim + fullSynConst::defaultSource;
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

