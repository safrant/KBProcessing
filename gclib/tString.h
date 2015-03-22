#ifndef _T_STRING_H_
#define _T_STRING_H_

#include <string>
#include <set>
#include <vector>
#include <map>
#include <cstring>			// C string functions, for Porters algorithm, external
#include <cctype>

using namespace std;

enum sentenceCase {allLower, allUpper, titleCase, mixedCase};

extern "C" 
{
	int stem(char * p, int i, int j);
}

class tString : public string 
{
private:
	bool m_tokenized;
	mutable vector<string> m_tokens;
	mutable vector<string>::iterator m_posToken;

	void init(void);
	string stemW(const string &);  // wrapper for porter's "stem" function

public:

	tString(void);
	tString(const char * c);
	tString(const char * c, string::size_type n);
	tString(const string & s, string::size_type pos = 0, string::size_type n = string::npos);
	tString(string::size_type n, char c);
	template <class InputIterator>
		tString(InputIterator first, InputIterator last);

	virtual ~tString();

	tString & operator= (const tString &);

	string getBaseString(void);

	void tokenize(void);
	void tokenize(const string & delimiters);

	int getNumberOfTokens(void);
	tString getTokenNumber(unsigned);			// returns empty string if range is not valid
	bool nextToken(void);
	bool previousToken(void);
	string getToken(void);
	string getTokenStem(void);
	string getTokenSequence(void);
	string getAlphaTokenSequence(void);
	void fillTokenSet(multiset<string> & );
	void fillTokenSet(multiset<tString> & );

	string getStemSequence(void);
	string getAlphaStemSequence(void);
	void fillStemSet(multiset<string> & );

	tString toLower(void) ;	
	string toLower(char) ;	//change of signature, any char will do
	static string toLower(const string & inString);
	static string toUpper(const string & inString);

	static string trim(const string & inString); // hasn't been implemented

	bool isTitleCase(void) const;			// applied to token
	bool isAllUpperCase(void) const;		// applied to token
	bool isAllLowerCase(void) const;		// applied to token

	int compareCase(tString, const map<sentenceCase,int> &);
	// returns -1, 0, 1 on comparison based on the **weight** handed in the sentenceCase map
	// if 1, my case > other's case.  Weight, i.e. 4,3,2,1 for desired order 1,2,3,4

	sentenceCase getSentenceCase(void);		// applied to sentence

	bool operator== (const tString & inTstr) const;
	bool operator< (const tString & inTstr) const;

};

#endif
