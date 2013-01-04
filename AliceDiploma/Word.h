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