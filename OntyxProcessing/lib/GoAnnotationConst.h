#ifndef _GO_ANNOT_CONST_
#define _GO_ANNOT_CONST_

#include <string>

using namespace std;

class GoAnnotationConst {

public:

	static string defaultSource;

	static string goIdTag;
	static string termTag;
	static string evidenceTag;
	static string sourceTag;
	static string dateTag;
	
	static string evidenceCode[];
	static string sources[];

	GoAnnotationConst(void) {}
	virtual ~GoAnnotationConst() {}

};

string GoAnnotationConst::defaultSource = "NCIEVS";

string GoAnnotationConst::goIdTag = "go-id";
string GoAnnotationConst::termTag = "go-term";
string GoAnnotationConst::evidenceTag = "go-evi";
string GoAnnotationConst::sourceTag = "go-source";
string GoAnnotationConst::dateTag = "source-date";

// ENTER TERM eviCode AND SOURCES IN UPPER CASE
string GoAnnotationConst::evidenceCode[] = {"IC", "IDA", "IEA", "IEP", "IGI", "IMP",
									"IPI", "ISS", "NAS", "ND", "TAS", "NR" };
string GoAnnotationConst::sources[] = { "NCIEVS", "CGAP" };


#endif



