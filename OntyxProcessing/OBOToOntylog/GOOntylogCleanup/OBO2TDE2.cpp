// OBO2TDE2Dlg.cpp : implementation file
//

#include "OBO2TDE2.h"
#include "OntyxKb.h"



void setKB(OntyxKb &kb)
{
    kb = kb;	
}



void OutputValue(FILE *gp, string value)
{
    int n1 = value.find_first_of('<');
    int n2 = value.find_first_of('&');
    if (n1 == -1 && n2 == -1)
	{
		fprintf(gp, "<value>%s</value>\n", (const char *) value.c_str());
	}
	else
	{
		fprintf(gp, "<value><![CDATA[%s]]></value>\n", (const char *) value.c_str());
	}
}


void OutputValue(FILE *gp, string indentation, string value)
{
    int n1 = value.find_first_of('<');
    int n2 = value.find_first_of('&');
    if (n1 == -1 && n2 == -1)
	{
		fprintf(gp, "%s<value>%s</value>\n", 
			(const char *) indentation.c_str(),
			(const char *) value.c_str());
	}
	else
	{
		fprintf(gp, "%s<value><![CDATA[%s]]></value>\n", 
			(const char *) indentation.c_str(),
			(const char *) value.c_str());
	}
}


void WriteConceptDef(FILE *gp, OntyxConcept &c)
{
	fprintf(gp, "\n");
    fprintf(gp, "<conceptDef>\n");
    fprintf(gp, "<name>%s</name>\n", (const char *) c.getName().c_str());
    fprintf(gp, "<code>%s</code>\n", (const char *) c.getCode().c_str());
    fprintf(gp, "<id>%u</id>\n", (const char *) c.getID());
    fprintf(gp, "<namespace>GO</namespace>\n", (const char *) c.getNamespace().c_str());
    bool isdefined = c.getState();
	if (isdefined)
	{
		fprintf(gp, "<defined/>\n");
	}
	else
	{
		fprintf(gp, "<primitive/>\n");
	}
    fprintf(gp, "<kind>%s</kind>\n", (const char *) c.getKind().c_str());

	fprintf(gp, "<definingConcepts>\n");
	set<string> parents = c.getParents();
	set<string>::iterator pPar = parents.begin();
	for( ; pPar != parents.end(); ++pPar) {
		string parent = *pPar;
        fprintf(gp, "<concept>%s</concept>\n", (const char *) parent.c_str());
	}
	fprintf(gp, "</definingConcepts>\n");


	fprintf(gp, "<definingRoles>\n");

	multimap<long, OntyxRole> roles = c.getRoles();
	multimap<long, OntyxRole >::const_iterator pRole = roles.begin();
	for( ; pRole != roles.end(); ++pRole)
	{
		OntyxRole r = (OntyxRole) pRole->second;
		string modifier = r.getModifier();
		string name = r.getName();
		string value = r.getValue();

		fprintf(gp, "<role>\n");
		fprintf(gp, "<%s/>\n", (const char *) modifier.c_str());
		fprintf(gp, "<name>%s</name>\n", (const char *) name.c_str());

		char b[2048];
		sprintf(b, "%s", (const char *) value.c_str());
		string val = b;
		OutputValue(gp, val);
		//fprintf(gp, "<value>%s</value>\n", (const char *) value.c_str());
		fprintf(gp, "</role>\n");
	}
	fprintf(gp, "</definingRoles>\n");

    fprintf(gp, "<properties>\n");

	multimap<long, OntyxProperty> properties = c.getProperties();

	multimap<long, OntyxProperty >::const_iterator pProp = properties.begin();
	for( ; pProp != properties.end(); ++pProp)
	{
		OntyxProperty p = (OntyxProperty) pProp->second;
		string name = p.getName();
		string value = p.getValue();
		fprintf(gp, "<property>\n");
		fprintf(gp, "<name>%s</name>\n", (const char *) name.c_str());
		char b[2048];
		sprintf(b, "%s", (const char *) value.c_str());
		string val = b;

		OutputValue(gp, val);
		//fprintf(gp, "<value>%s</value>\n", (const char *) value.c_str());
		// qualifiers
        vector<OntyxQualifier> qualifier_vec = p.getQualifiers();
		if (qualifier_vec.size() > 0)
		{
			fprintf(gp, "\t<qualifiers>\n");
			for (unsigned int i=0; i<qualifier_vec.size(); i++)
			{
				OntyxQualifier q = (OntyxQualifier) qualifier_vec[i];
				fprintf(gp, "\t<qualifier>\n");
				name = q.getName();
				value = q.getValue();
				fprintf(gp, "\t\t<name>%s</name>\n", (const char *) name.c_str());
				char b[2048];
				sprintf(b, "%s", (const char *) value.c_str());
				string val = b;

				OutputValue(gp, "\t\t", val);
				//fprintf(gp, "\t\t<value>%s</value>\n", (const char *) value.c_str());
				fprintf(gp, "\t</qualifier>\n");
			}
			fprintf(gp, "\t</qualifiers>\n");
		}
		fprintf(gp, "</property>\n");
    }
    fprintf(gp, "</properties>\n");
	fprintf(gp, "</conceptDef>\n");
}




void ProcessData(string m_input, string m_output)
{
	clock_t tstart, tend;
	OntyxKb kb;

	tstart = clock();

	string inputfile = m_input;
	kb.readOntyxXMLFile( inputfile );

char temp_buffer[225];
sprintf(temp_buffer, "c://test.xml");
string filename = temp_buffer;


kb.writeOntyxXMLFile (filename, true, false); 



    FILE *gp;
	if ((gp = fopen((const char *)m_output.c_str(), "w")) == NULL)
	{
		char buf[255];
		sprintf(buf, "Cannot open file %s", 
			(const char *) m_output.c_str());
		cerr << "Error opening file \"" << buf << "\", will EXIT." << endl;
		exit(0);
	} 


   // read until conceptDef
    FILE *fp;
	if ((fp = fopen((const char *)m_input.c_str(), "r")) == NULL)
	{
		char buf[255];
		sprintf(buf, "Cannot open file %s", 
			(const char *) m_input.c_str());
		cerr << "Error opening file \"" << buf << "\", will EXIT." << endl;
		fclose(gp);
		exit(0);
	}
	
	string target = "<conceptDef>";
	char buf[2048];
	while(!feof(fp) && (fgets(buf, 2048, fp) != NULL))
	{
		string line = buf;
		if (line[line.length()-1] == '\n')
		{
			line = line.substr(0, line.length()-1);
		}
		if (line.compare(target) == 0)
		{
			break;
		}
		fprintf(gp, "%s\n", (const char *) line.c_str());
	}

    fclose(fp);

	map<long, OntyxConcept>::iterator begin_it;
	map<long, OntyxConcept>::iterator end_it;

	begin_it = kb.getFirstConceptIterator();
	end_it = kb.getEndConceptIterator();


    map<long, OntyxConcept>::iterator posCon = begin_it;

	unsigned int total = 0;
	for( ; posCon != end_it; ++posCon )
	{
		// Concept
		OntyxConcept c = (OntyxConcept) posCon->second;
		total++;

		string name = c.getName();
		string code = c.getCode();
		//long id = c.getID();
		string namespaceName = c.getNamespace();
		string kind = c.getKind();

        vector<string> v;
		const multimap<long, OntyxProperty> properties
			= c.getProperties();

		// Property

		vector<string> prop_names;
        vector<string> prop_values;
		multimap<long, OntyxProperty >::const_iterator pProp = properties.begin();
		for( ; pProp != properties.end(); ++pProp)
		{
			OntyxProperty p = (OntyxProperty) pProp->second;
			string name = p.getName();
			string value = p.getValue();
            if (name.compare("exact_synonym") == 0)
			{
				prop_names.push_back(name);
				prop_values.push_back(value);
			}
		}
        unsigned int i;
		for (i=0; i<prop_names.size(); i++)
		{
			string name = prop_names[i];
			string value = prop_values[i];
			//c.deleteProperty(name, value);
			OntyxProperty tmpProperty("Synonym", value, "");
			c.addProperty(tmpProperty);
		}
        prop_names.clear();
		prop_values.clear();

        // Role
//		multimap<long, OntyxRole> roles = c.getRoles();
//		vector<string> role_names;
//		vector<string> role_values;
//		multimap<long, OntyxRole >::const_iterator pRole = roles.begin();
//		for( ; pRole != roles.end(); ++pRole)
//		{
//			OntyxRole r = (OntyxRole) pRole->second;
//			string modifier = r.getModifier();
//			string name = r.getName();
//			string value = r.getValue();

//		}

//		for (i=0; i<role_values.size(); i++)
//		{
//			string name = (string) role_names[i];
//			string value = (string) role_values[i]; 
//			OntyxProperty tmpProperty("part_of", value, "");
//			c.addProperty(tmpProperty);
//			c.addParent(value);
//			c.deleteRole(name, value);
//		}
//        role_names.clear();
//		role_values.clear();

		WriteConceptDef(gp, c);
	}

	fprintf(gp, "\n</terminology>");

	fclose(gp);
  
    tend = clock();
	char temp_buf[2048];
	sprintf(temp_buf, "Output file %s generated -- Execution took %f seconds.", 
		(const char *) m_output.c_str(), (tend-tstart)/1000.0);
   
    cout << temp_buf << endl;
}


int main (int argc, char * argv[])
{
	vector<string> commLine;
	for ( int i = 0; i < argc; i++)
		commLine.push_back(*argv++);

	if( commLine.size() != 3 ) {  // i.e. it takes two or three files
		cerr << "Error:  incorrect number of command line parameters." << endl;
		cerr << "\nUsage:\nOBO2TDE2 inputfile outputfile" << endl ;
		exit(0);
		}

    ProcessData(commLine[1], commLine[2]);

	return 0;
}
