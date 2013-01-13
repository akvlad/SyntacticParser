#pragma once

#include <string>
#include <vector>

#include "LemmOrganizer.h"

using namespace std;

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
		sentence_union,
		entrance_word
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

void sign_fn(vector<Word>& vecWord,vector<Word>::iterator it);

vector<Word> divide(const string& text, vector<string> &additional_patterns, LemmOrganizer* lo=NULL);

bool getPieces( string text,
	const string& reg_ex,
	vector<Word>& res,
	Word::Word_type type,
	vector<string> &additional_patterns,
	void (*fn)( vector<Word>&,vector<Word>::iterator ) = NULL,
	LemmOrganizer* lo = NULL);

void print_word(const Word& w);

bool is_dash(const Word& w);


class comparer
{
public:
	virtual bool operator () (const Word&) const = 0;
};

template <class Fn>
class type_comparer:public comparer
{
public:
	typedef Fn func_type;
	Fn m_fn;
	Word::Word_type m_r_op;
	type_comparer(Fn fn,Word::Word_type r_op): m_fn(fn),m_r_op(r_op)
	{}
	virtual bool operator () (const Word& w) const
	{
		return m_fn(w.type,m_r_op);
	}
};

template <class Fn>
class MI_comparer:public comparer
{
public:
	typedef Fn func_type;
	Fn m_fn; 
	unsigned long int m_r_op,m_mask;
	bool m_with_omonyms;
	MI_comparer(Fn fn,unsigned long int r_op,unsigned long int mask,
		bool with_omonyms):m_fn(fn),m_r_op(r_op),m_mask(mask),m_with_omonyms(with_omonyms)
	{}
	bool operator () (const Word& w) const
	{	if(!m_with_omonyms && w.m_all_lemms.size()>1)
			return false;
		return m_fn(w.m_MI & m_mask,m_r_op);
	}
};

template <class Fn>
class complex_comparer:public comparer
{
public:
	typedef Fn func_type;
	const comparer& m_l_op;
	const comparer& m_r_op;
	Fn m_fn; 
	complex_comparer(Fn fn,const comparer& l_op, const comparer& r_op):
		m_fn(fn),m_l_op(l_op),m_r_op(r_op){}
	bool operator () (const Word& w) const
	{return m_fn(m_l_op(w),m_r_op(w));}
};

class fake_comparer:public comparer
{
public:
	bool m_fake;
	fake_comparer(bool fake):
	m_fake(fake){}
	bool operator () (const Word& ) const
	{return m_fake;}
};

struct conll
{
	string m_word;
	string lemm;
	Word::Word_type type;
	string ps;
	string add_MI;
	unsigned long int m_MI;
	conll(string _word,string _lemm,Word::Word_type _type,int MI):
	m_word(_word),lemm(_lemm),type(_type),m_MI(MI){}
};

string translate_add_MI(const conll& w);
string translate_PS(const conll& w);