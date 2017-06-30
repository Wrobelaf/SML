/*
	CompilerInstructionTable.cpp

	The Symbol table.

*/

#include "../StdAfx.h"
#include "CompilerInstructionTable.h"

namespace CompileSML {
using namespace SML;

CompilerInstructionTable CI;					// Only a single instance of this is required.

CompilerInstructionTable::CompilerInstructionTable(void) :
operand_address(Instruction::OPERATOR_SHIFT),
dbug(false)
{
}

CompilerInstructionTable::~CompilerInstructionTable()
{
	vector<CompilerInstructions*>::iterator i;
	for (i=ci.begin();i<ci.end();i++)
	{
		delete *i;
	}

	ci.clear();
}

vector<CompilerInstructions*>::reference CompilerInstructionTable::Add(CompilerInstructions *i)
{
	ci.push_back(i);
	return ci.back();						// return a reference to the item just added.
}

const vector<CompilerInstructions*>& CompilerInstructionTable::GetCompilerInstructionTable(void) const
{
	return ci;
}

const unsigned int CompilerInstructionTable::GetNextOperandAddress(void)
{
	// Start from other end from program when allocating addresses.
	return --operand_address;
}

const unsigned int CompilerInstructionTable::GetNextInstructionAddress(void)
{
	// Grow from the bottom of address space toward the top.
	return ci.size();
}

ostream &operator<<(ostream & out, const CompilerInstructionTable & s)
{
	// Dump the instructions generated so far.

	vector<CompilerInstructions*>::const_iterator i;

	for (i=s.GetCompilerInstructionTable().begin();i<s.GetCompilerInstructionTable().end();i++)
	{
		(*i)->debug(CI.debug());
		if (CI.debug())
		{
			out.width(3);
			out.fill('0');
			out << i-s.GetCompilerInstructionTable().begin() << "  ";
		}
		out << **i << endl;
	}
	return out;
}

bool CompilerInstructionTable::debug(void) const
{
	return dbug;
}

bool CompilerInstructionTable::debug(const bool pb)
{
	bool old = dbug;
	dbug = pb;
	return old;
}
}		// End namespace CompileSML
