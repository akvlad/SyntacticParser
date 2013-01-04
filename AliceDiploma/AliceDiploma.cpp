// AliceDiploma.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <regex>
#include <string>
#include <iostream>
#include <map>
#include <strstream>
#include <set>
#include <fstream>
#include <string.h>
#include <windows.h>

using namespace std;
struct Segment;

bool operator < (string s1, string s2)
{
	return lexicographical_compare(s1.begin(),s1.end(),s2.begin(),s2.end());
}

struct LemmOrganizer
{
	map<string, unsigned long int> LemmDict;
	LemmOrganizer(string fname)
	{
		ifstream f(fname);
		while(!f.eof())
		{
			string wrd;
			f>>wrd;
			unsigned long int MI;
			f>>MI;
			LemmDict[wrd]=MI;
		}		
	}
	unsigned long int getMI(string wrd)
	{
		return LemmDict[wrd];
	}	
};

struct Word
{
	enum Word_type
	{
		unprocessible,
		word,
		dephis,
		dash,
		sign,
		border,
		sentence_union
	};
	string m_word;
	unsigned long int m_MI;
	vector<Word> inside_words;
	int m_union_pattern_id;
	vector<vector<string> >m_all_lemms;
	vector<vector<unsigned long int> >m_all_MI;
	vector<Word*> m_next;
	Word* m_parent;
	int m_connect_type;
	int m_seg_id;
	Word_type type;
	string get_string()
	{
		if(type==Word::sentence_union)
		{
			string res;
			for(vector<Word>::iterator it=inside_words.begin();it!=inside_words.end();++it)
			{
				res+=it->get_string();
				res+=" ";
			}
			return res;
		}
		return m_word;
	}
};

struct Segment
{
	int m_type;
	int m_type_sv;
	Word* first;
	vector<Word>::iterator m_left_border,m_right_border;		//TODO: baaad style
	vector<Word>::iterator m_left_union,m_right_union;
	Word* m_owner;
	Segment* m_neighbour;
	int m_connect_type;
	int m_id;
};

vector<Word> divide(const string& text,LemmOrganizer* lo);

vector<string> additional_patterns;

/*---------------TEXT BY WORDS DIVISION-------------------------*/
void trim(string& s)
{
	while (s.back()==' ')
		s.pop_back();
	while(s.front()==' ')
		s.erase(s.begin());
}

bool getPieces(string text,const string& reg_ex, vector<Word>& res,
	Word::Word_type type,void (*fn)(vector<Word>&,vector<Word>::iterator )=NULL,LemmOrganizer* lo=NULL)
{
	regex reg(reg_ex);
	smatch mtc;
	bool is_processed=false;
	while(regex_search(text,mtc,reg))
	{
		vector<Word>pref=divide(mtc.prefix(),lo);
		copy(pref.begin(),pref.end(),back_inserter(res));
		Word new_word;
		new_word.m_word=mtc[0];
		trim(new_word.m_word);
		new_word.type=type;
		if(type==Word::Word_type::word && lo!=NULL)
		{
			new_word.m_MI=lo->getMI(new_word.m_word);
		}
		res.push_back(new_word);
		if(fn)
			fn(res,res.end());
		text=mtc.suffix();
		is_processed=true;
	}
	if(is_processed)
	{
		vector<Word>pref=divide(text,lo);
		copy(pref.begin(),pref.end(),back_inserter(res));
	}
	return is_processed;
}

void sign_fn(vector<Word>& vecWord,vector<Word>::iterator it)
{
	Word bord;
	bord.type=Word::border;
	vecWord.push_back(bord);
}

vector<Word> divide(const string& text,LemmOrganizer* lo=NULL)
{
	vector<Word> res;
	for(vector<string>::iterator it=additional_patterns.begin();
		it!=additional_patterns.end();++it)
	{
		if(getPieces(text,*it,res,Word::Word_type::unprocessible,NULL,lo))
			return res;
	}
	if(getPieces(text,"([,\\.!?:;\\(\\)\\{\\}\\[\\]\"][,\\-\\.!?:;\\(\\)\\{\\}\\[\\]\" ]*)",res,
		Word::Word_type::sign,sign_fn,lo))
		return res;
	if(getPieces(text,"( - )",res,Word::Word_type::dash,NULL,lo))
		return res;
	if(getPieces(text,"(-)",res,Word::Word_type::dephis,NULL,lo))
		return res;
	if(getPieces(text,"([^ ]+)",res,Word::Word_type::word,NULL,lo))
		return res;
	return res;
}

/*----TEXT WORDS BY SEGMENTS DIVISION---------------*/
bool operator == (const string& str,const Word& wrd);
bool operator == (const Word& wrd,const string& str);
struct union_pattern
{
	enum union_types
	{
		copulative,
		separative,
		adversative,
		explanatory,
		bridge,
		connecting
	};
	int m_id;
	vector<string> m_words;
	bool m_is_in_first_part;
	int m_first_part_id;
	bool m_has_comma;
	bool m_has_opened_use;
	bool m_has_closed_use;
	union_types m_type;
	union_pattern(int id,string words,bool is_in_first_part,bool has_comma,bool has_opened_use,
		bool has_colsed_use,int type,int first_part_id=0):m_id(id),
			m_is_in_first_part(is_in_first_part),
			m_has_comma(has_comma),
			m_has_opened_use(has_opened_use),
			m_has_closed_use(has_colsed_use),
			m_type(union_types(type)),
			m_first_part_id(first_part_id)
	{
		char str[100];
		memcpy(str,words.c_str(),words.length());
		strstream s(str,words.length(),ios::in | ios::out);
		copy(istream_iterator<string>(s),istream_iterator<string>(strstream()),back_inserter(m_words));
	}
	bool apply_pattern(const vector<Word>& vec,vector<Word>::iterator it) const
	{
		if(vec.end()-it < m_words.size())
			return false;
		for(vector<string>::const_iterator it2=m_words.begin();it2!=m_words.end();++it2,++it)
		{
			if((it==vec.end() && it2!=m_words.end()) || !((*it2)==(*it)))
				return false;
		}
		if(m_is_in_first_part)
			return true;
		--it;
		for(;true;--it)
		{
			if((it->m_word.size()>0 && (it->m_word[0]=='.' || it->m_word[0]=='?' || it->m_word[0]=='!'))||it==vec.begin())
				break;
			if(it->type==Word::sentence_union && it->m_union_pattern_id==m_first_part_id)
				return true;
		}
		if(it==vec.begin() && it->type==Word::sentence_union && it->m_union_pattern_id==m_first_part_id)
			return true;
		return false;
	}
};

vector<union_pattern>un_pattterns;
set<string> un_set;


bool operator == (const string& str,const Word& wrd)
{
	string tmp=wrd.m_word;
	return tmp==str;
}
bool operator == (const Word& wrd,const string& str)
{
	return wrd.m_word==str;
}

vector<union_pattern> create_patterns_map(set<string>& union_set)
{
	vector<union_pattern> res;
	vector<union_pattern>::iterator it;
	ifstream f("union_patterns.txt");
	while(!f.eof())
	{
		int id;
		char first_wrd[10];
		char whole[20];
		bool is_in_first_part,has_comma,has_opened_use,has_colsed_use;
		int pat_type,first_part_id;
		f>>id>>first_wrd;
		f.get();
		f.getline(whole,19,';');
		f.get();
		f>>is_in_first_part>>has_comma>>has_opened_use>>has_colsed_use>>pat_type>>first_part_id;
		res.push_back(union_pattern (id,string(whole),is_in_first_part,has_comma,has_opened_use,has_colsed_use,pat_type,first_part_id));
		union_set.insert(first_wrd);
	}
	f.close();
	return res;	
}



void set_unions(vector<Word>& Words,set<string>& union_set,vector<union_pattern>& union_patterns)
{
	vector<Word>::iterator it=Words.begin();
	for(;it!=Words.end();++it)
	{
		if(it->type!=Word::word)
			continue;
		if(union_set.count(it->m_word)==0)
			continue;
		for(vector<union_pattern>::iterator it2=union_patterns.begin();it2!=union_patterns.end();++it2)
		{
			if(it2->apply_pattern(Words,it))
			{
				Word new_union;
				new_union.type=Word::sentence_union;
				new_union.m_union_pattern_id=it2->m_id;
				copy(it,it+it2->m_words.size(),back_inserter(new_union.inside_words));
				it=Words.erase(it,it+it2->m_words.size());
				it=Words.insert(it,new_union);
				if(it!=Words.begin() && (it-1)->type!=Word::border && (it+1)!=Words.end() && (it+1)->type!=Word::sign)
				{
					Word new_bord; new_bord.type=Word::border;
					it=Words.insert(it+1,new_bord);
				}
				break;
			}
		}
	}
}

vector<Segment> get_segments(vector<Word>& Words)
{
	vector<Segment> res;
	Segment new_seg;
	vector<Word>::iterator it=Words.begin();
	int seg_id=1;
	new_seg.m_id=seg_id;
	new_seg.m_left_border=Words.begin();
	for(;it!=Words.end();++it)
	{
		it->m_seg_id=seg_id;
		if(it->type==Word::border)
		{
			new_seg.m_right_border=it;
			res.push_back(new_seg);
			new_seg.m_right_border=new_seg.m_left_border=Words.end();
			new_seg.m_right_union=new_seg.m_left_union=Words.end();
			seg_id++;
			new_seg.m_id=seg_id;
			new_seg.m_left_border=it+1;
		}
		if(it->type==Word::sentence_union)
		{
			if(it==Words.begin() || (it-1)->type==Word::border)
			{
				new_seg.m_left_union=it;
				continue;
			}
			new_seg.m_right_union=it;

		}
	}
	return res;
}

void print_word(const Word& w)
{
	cout<<w.m_word<<" ";
}
/*-----ПРЕДИКАТЫ ДЛЯ ОПРЕДЕЛЕНИЯ ФОРМЫ СЕРМЕНТА--------*/
bool pred_form_1(const Word& w)
{
	if (w.type!=Word::Word_type::word || w.m_all_lemms.size()>=2) return false;
	return (w.m_MI & 0x00078000) == 0x00008000 && (w.m_MI & 0x00000600) != 0;
}

bool pred_form_2(const Word& w)
{
	if (w.type!=Word::Word_type::word || w.m_all_lemms.size()>=2) return false;
	return (w.m_MI & 0x00010000) == 0x00010000 && (w.m_MI & 0x00006000) == 0x00006000;
}


bool pred_form_3(const Word& w)
{
	if (w.type!=Word::Word_type::word || w.m_all_lemms.size()>=2) return false;
	return (w.m_MI & 0x00020000) == 0x00020000 && (w.m_MI & 0x00006000) == 0x00006000;
}

bool pred_form_4(const Word& w)
{
	if (w.type!=Word::Word_type::word || w.m_all_lemms.size()>=2) return false;
	return (w.m_MI & 0x00068000) == 0x00068000;
}

bool pred_form_5(const Word& w)
{
	if (w.type!=Word::Word_type::word || w.m_all_lemms.size()>=2) return false;
	return (w.m_MI & 0x00010000) == 0x00010000;
}

bool pred_form_6(const Word& w)
{
	if (w.type!=Word::Word_type::word || w.m_all_lemms.size()>=2) return false;
	return (w.m_MI & 0x00010000) == 0x00010000;
}

bool pred_form_def(const Word& w)
{
	return false;
}

bool pred_form_9(const Word& w)
{
	return true;
}

void setSegment(Segment& s)
{
	bool (*pred[9])(const Word&)={pred_form_1,pred_form_2,pred_form_3,pred_form_4,pred_form_5,pred_form_6,
		pred_form_def,pred_form_def,pred_form_9};
	for(int i=0;i<9;++i)
	{
		vector<Word>::iterator it=find_if(s.m_left_border,s.m_right_border,pred[i]);
		if(it!=s.m_right_border)
		{
			s.m_type=i+1;
			s.first=&(*it);
			return;
		}
	}
}



bool has_dash(const Word& w)
{
	return w.type==Word::dash;
}

bool is_dash(Segment& s)
{
	vector<Word>::iterator it=find_if(s.m_left_border,s.m_right_border,has_dash);
	if(it!=s.m_left_border && it!=s.m_right_border && it!=s.m_right_border-1)
		return true;
	return false;
}

int _tmain(int argc, _TCHAR* argv[])
{
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	un_pattterns=create_patterns_map(un_set);
	cout<<"Введите предложение \n";
	char sentence[1000];
	cin.getline(sentence,1000);
	int len=strlen(sentence);
	for(int i=0;i<strlen(sentence);++i)
	{
		sentence[i]=tolower(sentence[i],locale("Russian"));
	}
	LemmOrganizer lo("words.txt");
	vector<Word> res=divide(sentence,&lo);
	set_unions(res,un_set,un_pattterns);
	vector<Segment> segments=get_segments(res);
	for(vector<Segment>::iterator it=segments.begin();it!=segments.end();++it)
	{
		setSegment(&*it);
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

