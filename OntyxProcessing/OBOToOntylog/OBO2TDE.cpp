// OBO2TDEDlg.cpp : implementation file
//

#include "OBO2TDE.h"

str_map id2name;

str_map propertyname_tbl;
str_map conceptname_tbl;
str_map tagheader_tbl;

propertyqualifier_map propertyqualifier_tbl;

unsigned int concept_knt;

vector<string> namespace_vec;
str_map namespace_tbl;

string	m_input;
string	m_output;
string	m_template;
string	m_tagheader;

unsigned int num_codes = 0;
concept_vec concepts;

int ReverseFind(const string &str, char c)
{
	return (int)str.rfind( c );
}

string LSubString (const string &s, int n)
{
	if (n < 0) return("");
	return(s.substr(0, n));
}

string RSubString (const string &s, int n)
{
	if (n < 0) return("");
	int len = s.size();
	if (len < n)
	{
		return("");
	}
	else
	{
		string str = s.substr(len-n, n);
		return(str);
	}
}

string TrimRight(string &s)
{
   //if (s[s.length()-1] == ' ')
   if (RSubString(s, 1).compare(" ") == 0)
   {
	   string t = LSubString(s, s.length()-1);
	   return TrimRight(t);
   }
   return s;
}

string TrimLeft(string &s)
{
//   if (s[s.length()-1] == ' ')
   if (LSubString(s, 1).compare(" ") == 0)
   {
	   string t = RSubString(s, s.length()-1);
	   return TrimLeft(t);
   }
   return s;
}


string TrimRightChar(string s, char c)
{
	if (s.length() == 0) return s;
	char b[2];
	sprintf(b, "%c", c);
	string t = b;

	if (t.compare(s) == 0) return "";
	string str = s;
	str = TrimRight(str);
	if (RSubString(str,1).compare(t) == 0) return TrimRightChar(LSubString(str,str.length()-1), c);
	str = TrimRight(str);
	return str;
}


string TrimLeftChar(string s, char c)
{
	if (s.length() == 0) return s;
	char b[2];
	sprintf(b, "%c", c);
	string t = b;
	if (t.compare(s) == 0) return "";
	string str = s;
	str = TrimLeft(str);
	if (LSubString(str,1).compare(t) == 0) return TrimLeftChar(RSubString(str,str.length()-1), c);
	str = TrimLeft(str);
	return str;
}


string TrimTerminatingNonAlphaNumChars(string s)
{
	if (s.length() == 0) return s;
	if (!isalnum(s[s.length()-1])
		  && s[s.length()-1] != '.'
		  && s[s.length()-1] != '/'
		  && s[s.length()-1] != ')'
		  && s[s.length()-1] != '\"' // 6/20/08 RWW
		  && s[s.length()-1] != '[' // 01/29/09 TS
		  )
	{
		return TrimTerminatingNonAlphaNumChars(LSubString(s,s.length()-1));
	}
	return s;
}



string TrimStartingNonAlphaNumChars(string s)
{
	if (s.length() == 0) return s;
	if (!isalnum(s[0]))
	{
		return TrimStartingNonAlphaNumChars(RSubString(s,s.length()-1));
	}
	return s;
}

bool StartWith(string line, string tag)
{
    int tag_len = tag.length();
	if (LSubString(line,tag_len).compare(tag) == 0) return true;
	return false;
}


bool EndWith(string line, string tag)
{
    int tag_len = tag.length();
	if (RSubString(line,tag_len).compare(tag) == 0) return true;
	return false;
}


string GetValue(string line, string tag)
{
	if (!StartWith(line, tag)) return "";
    int tag_len = tag.length();
	int n = line.find_first_of(tag);
	if (n == -1) return "";
    string newline = RSubString(line,line.length()-(n+tag_len));
	newline = TrimLeft(newline);
	newline = TrimRight(newline);
	return newline;
}



//[PMID:10873824, PMID:11389764, SGD:mcc]
vector<string> ParseDbXrefString(string s0)
{
	string s = s0 + ",";
    vector<string> v;
    int knt = 0;
	int n = s.find_first_of(',');
	while (n != -1 && knt < 10) //max number of substrings
	{
		string t = LSubString(s,n);
		s = RSubString(s,s.length()-n-1);
		t = TrimTerminatingNonAlphaNumChars(t);
		t = TrimStartingNonAlphaNumChars(t);

		v.push_back(t);
		knt++;

		n = s.find_first_of(',');
	}
	return v;
}


//[PMID:10873824, PMID:11389764, SGD:mcc]
vector<string> ParseDbXrefString(FILE *gp, string s0)
{

//fprintf(gp, "ParseDbXrefString original string: " + s0 );
//fprintf(gp, "\n");
	string s = s0 + ",";
    vector<string> v;
    int knt = 0;
	int n = s.find_first_of(',');
	while (n != -1 && knt < 10) //max number of substrings
	{
		string t = LSubString(s,n);
		s = RSubString(s,s.length()-n-1);
		t = TrimTerminatingNonAlphaNumChars(t);
		t = TrimStartingNonAlphaNumChars(t);

		v.push_back(t);
		knt++;

		n = s.find_first_of(',');
	}
	return v;
}


string FindPropertyName(string name)
{
 	str_map::iterator theIterator = propertyname_tbl.find(name);
	if(theIterator != propertyname_tbl.end())
	{
		return (*theIterator).second;
	}
	return name;
}



string RemoveCharacter(string s0, char c)
{
	int len = s0.length();
	if (len == 0) return s0;
	char *buf = (char *) malloc(len * sizeof(char) + 2);
	int k = 0;
	for (int i=0; i<len; i++)
	{
		char ch = s0[i];
		if (ch != c)
		{
			buf[k] = ch;
			k++;
		}
	}
	buf[k] = '\0';
	string t = buf;
	free (buf);
	return t;
}


string RemoveEscapeCharacters(string s0)
{
    return RemoveCharacter(s0, '\\');
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

//080707 RWW, type_name changed to scope_specifier, dbxref uses OutputValue( FILE, string, string) for CDATA where needed
void OutputValue(FILE *gp, char delim, string value)
{
	if (value.length() == 0) return;
    string ref = "";
    int n0 = value.find_last_of("[");
    if (n0 != -1)
    {
		ref = RSubString(value,value.length()-n0-1);
        value = LSubString(value, n0);
	}

    int n1 = value.find_first_of(delim);
    if (n1 == -1)
    {
		OutputValue(gp, value);
	}
	else
	{
		string name = LSubString(value, n1);
		OutputValue(gp, name);
		string remainer = RSubString(value, value.length()-n1-1);
		string types = TrimLeft(remainer);
		string type = "";
		n1 = types.find_first_of(' ');
		if (n1 != -1)
		{
			fprintf(gp, "\t<qualifiers>\n");
			while (n1 != -1)
			{
				type = LSubString(types, n1);
				types = RSubString(types, types.length()-n1);
				types = TrimLeft(types);
				fprintf(gp, "\t\t<qualifier>\n");
				fprintf(gp, "\t\t\t<name>scope_specifier</name>\n");
				fprintf(gp, "\t\t\t<value>%s</value>\n", (const char *) type.c_str());
				fprintf(gp, "\t\t</qualifier>\n");
				n1 = types.find_first_of(' ');
			}

			if (types.length() > 0)
			{
				fprintf(gp, "\t\t<qualifier>\n");
				fprintf(gp, "\t\t\t<name>scope_specifier</name>\n");
				fprintf(gp, "\t\t\t<value>%s</value>\n", (const char *) types.c_str());
				fprintf(gp, "\t\t</qualifier>\n");
			}

			if (ref.length() > 0)
			{
				fprintf(gp, "\t\t<qualifier>\n");
				fprintf(gp, "\t\t\t<name>dbxref</name>\n");
				OutputValue(gp, "\t\t\t", ref);  // 080707 fprintf(gp, "\t\t\t<value>%s</value>\n", (const char *) ref.c_str());
				fprintf(gp, "\t\t</qualifier>\n");
			}
			fprintf(gp, "\t</qualifiers>\n");
		}
    }
}


void AddCodeToMap(string id, string name)
{
	str_map::iterator theIterator = id2name.find(id);
	if(theIterator == id2name.end())
	{
		id2name.insert(str_map::value_type(id, name));
	}
}






void ClearMemory()
{
	propertyname_tbl.clear();
    conceptname_tbl.clear();
    tagheader_tbl.clear();


	for (unsigned int i=0; i<concepts.size(); i++)
	{
         ConceptDef *ptr = (ConceptDef *) concepts[i];
		 for (unsigned int k=0; k<ptr->property_vec.size(); k++)
		 {
			 Property *ptr2 = (Property *) ptr->property_vec[k];
			 for (unsigned int k2=0; k2<ptr2->qualifier_vec.size(); k2++)
			 {
				Qualifier *ptr3 = (Qualifier *) ptr2->qualifier_vec[k2];
				delete ptr3;
			 }
			 ptr2->qualifier_vec.clear();
			 delete ptr2;
		 }
		 delete ptr;
	}
    concepts.clear();

}

bool AddTagHeader(string tag, string value)
{
	str_map::iterator theIterator = tagheader_tbl.find(tag);
	if(theIterator == tagheader_tbl.end())
	{
		tagheader_tbl.insert(str_map::value_type(tag, value));
		return true;
	}
	return false;
}

void LoadTagHeaders(string infile)
{
  	FILE *fp;
    if ((fp = fopen((const char *)infile.c_str(), "r")) == NULL)
    {
		char buf[255];
		sprintf(buf, "Cannot open file %s",
			(const char *)infile.c_str());
		cerr << buf << endl;
		return;
	}
	char buf[2048];
	int knt = 0;
    while(!feof(fp) && (fgets(buf, 2048, fp) != NULL))
    {
		string line = buf;
		char *p = strtok(buf, "|");
		string tag = p;
		p = strtok(NULL, "|\n");
		string value = p;

		bool retval = AddTagHeader(tag, value);
		if (retval)
		{
			knt++;
		}
	}


	fclose(fp);
}


string GetTagHeaderValue(string tag)
{
	str_map::iterator theIterator = tagheader_tbl.find(tag);
	if(theIterator != tagheader_tbl.end())
	{
		return (*theIterator).second;
	}
	return "";
}



bool IsDismissed(string tag)
{
    if (GetTagHeaderValue(tag).compare("dismissed") == 0) return true;
	return false;
}




void AddNamespace(string namespaceNm)
{
	str_map::iterator theIterator = namespace_tbl.find(namespaceNm);
	if(theIterator == namespace_tbl.end())
	{
		namespace_tbl.insert(str_map::value_type(namespaceNm, namespaceNm));
		namespace_vec.push_back(namespaceNm);
	}
}


string GetName(string id)
{
	if (id.compare("") == 0) return "";
	str_map::iterator theIterator = id2name.find(id);
	if(theIterator != id2name.end())
	{
		return (*theIterator).second;
	}
	return "";
}


Property *CreateProperty(string name, string value)
{
    Property *ptr = new Property();
	ptr->name = name;
	ptr->value = value;
	return ptr;

}

Qualifier *CreateQualifier(string name, string value)
{
    Qualifier *ptr = new Qualifier();
	ptr->name = name;
	ptr->value = value;
	return ptr;
}

void PrintConceptDefOpenTag(FILE *gp)
{
	fprintf(gp, "<conceptDef>\n");
}


void PrintConceptDefCloseTag(FILE *gp)
{
	fprintf(gp, "</conceptDef>\n");
}

/*
<name>Splatinplatin</name>
<code>C3</code>
<id>3</id>
<namespace>NCI</namespace>
<primitive/>
<kind>Chemicals_and_Drugs_Kind</kind>
*/

void PrintIdentifiers(FILE *gp,
									  string name,
									  string code,
									  //unsigned int cid,
									  string namespaceName,
                                      bool isPrimitive,
                                      string kind
									  )
{
	concept_knt++;
	fprintf(gp, "<name>%s</name>\n", (const char *) name.c_str());
	fprintf(gp, "<code>%s</code>\n", (const char *) code.c_str());
	fprintf(gp, "<id>%u</id>\n", concept_knt);
	fprintf(gp, "<namespace>%s</namespace>\n", (const char *) namespaceName.c_str());
	if (isPrimitive)
	{
		fprintf(gp, "<primitive/>\n");
	}
	fprintf(gp, "<kind>%s</kind>\n", (const char *) kind.c_str());
}

/*
<definingConcepts>
<concept>Platinum Compound</concept>
</definingConcepts>
*/
void PrintDefiningConcepts(FILE *gp, vector<string> &sups)
{
	fprintf(gp, "<definingConcepts>\n");
	for (unsigned int i=0; i<sups.size(); i++)
	{
		string sup = (string) sups[i];
		fprintf(gp, "<concept>%s</concept>\n", (const char *) sup.c_str());
	}
	fprintf(gp, "</definingConcepts>\n");
}


void PrintProperties(FILE *gp, vector<Property *> &property_vec)
{
	fprintf(gp, "<properties>\n");
	for (unsigned int i=0; i<property_vec.size(); i++)
	{
		Property *ptr = (Property *) property_vec[i];
		string name = ptr->name;
		string value = ptr->value;
		value = TrimTerminatingNonAlphaNumChars(value);
		value = TrimStartingNonAlphaNumChars(value);

		value = RemoveEscapeCharacters(value);

		fprintf(gp, "<property>\n");
		fprintf(gp, "<name>%s</name>\n", (const char *) name.c_str());
		if (name.compare("FULL_SYN") == 0)
		{
			fprintf(gp, "<value><![CDATA[%s]]></value>\n", (const char *) value.c_str());
		}

		else if (name.compare("Synonym") == 0)
		{
			char delim = '"';
			OutputValue(gp, delim, value);
		}

		else
		{
			OutputValue(gp, value);
		}

		if (ptr->qualifier_vec.size() > 0)
		{
			fprintf(gp, "\t<qualifiers>\n");
			for (unsigned int j=0; j<ptr->qualifier_vec.size(); j++)
			{
				Qualifier *qualifier_ptr = (Qualifier *) ptr->qualifier_vec[j];
				fprintf(gp, "\t\t<qualifier>\n");
				fprintf(gp, "\t\t\t<name>%s</name>\n", (const char *) qualifier_ptr->name.c_str());
                value = RemoveEscapeCharacters(qualifier_ptr->value);
				OutputValue(gp, "\t\t\t", (const char *) value.c_str());
				fprintf(gp, "\t\t</qualifier>\n");
			}
			fprintf(gp, "\t</qualifiers>\n");
		}
		fprintf(gp, "</property>\n");
	}
	fprintf(gp, "</properties>\n");
}

void PrintDefiningRoles(FILE *gp, vector<string> &modifier_vec,
									 vector<string> &name_vec,
									 vector<string> &value_vec)
{
	fprintf(gp, "<definingRoles>\n");
	for (unsigned int i=0; i<name_vec.size(); i++)
	{
		string modifier = (string) modifier_vec[i];
		string name = (string) name_vec[i];
		string value = (string) value_vec[i];

		fprintf(gp, "<role>\n");
        fprintf(gp, "<%s/>\n", (const char *) modifier.c_str());
		fprintf(gp, "<name>%s</name>\n", (const char *) name.c_str());
		OutputValue(gp, value);
		//fprintf(gp, "<value>%s</value>\n", (const char *) value.c_str());
		fprintf(gp, "</role>\n");
	}
	fprintf(gp, "</definingRoles>\n");
}

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
						  vector<string> &propertyname_vec,
						  vector<string> &propertyvalue_vec,
						  vector<Property *> property_vec)
	{
	     PrintConceptDefOpenTag(gp);

         PrintIdentifiers(gp,
						  name,
						  id,
						  //id,
                          //concept_knt,
						  namespaceName,
                          //isPrimitive,
						  true,
                          kind
						  );



	     PrintDefiningConcepts(gp, sups);
         PrintDefiningRoles(gp,
		                    role_modifier_vec,
							rolename_vec,
							rolevalue_vec);

	     PrintProperties(gp, property_vec);




         PrintConceptDefCloseTag(gp);

	 }


void PrintProperty(FILE *gp,
								bool isComplex,
							    string name,
							    string value)
{
	fprintf(gp, "<property>\n");
	fprintf(gp, "<name>%s</name>\n", (const char *) name.c_str());
	if (isComplex)
	{
		fprintf(gp, "<value><![CDATA[%s]]></value>\n", (const char *) value.c_str());
	}
	else
	{
		OutputValue(gp, value);
	}
	fprintf(gp, "</property>\n");
}


void PrintComplexProperty(FILE *gp,
									   string propertyname,
									   vector<string> &propertyvaluetag_vec,
									   vector<string> &propertyvalue_vec)
{
	char b[2048];
    string name = propertyname;
	string value = "";
	for (unsigned int i=0; i<propertyvalue_vec.size()-1; i++)
	{
		string tag = (string) propertyvaluetag_vec[i];
		string val = (string) propertyvalue_vec[i];
		sprintf(b, "<%s>%s</%s>",
			(const char *) tag.c_str(),
			(const char *) val.c_str(),
			(const char *) tag.c_str());
         string s = b;
		 value = value + s;
	}
	PrintProperty(gp, true, name, value);
}

string replaceChar(string &s, char c1, char c2)
{
    string t = s;
	for (unsigned int i=0; i<t.size(); i++)
	{
		if (t[i] == c1) t[i] = c2;
	}
	return t;
}

void PrintObsoleteConcept(FILE *gp,
									   string name,
									   string code
									   )
{
    string name0 = name;
	name = replaceChar(name, ' ', '_');

    vector<string> sups;
    vector<string> role_modifier_vec;
    vector<string> rolename_vec;
    vector<string> rolevalue_vec;

    vector<string> propertyname_vec;
    vector<string> propertyvalue_vec;

	vector<Property *> property_vec;

	propertyname_vec.push_back("Preferred_Name");
    propertyvalue_vec.push_back(name0);

//	propertyname_vec.push_back("Synonym");
//    propertyvalue_vec.push_back(name0);

	Property *ptr = CreateProperty("Preferred_Name", name0);
    property_vec.push_back(ptr);

//	ptr = CreateProperty("Synonym", name0);
//    property_vec.push_back(ptr);


    PrintConceptDef(gp,
					name0,
					"",
					code,
					"GO",
                    true,
                    "GO_Kind",
					sups,
		            role_modifier_vec,
					rolename_vec,
					rolevalue_vec,
					propertyname_vec,
					propertyvalue_vec,
					property_vec);

		 for (unsigned int k=0; k<property_vec.size(); k++)
		 {
			 Property *ptr = (Property *) property_vec[k];
			 for (unsigned int k2=0; k2<ptr->qualifier_vec.size(); k2++)
			 {
				Qualifier *ptr2 = (Qualifier *) ptr->qualifier_vec[k2];
				delete ptr2;
			 }
			 ptr->qualifier_vec.clear();
			 delete ptr;
		 }
         property_vec.clear();
}


void PrintObsoleteConcepts(FILE *gp)
{
//	for (unsigned int i=0; i<namespace_vec.size(); i++)
//	{
	int i=0;
		fprintf(gp, "\n");
		//concept_knt++;
		char b[10];
//		sprintf(b, "NCI00%u", i+1);
		sprintf(b, "NCI001", i+1);
		string code = b;
        PrintObsoleteConcept(gp,
//			"obsolete_" + namespace_vec[i],
			"ObsoleteClass",
            code
			);
//	}
}





string GetConceptName(string name)
{
 	str_map::iterator theIterator = conceptname_tbl.find(name);
	if(theIterator == conceptname_tbl.end())
	{
        conceptname_tbl.insert(str_map::value_type(name, name));
		return name;
	}
	else
	{
		string value = (*theIterator).second;
        string new_value = value + "_";
		//conceptname_tbl.insert(str_map::value_type(name, new_value));
        (*theIterator).second = new_value;
		return new_value;
	}
}


void InitializePropertyNameTable()
{
	propertyname_tbl.clear();
    propertyname_tbl.insert(str_map::value_type("synonym", "Synonym"));
}



string GetDefinitionSource(string line)
{
	string source = "";

    int n = ReverseFind(line,'[');
	if (n == -1) return source;

	char b[10];
	sprintf(b, "n=%d", n);

	source = RSubString(line,line.length()-n-1);

	source = TrimRight(source);

	source = TrimLeft(source);
    string retstr = TrimRightChar(source, ']');
	return retstr;
}



//"The action characteristic of a gene product." [GO:curators]

string GetDefinitionText(string line)
{
	string text = line;
	string t = TrimLeftChar(line, '"');
    int n = ReverseFind(t,'[');
	if (n == -1) return TrimRightChar(text, '"');

	text = LSubString(t,n);
	text = TrimRight(text);
	text = TrimLeft(text);
	return TrimRightChar(text, '"');
}

string GetLongValue(FILE *fp, string line, string tag)
{
	if (!StartWith(line, tag)) return "";
    //int tag_len = tag.length();
	int n = line.find_first_of(tag);
	if (n == -1) return "";

	string firstline = TrimRightChar(line, '\n');
	if (!EndWith(firstline, "\\")) return GetValue(firstline, tag);
	firstline = TrimRightChar(firstline, '\\');
    //bool cont_flag = true;
	char buf[2048];
    while(!feof(fp) && (fgets(buf, 2048, fp) != NULL))
    {
		string nextline = buf;
		nextline = TrimRightChar(nextline, '\n');
        nextline = TrimRight(nextline);

        if (!EndWith(nextline, "\\"))
		{
            firstline = firstline + " ";
			firstline = firstline + TrimRightChar(nextline, '\\');
			return GetValue(firstline, tag);
		}
		else
		{
            firstline = firstline + " ";
			firstline = firstline + TrimRightChar(nextline, '\\');
		}
	}

	return GetValue(firstline, tag);
}

void PrintLine(FILE *gp, string name, string value)
{
    fprintf(gp, "%s|%s\n", (const char *) name.c_str(), (const char *) value.c_str());
}


vector<string> getRelationNames(string infile)
{
	/**
	Read through the input file and find all instances of "relationship:"
	Grab the relationship name and put it into a map
	Example:  "relationship: regulates GO:0006312 ! mitotic recombination" 
	   would result in "regulates" being added to the relationship map
	**/
	string relationship_target = "relationship:";
	string disjoint_target = "disjoint_from";
    vector<string> relation_name_vec;
  	FILE *fp;
    if ((fp = fopen((const char *)infile.c_str(), "r")) == NULL)
    {
		char buf[255];
		sprintf(buf, "Cannot open file %s",
			(const char *)infile.c_str());
		return relation_name_vec;
	}

	str_map rel_nm_tbl;
	char buf[2048];
	//read through the input file
    while(!feof(fp) && (fgets(buf, 2048, fp) != NULL))
    {
		string line = buf;
		//search for instances or "relationship:"
		if (StartWith(line, relationship_target) )
		{
            int m = line.find_first_of('!');
			string t;
			if (m != -1)
			{
				t = LSubString(line,m);
			}
			else
			{
                t = line;
			}

			string related_code = GetValue(t, relationship_target);
			int k = ReverseFind(related_code, ' ');

			string relation_nm = LSubString(related_code,k);

			str_map::iterator theIterator = rel_nm_tbl.find(relation_nm);
			if(theIterator == rel_nm_tbl.end())
			{
				rel_nm_tbl.insert(str_map::value_type(relation_nm, relation_nm));
				relation_name_vec.push_back(relation_nm);
			}
		}
		//search for instances of "disjoint_from"
		//if any exist, then add "disjoint_from" to the relationship map
		if (StartWith(line, disjoint_target) )
		{
            int m = line.find_first_of('!');
			string t;
			if (m != -1)
			{
				t = LSubString(line,m);
			}
			else
			{
                t = line;
			}

			string related_code = GetValue(t, disjoint_target);
//			int k = ReverseFind(related_code, ' ');

//			string relation_nm = LSubString(related_code,k);
			string relation_nm = "disjoint_from";

			str_map::iterator theIterator = rel_nm_tbl.find(relation_nm);
			if(theIterator == rel_nm_tbl.end())
			{
				rel_nm_tbl.insert(str_map::value_type(relation_nm, relation_nm));
				relation_name_vec.push_back(relation_nm);
			}
		}
	}
	fclose(fp);
	rel_nm_tbl.clear();
    return relation_name_vec;
}


void Convert(string infile, string templatefile, string tagheader, string outfile)
{
	char temp_buffer[1024];
	sprintf(temp_buffer, "Input file: %s",
		(const char *) infile.c_str());
    cout << temp_buffer << endl;

	sprintf(temp_buffer, "templatefile: %s",
		(const char *) templatefile.c_str());
    cout << temp_buffer << endl;

	sprintf(temp_buffer, "tagheader file: %s",
		(const char *) tagheader.c_str());
    cout << temp_buffer << endl;

	sprintf(temp_buffer, "outfile: %s",
		(const char *) outfile.c_str());
    cout << temp_buffer << endl;

	m_input = infile;
	m_output = outfile;
	m_template = templatefile;
	m_tagheader = tagheader;

	vector<string> propertylabel_vec;
	LoadTagHeaders(m_tagheader);

	vector<string> relation_name_vec = getRelationNames(m_input);

	for (unsigned int i=0; i<relation_name_vec.size(); i++)
	{
		string nm = (string) relation_name_vec[i];
		cout << "relation: " << nm << endl;
	}
	relation_name_vec.clear();


  	FILE *fp;
    if ((fp = fopen((const char *)infile.c_str(), "r")) == NULL)
    {
		char buf[255];
		sprintf(buf, "Cannot open file %s",
			(const char *)infile.c_str());
		return;
	}


	FILE *gp;
    if ((gp = fopen((const char *)outfile.c_str(), "w")) == NULL)
    {
		char buf[255];
		sprintf(buf, "Cannot open file %s",
			(const char *)outfile.c_str());
		fclose(fp);
		return;
	}

    vector <string> v;
	v.clear();
    int knt = 0;
	char buf[2048];

	string id_target = "id:";
	string name_target = "name:";
	string is_a_target = "is_a:";
	string def_target = "def:";
	string relationship_target = "relationship:";
	string disjoint_target = "disjoint_from";
	string namespace_target = "namespace:";
	string namespaceStr;
	string obsolete_target = "is_obsolete:";
	string xref_analog_target = "xref_analog:";
	string xref_analog_target_v2 = "xref:";
    string dbxref_target = "dbxref:";
    string exact_synonym_target = "exact_synonym:";
    string broad_synonym_target = "broad_synonym:";
    string narrow_synonym_target = "narrow_synonym:";
    string related_synonym_target = "related_synonym:";
    string synonym_target = "synonym:";

	conceptname_tbl.clear();
	InitializePropertyNameTable();
	namespace_vec.clear();
	namespace_tbl.clear();


	FILE *fp2;
    if ((fp2 = fopen((const char *)m_template.c_str(), "r")) == NULL)
    {
		char buf[255];
		sprintf(buf, "Cannot open file %s",
			(const char *)m_template.c_str());

		fclose(fp);
		fclose(gp);
		return;
	}
    while(!feof(fp2) && (fgets(buf, 2048, fp2) != NULL))
    {
		string line = buf;
		if (!StartWith(line, "<conceptDef>"))
		{
			fprintf(gp, "%s", (const char*) line.c_str());
		}
	}
	fclose(fp2);

    string name;
    string code;
    string id;
    string namespaceName;
    bool isPrimitive = false;
    string kind;
	vector<string> sups;
	vector<string> role_modifier_vec;
	vector<string> rolename_vec;
	vector<string> rolevalue_vec;
	vector<string> propertyname_vec;
	vector<string> propertyvalue_vec;

	string curr_id = "";
	conceptname_tbl.clear();

    while(!feof(fp) && (fgets(buf, 2048, fp) != NULL))
    {
		string line = buf;
		if (RSubString(line,1).compare("\n") == 0)
		{
			line = LSubString(line,line.length()-1);
		}
		line = TrimRight(line);

		if (line.compare("[Term]") == 0)
		{
             curr_id = "";
		}

		if (StartWith(line, id_target))
		{
			string id = GetValue(line, id_target);
			curr_id = id;
		}
		else if (StartWith(line, name_target))
		{
			string name = GetValue(line, name_target);
			name = GetConceptName(name);
 			AddCodeToMap(curr_id, name);
		}
		else if (StartWith(line, namespace_target))
		{
			string name = GetValue(line, namespace_target);
			AddNamespace(name);
		}

		int n = line.find_first_of(':');
		if (n != -1)
		{
			string str = LSubString(line,n);
			n = str.find_first_of(' ');
			if (n == -1)
			{
				bool exists = false;
				for (unsigned int i=0; i<propertylabel_vec.size(); i++)
				{
					string s = (string) propertylabel_vec[i];
					if (s.compare(str) == 0)
					{
						exists = true;
						break;
					}
				}
				if (!exists) propertylabel_vec.push_back(str);
			}

		}
	}


	rewind(fp);
    propertyname_vec.clear();
    knt = 0;
	curr_id = "";

	name = "";
    code = "";
    id = "";
    namespaceName = "GO";
    isPrimitive = false;
    kind = "GO_Kind";

    sups.clear();
    role_modifier_vec.clear();
    rolename_vec.clear();
    rolevalue_vec.clear();
    propertyname_vec.clear();
    propertyvalue_vec.clear();

	vector<Property *> property_vec;

	string conceptname = "";
    concept_knt = 0;

    string curr_name = "";

    while(!feof(fp) && (fgets(buf, 2048, fp) != NULL))
    {
		string line = buf;
		if (RSubString(line,1).compare("\n") == 0)
		{
			line = LSubString(line,line.length()-1);
		}
		line = TrimRight(line);

		if (line.compare("[Typedef]") == 0)
		{
            break;
		}

		else if (line.compare("[Term]") == 0)
		{
			 fprintf(gp, "\n");
			 if (curr_id.compare("") != 0)
			 {
				 curr_name = GetName(id);

                     PrintConceptDef(gp,
						  GetName(id),
						  code,
						  id,
						  namespaceName,
                          isPrimitive,
                          kind,
						  sups,
		                  role_modifier_vec,
						  rolename_vec,
						  rolevalue_vec,
						  propertyname_vec,
						  propertyvalue_vec,
						  property_vec);


					 for (unsigned int k=0; k<property_vec.size(); k++)
					 {
						 Property *ptr = (Property *) property_vec[k];
						 for (unsigned int k2=0; k2<ptr->qualifier_vec.size(); k2++)
						 {
							Qualifier *ptr2 = (Qualifier *) ptr->qualifier_vec[k2];
							delete ptr2;
						 }
						 ptr->qualifier_vec.clear();
						 delete ptr;
					 }
					 property_vec.clear();

			 }

			 curr_id = "";
			 conceptname = "";
			 sups.clear();

			 role_modifier_vec.clear();
			 rolename_vec.clear();
			 rolevalue_vec.clear();
			 propertyname_vec.clear();
			 propertyvalue_vec.clear();
             knt++;
		}

		if (StartWith(line, id_target))
		{
			id = GetValue(line, id_target);
			curr_id = id;
		}
		else if (StartWith(line, name_target))
		{
			name = GetValue(line, name_target);
            conceptname = name;

			Property *prop_ptr = CreateProperty("Preferred_Name", name);
			property_vec.push_back(prop_ptr);
//			prop_ptr = CreateProperty("Synonym", name);
//			property_vec.push_back(prop_ptr);
		}
		else if (StartWith(line, namespace_target))
		{
			string name = GetValue(line, namespace_target);
			namespaceStr = name;
			Property *prop_ptr = CreateProperty("namespace", name);
			property_vec.push_back(prop_ptr);

		}
		else if (StartWith(line, is_a_target))
		{
            int m = line.find_first_of('!');
			string t;
			if (m != -1)
			{
				t = LSubString(line,m);
			}
			else
			{
                t = line;
			}

			string parent_code = GetValue(t, is_a_target);
			string parent = GetName(parent_code);
			sups.push_back(parent);
		}

		else if (StartWith(line, relationship_target))
		{
            int m = line.find_first_of('!');
			string t;
			if (m != -1)
			{
				t = LSubString(line,m);
			}
			else
			{
                t = line;
			}

			string related_code = GetValue(t, relationship_target);
			int k = ReverseFind(related_code, ' ');

			string relation_nm = LSubString(related_code,k);

			related_code = RSubString(related_code,related_code.length()-k-1);

			string rel_code = GetName(related_code);

			role_modifier_vec.push_back("some");

			rolename_vec.push_back(relation_nm);//"part_of");

			rolevalue_vec.push_back(rel_code);
		}

		else if (StartWith(line, disjoint_target))
		{
            int m = line.find_first_of('!');
			string t;
			if (m != -1)
			{
				t = LSubString(line,m);
			}
			else
			{
                t = line;
			}

			string related_code = GetValue(t, disjoint_target);
			int k = ReverseFind(related_code, ' ');

//			string relation_nm = LSubString(related_code,k);
			string relation_nm = "disjoint_from";

			related_code = RSubString(related_code,related_code.length()-k-1);

			string rel_code = GetName(related_code);

			role_modifier_vec.push_back("some");

			rolename_vec.push_back(relation_nm);//"part_of");

			rolevalue_vec.push_back(rel_code);
		}
		
		else if (StartWith(line, def_target))
		{
			string name = GetLongValue(fp, line, def_target);
			string text = RemoveEscapeCharacters(GetDefinitionText(name));

            string source = GetDefinitionSource(name);

			Property *prop_ptr = CreateProperty("DEFINITION", text);
			vector<string> v = ParseDbXrefString(gp, source);
            for (unsigned int k=0; k<v.size(); k++)
			{
				string val = (string) v[k];
                if (val.compare("") != 0)
				{
					Qualifier *qualifier_ptr = CreateQualifier("dbxref", v[k]);
					prop_ptr->qualifier_vec.push_back(qualifier_ptr);
				}
			}
			property_vec.push_back(prop_ptr);
		}

		else if (StartWith(line, exact_synonym_target))
		{
			string name = GetLongValue(fp, line, exact_synonym_target);
			string text = RemoveEscapeCharacters(GetDefinitionText(name));
            string source = GetDefinitionSource(name);
			Property *prop_ptr = CreateProperty("exact_synonym", text);
			vector<string> v = ParseDbXrefString(gp, source);

            for (unsigned int k=0; k<v.size(); k++)
			{
				string val = (string) v[k];
                if (val.compare("") != 0)
				{
					Qualifier *qualifier_ptr = CreateQualifier("dbxref", v[k]);
					prop_ptr->qualifier_vec.push_back(qualifier_ptr);
				}
			}

			property_vec.push_back(prop_ptr);
		}
		else if (StartWith(line, broad_synonym_target))
		{
			string name = GetLongValue(fp, line, broad_synonym_target);
			string text = RemoveEscapeCharacters(GetDefinitionText(name));

            string source = GetDefinitionSource(name);

			Property *prop_ptr = CreateProperty("broad_synonym", text);
			vector<string> v = ParseDbXrefString(gp, source);
            for (unsigned int k=0; k<v.size(); k++)
			{
				string val = (string) v[k];
                if (val.compare("") != 0)
				{
					Qualifier *qualifier_ptr = CreateQualifier("dbxref", v[k]);
					prop_ptr->qualifier_vec.push_back(qualifier_ptr);
				}
			}
			property_vec.push_back(prop_ptr);
		}

		else if (StartWith(line, narrow_synonym_target))
		{
			string name = GetLongValue(fp, line, narrow_synonym_target);
			string text = RemoveEscapeCharacters(GetDefinitionText(name));

            string source = GetDefinitionSource(name);

			Property *prop_ptr = CreateProperty("narrow_synonym", text);
			vector<string> v = ParseDbXrefString(gp, source);
            for (unsigned int k=0; k<v.size(); k++)
			{
				string val = (string) v[k];
                if (val.compare("") != 0)
				{
					Qualifier *qualifier_ptr = CreateQualifier("dbxref", v[k]);
					prop_ptr->qualifier_vec.push_back(qualifier_ptr);
				}
			}
			property_vec.push_back(prop_ptr);
		}

		else if (StartWith(line, related_synonym_target))
		{
			string name = GetLongValue(fp, line, related_synonym_target);
			string text = RemoveEscapeCharacters(GetDefinitionText(name));

            string source = GetDefinitionSource(name);

			Property *prop_ptr = CreateProperty("related_synonym", text);
			vector<string> v = ParseDbXrefString(gp, source);
            for (unsigned int k=0; k<v.size(); k++)
			{
				string val = (string) v[k];
                if (val.compare("") != 0)
				{
					Qualifier *qualifier_ptr = CreateQualifier("dbxref", v[k]);
					prop_ptr->qualifier_vec.push_back(qualifier_ptr);
				}
			}
			property_vec.push_back(prop_ptr);
		}

		else if (StartWith(line, xref_analog_target))
		{
			string xref_analog_value = GetValue(line, xref_analog_target);

			Property *prop_ptr = CreateProperty("xref_analog", xref_analog_value);
			property_vec.push_back(prop_ptr);

		}

        // OBO v1.2 modification
		else if (StartWith(line, xref_analog_target_v2))
		{
			string xref_analog_value = GetValue(line, xref_analog_target_v2);

			Property *prop_ptr = CreateProperty("xref", xref_analog_value);
			property_vec.push_back(prop_ptr);

		}

		else if (StartWith(line, dbxref_target))
		{
			string dbxref_value = GetValue(line, dbxref_target);
			Property *prop_ptr = CreateProperty("dbxref", dbxref_value);
			property_vec.push_back(prop_ptr);
		}

		else if (StartWith(line, obsolete_target))
		{
			string obsolete_str = GetValue(line, obsolete_target);
			if (obsolete_str.compare("true") == 0)
			{
//                sups.push_back("obsolete_" + namespaceStr);
                sups.push_back("ObsoleteClass");
			}
		}


		else
		{
			int n = line.find_first_of(':');
			if (n != -1)
			{
				string propertyname = LSubString(line,n);
				if (!IsDismissed(propertyname))
				{
					string tag = LSubString(line,n+1);
					string propertyvalue = GetValue(line, tag);
					Property *prop_ptr
						= CreateProperty(FindPropertyName(propertyname), propertyvalue);
					property_vec.push_back(prop_ptr);

				}
			}
		}
    }

    // Last concept
	fprintf(gp, "\n");

                 PrintConceptDef(gp,
						  GetName(id),
						  code,
						  id,
						  namespaceName,
                          isPrimitive,
                          kind,
						  sups,
		                  role_modifier_vec,
						  rolename_vec,
						  rolevalue_vec,
						  propertyname_vec,
						  propertyvalue_vec,
						  property_vec);

				 for (unsigned int k=0; k<property_vec.size(); k++)
				 {
					 Property *ptr = (Property *) property_vec[k];
					 for (unsigned int k2=0; k2<ptr->qualifier_vec.size(); k2++)
					 {
						Qualifier *ptr2 = (Qualifier *) ptr->qualifier_vec[k2];
						delete ptr2;
					 }
					 ptr->qualifier_vec.clear();
					 delete ptr;
				 }
				 property_vec.clear();


			 sups.clear();
			 role_modifier_vec.clear();
			 rolename_vec.clear();
			 rolevalue_vec.clear();
			 propertyname_vec.clear();
			 propertyvalue_vec.clear();

	 PrintObsoleteConcepts(gp);

	 fprintf(gp, "\n</terminology>");

	 fclose(fp);
	 fclose(gp);


     char b[255];
	 sprintf(b, "Output file %s generated. Number of terms: %d", (const char *) m_output.c_str(), knt);
 	 cout << b << endl;
	 id2name.clear();
	 conceptname_tbl.clear();
	 propertyname_tbl.clear();

	 role_modifier_vec.clear();
	 rolename_vec.clear();
	 rolevalue_vec.clear();
	 propertyname_vec.clear();
	 propertyvalue_vec.clear();
}


int main (int argc, char * argv[])
{
	vector<string> commLine;
	for ( int i = 0; i < argc; i++)
    {
		commLine.push_back(*argv++);
    }

	if( commLine.size() != 5 ) {  // i.e. it takes four file names
		cerr << "Error:  incorrect number of command line parameters." << endl;
		cerr << "\nUsage:\nOBO2TDE inputfile templatefile tagheaderfile outputfile" << endl ;
		exit(0);
		}

        // Findbinary source, from another standalone program.
        // RWW - Added a boolean to kickout processing if bad characters exist.

        bool dataIsGood = true;

	ifstream inputFile;
	string fileName = commLine[1].c_str();
	inputFile.open(fileName, ios::binary | ios::in);
	if (inputFile.bad()) {
		cerr << "Can't open \"" << commLine[1].c_str() << ".\"  Will EXIT." << endl;
		exit(0);
		}

	/**
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
					dataIsGood = false;  // RWW 080711
					}
				}
			else {
				mStrbuffer.clear();
				if( tmpChar == 13 )
					lineNumber++;
				}
			}
		else
			mStrbuffer.push_back(tmpChar);
		}
	cout << "Read " << filePos << " bytes" << endl;
	inputFile.close();
	**/

    if( dataIsGood )
        Convert(commLine[1], commLine[2], commLine[3], commLine[4]);
    else {
        cout << "Binary characters were found within the input file." << endl;
        cout << "These need to be reviewed and removed manually." << endl;
        cout << "Processing will end now." << endl;
    }    

    return 0;
}


/*
<property>
<name>Synonym</name>
<value>adaxial cells" RELATED PLURAL</value>
</property>
*/
