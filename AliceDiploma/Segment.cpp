#include "Segment.h"

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