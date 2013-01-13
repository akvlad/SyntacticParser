// AliceDiploma.cpp: ���������� ����� ����� ��� ����������� ����������.
//

#include "stdafx.h"
#include <regex>
#include <string>
#include <string.h>
#include <windows.h>

#include "LemmOrganizer.h"
#include "union_pattern.h"
#include "Segment.h"
#include "codecvt.h"

#define _TEST 1
using namespace std;

vector<union_pattern>un_pattterns;

vector<string> additional_patterns;

set<string> un_set;

bool operator < (string s1, string s2)
{
	return lexicographical_compare(s1.begin(),s1.end(),s2.begin(),s2.end());
}

void print_segment(const Segment& s)
{
	cout<<"���: "<<s.m_type<<"\t";
	for(vector<Word>::iterator it2=s.m_left_border;it2!=s.m_right_border;++it2)
	{
		cout<<it2->get_string()<<" ";
	}
}



int _tmain(int argc, _TCHAR* argv[])
{
	setlocale(LC_ALL,"rus");
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	// ��������� �����-����������� ���������.
	cout<<"��������� ���� ������ ����������...";
	un_pattterns=create_patterns_map(un_set);
	cout<<"������\n";
	// ��������� ���� ����.
	cout<<"��������� �������...";
	LemmOrganizer lo("words.txt");
	cout<<"������\n";
#ifndef _TEST
	const int powerOfTwo = 1024;
	char sentence[powerOfTwo];
	while(strcmp(sentence,"quit"))
	{
	cout<<"������� ����������� \n";

	cin.getline(sentence, powerOfTwo);
#endif
#ifdef _TEST
	char sentences[3][256]={"� ����� ����� ���������.",
		"���� � ���� ���� � �������.",
		"�������, ����� ������ ������� �������, ����� ������."};
	for(int c=0;c<3;++c)
	{
	char* sentence=sentences[c];
	cout<<"�����������: "<<sentence<<"\n";
#endif
	int len=strlen(sentence);
	for(int i=0;i<strlen(sentence);++i)
	{
		sentence[i]=tolower(sentence[i],locale("Russian"));
	}

	// ��������� ����������� �� �����.
	cout<<"��������� ����������� �� �����...";
	vector<Word> res = divide(sentence, additional_patterns, &lo);
	cout<<"������\n";
	cout<<"�������� �����...";
	// �����-�� �������� ����� � �������.
	set_unions(res,un_set,un_pattterns);
	cout<<"������\n"<<"�������� �����������:";
	for(vector<Word>::iterator it=res.begin();it!=res.end();++it)
	{
		if(it->type==Word::border)
			continue;
		cout<<it->get_string();
		switch(it->type)
		{
		case Word::word:
			cout<<"\t"<<it->m_MI;
			break;
		case Word::sentence_union:
			cout<<"\t ����";
			break;
		case Word::sign:
			cout<<"\t ���� ����������";
			break;
		}
		cout<<"\n";
	}
	// ��� ���� ��� �������� ����������� � ���� ���������.
	// � � ��� ������� ��� ������ ���� ��������� ��.
	cout<<"�������� ��������...";
	vector<Segment> segments=get_segments(res);
	for(vector<Segment>::iterator it=segments.begin();it!=segments.end();++it)
	{
		setSegment(*it);
		find_odnorodnost(segments,it);
		get_included(segments,it);
	}
	cout<<"������\n";
	cout<<"��������:\n";
	for(vector<Segment>::iterator it=segments.begin();it!=segments.end();++it){
		print_segment(*it);
		if(it->first!=res.end())
			cout<<"\t������� �����: "<<it->first->m_word;
		if(it->m_has_neighbour)
		{
			cout<<"\n  ��������� �������: ";
			print_segment(*it->m_neighbour);
			if(it->m_connect_type==2)
				cout<<"\n  ��� �����: ����������";
			if(it->m_connect_type==1)
				cout<<"\n  ��� �����: ���������� ������";
			if(it->has_owner())
			{
				cout<<"\n  ������� �����: "<<it->m_owner->m_word;
			}
		}
		cout<<"\n";
	}
	cout<<"������ ������� �������������� ������...";
	vector<conll> clls;
	for(vector<Word>::iterator it=res.begin();it!=res.end();++it)
	{
		switch(it->type)
		{
		case Word::word:
			{
			conll nc(it->m_word,it->m_all_lemms.front().front(),it->type,it->m_MI);
			nc.add_MI=translate_add_MI(nc);
			nc.ps=translate_PS(nc);
			clls.push_back(nc);
			break;
			}
		case Word::sentence_union:
			for(vector<Word>::iterator it2=it->inside_words.begin();it2!=it->inside_words.end();++it2)
			{
				conll nc(it2->m_word,it2->m_word,it->type,0);
				nc.add_MI=translate_add_MI(nc);
				nc.ps=translate_PS(nc);
				clls.push_back(nc);
			}
			break;
		case Word::sign:
		case Word::dash:
			conll nc(it->m_word,it->m_word,it->type,0);
			nc.add_MI="_";
			nc.ps="S";
			clls.push_back(nc);
			break;
		}
	}
	cout<<"������\n";
	cout<<"�������� ����������� ����...";
	wofstream ofs("in.conll", ios_base::binary);
	int i=1;
	setlocale(LC_ALL, "ru_RU.UTF-8");
	for(vector<conll>::iterator it=clls.begin();it!=clls.end();++it)
	{

		strstream conv;conv.clear();
		conv<<i<<"\t"<<it->m_word<<"\t"<<it->lemm<<"\t"<<it->ps<<"\t"<<it->ps<<"\t"<<it->add_MI<<"\t_\t_\n"<<ends;
		string ma(conv.str());
		wchar_t buf[256];
		memset(buf,0,sizeof(wchar_t)*256);
		char* buf2=conv.str();
		mbstowcs(buf,ma.c_str(),ma.length());
		ofs.rdbuf()->pubimbue(locale(locale(), new utf8_conversion()));
		ofs.write(buf,ma.length());
		++i;
	}
	ofs.close();
	cout<<"������\n";
	cout<<"��������� ��������� ��������� ����������...";
	system("java -jar dicts/maltparser-1.7.2.jar -c test -i in.conll -o out.conll -m parse 2>out");
	//wifstream ifs("out.conll");
	wifstream ifs("out.conll", ios_base::binary);
	ifs.rdbuf()->pubimbue(locale(locale(), new utf8_conversion() ));
	char buf[10000];
	wchar_t wbuf[10000];
	ifs.read(wbuf,10000);
	wcstombs(buf,wbuf,10000);
	ifs.close();
	strstream strstr; strstr<<buf<<ends;
	char matrix[100][100][10];
	memset(matrix,0,sizeof(char)*100*100*10);
	for(int i=0;i<clls.size();++i)
	{
		char s[60];
		int main_n;
		strstr>>s>>s>>s>>s>>s>>s;
		strstr>>main_n>>s;
		char buf[5];

		if(main_n!=0){
		strcpy(matrix[i][main_n-1],s);
		strcpy(matrix[main_n-1][i],s);
		}
				strstr>>s>>s;
	}
	cout<<"������\n";
	cout<<"���������� ������:\n";
	for(int i=0;i<clls.size();++i)
		cout<<"\t"<<clls[i].m_word;
	cout<<"\n";
	for(int i=0;i<clls.size();++i)
	{
		cout<<clls[i].m_word<<"\t";
		for(int j=0;j<clls.size();++j)
		{
			cout<<matrix[i][j]<<"\t";
		}
		cout<<"\n";
	}
	cout<<"���� ���������: \n"
		<<"�\t-\t������� \n"
		<<"��\t-\t�������� ������� \n"
		<<"��\t-\t���� �������� \n"
		<<"�\t-\t������� \n"
		<<"�\t-\t������ \n"
		<<"�\t-\t���� ���������� \n";
	}

	return 0;
}
