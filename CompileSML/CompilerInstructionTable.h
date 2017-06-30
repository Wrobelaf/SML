/*
	CompilerInstructionTable.h

	The compiled instrucitons.

*/

#ifndef __CompilerInstructionTable_
#define __CompilerInstructionTable_

#include "CompilerInstructions.h"

namespace CompileSML {
using namespace SML;

class CompilerInstructionTable
{
public:

	CompilerInstructionTable(void);
	~CompilerInstructionTable();

	vector<CompilerInstructions*>::reference Add(CompilerInstructions *);
	const vector<CompilerInstructions*>& GetCompilerInstructionTable(void) const;
	const unsigned int GetNextOperandAddress(void);
	const unsigned int GetNextInstructionAddress(void);
	friend ostream &operator<<(ostream &, const CompilerInstructionTable &);
	bool debug(void) const;
	bool debug(const bool);

private:

	vector<CompilerInstructions*> ci;

	int operand_address;

	bool dbug;
};

}			// End namespace CompileSML

#endif		// __CompilerInstructionTable_
