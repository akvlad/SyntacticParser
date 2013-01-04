#pragma once

#include <map>
#include <string>
#include <fstream>

using namespace std;

struct LemmOrganizer
{
	map<string, unsigned long int> LemmDict;

	LemmOrganizer(string fname)
	{
		ifstream f(fname);
		while(!f.eof())
		{
			string wrd;
			f>>wrd;
			unsigned long int MI;
			f>>MI;
			LemmDict[wrd]=MI;
		}		
	}

	unsigned long int getMI(string wrd)
	{
		return LemmDict[wrd];
	}	
};