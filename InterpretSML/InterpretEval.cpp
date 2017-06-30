/*
	InterpretEval.cpp

	Class to InterpretEvaluate a reverse polish stack.
	Either for compilation or interpritation.

*/

#include "../StdAfx.h"
#include "InterpretEval.h"
#include <math.h>

namespace InterpretSML {
using namespace SML;

InterpretEval::InterpretEval (vector<Token> &tokens,int po) :
Eval(tokens,po)
{
}

InterpretEval::~InterpretEval (void)
{
}

void InterpretEval::LoadAccumulator(const Token& op)
{
}

Token& InterpretEval::Run(const int evaluation_order,Token& op1,const Token& Operator,Token& op2)
{
	switch (evaluation_order)			// Cannot get zero type.
	{
	case 1:								// Op1 is a variable in memory, op2 is a constant.
		break;
	case 2:								// Op2 is a variable in memory, op1 is a constant.
	case 3:								// both op1 and op2 are variables.
		break;
	}

	return op1;
}
}			// end namespace InterpretSML

