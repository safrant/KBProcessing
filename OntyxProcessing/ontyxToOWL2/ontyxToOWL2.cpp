

#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <set>
#include <algorithm>
#include <ctime>

#include "OntyxKb.h"
#include "OntyxProperty.h"
#include "CommandLine.h"

using namespace std;

string getDayStamp (void);
void showUsage(string prog)
{	cerr << prog << " version compiled on " << __DATE__ << endl;
	cerr << endl << "Usage:" << endl
		<< "\t" << prog << " [-h|H] [-n \"namespace\"] [-v version] input-tde-file.xml" << endl

		<< endl << "where:" << endl
		<< "\t" << "h or H show this message" << endl
		<< "\t" << "n flags a URI namespace, also used for the xml:base" << endl
		<< "\t" << "v flags version" << endl << endl
		<< "if v or n are not specified, default namespace and version are used." << endl;
	return; 
}


int main (int argc, char * argv[])
{
	OntyxKb tdeKb;		// input/output kb
	clock_t tstart, tend;

	// check command line
	CommandLine cmd(argc, argv);
	if( cmd.hasOption('h') || cmd.hasOption('H') ) {
		showUsage(cmd.getProgramName());
		exit(0);
		}
	string inputfilename = "";
	if( cmd.hasArguments() )
		inputfilename = cmd.getVariableByPosition(1);
	else {
		showUsage(cmd.getProgramName());
		exit(0);
		}

	// use the namespace for the xml:base as well
	string locnamespace;
	if( cmd.hasOption('n') )
		locnamespace = cmd.getFlaggedVariable('n');
	else
		locnamespace = "http://evs.nci.nih.gov/ftp1/NDF-RT/NDF-RT.owl#";
	cerr << "Using namespace:  " << locnamespace << endl;
		
	string version;
	if( cmd.hasOption('v') )
		version = cmd.getFlaggedVariable('v');
	else
		version = "NDF-RT2 [Public Edition]";
	cerr << "Using version:  " << version << endl;
	
	tstart = clock();

	cerr << "Reading file " << inputfilename << endl;
	tdeKb.readOntyxXMLFile( inputfilename );


	string daystamp = getDayStamp();
	string filename = inputfilename + "-ByNUI-" + daystamp + ".owl";

	cerr << "Writing output OWL file " << filename << endl;
	vector<string> header;
	header.push_back("<rdf:RDF ");
	header.push_back("	xmlns=\"" + locnamespace + "\"");
	header.push_back("	xml:base=\"" + locnamespace + "\"");
	header.push_back("	xmlns:protege=\"http://protege.stanford.edu/plugins/owl/protege#\"");
	header.push_back("	xmlns:owl=\"http://www.w3.org/2002/07/owl#\"");
	header.push_back("	xmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\"");
	header.push_back("	xmlns:rdfs=\"http://www.w3.org/2000/01/rdf-schema#\">");
	header.push_back("<owl:Ontology rdf:about=\"\">");
	header.push_back("<owl:imports rdf:resource=\"http://protege.stanford.edu/plugins/owl/protege\"/>");
	header.push_back("<rdfs:comment>NDF-RT2 Public</rdfs:comment>");
	header.push_back("<owl:versionInfo>" + version + "</owl:versionInfo>");
	header.push_back("</owl:Ontology>") ;
	
	tdeKb.writeNDRFT2OWLFile (filename, header, locnamespace); 


	tend = clock();
	cerr << "Execution took " << (tend-tstart)/1000.0 << " seconds" << endl;

	return 0;
}


string getDayStamp (void)
{
	char sumFile[10] = {0};
	struct tm when;
	time_t now;   
	char *p;
	char tempS[20] = {0}, monS[3] = {0}, dayS[3] = {0}, yearS[5] = {0};

	time( &now );
	when = *localtime( &now );
	sprintf (yearS, "%d", when.tm_year);	// years since 1900
	if (strlen(yearS) > 2) {		// use only last two digits of year
		p = &yearS[strlen(yearS)-2];
		strcpy(tempS, p);
		strcpy(yearS, tempS);
		}
	sprintf (monS, "%d", when.tm_mon + 1);
	sprintf (dayS, "%d", when.tm_mday);
	if (strlen(monS) == 1) {		// use month with two digits
		strcpy(tempS, "0");
		strcat(tempS, monS);
		strcpy(monS, tempS);
		}
	if (strlen(dayS) == 1) {		// use days with two digits
		strcpy(tempS, "0");
		strcat(tempS, dayS);
		strcpy(dayS, tempS);
		}
	strcat(sumFile, yearS);
	strcat(sumFile, monS);
	strcat(sumFile, dayS);
	string stamp = sumFile;

	return stamp;
}



