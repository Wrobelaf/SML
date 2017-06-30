/*
	memory.h

	Class holding the memory of the SML machine

*/

#ifndef __MEMORY_
#define __MEMORY_

#include "../StdAfx.h"
#include "../SMLib/operand.h"

namespace SML {

class memory
{
public:

	memory ();
	memory (istream &);
	memory (istream &,ostream &);
	~memory ();

	double operator[] (const Operand&) const;
	void SetMemory (const Operand&,const double);
	void SetDumpAmount (const int);
	const int GetDumpAmount (void) const;

	friend ostream &operator<<(ostream &, const memory &);

private:

	const bool chk (const int) const;
	vector<double> data;
	int dumpAmount;
};

}

#endif		// __MEMORY_

