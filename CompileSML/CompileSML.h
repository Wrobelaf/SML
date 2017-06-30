/*
	CompileSML.h

	Class holding the CompileSML of the SML machine

*/

#ifndef __CompileSML_
#define __CompileSML_

#include "../SMLib/Main.h"

namespace CompileSML {
using namespace SML;

class CompileSML : public SML::Main
{
public:

	CompileSML (int,char **);
	~CompileSML ();

	void SetOptions (bool);
	void Print (ostream &);
	int  Pass1 (vector<Token>& t,string&);
	int  Pass2 (ostream &);
	void ProcessConstant(SymbolConstant *);
	void ProcessLine(SymbolLine *,bool *,bool);
	void ProcessVariable(SymbolVariable *);

private:

	void LoadAccumulatorWithN (unsigned int);

};

}			// End namespace CompileSML

#endif		// __CompileSML_

