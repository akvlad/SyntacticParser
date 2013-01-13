#pragma once

#include <map>
#include <string>
#include <fstream>

using namespace std;

struct SLI
{
	string LEMM;
	unsigned long int MI;
};

struct LemmOrganizer
{
	map<string, SLI> LemmDict;

	LemmOrganizer(string fname)
	{
		ifstream f(fname);
		while(!f.eof())
		{
			string wrd;
			f>>wrd;
			SLI MI;
			f>>MI.MI;
			f>>MI.LEMM;
			LemmDict[wrd]=MI;
		}		
	}

	unsigned long int getMI(string wrd)
	{
		return LemmDict[wrd].MI;
	}	
	string getLEMM(string wrd)
	{
		return LemmDict[wrd].LEMM;
	}	

};