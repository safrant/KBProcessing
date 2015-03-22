// OBO2TDE2.h : header file


#ifndef _OBO2TDE2_
#define _OBO2TDE2_


#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <set>
#include <algorithm>
#include <ctime>

#include "OntyxKb.h"
#include "tString.h"
#include "fullSyn.h"

using namespace std;

class OBO2TDE2 {
private:
	string	m_input;
	string	m_output;

public:
    void setKb(OntyxKb &kb);
    void ProcessData(string m_input, string m_output);
    void WriteConceptDef(FILE *gp, OntyxConcept &c);
    void OutputValue(FILE *gp, string value);
    void OutputValue(FILE *gp, string indentation, string value);

};  // end class definition

#endif

