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
	/*    the function helps to identify the starting keyword and breaks the sql statement intotoken */
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
void print(set<vector<string> > result)
{
	/* given a matrix of strings  this function prints the matrix */
	vector<vector<string> >:: iterator it;
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
vector<vector<string> > table_func(string file,vector<string> attributes)
{
	/* This function extracts given set of attributes from a file */
	ifstream f;
	vector<vector<string> > s;
	vector < string > list, record;

	char data[200];
	f.open(file.c_str());

	f.getline(data,200);
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
		record = get_record(data);
		for(int i=0; i<attributes.size() ;i++)
		{
			temp.push_back(record[mp[attributes[i]]]);
		}
		s.push_back(temp);
		temp.clear();
	}
	return s;
}
//----------------------------------------------------------------------------------------------
void execute_project(string sql_statement,vector<vector<string> >);
set<vector<string> > execute_select(string,vector<vector<string> >);
set<vector<string> > execute_intersect(string);
set<vector<string> > execute_union(string);
set<vector<string> > execute_minus(string);
set<vector<string> > execute_crossProduct(string,vector<vector<string> >);
map<string,string > rename(string);
void execute_nested();

int main()
{
	// Read the queries from the file query.txt and save the result in outputf.txt
	// --------------------------------------------------------------------------------------
	//freopen("query_file.txt","r",stdin);
	//freopen("output_file.txt","w",stdout);
	// --------------------------------------------------------------------------------------
	int query_count = 1;
	while(true)
	{
		string sql_statement;
		getline(cin,sql_statement);
		vector<string> token = get_token(sql_statement);
		vector<vector<string> > dummy;

		cout << "query_count = "<<query_count<< endl;

		if(token[0]=="exit")           break;
		else if(token[0]=="Select")    print(execute_select(sql_statement,dummy));
		else if(token[0]=="Project")   execute_project(sql_statement,dummy);
		else if(token[0]=="Cross_product") print(execute_crossProduct(sql_statement,dummy));
		else if(token[0]=="rename")    rename_map = rename(sql_statement);
		else if(token[0]=="nested")    execute_nested();
		query_count ++;
	}
	return 0;
}

void execute_project(string sql_statement,vector<vector<string> > s)
{
	vector<string> token = get_token(sql_statement);
	if(s.empty())
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
		set<vector<string> > result;

		f.open(filename.c_str());

		f.getline(data,200);
		attribute_list = get_record(data);
		map < string , int > mp;

		if(check)
		{
			for(int i=1;i<token.size()-1;i++)token[i] = rename_map[token[i]];
		}
		// map index to attributes
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
			if(!temp.empty())result.push_back(temp);
		}
		print(result);
		f.close();
	}
	else
	{
		vector<vector<string> >::iterator it = s.begin();
		vector<int> index;
		vector<string> t = *it;
		for(int i=1;i<token.size();i++)
		{
			for(int j=0;j<t.size();j++)
			{
				if(token[i] == t[j]) index.push_back(j);
			}
		}
		it++;
		while(it!=s.end())
		{
			t=*it;
			for(int i=0;i<index.size();i++)cout<<t[index[i]]<<" ";
			cout<<endl;
			it++;
		}
	}
}
//----------------------------------------------------------------------------------------------
vector<vector <string> > execute_select(string sql_statement,vector<vector<string> > s)
{
	vector<string> token = get_token(sql_statement);
	int len = token.size();
	vector<vector <string> > result;
	if(s.empty())
	{
		vector<string> token = get_token(sql_statement);
		ifstream f;
		string filename = token[len-1];
		// This to just confirm if rename was used for the sql command.
		int check = 0;
		if(rename_map.find(filename) != rename_map.end())
		{
			filename = rename_map[filename];
			check = 1;
		}
		filename = filename + ".txt";
		if(!ifstream( filename.c_str())){cout<<"No file exits\n\n";return result;}

		char data[200];
		vector< string > record, attribute_list;

		f.open(filename.c_str());

		f.getline(data,200);
		attribute_list = get_record(data);
		result.push_back(attribute_list);
		map < string , int > mp;

		for(int i = 0;i < attribute_list.size() ; i++)
		{
			mp[attribute_list[i]] = i;
		}
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
				oper = token[i+1];
				val = token[i+2];
				if(oper == "=" && val == record[mp[attribute]] )c++;
				else if(oper == ">" && val < record[mp[attribute]] )c++;
				else if(oper == ">=" && val <= record[mp[attribute]] )c++;
				else if(oper == "<>" && val != record[mp[attribute]] )c++;
				else if(oper == "<" && val > record[mp[attribute]] )c++;
				else if(oper == "<=" && val >= record[mp[attribute]] )c++;
			}
			if(c == condition_count)result.push_back(record);
		}
		f.close();
	}
	else
	{
		set<vector<string> >:: iterator it = s.begin();
		vector<string> attribute_list,record;
		attribute_list = *it;
		string attribute, oper, val;
		it++;
		map < string , int > mp;
		for(int i = 0;i < attribute_list.size() ; i++)
		{
			mp[attribute_list[i]] = i;
		}
		int condition_count = (token.size()-1)/3;
		while(it!=s.end())
		{
			record = *it;
			int c = 0;
			for(int i=1;i<len-1;i+=3)
			{
				attribute = token[i];
				oper = token[i+1];
				val = token[i+2];
				if(oper == "=" && val == record[mp[attribute]] )c++;
				else if(oper == ">" && val < record[mp[attribute]] )c++;
				else if(oper == ">=" && val <= record[mp[attribute]] )c++;
				else if(oper == "<>" && val != record[mp[attribute]] )c++;
				else if(oper == "<" && val > record[mp[attribute]] )c++;
				else if(oper == "<=" && val >= record[mp[attribute]] )c++;
			}
			if(c == condition_count)result.push_back(record);
			it++;
		}
	}
	return result;
}

//----------------------------------------------------------------------------------------------
vector<vector<string> > execute_crossProduct(string sql_statement,set<vector<string> > s)
{

	vector<string> token = get_token(sql_statement);
	vector<vector<string> > result;
	if(s.empty())
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
		set< vector<string> > s1 , s2;
		set< vector<string> >:: iterator it1, it2;

		ifstream f;
		if(!ifstream( file1.c_str()) || !(ifstream(file2.c_str()))){cout<<"One of the files do not exits\n\n";return result;}

		vector <string> att1,att2;
		char data[200];

		// for table 1 ----------------------------------
		f.open(file1.c_str());
		f.getline(data,200);
		att1 = get_record(data);
		s1 = table_func(file1,att1);
		f.close();

		// for table 2------------------------------------
		f.open(file2.c_str());
		f.getline(data,200);
		att2 = get_record(data);
		s2 = table_func(file2,att2);

		for(int i=0;i<att2.size();i++)att1.push_back(att2[i]);
		result.push_back(att1);

		vector<string> temp1, temp2;
		for(it1=s1.begin();it1!=s1.end();it1++)
		{
			for(it2=s2.begin();it2!=s2.end();it2++)
			{
				temp1 = *it1;
				temp2 = *it2;
				for(int i=0 ;i<temp2.size();i++)temp1.push_back(temp2[i]);
				result.push_back(temp1);
			}
		}
		f.close();
	}
	else 
	{
		ifstream f;
		string file = token[1];
		if(rename_map.find(file) != rename_map.end()) file = rename_map[file];
		file = file +".txt";
		if(!ifstream(file.c_str())){cout<<"file do not exits\n\n";return result;}
		vector <string> att,temp;
		char data[200];

		// for table 1 ----------------------------------
		f.open(file.c_str());
		f.getline(data,200);	
		att = get_record(data);
		set<vector<string> > s1 = table_func(file,att);
		f.close();
		set<vector<string> >::iterator it, it1, it2;
		it=s.begin();
		temp = *it;
		it++;
		for(int i=0;i<temp.size();i++)att.push_back(temp[i]);
		result.push_back(att);

		vector<string> temp1, temp2;
		for(it1=s1.begin();it1!=s1.end();it1++)
		{
			for(it2=it;it2!=s.end();it2++)
			{
				temp1 = *it1;
				temp2 = *it2;
				for(int i=0 ;i<temp2.size();i++)temp1.push_back(temp2[i]);
				result.push_back(temp1);
			}
		}
	}
	return result;
}

map<string,string> rename(string sql_statement)
{
	/* The idea used here is create a mapping from new tablename to new tablename and 
	a mapping from new attributes to old attributes.
	*/
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

void execute_nested()
{
	string sql;
	vector<vector<string> >result;
	while(true)
	{
		getline(cin,sql);
		vector<string> token = get_token(sql);
		if(token[0]=="close")break;
		else if(token[0]=="Select") result = execute_select(sql,result);
		else if(token[0]=="Project") execute_project(sql,result);
		else if(token[0]=="Cross_product") result = execute_crossProduct(sql,result);
	}
	vector<vector<string> >::iterator it;
	for(it = result.begin();it!=result.end();it++)
	{
		vector<string> t;*it;
		for(int i=0;i<t.size();i++)cout<<t[i]<<" ";
		cout<<endl;
	}
}
//----------------------------------------END OF THE PROJECT !-----------------------------------