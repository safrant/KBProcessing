// haven't done anything yet,
// what I want to do is strip any character at the binary level, targets are
// control characters
//

#include <fstream>
#include <iomanip>
#include <vector>
#include <string>
#include <cstdlib>				// exit()
using namespace std;


void usage(void);

int main(int argc, char *argv[])
{
	vector<string> commLine;
	for (int i = 0; i < argc; i++)
		commLine.push_back(*argv++);
	if (commLine.size() != 2) {
		usage();
		exit(0);
		}
	ifstream inputFile;
	inputFile.open(commLine[1].c_str(), ios::binary | ios::in);
	if (inputFile.bad()) {
		cerr << "Can't open \"" << commLine[1].c_str() << ".\"  Will EXIT." << endl;
		exit(0);
		}

	char tmpChar;
	long filePos = 0;
	long lineNumber = 1;
	short mask = 0x00ff;
	vector<char> mStrbuffer;
	while( !inputFile.eof() ) {
		inputFile.get(tmpChar);
		filePos++;
		if( tmpChar < 32 || tmpChar > 126 ) {
			if( tmpChar != 10 && tmpChar != 13) {
				if( tmpChar != 9 ) {
					cout << "Found char # " << setiosflags (ios::showbase)
						<< hex << ((static_cast<unsigned short> (tmpChar)) & mask) ;
					cout << " at byte " << resetiosflags (ios::showbase)
						<< dec << filePos << ", at line " << lineNumber << endl;
					cout << '\t';
					for(unsigned i = 0; i < mStrbuffer.size(); i++)
						cout << mStrbuffer[i];
					cout << endl << endl;
					}
				}
			else {
				mStrbuffer.clear();
				if( tmpChar == 13 || tmpChar == 10)
					lineNumber++;
				}
			}
		else
			mStrbuffer.push_back(tmpChar);
		}
	cout << "Read " << filePos << " bytes" << endl;
	inputFile.close();
	return 0;
}

// *****************************************************************
void usage (void)
{
	cerr << "Usage:" << endl << "findbinary file" << endl;
}
