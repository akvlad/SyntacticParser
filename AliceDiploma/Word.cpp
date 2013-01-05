#include "Word.h"

#include <regex>
#include <iostream>

void trim(string& s)
{
	while (s.back()==' ')
		s.pop_back();
	while(s.front()==' ')
		s.erase(s.begin());
}

bool getPieces( string text,
	const string& reg_ex,
	vector<Word>& res,
	Word::Word_type type,
	vector<string> &additional_patterns,
	void (*fn)( vector<Word>&,vector<Word>::iterator ),
	LemmOrganizer* lo)
{
	regex reg(reg_ex);
	smatch mtc;
	bool is_processed=false;
	while(regex_search(text,mtc,reg))
	{
		vector<Word>pref=divide(mtc.prefix(), additional_patterns, lo);
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
		vector<Word>pref=divide(text, additional_patterns, lo);
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

vector<Word> divide(const string& text, vector<string> &additional_patterns, LemmOrganizer* lo)
{
	vector<Word> res;
	for(vector<string>::iterator it=additional_patterns.begin();
		it!=additional_patterns.end();++it)
	{
		if(getPieces(text,*it,res,Word::Word_type::unprocessible,additional_patterns,NULL,lo))
			return res;
	}
	if(getPieces(text,"([,\\.!?:;\\(\\)\\{\\}\\[\\]\"][,\\-\\.!?:;\\(\\)\\{\\}\\[\\]\" ]*)",res,
		Word::Word_type::sign,additional_patterns,sign_fn,lo))
		return res;
	if(getPieces(text,"( - )",res,Word::Word_type::dash,additional_patterns,NULL,lo))
		return res;
	if(getPieces(text,"(-)",res,Word::Word_type::dephis,additional_patterns,NULL,lo))
		return res;
	if(getPieces(text,"([^ ]+)",res,Word::Word_type::word,additional_patterns,NULL,lo))
		return res;
	return res;
}

void print_word(const Word& w)
{
	cout<<w.m_word<<" ";
}

bool is_dash(const Word& w)
{
	return w.type==Word::dash;
}

template <class Fn>
type_comparer<Fn>::type_comparer(Fn fn,Word::Word_type r_op)
{m_fn=fn;m_r_op=r_op;}

template <class Fn>
bool type_comparer<Fn>::operator ()(const Word& w) const
{return fn(w.type,m_r_op);}

template <class Fn>
MI_comparer<Fn>::MI_comparer(Fn fn,unsigned long int r_op,unsigned long int mask)
{m_fn=fn;m_r_op=r_op;m_mask=mask;}

template <class Fn>
bool MI_comparer<Fn>::operator ()(const Word& w) const
{return fn(w.m_MI & m_mask,m_r_op);}

template <class Fn>
bool complex_comparer<Fn>::operator ()(const Word& w) const
{return fn(m_l_op(w),m_r_op(w));}