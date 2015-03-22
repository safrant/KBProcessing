#ifndef _FULL_SYN_CONST_
#define _FULL_SYN_CONST_

#include <string>

using namespace std;

class fullSynConst {

public:
	static string defaultType;
	static string defaultSource;

	static string termTag;
	static string typeTag;
	static string sourceTag;
	static string codeTag;
	
	static string types[];
	static string sources[];

	fullSynConst(void) {}
	virtual ~fullSynConst() {}

};


string fullSynConst::defaultType = "SY";
string fullSynConst::defaultSource = "NCI";

string fullSynConst::termTag = "term-name";
string fullSynConst::typeTag = "term-group";
string fullSynConst::sourceTag = "term-source";
string fullSynConst::codeTag = "source-code";

// ENTER TERM TYPES AND SOURCES IN UPPER CASE
string fullSynConst::types[] = {"PT", "SY", "AB", "CI", "CU", "CS", "AQ", "HV",
								"HD", "BR", "FB", "CN", "SN", "DN", "AD" };
string fullSynConst::sources[] = { "NCI", "NCI-GLOSS", "RAEB-1", "RAEB-2", "CADSR", 
								"DTP", "CTRM", "JAX", "DCP", "BioCarta", "KEGG", 
								"CABIO", "MMHCC", "FDA", "NCICB", "SEER", "CDC", "CDISC" };


#endif



