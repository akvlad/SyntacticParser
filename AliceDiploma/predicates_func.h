#pragma once

#include "Word.h"

/*-----ÏĞÅÄÈÊÀÒÛ ÄËß ÎÏĞÅÄÅËÅÍÈß ÔÎĞÌÛ ÑÅĞÌÅÍÒÀ--------*/

bool pred_form_1(const Word& w)
{
	if (w.type!=Word::Word_type::word || w.m_all_lemms.size()>=2) return false;
	return (w.m_MI & 0x00078000) == 0x00008000 && (w.m_MI & 0x00000600) != 0;
}

bool pred_form_2(const Word& w)
{
	if (w.type!=Word::Word_type::word || w.m_all_lemms.size()>=2) return false;
	return (w.m_MI & 0x00010000) == 0x00010000 && (w.m_MI & 0x00006000) == 0x00006000;
}

bool pred_form_3(const Word& w)
{
	if (w.type!=Word::Word_type::word || w.m_all_lemms.size()>=2) return false;
	return (w.m_MI & 0x00020000) == 0x00020000 && (w.m_MI & 0x00006000) == 0x00006000;
}

bool pred_form_4(const Word& w)
{
	if (w.type!=Word::Word_type::word || w.m_all_lemms.size()>=2) return false;
	return (w.m_MI & 0x00068000) == 0x00068000;
}