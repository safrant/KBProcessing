//#include "stdafx.h"

#include "OntyxKBQA.h"


	OntyxKBQA::OntyxKBQA()
	{

	}


	OntyxKBQA::OntyxKBQA(OntyxKb &kb)
	{
		kb = kb;
		begin_it = kb.getFirstConceptIterator();
		end_it = kb.getEndConceptIterator();
	}

	OntyxKBQA::~OntyxKBQA()
	{
	}

	void OntyxKBQA::setKB(OntyxKb &kb)
	{ 
		kb = kb;
	}


	void OntyxKBQA::performQA(string outfile)
	{
		if ((gp = fopen((const char *)outfile.c_str(), "w")) == NULL)
		{
			char buf[255];
			sprintf(buf, "Cannot open file %s", 
				(const char *)outfile.c_str());
			cerr << "Error opening file \"" << buf << "\", will EXIT." << endl;
			exit(0);
		} 

//   ::SetCursor(::LoadCursor(NULL, IDC_WAIT));  

		checkSamePTs();
		checkDuplicateRoles();
		checkDuplicateProperties();
		checkPTExistenceAndUniqueness();
		checkNCIPTFullSynName();
		checkNCIPTFullSynExistenceAndUniqueness();
		checkSemanticTypes("semantictype.dat");
		checkSameAtoms();
		checkHighBitCharacters();
		checkCharacter('\n');
		checkCharacter('@');
		checkCharacter('|');
		

//   ::SetCursor(::LoadCursor(NULL, IDC_ARROW));  

		fclose(gp);

	}


    string OntyxKBQA::XML2Pipe(string pattern) {
		//string pattern = "<term-name>Basal Transcription Factor</term-name><term-group>PT</term-group><term-source>NCI</term-source>";

        int n = pattern.find(">");
        string retstr = "";
        int lcv = 0;
        while (n != -1)
        {
			string tag = pattern.substr(0, n+1);
			pattern = pattern.substr(n+1, pattern.length());
			n = pattern.find("<");
			if (n == -1) break;

			string value = pattern.substr(0, n);
			retstr = retstr + value;
            retstr = retstr + "|";
            lcv++;

			pattern = pattern.substr(n, pattern.length());

			n = pattern.find(">");
			if (n == -1) break;

			tag = pattern.substr(0, n+1);

            if (pattern.length() > tag.length())
            {
				pattern = pattern.substr(n+1, pattern.length());
		    }
		    n = pattern.find(">");
	    }

	    if (lcv > 0)
	    {
			retstr = retstr.substr(0, retstr.length()-1);
		}
		else
		{
			retstr = pattern;
		}
		return(retstr);

	}


    void OntyxKBQA::checkDuplicateRoles()
	{
		fprintf(gp, "Checking duplicate roles ...\n");
		int num = 0;
		vector<string> v;

        map<long, OntyxConcept>::iterator posCon = begin_it;
		for( ; posCon != end_it; ++posCon )
		{
			OntyxConcept c = (OntyxConcept) posCon->second;
			const multimap<long, OntyxRole> roles
				= c.getRoles();

			multimap<long, OntyxRole >::const_iterator pRole = roles.begin();
			for( ; pRole != roles.end(); ++pRole)
			{
				OntyxRole r = (OntyxRole) pRole->second;
				string modifier = r.getModifier();
				string name = r.getName();
				string value = r.getValue();
				string t = modifier + "|" + name + "|" + value;
				v.push_back(t);
			}
			sort(v.begin(), v.end());
			//v = sort(v);
			string s = "";
			for (unsigned i=0; i<v.size(); i++)
			{
				string t = (string) v[i];
				//fprintf(gp, "%s\n", (const char *) t.c_str());
				if (t.compare(s) == 0)
				{
					fprintf(gp, "\t%s\t%s\n",
						(const char *) c.getName().c_str(),
                        (const char *) t.c_str());
					num++;
				}
				s = t;
			}
			v.clear();
		}
		if (num == 0)
		{
			fprintf(gp, "\t None.\n\n");
		}
		else
		{
            fprintf(gp, "\n");
		}
		fprintf(gp, "---------------------------------------------------------\n\n");

	}


	vector<string> OntyxKBQA::getPropertyValue(OntyxConcept &c, string propertyname)
	{
        vector<string> v;
		const multimap<long, OntyxProperty> properties
			= c.getProperties();

		multimap<long, OntyxProperty >::const_iterator pProp = properties.begin();
		for( ; pProp != properties.end(); ++pProp)
		{
			OntyxProperty p = (OntyxProperty) pProp->second;
			if (p.getName().compare(propertyname) == 0)
			{
				v.push_back(p.getValue());
			}
		}
		return v;
	}

/*
    // PT shared by different concepts
	void OntyxKBQA::checkSamePTs()
	{
		fprintf(gp, "Checking PT shared by different concepts ...\n");
		int num = 0;
		string_mmap pt_name_map;

		vector<string> v;
		string propertyname = "Preferred_Name";

		str_map pt_tbl;
		vector<string> pt_vec;

//		map<long, OntyxConcept>::iterator posCon = kb.getFirstConceptIterator();
//		for( ; posCon != kb.getEndConceptIterator(); ++posCon )

        map<long, OntyxConcept>::iterator posCon = begin_it;
		for( ; posCon != end_it; ++posCon )

		{
			OntyxConcept c = (OntyxConcept) posCon->second;
            string name = c.getName();

			vector<string> v = getPropertyValue(c, propertyname);
			for (int i=0; i<v.size(); i++)
			{
				string pt = (string) v[i];
				str_map::iterator it = pt_tbl.find(pt);
				if (it == pt_tbl.end())
				{
                    pt_tbl.insert(str_map::value_type(pt, pt));
					pt_vec.push_back(pt);
				}
                pt_name_map.insert(string_mmap::value_type(pt, name));
			}
		}

		for (int i=0; i<pt_vec.size(); i++)
		{
			string pt = (string) pt_vec[i];
			string_mmap::iterator it2 = pt_name_map.find(pt); // find the first match
			int count = pt_name_map.count(pt);

			if (count > 1)
			{

fprintf(gp, "Preferred_Name (%s) Count : %d\n", (const char *) pt.c_str(), count);

				int knt = 0;
				while (it2 != pt_name_map.end() && knt < count) 
				{
					string s = (*it2).second;
					fprintf(gp, "\tConcept_Name: %s\n", (const char *) s.c_str());

					knt++;
					it2++;
				}
			}
		}
        pt_name_map.clear();
		pt_tbl.clear();
		pt_vec.clear();
	}
*/
    // PT shared by different concepts
	void OntyxKBQA::checkSamePTs()
	{
		fprintf(gp, "Checking PT shared by different concepts ...\n");
//		int num = 0;

		str2vec_map pt_name_map;

		vector<string> v;
		string propertyname = "Preferred_Name";

		str_map pt_tbl;
		vector<string> pt_vec;

        map<long, OntyxConcept>::iterator posCon = begin_it;
		for( ; posCon != end_it; ++posCon )

		{
			OntyxConcept c = (OntyxConcept) posCon->second;
            string name = c.getName();

			vector<string> v = getPropertyValue(c, propertyname);
			for (unsigned i=0; i<v.size(); i++)
			{
				string pt = (string) v[i];
				str_map::iterator it = pt_tbl.find(pt);
				if (it == pt_tbl.end())
				{
                    pt_tbl.insert(str_map::value_type(pt, pt));
					pt_vec.push_back(pt);
				}

				str2vec_map::iterator it2 = pt_name_map.find(pt);
				if (it2 == pt_name_map.end())
				{
					vec_wrapper *ptr = new vec_wrapper();
					ptr->v.push_back(name);
                    pt_name_map.insert(str2vec_map::value_type(pt, ptr));
				}
				else
				{
                    vec_wrapper *ptr = (vec_wrapper *) (*it2).second;
					ptr->v.push_back(name);
				}
			}
		}

		for (unsigned i=0; i<pt_vec.size(); i++)
		{
			string pt = (string) pt_vec[i];
			str2vec_map::iterator it2 = pt_name_map.find(pt); // find the first match

			vec_wrapper *ptr = (*it2).second;

			int count = ptr->v.size();

			if (count > 1)
			{

fprintf(gp, "\tPreferred_Name (%s) Count : %d\n", (const char *) pt.c_str(), count);
                for (unsigned j=0; j < ptr->v.size(); j++)
				{
					string s = (string)ptr->v[j];
					fprintf(gp, "\tConcept_Name: %s\n", (const char *) s.c_str());
				}
				fprintf(gp, "\n");

			}
			ptr->v.clear();
			delete ptr;
		}
        pt_name_map.clear();
		pt_tbl.clear();
		pt_vec.clear();
		fprintf(gp, "---------------------------------------------------------\n\n");
	}



    void OntyxKBQA::checkDuplicateProperties()
	{
		fprintf(gp, "Checking duplicate properties ...\n");
		int num = 0;
		vector<string> v;

        map<long, OntyxConcept>::iterator posCon = begin_it;
		for( ; posCon != end_it; ++posCon )
		{
			OntyxConcept c = (OntyxConcept) posCon->second;
			const multimap<long, OntyxProperty> properties
				= c.getProperties();
			string name = c.getName();

			multimap<long, OntyxProperty >::const_iterator pProp = properties.begin();
			for( ; pProp != properties.end(); ++pProp)
			{
				OntyxProperty r = (OntyxProperty) pProp->second;
				string name = r.getName();
				string value = r.getValue();
				string t = name + "|" + value;
				v.push_back(t);
			}
			sort(v.begin(), v.end());
			string s = "";
			for (unsigned i=0; i<v.size(); i++)
			{
				string t = (string) v[i];
				if (t.compare(s) == 0)
				{
					fprintf(gp, "\t%s\t%s\n",
						(const char *) name.c_str(),
                        (const char *) t.c_str());
					num++;
				}
				s = t;
			}

			v.clear();
		}
		if (num == 0)
		{
			fprintf(gp, "\t None.\n\n");
		}
		else
		{
            fprintf(gp, "\n");
		}
		fprintf(gp, "---------------------------------------------------------\n\n");

	}


	void OntyxKBQA::checkPTExistenceAndUniqueness()
	{
		fprintf(gp, "Checking existence and uniqueness of Preferred_Name property...\n");
		vector<string> v;
		string propertyname = "Preferred_Name";

        map<long, OntyxConcept>::iterator posCon = begin_it;
		for( ; posCon != end_it; ++posCon )
		{
			OntyxConcept c = (OntyxConcept) posCon->second;
            string name = c.getName();

			vector<string> v = getPropertyValue(c, propertyname);
			if (v.size() == 0)
			{
				fprintf(gp, "Concept %s does not have any Preferred_Name property.\n", (const char *) name.c_str());
			}
			else if (v.size() > 1)
			{
				fprintf(gp, "\tConcept %s has multiple Preferred_Name properties.\n", (const char *) name.c_str());

				for (unsigned i=0; i<v.size(); i++)
				{
					string pt = (string) v[i];
				    fprintf(gp, "\t%s\n", (const char *) pt.c_str());
				}
				v.clear();
			}
		}
		fprintf(gp, "---------------------------------------------------------\n\n");

	}


////////////////////////////////////////////////////////////////////////////////////////
    vector<string> OntyxKBQA::tokenize(string pattern0) // bar delimited string
	{
		string pattern = pattern0 + "|";
		vector<string> v;
		char b[2048];
		sprintf(b, "%s", (const char *) pattern.c_str());
		char *p = strtok(b, "|\n");
		while (p != NULL)
		{
			string s = p;
			v.push_back(s);
            p = strtok(NULL, "|\n");
		}
		return v;
	}

	
	void OntyxKBQA::checkNCIPTFullSynName()
	{
		fprintf(gp, "Checking consistency of NCI PT FULL_SYN name...\n");

		string preferredname = "Preferred_Name";
		string fullsyn = "FULL_SYN";

        map<long, OntyxConcept>::iterator posCon = begin_it;
		for( ; posCon != end_it; ++posCon )
		{
			OntyxConcept c = (OntyxConcept) posCon->second;
			string name = c.getName();
			
            vector<string> v1 = getPropertyValue(c, preferredname);
            vector<string> v2 = getPropertyValue(c, fullsyn);

			if (v1.size() > 0 && v2.size() > 0)
			{
				string pt = (string) v1[0];
				string full_syn_value = (string) v2[0];
				vector<string> full_syn_vec = tokenize(XML2Pipe(full_syn_value));
				if (full_syn_vec[1].compare("PT") == 0 &&
                    full_syn_vec[2].compare("NCI") == 0)
				{
					if (pt.compare(full_syn_vec[0]) != 0)
					{
					fprintf(gp, "\tConcept %s PT and NCI PT FULL_SYN have different names.\n",
						(const char *) name.c_str()); 
					fprintf(gp, "\t\tPreferred_Name: %s\n", (const char *) pt.c_str());
					fprintf(gp, "\t\tFUYLL_SYN: %s\n\n", (const char *) full_syn_value.c_str());

					}
				}
			}

		}
		fprintf(gp, "---------------------------------------------------------\n\n");

	}



	void OntyxKBQA::checkNCIPTFullSynExistenceAndUniqueness()
	{
		fprintf(gp, "Checking existence and uniqueness of NCI PT FULL_SYN property ...\n");
        vector<string> u;
		vector<string> full_syn_vec;
		string propertyname = "FULL_SYN";
        map<long, OntyxConcept>::iterator posCon = begin_it;
		for( ; posCon != end_it; ++posCon )
		{
			OntyxConcept c = (OntyxConcept) posCon->second;
            string name = c.getName();
//            bool found = false;
			int knt = 0;
			vector<string> v = getPropertyValue(c, propertyname);
			
			for (unsigned i=0; i<v.size(); i++)
			{
				string fullsyn_str = (string) v[i];
				full_syn_vec = tokenize(XML2Pipe(fullsyn_str));

				if (full_syn_vec.size() != 3 && full_syn_vec.size() != 4)
				{
					fprintf(gp, "\tConcept %s does not have a valid FULL_SYN format.\n",
						(const char *) name.c_str()); 
					fprintf(gp, "\t%s\n",
						(const char *) fullsyn_str.c_str());
				}
				else
				{
					if (full_syn_vec[1].compare("PT") == 0 &&
						full_syn_vec[2].compare("NCI") == 0)
					{
						knt++;
						u.push_back(fullsyn_str);
					}

				}
			}

			if (knt == 0)
			{
				fprintf(gp, "\tConcept %s does not have any NCI PT FULL_SYN property.\n\n",
					(const char *) name.c_str()); 
			}
			else if (knt > 1)
			{
				fprintf(gp, "\tConcept %s have multiple NCI PT FULL_SYN properties.\n\n",
					(const char *) name.c_str()); 
				for (unsigned i=0; i<u.size(); i++)
				{
					string fullsyn_str = (string) v[i];
					fprintf(gp, "\t\t%s\n", (const char *) fullsyn_str.c_str());
				}
                fprintf(gp, "\n");
			}
			u.clear();
		}
		fprintf(gp, "---------------------------------------------------------\n\n");

	}

	void OntyxKBQA::checkSemanticTypes(string filename)
	{
		FILE *fp;
		if ((fp = fopen((const char *)filename.c_str(), "r")) == NULL)
		{
			char buf[255];
			sprintf(buf, "Cannot open file %s", 
				(const char *)filename.c_str());
			cerr << "Error opening file \"" << buf << "\", will EXIT." << endl;

sprintf(buf, "Cannot open %s", (const char *) filename.c_str());
//			AfxMessageBox(buf);
			exit(0);
		} 

		str_map sty_tbl;
		char buf[255];
		while(!feof(fp) && (fgets(buf, 255, fp) != NULL))
		{
			string line = buf;
			if (line[line.length()-1] == '\n')
			{
				line = line.substr(0, line.length()-1);
			}
			sty_tbl.insert(str_map::value_type(line, line));
			
		}
		fclose(fp);

		fprintf(gp, "Checking Semantic Type ...\n");
		vector<string> v;
		string propertyname = "Semantic_Type";

		int num = 0;
        map<long, OntyxConcept>::iterator posCon = begin_it;
		for( ; posCon != end_it; ++posCon )
		{
			OntyxConcept c = (OntyxConcept) posCon->second;
            string name = c.getName();

			vector<string> v = getPropertyValue(c, propertyname);
			for (unsigned i=0; i<v.size(); i++)
			{
				string sty = (string) v[i];
				str_map::iterator it = sty_tbl.find(sty);
				if (it == sty_tbl.end())
				{
				fprintf(gp, "\tConcept %s with unknown semantic type: %s.\n\n", 
					(const char *) name.c_str(),
					(const char *) sty.c_str());
				}
			}
			v.clear();
		}
		sty_tbl.clear();

		if (num == 0)
		{
			fprintf(gp, "\t None.\n\n");
		}
		else
		{
            fprintf(gp, "\n");
		}
		fprintf(gp, "---------------------------------------------------------\n\n");

	}


    // same source, code, termgroup in different concepts
	void OntyxKBQA::checkSameAtoms()
	{
		fprintf(gp, "Checking different concepts sharing the same FULL_SYN property ...\n");
		vector<string> v;
		string propertyname = "FULL_SYN";

		str_map full_syn_tbl;

        map<long, OntyxConcept>::iterator posCon = begin_it;
		for( ; posCon != end_it; ++posCon )
		{
			OntyxConcept c = (OntyxConcept) posCon->second;
            string name = c.getName();

			vector<string> v = getPropertyValue(c, propertyname);
			for (unsigned i=0; i<v.size(); i++)
			{
				string full_syn_value = (string) v[i];
				str_map::iterator it = full_syn_tbl.find(full_syn_value);
				if (it == full_syn_tbl.end())
				{
                    full_syn_tbl.insert(str_map::value_type(full_syn_value, name));
				}
				else
				{
					string t = (string) it->second;
					fprintf(gp, "\tConcepts with same FULL_SYN value: %s.\n", 
						(const char *) full_syn_value.c_str());

					fprintf(gp, "\t\t%s\n", 
						(const char *) t.c_str());
					fprintf(gp, "\t\t%s\n\n", 
						(const char *) name.c_str());

				}
			}
			v.clear();
		}
		full_syn_tbl.clear();
		fprintf(gp, "---------------------------------------------------------\n\n");

	}



	int OntyxKBQA::checkHighBitCharacters(string name, 
		                                   string propertyname,
		                                   string s)
	{
		int knt = 0;
		int len = s.length();
		for (int i=0;i<len;i++)
		{
			char c = s[i];
			if ((c < 32) || (c > 126))
			{
				if ((c != 10) && (c != 13) && (c != 9))
				{
					knt++;
					fprintf(gp,"Concept: %s Property: %s\n",
                        (const char *) name.c_str(),
						(const char *) propertyname.c_str());

					fprintf(gp,"Value: %s Position:%d Char:%c\n\n",
						(const char *) s.c_str(), i+1, s[i]);
				}
			}
        }
		return knt;
	}



	void OntyxKBQA::checkHighBitCharacters()
	{
		fprintf(gp, "Checking High Bit Characters ...\n");

        map<long, OntyxConcept>::iterator posCon = begin_it;
		for( ; posCon != end_it; ++posCon )
		{
			OntyxConcept c = (OntyxConcept) posCon->second;
			const multimap<long, OntyxProperty> properties
				= c.getProperties();

			multimap<long, OntyxProperty >::const_iterator pProp = properties.begin();
			for( ; pProp != properties.end(); ++pProp)
			{
				OntyxProperty p = (OntyxProperty) pProp->second;
	            int knt = checkHighBitCharacters(c.getName(), 
					                   p.getName(), 
									   p.getValue()); 
				if (knt > 0) fprintf(gp, "\n");
			}
		}
		fprintf(gp, "---------------------------------------------------------\n\n");
	}


	void OntyxKBQA::checkCharacter(char c)
	{
		char b[2];
		sprintf(b, "%c", c);
		string target = b;

		if (target.compare("\n") == 0)
		{
			fprintf(gp, "Checking property values containing carriage return character.\n"); 
        }
		else
		{
			fprintf(gp, "Checking property values containing character %s.\n", 
				(const char *) target.c_str());

		}
        map<long, OntyxConcept>::iterator posCon = begin_it;
		for( ; posCon != end_it; ++posCon )
		{
			OntyxConcept c = (OntyxConcept) posCon->second;
			string name = c.getName();
			const multimap<long, OntyxProperty> properties
				= c.getProperties();

			multimap<long, OntyxProperty >::const_iterator pProp = properties.begin();
			for( ; pProp != properties.end(); ++pProp)
			{
				OntyxProperty p = (OntyxProperty) pProp->second;
				string value = p.getValue();
// gf                if (value.find(target) != -1)
                if (value.find(target) != string::npos)
					{
					if (target.compare("\n") == 0)
					{
						fprintf(gp, "\tConcept %s Property %s contains carriage return character.\n", 
							(const char *) name.c_str(), 
							(const char *) p.getName().c_str());

						fprintf(gp, "\t\t%s\n", (const char *) value.c_str());

					}
					else
					{
						fprintf(gp, "\tConcept %s Property %s contains character %s.\n", 
							(const char *) name.c_str(), 
							(const char *) p.getName().c_str(),
							(const char *) target.c_str());
						fprintf(gp, "\t\t%s\n", (const char *) value.c_str());

					}
				}
			}
		}
		fprintf(gp, "---------------------------------------------------------\n\n");
	}

