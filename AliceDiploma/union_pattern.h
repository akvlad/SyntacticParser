#pragma once

#include <vector>
#include <string>
#include <strstream>
#include <iostream>
#include <iterator>
#include <set>

#include "Word.h"
#include "union_pattern.h"

using namespace std;

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
		bool has_colsed_use,int type,int first_part_id=0) :
		m_id(id),
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

vector<union_pattern> create_patterns_map(set<string>& union_set);

void set_unions(vector<Word>& Words,set<string>& union_set,vector<union_pattern>& union_patterns);