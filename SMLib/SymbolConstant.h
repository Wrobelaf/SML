/*
	SymbolConstant.h

	A 'constant' entry in the Symbol table.

*/

#ifndef __SymbolConstant_
#define __SymbolConstant_

#include "Symbol.h"

namespace SML {

class SymbolConstant : public Symbol
{
public:

	SymbolConstant(const Token&);
	~SymbolConstant();

	bool operator==(const Symbol&) const;
	static SymbolConstant& Find(const Token&,bool* = 0);

private:

};

}			// End namespace SML
#endif		// __SymbolConstant_
