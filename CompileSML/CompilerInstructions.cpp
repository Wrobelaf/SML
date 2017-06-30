/*
	CompilerInstructions.cpp

	Compiler generated instruction, part of which is the assember code that's why it is derived from
	the SML machine instruction class.

*/

#include "../StdAfx.h"
#include "CompilerInstructions.h"

namespace CompileSML {
using namespace SML;

CompilerInstructions::CompilerInstructions(const int pi,const unsigned int o,const int pfix)  :
Instruction(pi*Instruction::OPERATOR_SHIFT),
Op(o),
fixup_to_line(pfix)
{
}

CompilerInstructions::~CompilerInstructions()
{
}

ostream &operator<<(ostream & out, CompilerInstructions & s)
{
	// Dump the instruction

	out << static_cast<Instruction>(s);
	out << s.GetOperand();
	return out;
}

const int CompilerInstructions::GetAddressToLine(void)
{
	return fixup_to_line;
}

void CompilerInstructions::SetAddressToLine(const unsigned int pfix)
{
	Op.SetOperand (pfix);
}

Operand& CompilerInstructions::GetOperand(void)
{
	return Op;
}

void CompilerInstructions::debug(const bool pb)
{
	Instruction::debug(pb);
	Op.debug(pb);
}
}			// End namespace CompileSML
