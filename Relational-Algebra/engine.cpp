//------------------------------------------------------------------------------------------------
//-----------------------------Assignment 2:Relational Algebra------------------------------------

#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <map>
#include <stdio.h>

using namespace std;
map<string,string> rename_map;

vector<string> get_token(string str)
{
	/*    the function helps to identify the starting keyword and breaks the sql statement into tokens
	on encountering a space */
	vector<string> v;
	string tmp="";
	for(int i = 0 ; i < str.size(); i++ )
	{
		if(str[i]==' ')
			{
				if(tmp=="")continue;
				v.push_back(tmp);
				tmp="";
			}
		else tmp=tmp+str[i];
	}
	v.push_back(tmp);
	return v;
}
//----------------------------------------------------------------------------------------------

vector< string > get_record(char *data)
{
	/*   this function extracts the tuples from the table */
	/*   works for file space separated or comma separated */
	vector<string> record;
	int i=0;
	string tmp="";
	while(data[i]!='\0')
	{
		if(data[i]==',' || data[i]==' ')
			{
				if(tmp!="")record.push_back(tmp);
				tmp="";
			}
		else tmp=tmp+data[i];
		i++;
	}
	record.push_back(tmp);
	return record;
}
//----------------------------------------------------------------------------------------------
void print(set< vector<string> > result)
{
	/* given a matrix of strings  this function prints the matrix */
	set< vector<string> >:: iterator it;
	vector<string> vtmp;
	for(it=result.begin();it!=result.end();it++)
	{
		vtmp=*it;
		for(int i=0;i<vtmp.size();i++)cout<<vtmp[i]<<" ";
		cout<<endl;
	}
	cout<<endl;
}
//----------------------------------------------------------------------------------------------
set< vector <string> > table_func(string file,vector<string> attributes)
{
	/* This function extracts given set of attributes from a file */
	ifstream f;
	set < vector <string> > s; // to store the result
	vector < string > list, record;

	char data[200];
	f.open(file.c_str());

	f.getline(data,200);
	// get the list of attributes from the file
	list = get_record(data);
	/* map data structure is used to map attributes to their locations in actual file or table */
	map < string , int > mp;

	for(int i = 0;i < list.size() ; i++)
	{
		mp[list[i]] = i;
	}
	vector<string> temp;
	while(!f.eof())
	{
		f.getline(data,200);
		// use get_record function to get the record and use the above map for corresponding values
		record = get_record(data);
		for(int i=0; i<attributes.size() ;i++)
		{
			temp.push_back(record[mp[attributes[i]]]);
		}
		s.insert(temp);
		temp.clear();
	}
	return s;
}
//----------------------------------------------------------------------------------------------
void execute_project(vector<string>);
set<vector<string> > execute_select(vector<string>);
set<vector<string> > execute_intersect(string);
set<vector<string> > execute_union(string);
set<vector<string> > execute_minus(string);
set<vector<string> > execute_crossProduct(vector<string>);
map<string,string > rename(string);

int main()
{
	// Read the queries from the file query.txt and save the result in outputf.txt
	// --------------------------------------------------------------------------------------
	freopen("query_file.txt","r",stdin);
	freopen("output_file.txt","w",stdout);
	// --------------------------------------------------------------------------------------
	int query_count = 1;
	while(true)
	{
		string sql_statement;
		getline(cin,sql_statement);
		vector<string> token = get_token(sql_statement);
		cout << "query_count = "<<query_count<< endl;
		if(token[0]=="exit")           break;
		else if(token[0]=="Select")    print(execute_select(token));
		else if(token[0]=="Project")   execute_project(token);
		else if(token[0]=="Intersect") print(execute_intersect(sql_statement));
		else if(token[0]=="Union")     print(execute_union(sql_statement));
		else if(token[0]=="Minus")     print(execute_minus(sql_statement));
		else if(token[0]=="Cross_product") print(execute_crossProduct(token));
		else if(token[0]=="rename")    rename_map = rename(sql_statement);
		query_count ++;
	}
	return 0;
}

void execute_project(vector<string> token)
{
	ifstream f;
	int len = token.size();
	string filename = token[len-1];
	// This to just confirm if rename was used for the sql command.
	int check = 0;
	if(rename_map.find(filename) != rename_map.end())
	{
		filename = rename_map[filename];
		check = 1;
	}
	filename=filename+".txt";
	if(!ifstream( filename.c_str())){cout<<"No file exits\n\n";return;}

	char data[200];
	vector< string > record, attribute_list;
	set< vector <string> > result;

	f.open(filename.c_str());

	f.getline(data,200);
	// get the list of attributes from the file
	attribute_list = get_record(data);
	map < string , int > mp;
	// if rename was used change to actual attributes.
	if(check)
	{
		for(int i=1;i<token.size()-1;i++)token[i] = rename_map[token[i]];
	}
	// map -> index to attributes
	for(int i = 0;i < attribute_list.size() ; i++)
	{
		mp[attribute_list[i]] = i;
	}

	// index of the column which needs to be printed
	vector<int> index;
	for(int i=1 ;i<token.size()-1;i++)
	{
		for(int j=0;j<attribute_list.size();j++)
		{
			if(token[i] == attribute_list[j])
			{
				index.push_back(j);
			}
		}
	}
	while(!f.eof())
	{
		f.getline(data,200);
		record = get_record(data);
		vector<string> temp;
		for(int i=0;i<index.size();i++)temp.push_back(record[index[i]]);
		if(!temp.empty())result.insert(temp);
	}
	print(result);
	f.close();
}
//----------------------------------------------------------------------------------------------
set<vector <string> > execute_select(vector<string> token)
{
	ifstream f;
	int len = token.size();
	string filename = token[len-1];
	// This to just confirm if rename was used for the sql command.
	int check = 0;
	if(rename_map.find(filename) != rename_map.end())
	{
		filename = rename_map[filename];
		check = 1;
	}
	set< vector <string> > result;
	filename = filename + ".txt";
	if(!ifstream( filename.c_str())){cout<<"No file exits\n\n";return result;}

	char data[200];
	vector< string > record, attribute_list;

	f.open(filename.c_str());

	f.getline(data,200);
	attribute_list = get_record(data);
	map < string , int > mp;

	for(int i = 0;i < attribute_list.size() ; i++)
	{
		mp[attribute_list[i]] = i;
	}

	// first keyword is select and last is the tablename and rest (att operator value) 
	int condition_count = (token.size()-2)/3;
	string attribute, oper, val;
	while(!f.eof())
	{
		int c = 0;
		f.getline(data,200);
		record = get_record(data);
		for(int i=1;i<len-1;i+=3)
		{
			if(check)attribute = rename_map[token[i]];
			else attribute = token[i];
			// get the operator value and the corresponding attribute..
			oper = token[i+1];
			val = token[i+2];
			if(oper == "=" && val == record[mp[attribute]] )c++;
			else if(oper == ">" && val < record[mp[attribute]] )c++;
			else if(oper == ">=" && val <= record[mp[attribute]] )c++;
			else if(oper == "<>" && val != record[mp[attribute]] )c++;
			else if(oper == "<" && val > record[mp[attribute]] )c++;
			else if(oper == "<=" && val >= record[mp[attribute]] )c++;
		}
		// since "&&" is implemented by default check if all conditions are met 
		if(c == condition_count)result.insert(record);
	}
	f.close();
	return result;
}

//----------------------------------------------------------------------------------------------
set<vector <string > > execute_intersect(string sql_statement)
{
	string file1,file2,tmp;
	vector< string > attributes, list, record, temp1, temp2;
	int c=0;
	// parse the sql statement to get the tablename and set of attributes
	// 
	for(int i=11;i<sql_statement.size();i++)
	{
		if(sql_statement[i]==',' || sql_statement[i]==')')
		{
			if(c==0)file1 = tmp;
			else if(c==1)file2 = tmp;
			else attributes.push_back(tmp); 
			tmp = "";
			c++;
		}
		else tmp += sql_statement[i];
	}
	//------------------------------------------------------------------------------------------------------
	// check if one of the files is an invalid one

	file1 = file1+".txt";
	file2 = file2+".txt";
	set< vector <string> > s1 , s2, result;
	if(!ifstream(file1.c_str()) || !ifstream(file2.c_str())){cout<<"One of the file does not exit.\n\n";return result;}
	//-------------------------------------------------------------------------------------------------------
	// get the data into two set s1  and s1 
	s1 = table_func(file1,attributes);
	s2 = table_func(file2,attributes);
	set< vector<string> >::iterator it1,it2;
	/*
	alternate method
	bool flag;
	for(it1 = s1.begin() ;it1 != s1.end() ;it1++)
	{
		temp1 = *it1;
		for(it2 = s2.begin(); it2 != s2.end(); it2++)
		{
			temp2 = *it2;
			flag = true ; 
			for( int i=0;i<temp1.size() ;i ++)
			{
				if(temp1[i] != temp2[i])flag=false;
			} 
			if(flag)
			{
				result.insert(temp1);
				break;
			}
		}
	}*/
	// if a in s1 and s2 remove a from s1
	for(it1 = s1.begin() ;it1 != s1.end() ;it1++)
	{
		for(it2 = s2.begin(); it2 != s2.end(); it2++)
		{
			if(*it1 == *it2)
			{
				result.insert(*it1);
				break;
			}
		}
	}
	return result;
}

//-----------------------------------------------------------------------------------------------
set<vector <string> > execute_union(string sql_statement)
{
	string file1,file2,tmp;
	vector< string > attributes, list, record, temp1, temp2;
	int c=0;
	// parse the sql statement to get the tablename and set of attributes

	for(int i=7;i<sql_statement.size();i++)
	{
		if(sql_statement[i]==',' || sql_statement[i]==')')
		{
			if(c==0)file1 = tmp;
			else if(c==1)file2 = tmp;
			else attributes.push_back(tmp); 
			tmp = "";
			c++;
		}
		else tmp += sql_statement[i];
	}
	//---------------------------------------------------------------
	// check if one of the files is an invalid one

	file1 = file1+".txt";
	file2 = file2+".txt";
	set< vector <string> > s1 , s2;
	if(!ifstream(file1.c_str()) || !ifstream(file2.c_str())){cout<<"One of the file does not exit.\n\n";return s1;}
	//-------------------------------------------------------------------------------------------------------
	// get the data into s1 and s2.
	s1 = table_func(file1,attributes);
	s2 = table_func(file2,attributes);
	set< vector<string> >::iterator it;
	// insert s2 into s1 .Since set is used no redundant data can present. 
	for(it = s2.begin();it != s2.end() ;it++)
	{
		s1.insert(*it);
	}
	return s1;
}

//----------------------------------------------------------------------------------------------
set<vector <string > > execute_minus(string sql_statement)
{
	string file1,file2,tmp;
	vector< string > attributes, list, record, temp1, temp2;
	int c=0;
	// parse the sql statement to get the tablename and set of attributes

	for(int i=7;i<sql_statement.size();i++)
	{
		if(sql_statement[i]==',' || sql_statement[i]==')')
		{
			if(c==0)file1 = tmp;
			else if(c==1)file2 = tmp;
			else attributes.push_back(tmp); 
			tmp = "";
			c++;
		}
		else tmp += sql_statement[i];
	}
	//------------------------------------------------------------------------------------------------------
	// check if one of the files is an invalid one
	file1 = file1+".txt";
	file2 = file2+".txt";
	set< vector <string> > s1 , s2;
	if(!ifstream(file1.c_str()) || !ifstream(file2.c_str())){cout<<"One of the file does not exit.\n\n";return s1;}
	//-------------------------------------------------------------------------------------------------------

	s1 = table_func(file1,attributes);
	s2 = table_func(file2,attributes);
	set< vector<string> >::iterator it;

	for(it = s2.begin () ; it != s2.end (); it++)
	{
		if(s1.find (*it)!=s1.end())s1.erase(*it);
	}
	return s1;
}

//----------------------------------------------------------------------------------------------
set< vector <string > > execute_crossProduct(vector<string> token)
{
	string file1 = token[1],file2 = token[2];
	if(rename_map.find(file1) != rename_map.end())
	{
		file1 = rename_map[file1];
	}
	if(rename_map.find(file2) != rename_map.end())
	{
		file2 = rename_map[file2];
	}
	file1 = file1 +".txt";
	file2 = file2 +".txt";
	set< vector<string> > s1 , s2, result;
	set< vector<string> >:: iterator it1, it2;

	ifstream f;
	if(!ifstream( file1.c_str()) || !(ifstream(file2.c_str()))){cout<<"One of the files do not exits\n\n";return result;}

	vector <string> att;
	char data[200];

	// for table 1 ----------------------------------
	f.open(file1.c_str());
	f.getline(data,200);
	att = get_record(data);
	s1 = table_func(file1,att);
	f.close();

	// for table 2------------------------------------
	f.open(file2.c_str());
	f.getline(data,200);
	att = get_record(data);
	s2 = table_func(file2,att);

	vector<string> temp1, temp2;
	for(it1=s1.begin();it1!=s1.end();it1++)
	{
		for(it2=s2.begin();it2!=s2.end();it2++)
		{
			temp1 = *it1;
			temp2 = *it2;
			for(int i=0 ;i<temp2.size();i++)temp1.push_back(temp2[i]);
			result.insert(temp1);
		}
	}
	f.close();
	return result;
}

//------------------------------------------------------------------------------------------------
map<string,string> rename(string sql_statement)
{
	/* The idea used here is create a mapping from new tablename to new tablename and 
	a mapping from new attributes to old attributes.
	*/

	/* On encountering a table with new tablename as renamed , in select and project map is
	used to get to the actual table and the set of  attributes.*/ 
	rename_map.clear();
	vector<string> new_att,old_att;
	int c=0;
	string tmp="",table1,table2;
	// new attribute names
	for(int i=8;i<sql_statement.size();i++)
	{
		if(sql_statement[i]==',' || sql_statement[i]==')')
		{
			if(c==0)table1 = tmp;
			else if(c==1)table2 = tmp;
			else new_att.push_back(tmp); 
			tmp = "";
			c++;
		}
		else tmp += sql_statement[i];
	}
	string file = table1+".txt";
	ifstream f;
	map < string , string > mp;
	if(!ifstream(file.c_str())){cout<<"No file exits\n\n";return mp;}
	char data[200];
	f.open(file.c_str());
	f.getline(data,200);
	old_att = get_record(data);
	f.close();

	mp[table2] = table1;
	for(int i=0;i<new_att.size();i++)
	{
		mp[new_att[i]] = old_att[i];
	}
	return mp;
}


//----------------------------------------END OF THE PROJECT !-----------------------------------