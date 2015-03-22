// OBO2TDE.h : header file
//

#ifndef _OBO2TDE_
#define _OBO2TDE_

#include <string>
#include <list>
#include <vector>
#include <map> 

// from findbinary
#include <fstream>
#include <iomanip>
#include <cstdlib>				// exit()

#include <iostream>  
using namespace std;


typedef map<string, string> str_map;

/////////////////////////////////////////////////////////////////////////////








class Qualifier
{
public:
	string			name;
	string			value;
};

class Property
{
public:
	string			name;
	string			value;
	vector<Qualifier *> qualifier_vec;
};


class ConceptDef
{
public:
    string name;
    string code;
    string id;
    string namespaceName;
    bool isPrimitive;
    string kind;
    vector<string> sup_vec;
    vector<string> role_modifier_vec;
    vector<string> rolename_vec;
    vector<string> rolevalue_vec;
    vector<Property *> property_vec;
};

typedef vector<Qualifier *> qualfier_vec;

typedef map<string, qualfier_vec> propertyqualifier_map;

typedef vector<ConceptDef *> concept_vec;


class OBO2TDE
{
// Construction
	/*
public:
    str_map id2name;

	str_map propertyname_tbl;
    str_map conceptname_tbl;
    str_map tagheader_tbl;

    concept_vec concepts;

	propertyqualifier_map propertyqualifier_tbl;

	unsigned int concept_knt;

	vector<string> namespace_vec;
	str_map namespace_tbl;

	string	m_input;
	string	m_output;
	string	m_template;
	string	m_tagheader;

*/

// Implementation
public:
    int ReverseFind(string s, char c);
    string replaceChar(string &s, char c1, char c2);
    string replaceAmp( string &s );
	int ReverseFind(const string &str, char c); 
    string TrimRight(string &s);
    string TrimLeft(string &s);
    string LSubString (const string &s, int n);
    string RSubString (const string &s, int n);

	Property *CreateProperty(string name, string value); 
    Qualifier *CreateQualifier(string name, string value); 

	void LoadTagHeaders(string infile); 
	void ClearMemory();
    //void Convert(string infile, string templatefile, string outfile); 
void Convert(string infile, string templatefile, string m_tagheader, string outfile); 

    string GetValue(string line, string tag); 
    bool StartWith(string line, string tag); 
    string GetLongValue(FILE *fp, string line, string tag); 
    bool EndWith(string line, string tag); 
    string GetDefinitionSource(string line); 
    string GetDefinitionText(string line); 
    void PrintLine(FILE *gp, string name, string value); 
    string TrimLeftChar(string s, char c);
    string TrimRightChar(string s, char c);
    void AddCodeToMap(string id, string name);
    string GetName(string id);
	void PrintConceptDefOpenTag(FILE *gp);
    void PrintConceptDefCloseTag(FILE *gp);

    void PrintIdentifiers(FILE *gp,
						  string name,
						  string code,
						  string namespaceName,
                          bool isPrimitive,
                          string kind
						  );

	void PrintDefiningConcepts(FILE *gp, vector<string> &sups);
    void PrintDefiningRoles(FILE *gp, vector<string> &modifier_vec,
							 vector<string> &rolename_vec,
							 vector<string> &rolevalue_vec);

	void PrintConceptDef(FILE *gp, 
						  string name,
						  string code,
						  string id,
						  string namespaceName,
                          bool isPrimitive,
                          string kind,
						  vector<string> &sups,
		                  vector<string> &role_modifier_vec,
						  vector<string> &rolename_vec,
						  vector<string> &rolevalue_vec,
						  //vector<string> &propertyname_vec,
						  //vector<string> &propertyvalue_vec,
						  vector<Property *> property_vec);

	void PrintProperty(FILE *gp, 
	                   	  bool isComplex,
						  string name,
						  string value);

	void InitializePropertyNameTable();
	string FindPropertyName(string name);
    string GetConceptName(string name);
    string TrimTerminatingNonAlphaNumChars(string s);
    string TrimStartingNonAlphaNumChars(string s);

    void PrintComplexProperty(FILE *gp,
		                       string propertyname, 
							   vector<string> &propertyvaluetag_vec,
							   vector<string> &propertyvalue_vec);

    void AddNamespace(string namespaceNm);
    void PrintObsoleteConcepts(FILE *gp);
	void PrintObsoleteConcept(FILE *gp, 
									   string name, 
									   string id
									   );

	bool AddTagHeader(string tag, string value);
    string GetTagHeaderValue(string tag);
	bool IsDismissed(string tag);
    vector<string> ParseDbXrefString(string s);
    string RemoveEscapeCharacters(string s0);
    string RemoveCharacter(string s0, char c);

	void PrintProperties(FILE *gp, vector<Property *> &property_vec);
    vector<string> ParseDbXrefString(FILE *gp, string s0);
    void OutputValue(FILE *gp, string value);
    void OutputValue(FILE *gp, string indentation, string value);

    string GetValue(FILE *gp, string line, string tag); 
    void AddCodeToMap(FILE *gp, string id, string name);
    string GetName(FILE *gp, string id);

void SaveConceptDef(      FILE *gp,
					      string name,
						  string code,
						  string id,
						  string namespaceName,
                          bool isPrimitive,
                          string kind,
						  vector<string> &sups,
		                  vector<string> &role_modifier_vec,
						  vector<string> &rolename_vec,
						  vector<string> &rolevalue_vec,
						  vector<Property *> property_vec);
};

#endif 
