/*
    CompileEval.h

    Abstract Class to Interpret a postfix numeric expression stack.

*/

#ifndef __CompileEval_
#define __CompileEval_

#include "../SMLib/SymbolVariable.h"
#include "../SMLib/Token.h"
#include "../SMLib/Eval.h"

namespace CompileSML {
using namespace SML;

class CompileEval : public Eval
{
public:

    CompileEval (vector<Token> &,int);
    ~CompileEval (void);

    Symbol& LoadAccumulator(const Token&);
    Token&  Run(const int,Token&,const Token&,Token&);

private:

    void StoreAccumulator(const Token&);
    void OperateOnAccumulator(const Token::elements, Symbol&);
    void OperateOnAccumulator(const Token::elements, Token&);
    void GenerateComparitor (Symbol&) const;

};
}			// End namespace CompileSML

#endif      // __CompileEval_
