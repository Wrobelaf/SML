/*
	Symbol.h

	A entry in the symbol table.

*/

#ifndef __Symbol_
#define __Symbol_

#include "../SMLib/Token.h"

namespace SML {

class Symbol
{
public:

	virtual ~Symbol();
	virtual bool operator==(const Symbol&) const=0;
	const int GetAddress(void);
	void SetAddress(const int);
	const Token &GetToken(void) const;
	virtual ostream &Print(ostream &out) const;

protected:

	Symbol(const Token &);

private:

	int   address;			// Machine code address of this symbol item.
	const Token t;			// What this item actually is.

};

}			// Namespace SML

#endif		// __Symbol_
