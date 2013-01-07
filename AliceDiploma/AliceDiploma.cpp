// AliceDiploma.cpp: определяет точку входа для консольного приложения.
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

	// загрузили союзы-разделители сегментов.
	un_pattterns=create_patterns_map(un_set);
	// загрузили базу лемм.
	LemmOrganizer lo("words.txt");

	cout<<"Введите предложение \n";
	const int powerOfTwo = 1024;
	char sentence[powerOfTwo];
	cin.getline(sentence, powerOfTwo);
	int len=strlen(sentence);
	for(int i=0;i<strlen(sentence);++i)
	{
		sentence[i]=tolower(sentence[i],locale("Russian"));
	}

	// разбиваем предложение на слова.
	vector<Word> res = divide(sentence, additional_patterns, &lo);
	// какая-то странная магия с союзами.
	set_unions(res,un_set,un_pattterns);

	// тут типа как получаем предложение в виде сегментов.
	// и я так понимаю оно должно быть заполнено МИ.
	vector<Segment> segments=get_segments(res);
	for(vector<Segment>::iterator it=segments.begin();it!=segments.end();++it)
	{
		setSegment(*it);
	}
	// дебаг показывает, что в структуре много чего не иницилизировано.
	// да там вообще все наполовину мертво! *wall*

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
