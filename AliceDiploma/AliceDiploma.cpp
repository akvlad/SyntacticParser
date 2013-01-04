// AliceDiploma.cpp: определ€ет точку входа дл€ консольного приложени€.
//

#include "stdafx.h"
#include <regex>
#include <string>
#include <string.h>
#include <windows.h>

#include "LemmOrganizer.h"
#include "union_pattern.h"
#include "Segment.h"

using namespace std;

vector<union_pattern>un_pattterns;

vector<string> additional_patterns;

set<string> un_set;

bool operator < (string s1, string s2)
{
	return lexicographical_compare(s1.begin(),s1.end(),s2.begin(),s2.end());
}

int _tmain(int argc, _TCHAR* argv[])
{
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	un_pattterns=create_patterns_map(un_set);
	cout<<"¬ведите предложение \n";
	char sentence[1000];
	cin.getline(sentence,1000);
	int len=strlen(sentence);
	for(int i=0;i<strlen(sentence);++i)
	{
		sentence[i]=tolower(sentence[i],locale("Russian"));
	}
	LemmOrganizer lo("words.txt");
	vector<Word> res=divide(sentence, additional_patterns, &lo);
	set_unions(res,un_set,un_pattterns);
	vector<Segment> segments=get_segments(res);
	for(vector<Segment>::iterator it=segments.begin();it!=segments.end();++it)
	{
		setSegment(*it);
		//if()
	}
	for(vector<Segment>::iterator it=segments.begin();it!=segments.end();++it){
		cout<<it->m_type;
		for(vector<Word>::iterator it2=it->m_left_border;it2!=it->m_right_border;++it2)
		{
			cout<<it2->get_string()<<" "<<it2->m_MI<<" ";
		}
		cout<<"\n";
	}
	return 0;
}
