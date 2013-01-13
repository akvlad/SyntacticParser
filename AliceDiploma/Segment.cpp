#include "Segment.h"

#include "predicates_func.h"
#include <algorithm>

vector<Segment> get_segments(vector<Word>& Words)
{
	vector<Segment> res;
	Segment new_seg(Words);
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
	for(int i=0;i<8;++i)
	{
		vector<Word>::iterator it=find_if(s.m_left_border,s.m_right_border,pred[i]);
		if(it!=s.m_right_border)
		{
			s.m_type=i+1;
			s.first=it;
			return;
		}
	}
	if(has_dash(s))
		s.m_type=0;
	else s.m_type=9;
}


vector<Word>::iterator find_word_if(vector<Word>::iterator begin,vector<Word>::iterator end,const comparer& Pred)
{
	vector<Word>::iterator it=begin;
	for(;it!=end;++it)
	{
		if(Pred(*it))
			return it;
	}
	return it;
}
typedef unsigned long int MYLONGLONG;

bool has_dash(Segment& s)
{
	// TODO: у меня не линковался код "type_comparer<equal_to <int> >(equal_to <int>(), Word::dash)" по этому я временно выкинул это.
	vector<Word>::iterator it=find_if(s.m_left_border,s.m_right_border,type_comparer<equal_to <int> >(equal_to <int>(), Word::dash));
	if(it!=s.m_left_border && it!=s.m_right_border && it!=s.m_right_border-1)
	{
		s.first=(--it);
		return true;
	}

/*	it=find_word_if(s.m_left_border,s.m_right_border,complex_comparer<logical_and<bool> >(logical_and<bool> (),
		MI_comparer<equal_to<MYLONGLONG> > (equal_to<MYLONGLONG>(),0x00028000,0x00028000,false),
		MI_comparer<equal_to<MYLONGLONG> > (equal_to<MYLONGLONG>(),0x00000001,0x00000001,false)));
	if(it*/
	return false;
}



int get_type(Segment& s)
{
	tr1::shared_ptr<comparer> sg_type_preds[9];
	
	//Глагол в личной форме
	sg_type_preds[0]=tr1::shared_ptr<comparer>(new complex_comparer<logical_and<bool> > (logical_and<bool>(),
		MI_comparer<equal_to<MYLONGLONG> >(equal_to<MYLONGLONG>(),32768,491520,false),
		MI_comparer<not_equal_to<MYLONGLONG> >(not_equal_to<MYLONGLONG>(),0,1536,false) ));
	
	//Краткое причастие
	sg_type_preds[1]=tr1::shared_ptr<comparer>(new complex_comparer<logical_and<bool> > (logical_and<bool>(),
		MI_comparer<equal_to<MYLONGLONG> >(equal_to<MYLONGLONG>(),65536,65536,false),
		MI_comparer<equal_to<MYLONGLONG> >(equal_to<MYLONGLONG>(),24576,24576,false) ));

	//краткое прилагательное
	sg_type_preds[2]=tr1::shared_ptr<comparer>(new complex_comparer<logical_and<bool> > (logical_and<bool>(),
		MI_comparer<equal_to<MYLONGLONG> >(equal_to<MYLONGLONG>(),131072,131072,false),
		MI_comparer<equal_to<MYLONGLONG> >(equal_to<MYLONGLONG>(),24576,24576,false) ));
	
	//предикатив
	sg_type_preds[3]=tr1::shared_ptr<comparer>(new 	MI_comparer<equal_to<MYLONGLONG> >(equal_to<MYLONGLONG>(),425984,425984,false));
	
	//причастие
	sg_type_preds[4]=tr1::shared_ptr<comparer>(new 	MI_comparer<equal_to<MYLONGLONG> >(equal_to<MYLONGLONG>(),65536,65536,false));
	
	//деепричастие
	sg_type_preds[5]=tr1::shared_ptr<comparer>(new 	MI_comparer<equal_to<MYLONGLONG> >(equal_to<MYLONGLONG>(),98304,98304,false));


	//инфинитив
	sg_type_preds[6]=tr1::shared_ptr<comparer>(new 	fake_comparer(false));
	
	//вводное слово
	sg_type_preds[7]=tr1::shared_ptr<comparer>(new 	type_comparer<equal_to <int> >(equal_to<int> (),Word::entrance_word));

	for(int i=0;i<8;++i)
	{
		comparer& cur_c=*sg_type_preds[i];
		for(vector<Word>::iterator it=s.m_left_border;it!=s.m_right_border;++it)
			if(cur_c(*it))
			{
				s.first=it;
				return i+1;
			}
	}
	if(has_dash(s))
		return 0;
	return 9;

};

void find_odnorodnost(vector<Segment>& segs,vector<Segment>::iterator cur_seg)
{
	if(cur_seg == segs.end()-1)
		return;
	if(!(cur_seg->has_right_union() || (cur_seg+1)->has_left_union()))
		return;
	vector<Segment>::iterator it=cur_seg+1;
		for(vector<Word>::iterator cur_seg_it=cur_seg->m_left_border;
			cur_seg_it!=cur_seg->m_right_border;++cur_seg_it)
		{
			odnorodnost_getter og(cur_seg_it->m_MI);
			vector<Word>::iterator odnor_word=find_if(it->m_left_border,it->m_right_border,og);
			if(!(odnor_word==it->m_right_border))
			{
				it->m_has_neighbour=true;
				it->m_neighbour=it-1;
				it->m_connect_type=2;
			}
		}
}


bool odnorodnost_getter::operator ()(const Word& w) const
{
	if((m_MI & 0x00078000) != (w.m_MI&0x00078000))
		return false;
	switch(m_MI & 0x00078000)
	{
	case 0x00028000:
		return (m_MI & 7) == (w.m_MI & 7);
	}
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

void get_included(vector<Segment>& ws,vector<Segment>::iterator cur_seg)
{
	if(cur_seg->m_type!=5)
		return;
	unsigned long int first_MI=cur_seg->first->m_MI;
	unsigned long int find_MI=first_MI & 127 + 0x00028000;
	vector<Word>::iterator it=find_if(cur_seg->m_left_border,cur_seg->m_right_border,
		MI_comparer<equal_to<unsigned long int> >(equal_to<unsigned long int>(),
			find_MI,find_MI,false));
	if(it==cur_seg->m_right_border || cur_seg==ws.begin())
		return;
	vector<Segment>::iterator it2=cur_seg-1;
	it=find_if(it2->m_left_border,it2->m_right_border,
		MI_comparer<equal_to<unsigned long int> >(equal_to<unsigned long int>(),
		find_MI,find_MI,false));
	if(it!=it2->m_right_border)
	{
		cur_seg->m_type_sv=1;
		cur_seg->m_has_neighbour=true;
		cur_seg->m_neighbour=it2;
		cur_seg->m_owner=it;
	}
}