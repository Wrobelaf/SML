// InterpretSML.cpp : Defines the entry point for the console application.
//
#include "../StdAfx.h"
#include "../SMLib/RecoverableError.h"
#include "InterpretSML.h"

extern int   					NUM_INVALID;
extern int   					NUM_DEC;
extern int   					NUM_HEX;
extern int   					NUM_REAL;

int main(int argc, char**argv)
{
	//string s = "10100\n10101\n20100\n31101\n41008\n11100\n12010\n43000\n11101\n12010\n43000\n-99999\n";
	//istringstream ss(s);
	//mem = new memory (ss,cout);
	//string s = "( 5 + 4 ) * 2";
	//string s = "3 * ( 5 + 4 )";
	//string s = " ( 32.4 * ( 5 + 4 ) / ( ( 8 - 2 )  + ( 2 ^ 1.7 ) ) )";     // answer = 31.5277
	//string s = "3 + 5 * 4";
	//string s = "3 + 5 * ( 4 - 8 ) / 2";
	//string s = "3 * 2 ^ 4";
	//string s = "2 ^ 0.5 * 3";
	//istringstream ss(s);
	//InterpretEval  inf (ss);
	InterpretSML::InterpretSML itm (argc,argv);
	SML::SymbolTable::Instance()->SetHarness(&itm);
	return itm.Run ();
}

namespace InterpretSML {
using namespace SML;

InterpretSML::InterpretSML (int pargc,char **pargv) :
Main(pargc,pargv)
{
}

InterpretSML::~InterpretSML ()
{
}

void InterpretSML::SetOptions(bool pdebug)
{
}

int InterpretSML::Pass1 (vector<Token>& line,string &s)
{
	// e.g. line is:
	//  10  LET a = 12.7   'line' holds each of these text fragments as a seperate 'Token' in its array.
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
	case Token::EXIT:

		ret = 1;													// Just Finish compilation
		break;
	}
	return ret;
}

int InterpretSML::Pass2 (ostream &out)
{
	// Read program in from console, so chivvy the user on with comments.
	// 'out' is for the output of the compiled program, not for communication with user.

	return 0;
}

void InterpretSML::Print (ostream & out)
{
}

void InterpretSML::ProcessConstant(SymbolConstant *c)
{
}

void InterpretSML::ProcessLine(SymbolLine *l,bool *found,bool IsalreadyInST)
{
}

void InterpretSML::ProcessVariable(SymbolVariable *v)
{
}

}		// end namespace InterpretSML
