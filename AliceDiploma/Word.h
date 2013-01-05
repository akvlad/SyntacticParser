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
	Fn m_fn;
	Word::Word_type m_r_op;
	type_comparer(Fn fn,Word::Word_type r_op);
	bool operator () (const Word& ) const;
};

template <class Fn>
class MI_comparer:public comparer
{
public:
	Fn m_fn; 
	unsigned long int m_r_op,m_mask;
	MI_comparer(Fn fnm,unsigned long int r_op,unsigned long int mask);
	bool operator () (const Word& ) const;
};

template <class Fn>
class complex_comparer:public comparer
{
public:
	const comparer& m_l_op;
	const comparer& m_r_op;
	Fn m_fn; 
	complex_comparer(Fn fn,const comparer& l_op, const comparer& r_op):
		m_fn(fn),m_l_op(l_op),m_r_op(r_op){}
	bool operator () (const Word& ) const;
};