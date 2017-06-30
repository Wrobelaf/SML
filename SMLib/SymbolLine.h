/*
	SymbolLine.h

	A line entry in the Symbol table.

*/

#ifndef __SymbolLine_
#define __SymbolLine_

#include "Symbol.h"

namespace SML {

class SymbolLine : public Symbol
{
public:

	SymbolLine(const Token &);
	~SymbolLine();

	bool operator==(const Symbol&) const;
	static SymbolLine& Find(const Token&,bool* = 0);
	const unsigned int GetLineNo(void) const;


};

}			// Namespace SML

#endif		// __SymbolLine_
