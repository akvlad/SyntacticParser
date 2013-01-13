#pragma once

#include <vector>

#include "Word.h"

using namespace std;

typedef std::vector<std::pair<string,string> > MainMembers;

struct Segment
{
	vector<Word>::iterator first;

	vector<Word>::iterator m_left_union;

	vector<Word>::iterator m_right_union;

	vector<Word>::iterator m_owner;

	bool m_has_neighbour;
	vector<Segment>::iterator m_neighbour;

	int m_type;

	int m_type_sv;

	vector<Word>::iterator m_left_border;

	vector<Word>::iterator m_right_border;

	vector<Word>& m_text;

	int m_connect_type;

	int m_id;

	Segment(vector<Word>& text):first(text.end()), m_left_union(text.end()),
		m_right_union(text.end()),m_owner(text.end()),m_text(text),m_has_neighbour(false){}
	Segment(const Segment& s):first(s.first), m_left_union(s.m_left_union),
		m_right_union(s.m_right_union),m_owner(s.m_owner),m_text(s.m_text),
		m_has_neighbour(s.m_has_neighbour),m_neighbour(s.m_neighbour),m_type(s.m_type),m_type_sv(s.m_type_sv),
		m_left_border(s.m_left_border),m_right_border(s.m_right_border),m_connect_type(s.m_connect_type),m_id(s.m_id){}
	void operator = (const Segment& s)
	{
		first=s.first; m_left_union=s.m_left_union;
		m_right_union=s.m_right_union;m_owner=s.m_owner;m_text=s.m_text;
		m_has_neighbour=s.m_has_neighbour;m_neighbour=s.m_neighbour;m_type=s.m_type;m_type_sv=s.m_type_sv;
		m_left_border=s.m_left_border;m_right_border=s.m_right_border;m_connect_type=s.m_connect_type;m_id=s.m_id;
	}
		
	bool has_left_union()
	{return m_left_union!=m_text.end();}
	bool has_right_union()
	{return m_right_union!=m_text.end();}
	bool has_owner()
	{return m_owner!=m_text.end();}
};

vector<Segment> get_segments(vector<Word>& Words);

void setSegment(Segment& s);

bool has_dash(Segment& s);

MainMembers get_main_dicks_of_sentence(vector<Segment> &segments);
class odnorodnost_getter{
private: 
	unsigned long int m_MI;
public:
	odnorodnost_getter(unsigned long int MI):m_MI(MI)
	{}
	bool operator()(const Word& ) const;
};
void get_included(vector<Segment>& ws,vector<Segment>::iterator cur_seg);

void find_odnorodnost(vector<Segment>& segs,vector<Segment>::iterator cur_seg);
int get_type(Segment& s);
vector<Word>::iterator find_word_if(vector<Word>::iterator begin,vector<Word>::iterator end,const comparer& Pred);
