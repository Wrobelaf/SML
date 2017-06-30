/*
	CompilerInstructions.h

	Compiler generated instruction, That' opcode and operand. Also retain the address this instance is created at.

*/

#ifndef __COMPILERINSTRUCTIONS_
#define __COMPILERINSTRUCTIONS_

#include "../SMLib/instructions.h"
#include "../SMLib/operand.h"

namespace CompileSML {
using namespace SML;

class CompilerInstructions : public Instruction
{
public:

	CompilerInstructions(const int,const unsigned int, const int = 0);
	~CompilerInstructions();

	const int GetAddressToLine(void);
	void SetAddressToLine(const unsigned int);
	friend ostream &operator<<(ostream &, CompilerInstructions &);
	Operand& GetOperand(void);
	void debug(const bool);

private:

	Operand Op;
	const int fixup_to_line;
};

}			// End namespace CompileSML

#endif		// __COMPILERINSTRUCTIONS_

