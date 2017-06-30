/*
	InterpretEval.h

	Abstract Class to Interpret a postfix numeric expression stack.

*/

#ifndef __InterpretEval_
#define __InterpretEval_

#include "../SMLib/Token.h"
#include "../SMLib/Eval.h"

namespace InterpretSML {
using namespace SML;

class InterpretEval : public Eval
{
public:

	InterpretEval (vector<Token> &,int);
	~InterpretEval (void);

	void   LoadAccumulator(const Token&);
	Token& Run(const int,Token&,const Token&,Token&);

private:

};

} 			// end namespace InterpretSML

#endif		// __InterpretEval_
