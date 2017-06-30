/*
	InterpretSML.h

	Class holding the InterpretSML of the SML machine

*/

#ifndef __InterpretSML_
#define __InterpretSML_

#include "../SMLib/Main.h"

namespace InterpretSML {
using namespace SML;

class InterpretSML : public Main
{
public:

	InterpretSML (int,char **);
	~InterpretSML ();

	void SetOptions (bool);
	void Print (ostream &);
	int  Pass1 (vector<Token>&,string&);
	int  Pass2 (ostream &);
	void ProcessConstant(SymbolConstant *);
	void ProcessLine(SymbolLine *,bool *,bool);
	void ProcessVariable(SymbolVariable *);

private:

};

} 			// end namespace InterpretSML

#endif		// __InterpretSML_

