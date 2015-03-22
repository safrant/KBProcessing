

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <ctime>

#include "OntyxKb.h"

using namespace std;



	OntyxKb masterKb;

int main (int argc, char * argv[])
{
	clock_t tstart, tend;

	vector<string> commLine;
	for ( int i = 0; i < argc; i++)
		commLine.push_back(*argv++);

	if( commLine.size() != 2 ) {
		cerr << "Error:  incorrect number of command line parameters." << endl
			<< "\nUsage:\n\treportRoots file" << endl << endl;
		exit(0);
		}

	tstart = clock();

	masterKb.readOntyxXMLFile( commLine[1].c_str() );	cerr << "Read \"" << commLine[1].c_str() << "\"" << endl;

	ofstream outFile;
	outFile.open("roots.txt");
	if( !outFile.good() ) {
		cerr << "Error opening output roots.txt file, will EXIT." << endl;
		exit(0);
		}


	set<string> roots;
	masterKb.findRootConcepts(roots);

	set<string>::iterator pRoots = roots.begin();
	outFile << "Root concepts in " << commLine[1] << endl;
	for( ; pRoots != roots.end(); ++pRoots )
		outFile << *pRoots << endl;


	outFile.close();

	tend = clock();
	cerr << "Execution took " << (tend-tstart)/1000.0 << " seconds" << endl;

	return 0;
}



