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
	cout<<"Тип: "<<s.m_type<<"\t";
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

	// загрузили союзы-разделители сегментов.
	cout<<"Загружаем базу знаков препинания...";
	un_pattterns=create_patterns_map(un_set);
	cout<<"Готово\n";
	// загрузили базу лемм.
	cout<<"Загружаем словарь...";
	LemmOrganizer lo("words.txt");
	cout<<"Готово\n";
#ifndef _TEST
	const int powerOfTwo = 1024;
	char sentence[powerOfTwo];
	while(strcmp(sentence,"quit"))
	{
	cout<<"Введите предложение \n";

	cin.getline(sentence, powerOfTwo);
#endif
#ifdef _TEST
	char sentences[3][256]={"Я очень люблю мороженое.",
		"Мама и папа идут в магазин.",
		"Мальчик, очень хорошо сдавший экзамен, пошел гулять."};
	for(int c=0;c<3;++c)
	{
	char* sentence=sentences[c];
	cout<<"Предложение: "<<sentence<<"\n";
#endif
	int len=strlen(sentence);
	for(int i=0;i<strlen(sentence);++i)
	{
		sentence[i]=tolower(sentence[i],locale("Russian"));
	}

	// разбиваем предложение на слова.
	cout<<"Разбиваем предложение на слова...";
	vector<Word> res = divide(sentence, additional_patterns, &lo);
	cout<<"Готово\n";
	cout<<"Выделяем союзы...";
	// какая-то странная магия с союзами.
	set_unions(res,un_set,un_pattterns);
	cout<<"Готово\n"<<"Элементы предложения:";
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
			cout<<"\t союз";
			break;
		case Word::sign:
			cout<<"\t знак препинания";
			break;
		}
		cout<<"\n";
	}
	// тут типа как получаем предложение в виде сегментов.
	// и я так понимаю оно должно быть заполнено МИ.
	cout<<"Выделяем сегменты...";
	vector<Segment> segments=get_segments(res);
	for(vector<Segment>::iterator it=segments.begin();it!=segments.end();++it)
	{
		setSegment(*it);
		find_odnorodnost(segments,it);
		get_included(segments,it);
	}
	cout<<"Готово\n";
	cout<<"Сегменты:\n";
	for(vector<Segment>::iterator it=segments.begin();it!=segments.end();++it){
		print_segment(*it);
		if(it->first!=res.end())
			cout<<"\tГлавное слово: "<<it->first->m_word;
		if(it->m_has_neighbour)
		{
			cout<<"\n  Связанный сегмент: ";
			print_segment(*it->m_neighbour);
			if(it->m_connect_type==2)
				cout<<"\n  Тип связи: однородная";
			if(it->m_connect_type==1)
				cout<<"\n  Тип связи: причастный оборот";
			if(it->has_owner())
			{
				cout<<"\n  Главное слово: "<<it->m_owner->m_word;
			}
		}
		cout<<"\n";
	}
	cout<<"Строим простые синтаксические группы...";
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
	cout<<"Готово\n";
	cout<<"Выделяем предикатное ядро...";
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
	cout<<"Готово\n";
	cout<<"Заполняем актантную структуру сказуемого...";
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
	cout<<"Готово\n";
	cout<<"Результаты работы:\n";
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
	cout<<"Виды отношений: \n"
		<<"С\t-\tСубъект \n"
		<<"КЛ\t-\tконечный локатив \n"
		<<"МП\t-\tмера признака \n"
		<<"П\t-\tпризнак \n"
		<<"О\t-\tобъект \n"
		<<"З\t-\tзнак препинания \n";
	}

	return 0;
}
