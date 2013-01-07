#include "Segment.h"

#include "predicates_func.h"
#include <algorithm>

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

bool has_dash(Segment& s)
{
	// TODO: у меня не линковался код "type_comparer<equal_to <int> >(equal_to <int>(), Word::dash)" по этому я временно выкинул это.
	//vector<Word>::iterator it=find_if(s.m_left_border,s.m_right_border,type_comparer<equal_to <int> >(equal_to <int>(), Word::dash));
	//if(it!=s.m_left_border && it!=s.m_right_border && it!=s.m_right_border-1)
	//	return true;
	
	return false;
}

MainMembers get_main_dicks_of_sentence(vector<Segment> &segments)
{
	MainMembers mm;
	// я сохраняю дурацкие именования как и в алгоритме. чтобы не мучаться потом.
	bool Flag_Cop = false; // булева переменная, указывает на наличие в явном виде Cop(f).
	string Subj;
	string Pred;

	vector<Segment>::iterator it = segments.begin();
	// Поиск по МИ слева направо V(f).
	for (; it != segments.end(); ++it)
	{

	}

	return mm;
}