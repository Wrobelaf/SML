/*
    CompileEval.cpp

    Class to CompileEvaluate a reverse polish stack.
    Either for compilation or interpritation.

*/

#include "../StdAfx.h"
#include "../SMLib/common_sml_header.h"
#include "../SMLib/RecoverableError.h"
#include "../SMLib/SymbolConstant.h"
#include "../SMLib/SymbolLine.h"
#include "../SMLib/SymbolTable.h"
#include "../SMLib/SymbolVariable.h"
#include "CompileEval.h"
#include "CompilerInstructionTable.h"
#include <math.h>
extern double round_to (double,int);

namespace CompileSML {
using namespace SML;

extern CompilerInstructionTable CI;                 // Only a single instance of this is required.

CompileEval::CompileEval (vector<Token> &tokens,int po) :
Eval(tokens,po)
{
}

CompileEval::~CompileEval (void)
{
}

Symbol& CompileEval::LoadAccumulator(const Token& op)
{
    // Generate instructions to load the accumulator with literal valus in 'Token'
    // generate a load literal integer followed by divide or multiply to get decimal
    // place in correct position.  store -ve numbers as nines complement so most sig digit
    // of the operand cannot be '9'
    // Note That on loading a constant item, if not already in the symbol table, the constant
    // item will be created on the fly (allocated next free space) and the accumulator will
    // be left with the item in it (or address of item in cast of string constants) so nothing needs
    // to be done here.

    stringstream m;

    switch (*op)
    {
    case Token::STRING:             // Load address of string into accumulator
    {
        bool found;
        SymbolConstant &c = SymbolConstant::Find (op);
		SymbolConstant &d = SymbolConstant::Find (Token(c.GetAddress()),&found);  // Address to string left in accumulator.
		if (found)
		{
            CI.Add(new CompilerInstructions(Instruction::LOAD,d.GetAddress()));
		}
        return c;
    }
    case Token::NUMBER:         // Take mantisa * (-1*sign) * (10 ^ shift)
    {
        bool found;
        SymbolConstant &c = SymbolConstant::Find (op,&found);

        if (found)                  // If constant already in a memory location - reuse.
        {
            CI.Add(new CompilerInstructions(Instruction::LOAD,c.GetAddress()));
        }
        return c;
    }
    case Token::VAR:
    {
        // Find address in symbol table to the variable:

        SymbolVariable &v = SymbolVariable::Find (op);
        CI.Add(new CompilerInstructions(Instruction::LOAD,v.GetAddress()));
        return v;
    }
    default:
        m << "Error: Internal error 1 in compiler, contact support: " <<  *this << endl;
        throw runtime_error (m.str());
    }
}

void CompileEval::GenerateComparitor (Symbol& s) const
{
    switch (*(s.GetToken()))
    {
    case Token::STRING:
        // The operand will be the address to the start of the constant string, so can do a direct comparison.
        CI.Add(new CompilerInstructions(Instruction::CMP_STRING,s.GetAddress()));
        break;
    case Token::NUMBER:
        CI.Add(new CompilerInstructions(Instruction::SUBTRACT,s.GetAddress()));
        break;
    case Token::VAR:
        // The memory location allocated to the VAR will contain the address to the length of the string (start of string)
        // so the comparison must do one level of indirection when the operand is a VAR STRING.
        // Note if the VAR has been loaded into the accumulator the indirection will have already been
        // done and the accumulator will hold the address to the length of string (start of string)
        // Note, for numbers, the memory location allocated to the VAR will just contain the number so no
        // indirection needs to be done for NUMBERS.
        CI.Add(new CompilerInstructions((s.GetToken().GetBaseType() == Token::STRING) ? Instruction::INDIRECT_CMP_STRING : Instruction::SUBTRACT,s.GetAddress()));
        break;
    }
}

void CompileEval::OperateOnAccumulator(const Token::elements e, Symbol& Op)
{
    // Op is a symbol table entry acting upon the accumulator. So the operand is in a memory location

    switch (e)
    {
    case Token::ADD:
        CI.Add(new CompilerInstructions(Instruction::ADD,Op.GetAddress()));
        break;
    case Token::MULTIPLY:
        CI.Add(new CompilerInstructions(Instruction::MULTIPLY,Op.GetAddress()));
        break;
    case Token::SUBTRACT:
        CI.Add(new CompilerInstructions(Instruction::SUBTRACT,Op.GetAddress()));
        break;
    case Token::DIVIDE:
        CI.Add(new CompilerInstructions(Instruction::DIVIDE,Op.GetAddress()));
        break;
    case Token::POWER:
        CI.Add(new CompilerInstructions(Instruction::POWER,Op.GetAddress()));
        break;
    case Token::MODULUS:
        CI.Add(new CompilerInstructions(Instruction::MODULUS,Op.GetAddress()));
        break;
    case Token::LT:
        GenerateComparitor (Op);
        CI.Add(new CompilerInstructions(Instruction::BRANCH_NEG,CI.GetNextInstructionAddress()+3));
        CI.Add(new CompilerInstructions(Instruction::LOAD_LITERAL,0));
        CI.Add(new CompilerInstructions(Instruction::BRANCH,CI.GetNextInstructionAddress()+2));
        CI.Add(new CompilerInstructions(Instruction::LOAD_LITERAL,1));
        break;
    case Token::LT_E:
        GenerateComparitor (Op);
        CI.Add(new CompilerInstructions(Instruction::BRANCH_ZERO,CI.GetNextInstructionAddress()+4));
        CI.Add(new CompilerInstructions(Instruction::BRANCH_NEG,CI.GetNextInstructionAddress()+3));
        CI.Add(new CompilerInstructions(Instruction::LOAD_LITERAL,0));
        CI.Add(new CompilerInstructions(Instruction::BRANCH,CI.GetNextInstructionAddress()+2));
        CI.Add(new CompilerInstructions(Instruction::LOAD_LITERAL,1));
        break;
    case Token::EQUAL:
        GenerateComparitor (Op);
        CI.Add(new CompilerInstructions(Instruction::BRANCH_ZERO,CI.GetNextInstructionAddress()+3));
        CI.Add(new CompilerInstructions(Instruction::LOAD_LITERAL,0));
        CI.Add(new CompilerInstructions(Instruction::BRANCH,CI.GetNextInstructionAddress()+2));
        CI.Add(new CompilerInstructions(Instruction::LOAD_LITERAL,1));
        break;
    case Token::N_EQUAL:
        GenerateComparitor (Op);
        CI.Add(new CompilerInstructions(Instruction::BRANCH_ZERO,CI.GetNextInstructionAddress()+3));
        CI.Add(new CompilerInstructions(Instruction::LOAD_LITERAL,1));
        CI.Add(new CompilerInstructions(Instruction::BRANCH,CI.GetNextInstructionAddress()+2));
        CI.Add(new CompilerInstructions(Instruction::LOAD_LITERAL,0));
        break;
    case Token::GT_E:
        GenerateComparitor (Op);
        CI.Add(new CompilerInstructions(Instruction::BRANCH_ZERO,CI.GetNextInstructionAddress()+2));
        CI.Add(new CompilerInstructions(Instruction::BRANCH_NEG,CI.GetNextInstructionAddress()+3));
        CI.Add(new CompilerInstructions(Instruction::LOAD_LITERAL,1));
        CI.Add(new CompilerInstructions(Instruction::BRANCH,CI.GetNextInstructionAddress()+2));
        CI.Add(new CompilerInstructions(Instruction::LOAD_LITERAL,0));
        break;
    case Token::GT:
        GenerateComparitor (Op);
        CI.Add(new CompilerInstructions(Instruction::BRANCH_ZERO,CI.GetNextInstructionAddress()+4));
        CI.Add(new CompilerInstructions(Instruction::BRANCH_NEG,CI.GetNextInstructionAddress()+3));
        CI.Add(new CompilerInstructions(Instruction::LOAD_LITERAL,1));
        CI.Add(new CompilerInstructions(Instruction::BRANCH,CI.GetNextInstructionAddress()+2));
        CI.Add(new CompilerInstructions(Instruction::LOAD_LITERAL,0));
        break;
    }
}

void CompileEval::OperateOnAccumulator(const Token::elements e, Token& op)
{
    // Op is a varaible/constant acting upon the accumulator

    switch (*op)
    {
    case Token::NUMBER:
    case Token::STRING:
    {
        OperateOnAccumulator (e,SymbolConstant::Find (op));
        break;
    }
    case Token::VAR:
        OperateOnAccumulator (e,SymbolVariable::Find (op));
        break;
    }
}

SymbolVariable& CreateTempLocation(Token &t)
{
    // Create a temporary name from the size of the current symbol table.
    // Set its 'type' to the type of 't'. If 't' is a variable then find the type
    // of the variable and assign the variables type to this temp item.

    stringstream s;
    s << "T" << SymbolTable::Instance()->GetSymbolTable().size()+1;

    SymbolVariable& v = SymbolVariable::Find(s.str());  // Creates a temporary variable as 's' does not exist.

    if (*t == Token::VAR)
    {
        bool found;
        SymbolVariable tv = SymbolVariable::Find (t,&found);

        if (!found)
        {
            stringstream m;
            m << "Error: Attempting to use an uninitialised variable:  " << t.GetString() << endl;
            throw RecoverableError (m.str());
        }
    }
    return v;
}

Token& CompileEval::Run(const int evaluation_order,Token& op1,const Token& Operator,Token& op2)
{
    // On entry we know the operands are of compatible types, as earlier check made in common code
    // for errors of the form 'IF 4 == "ABC" THEN GOTO 40.
    // Also checked for are operators make sence for the operands involved.  e.g.  "abc" * "def" is wrong.

    stringstream m;

    switch (evaluation_order)               // Note cannot get zero.
    {
    case 1:                                 // Op1 is a variable in memory, op2 is a constant.
    {
        SymbolVariable& v = CreateTempLocation (op2);

        switch (*Operator)
        {
        case Token::ADD:					// Commutitive stuff
            this->LoadAccumulator (op2);
            this->OperateOnAccumulator (*Operator,op1);
        	v.SetAssignedTo(Token(op2.GetBaseType()));
            break;

        case Token::MULTIPLY:               // Commutitive stuff
            this->LoadAccumulator (op2);
            this->OperateOnAccumulator (*Operator,op1);
        	v.SetAssignedTo(Token(0.0));	// Result is numeric, so set temp variables base type up.
            break;

        case Token::DIVIDE:
        case Token::MODULUS:
            if (op2 == 0.0)
            {
                m << "Zero divide.";
                throw RecoverableError (m.str());
            }
            // Fall though

        case Token::SUBTRACT:
        case Token::POWER:
        case Token::LT:
        case Token::LT_E:
        case Token::EQUAL:
        case Token::N_EQUAL:
        case Token::GT_E:
        case Token::GT:
        {
            // Side effect of leaving constant in accumulator. (Don't use facility here)

            SymbolConstant& s = SymbolConstant::Find(op2);
            this->LoadAccumulator (op1);
            this->OperateOnAccumulator (*Operator,s);           // Will now exist in memory.
        	v.SetAssignedTo(Token(0.0));	// Result is numeric, so set temp variables base tipe up.
            break;
        }
        case Token::OR:
            this->LoadAccumulator (op2);                        // get constant into memory, so we know the jump amount.
            this->LoadAccumulator (op1);
            CI.Add(new CompilerInstructions(Instruction::BRANCH_ZERO,CI.GetNextInstructionAddress()+3));
            CI.Add(new CompilerInstructions(Instruction::LOAD_LITERAL,1));
            CI.Add(new CompilerInstructions(Instruction::BRANCH,CI.GetNextInstructionAddress()+6));
            this->LoadAccumulator (op2);
            CI.Add(new CompilerInstructions(Instruction::BRANCH_ZERO,CI.GetNextInstructionAddress()+3));
            CI.Add(new CompilerInstructions(Instruction::LOAD_LITERAL,1));
            CI.Add(new CompilerInstructions(Instruction::BRANCH,CI.GetNextInstructionAddress()+2));
            CI.Add(new CompilerInstructions(Instruction::LOAD_LITERAL,0));
        	v.SetAssignedTo(Token(0.0));	// Result is numeric, so set temp variables base type up.
            break;

        case Token::AND:
            this->LoadAccumulator (op2);                        // get constant into memory, so we know the jump amount.
            this->LoadAccumulator (op1);
            CI.Add(new CompilerInstructions(Instruction::BRANCH_ZERO,CI.GetNextInstructionAddress()+5));
            this->LoadAccumulator (op2);
            CI.Add(new CompilerInstructions(Instruction::BRANCH_ZERO,CI.GetNextInstructionAddress()+3));
            CI.Add(new CompilerInstructions(Instruction::LOAD_LITERAL,1));
            CI.Add(new CompilerInstructions(Instruction::BRANCH,CI.GetNextInstructionAddress()+2));
            CI.Add(new CompilerInstructions(Instruction::LOAD_LITERAL,0));
        	v.SetAssignedTo(Token(0.0));	// Result is numeric, so set temp variables base type up.
            break;
        }
	    CI.Add(new CompilerInstructions (Instruction::STORE,v.GetAddress()));
		op1 = Token (Token::VAR,v.GetString());
        break;
    }
    case 2:         // Op2 is a variable in memory, op1 is a constant.
    case 3:         // both op1 and op2 are variables.
    {
        SymbolVariable &v = CreateTempLocation (op1);

        switch (*Operator)
        {
        case Token::ADD:
            this->LoadAccumulator (op1);
            this->OperateOnAccumulator (*Operator,op2);
        	v.SetAssignedTo(Token(op2.GetBaseType()));
            break;

        case Token::MULTIPLY:
        case Token::SUBTRACT:
        case Token::DIVIDE:
        case Token::POWER:
        case Token::MODULUS:
        case Token::LT:
        case Token::LT_E:
        case Token::EQUAL:
        case Token::N_EQUAL:
        case Token::GT_E:
        case Token::GT:
            this->LoadAccumulator (op1);
            this->OperateOnAccumulator (*Operator,op2);
        	v.SetAssignedTo(Token(0.0));	// Result is numeric, so set temp variables base type up.
            break;

        case Token::OR:
            this->LoadAccumulator (op1);
            CI.Add(new CompilerInstructions(Instruction::BRANCH_ZERO,CI.GetNextInstructionAddress()+3));
            CI.Add(new CompilerInstructions(Instruction::LOAD_LITERAL,1));
            CI.Add(new CompilerInstructions(Instruction::BRANCH,CI.GetNextInstructionAddress()+6));
            this->LoadAccumulator (op2);
            CI.Add(new CompilerInstructions(Instruction::BRANCH_ZERO,CI.GetNextInstructionAddress()+3));
            CI.Add(new CompilerInstructions(Instruction::LOAD_LITERAL,1));
            CI.Add(new CompilerInstructions(Instruction::BRANCH,CI.GetNextInstructionAddress()+2));
            CI.Add(new CompilerInstructions(Instruction::LOAD_LITERAL,0));
        	v.SetAssignedTo(Token(0.0));	// Result is numeric, so set temp variables base type up.
            break;

        case Token::AND:
            this->LoadAccumulator (op1);
            CI.Add(new CompilerInstructions(Instruction::BRANCH_ZERO,CI.GetNextInstructionAddress()+5));
            this->LoadAccumulator (op2);
            CI.Add(new CompilerInstructions(Instruction::BRANCH_ZERO,CI.GetNextInstructionAddress()+3));
            CI.Add(new CompilerInstructions(Instruction::LOAD_LITERAL,1));
            CI.Add(new CompilerInstructions(Instruction::BRANCH,CI.GetNextInstructionAddress()+2));
            CI.Add(new CompilerInstructions(Instruction::LOAD_LITERAL,0));
        	v.SetAssignedTo(Token(0.0));	// Result is numeric, so set temp variables base type up.
            break;
        }
	    CI.Add(new CompilerInstructions (Instruction::STORE,v.GetAddress()));
		op1 = Token (Token::VAR,v.GetString());
		break;
    }
    }
    return op1;
}

}		// end namespace CompileSML

