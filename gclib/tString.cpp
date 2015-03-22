#include <string>
#include <cstring>			// C string functions, for Porters algorithm, external
#include <cctype>
#include "tString.h"

using namespace std;


tString::tString(void) : string()
{
	init();
}

tString::tString(const char * c)
	: string(c)
{
	init();
}

tString::tString(const char * c, string::size_type n)
	: string(c, n)
{
	init();
}

tString::tString(const string & s, string::size_type pos, string::size_type n)
	: string(s, pos, n)
{
	init();
}

tString::tString(string::size_type n, char c)
	: string(n , c)
{
	init();
}

template <class InputIterator>
	tString::tString(InputIterator first, InputIterator last)
	: string(first, last)
{
	init();
}

tString::~tString() {}

tString & tString::operator= (const tString & inRight)
{
	if( this != &inRight) {
		string::operator= (inRight);
		m_tokenized = inRight.m_tokenized;
		m_tokens = inRight.m_tokens;
		m_posToken = inRight.m_posToken;
		}
	return *this;
}

void tString::init(void)
{
	m_tokenized = false;
	m_posToken = m_tokens.begin();
}
string tString::stemW(const string & inString)  // wrapper for porter's "stem" function
{
	unsigned new_end = 1;
	char strArr[1024];
	string retStr;

	strcpy( strArr, inString.c_str());
	if( strlen(strArr) > 4 ) {
		new_end = stem( strArr, 0, strlen(strArr) - 1 );
		strArr[new_end + 1] = '\0';
		}
	return retStr.assign(strArr);
}


string tString::getBaseString(void)
{
	return static_cast<string> (*this);
}


void tString::tokenize(void)
{
	string delimiters = "| ,-(){}[]*&@\\\t/.:;!?_\"'";
	tokenize(delimiters);
}

void tString::tokenize(const string & delimiters)
{
	unsigned pos = 0, endPos = 0;
	string localString = getBaseString();
	bool endProc = false;

	// need to test whether a delimiter is found at the end of the string,
	// otherwise, the complete string gets added as a "token", variation
	// from the condition where the string does not have delimiters
	// not yet done...

	while( !endProc ) {
		pos = localString.find_first_not_of( delimiters, endPos );
		if( pos == string::npos ) {
			m_tokens.push_back(localString);
			break;
			}
		endPos = localString.find_first_of( delimiters, pos );
		if( endPos != string::npos) {
			m_tokens.push_back( localString.substr( pos, endPos - pos ) );
			}
		else if( localString.size() != 0 ) {	// last token
			m_tokens.push_back( localString.substr( pos, endPos - pos ) ); 
			endProc = true;
			}
		}
	m_posToken = m_tokens.begin();
	m_tokenized = true;
}

int tString::getNumberOfTokens(void)
{
	return m_tokens.size();
}

tString tString::getTokenNumber(unsigned pos)
{
	if( pos < m_tokens.size() && pos >= 0 )
		return m_tokens[pos];
	else
		return "";
}

bool tString::nextToken(void)
{
	if( !m_tokenized )
		tokenize();
	if( m_posToken != m_tokens.end() )
		return true;
	return false;
}
bool tString::previousToken(void)
{
	if( !m_tokenized )
		tokenize();
	if( m_posToken == m_tokens.begin() )
		return false;
	return true;
}
string tString::getToken(void)
{
	if( !m_tokenized )
		tokenize();
	string tmpString = *m_posToken;
	++m_posToken;
	return tmpString;
}
string tString::getTokenStem(void)
{
	if( !m_tokenized )
		tokenize();
	return stemW(*m_posToken);
}

string tString::getTokenSequence(void)
{
	if( !m_tokenized )
		tokenize();
	m_posToken = m_tokens.begin();
	string retStr = "";

	if( m_tokens.size() > 0 ) {
		retStr = *m_posToken++;
		for( ; m_posToken != m_tokens.end(); ++m_posToken )
//			if( *m_posToken != "of" && *m_posToken != "the" && *m_posToken != "and" && *m_posToken == "or" )
				retStr.append(" " + *m_posToken);
		}
	return retStr;
}
string tString::getAlphaTokenSequence(void)
{
	if( !m_tokenized )
		tokenize();
	string retStr = "";
	multiset<string> inSet;
	fillTokenSet(inSet);
	if( inSet.size() > 0 ) {
		set<string>::iterator posSet = inSet.begin();
		retStr = *posSet;
		++posSet;
		for( ; posSet != inSet.end(); ++posSet ) {
			retStr.append(" " + *posSet);
			}
		}
	return retStr;
}
void tString::fillTokenSet(multiset<string> & inSet) 
{
	if( !m_tokenized )
		tokenize();
	m_posToken = m_tokens.begin();
	for( ; m_posToken != m_tokens.end(); ++m_posToken ) {
//		if( *m_posToken != "of" && *m_posToken != "the" && *m_posToken != "and" && *m_posToken == "or" )
			inSet.insert(*m_posToken);
		}
}
void tString::fillTokenSet(multiset<tString> & inSet) 
{
	if( !m_tokenized )
		tokenize();
	m_posToken = m_tokens.begin();
	for( ; m_posToken != m_tokens.end(); ++m_posToken ) {
//		if( *m_posToken != "of" && *m_posToken != "the" && *m_posToken != "and" && *m_posToken == "or" )
			inSet.insert(*m_posToken);
		}
}


string tString::getStemSequence(void)
{
	if( !m_tokenized )
		tokenize();
	m_posToken = m_tokens.begin();
	string retStr = "";

	if( m_tokens.size() > 0 ) {
		retStr = stemW(*m_posToken++);
		for( ; m_posToken != m_tokens.end(); ++m_posToken )
			retStr.append(" " + stemW(*m_posToken));
		}
	return retStr;
	
}
string tString::getAlphaStemSequence(void)
{
	if( !m_tokenized )
		tokenize();
	string retStr = "";
	multiset<string> inSet;
	fillStemSet(inSet);
	if( inSet.size() > 0 ) {
		set<string>::iterator posSet = inSet.begin();
		retStr = *posSet;
		++posSet;
		for( ; posSet != inSet.end(); ++posSet ) {
			retStr.append(" " + *posSet);
			}
		}
	return retStr;
}

void tString::fillStemSet(multiset<string> & inSet)
{
	if( !m_tokenized )
		tokenize();
	m_posToken = m_tokens.begin();
	for( ; m_posToken != m_tokens.end(); ++m_posToken ) {
		if( *m_posToken != "of" && *m_posToken != "the" && *m_posToken != "and" && *m_posToken != "or" ) {
			inSet.insert(stemW(*m_posToken));
			}
		}
}

tString tString::toLower(void)			// returns a tString
{
	unsigned i = 0;
	tString tmpString = getBaseString();
	while( i < tmpString.size() ) {
		tmpString[i] = tolower(tmpString[i]);
		i++;
	}
	return tmpString;
}
string tString::toLower(char c)			// change of signature, returns a string
{
	unsigned i = 0;
	string tmpString = getBaseString();
	while( i < tmpString.size() ) {
		tmpString[i] = tolower(tmpString[i]);
		i++;
	}
	return tmpString;
}
string tString::toLower(const string & inString)
{
	unsigned i = 0;
	string tmpString = inString;
	while( i < tmpString.size() ) {
		tmpString[i] = tolower(tmpString[i]);
		i++;
	}
	return tmpString;
}
string tString::toUpper(const string & inString)
{
	unsigned i = 0;
	string tmpString = inString;
	while( i < tmpString.size() ) {
		tmpString[i] = toupper(tmpString[i]);
		i++;
	}
	return tmpString;
}

bool tString::isTitleCase(void) const
{
	unsigned i = 0;
	if( !isupper((static_cast<string> (*this))[0]) )
		return false;
	i++;
	for( ; i < size(); ++i) {
		if( !islower((static_cast<string> (*this))[i]) )
			return false;
		}
	return true;
}
bool tString::isAllUpperCase(void) const
{
	unsigned i = 0;
	for( ; i < size(); ++i) {
		if( !isupper((static_cast<string> (*this))[i]) )
			return false;
		}
	return true;
}
bool tString::isAllLowerCase(void) const
{
	unsigned i = 0;
	for( ; i < size(); ++i) {
		if( !islower((static_cast<string> (*this))[i]) )
			return false;
		}
	return true;
}
sentenceCase tString::getSentenceCase(void)
{
	multiset<tString> allTokens;
	fillTokenSet(allTokens);
	multiset<tString>::const_iterator pTok;
	bool titleFlag = true,
		upperFlag = true,
		lowerFlag = true;
	for( pTok = allTokens.begin(); pTok != allTokens.end(); ++pTok) {
		if( !(*pTok).isTitleCase() )
			titleFlag = false;
		}
	for( pTok = allTokens.begin(); pTok != allTokens.end(); ++pTok) {
		if( !(*pTok).isAllUpperCase() )
			upperFlag = false;
		}
	for( pTok = allTokens.begin(); pTok != allTokens.end(); ++pTok) {
		if( !(*pTok).isAllLowerCase() )
			lowerFlag = false;
		}
	if( titleFlag )
		return titleCase;
	else if( upperFlag )
		return allUpper;
	else if( lowerFlag )
		return allLower;
	return mixedCase;
}

int tString::compareCase(tString inStrg, const map<sentenceCase, int> & caseOrder)
// returns -1, 0, 1 on comparison based on the **weight** handed in the sentenceCase array
// if 1, my case > other's case
{
	map<sentenceCase, int>::const_iterator pCaseOrder; 
	sentenceCase locCase = getSentenceCase();
	pCaseOrder = caseOrder.find(locCase); 
	int myLocation = pCaseOrder->second;	// get sentenceCase for *this, & its order
	locCase = inStrg.getSentenceCase();
	pCaseOrder = caseOrder.find(locCase);
	int inLocation = pCaseOrder->second;	// as well as for the input tString
	return (myLocation == inLocation ? 0 : (myLocation > inLocation ? 1 : -1));
}



bool tString::operator== (const tString & inTstr) const
{
	return static_cast<string> (*this) == static_cast<string> (inTstr);
}
bool tString::operator< (const tString & inTstr) const
{
	return static_cast<string> (*this) < static_cast<string> (inTstr);
}



