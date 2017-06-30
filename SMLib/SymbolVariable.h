/*
	SymbolVariable.h

	A entry in the Symbol table of type 'variable'.

*/

#ifndef __SymbolVariable_
#define __SymbolVariable_

#include "Symbol.h"

namespace SML {

class SymbolVariable : public Symbol
{
public:

	SymbolVariable(const Token&);
	~SymbolVariable();

	bool operator==(const Symbol&) const;
	const string &GetString(void) const;
	static SymbolVariable& Find(const Token &,bool* = 0);
	ostream &Print(ostream &out) const;
	const Token& GetAssignedTo(void) const;
	void SetAssignedTo(const Token&);

private:

	Token	assigned_to;			// What the variable is assigned to, will be one of INVALID, NUMBER or STRING.
};

}			// Namespace SML

#endif		// __SymbolVariable_
