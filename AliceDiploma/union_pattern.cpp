#include "union_pattern.h"

#include <fstream>

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