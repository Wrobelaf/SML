/*
	Eval.h

	Abstract Class to evaluate a postfix numeric expression stack.
	Can be used for Compilation or interpritation.

*/

#ifndef __Eval_
#define __Eval_

#include "Token.h"

namespace SML {

class Eval
{
public:

	int  Evaluate (void);
	const Token& GetResult(void) const;
	friend ostream &operator<<(ostream &, const Eval &);

protected:

	Eval (vector<Token> &,int);
	~Eval (void);

	virtual Token&  Run(const int,Token&,const Token&,Token&)=0;

	vector<Token> postfix;

private:

	int offset;					// First operand in 'prefix' to process.
	vector<Token>&prefix;

	int    evaluationorder(const Token&,const Token&);
	const vector<Token>& GetPostfix(void) const;

};

}			// Namespace SML

#endif		// __Eval_





