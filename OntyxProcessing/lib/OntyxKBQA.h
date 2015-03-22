#ifndef _ONTYX_KBQA_
#define _ONTYX_KBQA_

#include "OntyxKb.h"
#include <cstdlib>
#include <iostream>
#include <string>


#include "OntyxConcept.h"


using namespace std;

typedef map<string, string> str_map;
typedef multimap<string, string> str_multimap;

struct less_s
{
	bool operator () (const std::string x, const std::string y) const
	{
		//return (x.compare(y)); 
		if (x.compare(y) < 0) return true;
		return false;
	}
}; 


typedef std::multimap <std::string, std::string, less_s> string_mmap;


class vec_wrapper
{
public:
	vector<string> v;
};

typedef map<string, vec_wrapper* > str2vec_map;


class OntyxKBQA
{
private: 
    string XML2Pipe(string pattern);
	map<long, OntyxConcept>::iterator begin_it;
	map<long, OntyxConcept>::iterator end_it;


public:
    OntyxKb kb;
	FILE *gp;

	OntyxKBQA(void);
	//OntyxKBQA(OntyxKb &kb);

	OntyxKBQA(OntyxKb &kb);

	~OntyxKBQA();

	void setKB(OntyxKb &kb);

	void performQA(string infile);
	void checkSamePTs();
	void checkNCIPTFullSynName();
	void checkDuplicateRoles();
	void checkDuplicateProperties();
	void checkPTExistenceAndUniqueness();
	//void checkNCIPTFullSynExistence(); // for review
    //void checkNCIPTFullSynUniqueness();
	void checkSemanticTypes(string filename);
	void checkSameAtoms();
	void checkHighBitCharacters();
	void checkCharacter(char c); //CarriageReturn, @, |

	void getAllConceptNames();
    vector<string> getPropertyValue(OntyxConcept &c, string propertyname);
	int checkHighBitCharacters(string name, 
		                        string propertyname,
		                        string propertyvalue);

    vector<string> tokenize(string pattern0); 
    void checkNCIPTFullSynExistenceAndUniqueness();

};
#endif


