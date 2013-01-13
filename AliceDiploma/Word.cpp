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
			new_word.m_all_lemms.push_back(vector<string>(1,lo->getLEMM(new_word.m_word)));
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


string translate_add_MI(const conll& w)
{
	if(w.type!=Word::word)
		return "_";
	string res("");
	if((w.m_MI & 0x00000001) == 0x00000001) res+="|NO";
	if((w.m_MI & 0x00000002) == 0x00000002)  res+="|GE";
	if((w.m_MI & 0x00000003) == 0x00000003)  res+="|DA";
	if((w.m_MI & 0x00000004) == 0x00000004)  res+="|AC";
	if((w.m_MI & 0x00000005) == 0x00000005)  res+="|AB";
	if((w.m_MI & 0x00000006) == 0x00000006)  res+="|LO";
	if((w.m_MI & 0x00000007) == 0x00000007)  res+="|VO";
	if((w.m_MI & 0x00000008) == 0x00000008)  res+="|MA";
	if((w.m_MI & 0x00000010) == 0x00000010)  res+="|FE";
	if((w.m_MI & 0x00000018) == 0x00000018)  res+="|NE";
	if((w.m_MI & 0x00000020) == 0x00000020)  res+="|SI";
	if((w.m_MI & 0x00000040) == 0x00000040)  res+="|PL";
	if((w.m_MI & 0x00000080) == 0x00000080)  res+="|PS";
	if((w.m_MI & 0x00000100) == 0x00000100)  res+="|FU";
	if((w.m_MI & 0x00000180) == 0x00000180)  res+="|CO";
	if((w.m_MI & 0x00000200) == 0x00000200)  res+="|FF";
	if((w.m_MI & 0x00000400) == 0x00000400)  res+="|FS";
	if((w.m_MI & 0x00000600) == 0x00000600)  res+="|FT";
	if((w.m_MI & 0x00000800) == 0x00000800)  res+="|ACT";
	if((w.m_MI & 0x00001000) == 0x00001000)  res+="|PAS";
	if((w.m_MI & 0x00002000) == 0x00002000)  res+="|CFA";
	if((w.m_MI & 0x00004000) == 0x00004000)  res+="|EX";
	if((w.m_MI & 0x00006000) == 0x00006000)  res+="|SH";
	if((w.m_MI & 0x00080000) == 0x00080000)  res+="|AN";
	if((w.m_MI & 0x00100000) == 0x00100000)  res+="|NAN";
	if((w.m_MI & 0x00200000) == 0x00200000)  res+="|IM";
	if((w.m_MI & 0x00400000) == 0x00400000)  res+="|ACC";
	if((w.m_MI & 0x00800000) == 0x00800000)  res+="|INC";
	if((w.m_MI & 0x00C00000) == 0x00C00000)  res+="|AI";
	if((w.m_MI & 0x01000000) == 0x01000000)  res+="|CN";
	if((w.m_MI & 0x02000000) == 0x02000000)  res+="|OR";
	if((w.m_MI & 0x03000000) == 0x03000000)  res+="|PLV";
	if((w.m_MI & 0x04000000) == 0x04000000)  res+="|PN";
	if((w.m_MI & 0x08000000) == 0x08000000)  res+="|PRA";
	if((w.m_MI & 0x0C000000) == 0x0C000000)  res+="|PPL";
	if((w.m_MI & 0x10000000) == 0x10000000)  res+="|PIM";
	if((w.m_MI & 0x10000000) == 0x10000000)  res+="|VIT";
	if((w.m_MI & 0x10000000) == 0x10000000)  res+="|VRF";
	if((w.m_MI & 0x800000000) == 0x800000000)  res+="|IN";
	if(res.size()==0)
		res="|_";
	res.erase(res.begin());
	return res;
}

string translate_PS(const conll& w)
{
	if(w.type==Word::sentence_union)
		return "U";
	if(w.type==Word::sign || w.type==Word::dash)
		return "S";
	string res("");
	if((w.m_MI & 0x00008000) == 0x00008000) res="V";
	if((w.m_MI & 0x00010000) == 0x00010000) res="P";
	if((w.m_MI & 0x00018000) == 0x00018000) res="G";
	if((w.m_MI & 0x00020000) == 0x00020000) res="A";
	if((w.m_MI & 0x00028000) == 0x00028000) res="N";
	if((w.m_MI & 0x00030000) == 0x00030000) res="PRN";
	if((w.m_MI & 0x00038000) == 0x00038000) res="NM";
	if((w.m_MI & 0x00040000) == 0x00040000) res="AV";
	if((w.m_MI & 0x00048000) == 0x00048000) res="PRP";
	if((w.m_MI & 0x00050000) == 0x00050000) res="C";
	if((w.m_MI & 0x00058000) == 0x00058000) res="PA";
	if((w.m_MI & 0x00060000) == 0x00060000) res="I";
	if((w.m_MI & 0x00068000) == 0x00068000) res="PR";
	if((w.m_MI & 0x00070000) == 0x00070000) res="PO";
	if((w.m_MI & 0x00078000) == 0x00078000) res="CW";
	return res;
}