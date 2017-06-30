// CompileSML.cpp : Defines the entry point for the console application.
//

#include "../StdAfx.h"
#include "../SMLib/common_sml_header.h"
#include "../SMLib/RecoverableError.h"
#include "CompileSML.h"
#include "CompilerInstructionTable.h"
#include "CompileEval.h"


extern int   NUM_INVALID;
extern int   NUM_DEC;
extern int   NUM_HEX;
extern int   NUM_REAL;

int main(int argc, char**argv)
{
	CompileSML::CompileSML itm (argc,argv);
	SML::SymbolTable::Instance()->SetHarness(&itm);
	return itm.Run ();
}

namespace CompileSML {
using namespace SML;

extern CompilerInstructionTable	CI;					// Only a single instance of this is required.


extern SymbolVariable& CreateTempLocation(Token &);


CompileSML::CompileSML (int pargc,char **pargv) :
Main(pargc,pargv)
{
	// Put instruction that wiSybolTable::Instance()->ll tell machine where free space starts.
	// Fix up address to actual start of free space on second pass.
	CI.Add(new CompilerInstructions(Instruction::SET_HEAP_ADD,0));
}

CompileSML::~CompileSML ()
{
}

void CompileSML::SetOptions(bool pdebug)
{
	CI.debug (pdebug);
}

static void fix (CompilerInstructions* i)
{
	// fixing up any unresolved target addresses.CI
	int lineno;
	if ((lineno=(*i).GetAddressToLine()) != 0)
	{
		SymbolLine *l = new SymbolLine(lineno);
		vector<Symbol*>::iterator li = SymbolTable::Instance()->Find(l);

		if (li == SymbolTable::Instance()->GetSymbolTable().end())
		{
			stringstream m;
			m << "Target line number " << lineno << " not found." << endl;
			throw RecoverableError (m.str());
		}
		(*i).SetAddressToLine((*li)->GetAddress());
	}
}

int CompileSML::Pass1 (vector<Token>& line,string &s)
{
	// e.g. line is:
	//  10  LET a = 12.7   'line' holds each of these text fragments as a seperate 'Token'.
	// Return 0 to continue processing or 1 to stop the compilation process.

	int ret = 0;
	stringstream m;
	bool found;

	// Process the line number

	if (*line[0] != Token::NUMBER)
	{
		m << "Every line must start with a integer line number:   " << s << endl;
		throw RecoverableError (m.str());
	}

	if (line[0].GetBw() & NUM_REAL)
	{
		m << "Line number must be a integer:   " << s << endl;
		throw RecoverableError (m.str());
	}

	SymbolLine::Find (line[0],&found);
	if (found)
	{
		m << "Error: Duplicate line number  encountered:   " << s << endl;
		throw RecoverableError (m.str());
	}

	// Analyse the actual command

	Token command = line[1];

	switch (*command)
	{
	case Token::REM:					// Input line number into the symbol table that's all there is to do.
		break;
	case Token::INPUT:
	{
		// INPUT <variable> , <variable>  Where 'variable' must have already been assigned a type,
		// i.e. can not be unassigned.

		if (line.size() < 3)
		{
			m << "Error: " << command.GetId() << " command requires at least one operand: " << s << endl;
			throw RecoverableError (m.str());
		}

		int toki = 2;
		do
		{
			switch (*line[toki])
			{
			case Token::COMMA:
				m << "Error: Trailing or mismatched comma seperator. " << s << endl;
				throw RecoverableError (m.str());

			case Token::VAR:
			{
				SymbolVariable &v = SymbolVariable::Find(line[toki],&found);
				if (!found)
				{
					m << "Error: Attempting to use an uninitialised variable:  " << s << endl;
					throw RecoverableError (m.str());
				}

				if (v.GetAssignedTo() == Token::STRING)
				{
					CI.Add(new CompilerInstructions(Instruction::READ_STRING,v.GetAddress()));
				}
				else if (v.GetAssignedTo() == Token::NUMBER)
				{
					CI.Add(new CompilerInstructions(Instruction::READ,v.GetAddress()));
				}
				else
				{
					m << "Error: Only strings or numbers can be input or output (assign type to var before using):  " << s << endl;
					throw RecoverableError (m.str());
				}
				break;
			}
			default:
				m << "Error: INPUT command's operands must be a variable identifer or constant: " << s << endl;
				throw RecoverableError (m.str());
			}

			if (line.size() > toki+2)
			{
				if (*line[++toki] != Token::COMMA)
				{
					m << "Error: Comma is the only operand seperator allowed. " << s << endl;
					throw RecoverableError (m.str());
				}
			}

			toki++;

		} while (toki < line.size());

		break;
	}
	case Token::PRINT:
	{
		// PRINT <expression> , <expression>

		if (line.size() < 3)
		{
			m << "Error: " << command.GetId() << " command requires at least one operand: " << s << endl;
			throw RecoverableError (m.str());
		}

		int toki = 2;
		do
		{
			vector<Token>::iterator i = find (line.begin()+toki,line.end(),Token::COMMA);

			int diff = i-line.begin()-toki;

			if (diff == 0)
			{
				m << "Error: Trailing or mismatched comma seperator. " << s << endl;
				throw RecoverableError (m.str());
			}
			vector<Token> lhs(diff);
			copy (line.begin()+toki,i,lhs.begin());

			//copy(lhs.begin(), lhs.end(), ostream_iterator<Token>(cout, "\n"));
			//cout << endl;

			switch (*line[toki])
			{
			default:
			{
  				CompileEval expr(lhs,0);
				expr.Evaluate();
				SymbolVariable &v = SymbolVariable::Find(expr.GetResult(),&found);

				if (!found)
				{
					m << "Error: Attempting to use an uninitialised variable:  " << s << endl;
					throw RecoverableError (m.str());
				}
				if (v.GetAssignedTo() == Token::STRING)
				{
					CI.Add(new CompilerInstructions(Instruction::WRITE_STRING,v.GetAddress()));
				}
				else if (v.GetAssignedTo() == Token::NUMBER)
				{
					CI.Add(new CompilerInstructions(Instruction::WRITE,v.GetAddress()));
				}
				else
				{
					m << "Error: Only strings or numbers can be input or output (assign type to var before using):  " << s << endl;
					throw RecoverableError (m.str());
				}
				break;
			}
			case Token::STRING:
			{
				SymbolConstant &c = SymbolConstant::Find(line[toki]);
				SymbolConstant::Find (Token(c.GetAddress()));  // Address to string left in accumulator.
				SymbolVariable &v = CreateTempLocation(line[toki]);
				CI.Add(new CompilerInstructions(Instruction::STORE,v.GetAddress()));
				CI.Add(new CompilerInstructions(Instruction::WRITE_STRING,v.GetAddress()));
				break;
			}
			}

			toki += diff+1;				// Get over COMMA to start processing the next <expression>

		} while (toki < line.size());

		break;
	}
	case Token::LET:
	{
		// Assign a memory location to hold the contents of this variable.

		SymbolVariable& v = SymbolVariable::Find (line[2]);

		if (line.size() < 5)
		{
			m << "Error: LET command requires at least one assignment operation: " << s << endl;
			throw RecoverableError (m.str());
		}

		if (*line[2] != Token::VAR)
		{
			m << "Error: LET must assign an expression to a variable. " << s << endl;
			throw RecoverableError (m.str());
		}

		if (*line[3] != Token::EQUAL)
		{
			m << "Error: LET misses a '=' after the l-value variable. " << s << endl;
			throw RecoverableError (m.str());
		}


		CompileEval expr(line,4);
		expr.Evaluate();
		v.SetAssignedTo (expr.GetResult());

		expr.LoadAccumulator (v.GetAssignedTo());

		// Save result that's currently in the accumulator.

		CI.Add(new CompilerInstructions(Instruction::STORE,v.GetAddress()));
		break;
	}
	case Token::GOTO:
	{
		// GOTO 50  We can either fixup address now or wait till the second pass.

		if (*line[2] != Token::NUMBER)
		{
			m << "Error: GOTO's operand must be a line number. " << s << endl;
			throw RecoverableError (m.str());
		}

		if (line[2].GetBw() & NUM_REAL)
		{
			m << "Line number must be a integer:   " << s << endl;
			throw RecoverableError (m.str());
		}

		SymbolLine::Find(line[2].GetNumber());		// Side effect of generating a BRANCH instruction.
		break;
	}
	case Token::IF:
	{
		// IF <expression> <comparison> <expression> GOTO <line number>
		// IF <expression> GOTO <line number>							# here if expression returns non-zero jump

		int run_type_expression;

		// First analise the lhs expression, extract all tokens upto GOTO

		vector<Token>::iterator i = find (line.begin()+2,line.end(),Token::GOTO);

		int diff = line.size()-(line.end()-i)-2;

		if ((i == line.end()) || (diff == 0))
		{
			m << "Syntax error: Format of a IF statment is:" << endl;
			m << "IF <expression> <comparison> <expression> GOTO <line number>" << endl;
			m << "  or" << endl;
			m << "IF <expression> GOTO <line number>" << endl;
			m << *this << endl;
			throw RecoverableError (m.str());
		}

		vector<Token> lhs(diff);
		copy (line.begin()+2,i,lhs.begin());

		CompileEval expr(lhs,0);
		run_type_expression = expr.Evaluate();
		const Token lhs_result = expr.GetResult();

		Token t = *(++i);
		if (*t != Token::NUMBER)
		{
			m << "Syntax error: IF <expression> GOTO <line number>, you have " << s << endl;
			throw RecoverableError (m.str());
		}

		if (! run_type_expression)
		{
			// Result worked out at compile time.  0=false=don't jump    1=true=jump

			if (lhs_result != 0.0)
				SymbolLine::Find(t.GetNumber());		// Side effect of generating a BRANCH instruction.
		}
		else		// Run time comparison.
		{
			// Jumping on <> 0 so we must branch over the next goto instruction if acc is ZERO.

			expr.LoadAccumulator (lhs_result);
			CI.Add(new CompilerInstructions(Instruction::BRANCH_ZERO,CI.GetNextInstructionAddress()+2));

			SymbolLine::Find(t.GetNumber());		// Side effect of generating a BRANCH instruction.
		}
		break;
	}
	case Token::END:

		CI.Add(new CompilerInstructions(Instruction::HALT,0));		// Just generate the HALT instruction
		break;

	case Token::EXIT:

		ret = 1;													// Just Finish compilation
		break;

	case Token::INVALID:
	default:
		m << "Unrecognised command in line: " << s << endl;
		throw RecoverableError (m.str());
	}
	return ret;
}

int CompileSML::Pass2 (ostream & out)
{
	// Sweap through all the compiler instructions, fixing up any unresolved target addresses.
	// Then outputing the compiled instructions.

	const vector<CompilerInstructions*>& ci = CI.GetCompilerInstructionTable();

	(*ci[0]).SetAddressToLine(CI.GetNextOperandAddress()+1);		// 1st instruction is SET_HEAP_ADD

	for_each (ci.begin()+1 , ci.end(), fix);
	return 0;
}


void CompileSML::Print(ostream & out)
{
	if (CI.debug())
		out << *(SymbolTable::Instance()) << endl;			// First output the symbol table

	out << CI;						// Followed by the current generation state (the program)

	if (!CI.debug())
		out << EOS;					// In 'program mode' i.e. not debugging, output the input stream terminator.

	out << endl;
}

void CompileSML::LoadAccumulatorWithN (unsigned int n)
{
	// Parcel up the number into bite size sections that fit into the operand.

	if (n > Instruction::MAX_POSITIVE_NUM)
	{
		unsigned int newn = n/Instruction::MAX_POSITIVE_NUM;

		LoadAccumulatorWithN (newn);

		CI.Add(new CompilerInstructions (Instruction::MULTIPLY_LITERAL,Instruction::MAX_POSITIVE_NUM));

		int remander = n-(newn*Instruction::MAX_POSITIVE_NUM);

		if (remander != 0)
			CI.Add(new CompilerInstructions (Instruction::ADD_LITERAL,remander));
	}
	else
	{
		CI.Add(new CompilerInstructions(Instruction::LOAD_LITERAL,n));
	}
}

void CompileSML::ProcessConstant(SymbolConstant *v)
{
	// New 'constant' encountered in the program. Do compiler specific things to it.

	CompilerInstructions* ci = 0;

	SymbolTable::Instance()->Add(v);

	if (*(v->GetToken()) == Token::NUMBER)
	{
		// For Numbers note this has the consiquence that if returning found=false the constant will also
		// be in the accumulator.

		v->SetAddress(CI.GetNextOperandAddress());

		int   power;
		bool  negative;

		LoadAccumulatorWithN (v->GetToken().GetNormalisedNumber (power,negative));

		if (negative)
		{
			ci = new CompilerInstructions(Instruction::MULTIPLY_LITERAL,1);
			CI.Add(ci);
			ci->GetOperand().MakeAsSignedValue();
		}


		if (power != 0)
		{
			// Now adjust the decimal point of the item just loaded into the accumulator : n*(10^pow)
			ci = new CompilerInstructions(Instruction::SHIFT,abs(power));
			CI.Add(ci);
			if (power < 0)
				ci->GetOperand().MakeAsSignedValue();
		}
		CI.Add(new CompilerInstructions(Instruction::STORE,v->GetAddress()));
	}
	else		// Load constant string into memory
	{
		// Constant strings start with a length word (that does not include itself) followed by
		// 'length' number of characters in ascending memory locations.
		// The symbol table records the address to the length word.

		string::const_reverse_iterator rit;
		for ( rit=v->GetToken().GetString().rbegin() ; rit < v->GetToken().GetString().rend(); rit++ )
		{
			CI.Add(new CompilerInstructions(Instruction::LOAD_LITERAL,*rit));
			CI.Add(new CompilerInstructions(Instruction::STORE,CI.GetNextOperandAddress()));
		}
		CI.Add(new CompilerInstructions(Instruction::LOAD_LITERAL,v->GetToken().GetString().length()));
		unsigned int add = CI.GetNextOperandAddress();
		v->SetAddress(add);
		CI.Add(new CompilerInstructions(Instruction::STORE,add));
	}
}

void CompileSML::ProcessLine(SymbolLine *l,bool *found,bool alreadyinST)
{
	// Line Number encountered in the program. Do compiler specific things to it.
	// If already in the symbol table then it will be a reference to a line number, e.g. goto 50

	if (!alreadyinST)
	{
		if (found)				// This item is a line number item
		{
			*found = false;
			l->SetAddress(CI.GetNextInstructionAddress());
			SymbolTable::Instance()->Add(l);
		}
		else					// This is referencing a line number item, so gnerate a BRANCH to it.
		{
			CI.Add(new CompilerInstructions(Instruction::BRANCH,0,(int) l->GetLineNo()));
		}
	}
	else
	{
		if (found)				// Weather this is a error or not depends upon the context so let caller sort it out.
			*found = true;
		CI.Add(new CompilerInstructions(Instruction::BRANCH,l->GetAddress()));
	}
}

void CompileSML::ProcessVariable(SymbolVariable *v)
{
	// New 'variable' encountered in the program. Do compiler specific things to it.

	SymbolTable::Instance()->Add(v);
	v->SetAddress(CI.GetNextOperandAddress());	// All variables have a memory location allocated to them.
}

}		// end namespace CompileSML
