#pragma once

#include <vector>

#include "Word.h"

using namespace std;

struct Segment
{
	int m_type;

	int m_type_sv;

	Word* first;

	vector<Word>::iterator m_left_border;

	vector<Word>::iterator m_right_border;

	vector<Word>::iterator m_left_union;

	vector<Word>::iterator m_right_union;

	Word* m_owner;

	Segment* m_neighbour;

	int m_connect_type;

	int m_id;
};

vector<Segment> get_segments(vector<Word>& Words);

void setSegment(Segment& s);

bool is_dash(Segment& s);