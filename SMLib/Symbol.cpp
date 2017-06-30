/*
	Symbol.cpp

	A Symbol table entry.

*/

#include "../StdAfx.h"
#include "Symbol.h"

namespace SML {

Symbol::Symbol(const Token &pt) :
t(pt),
address(0)
{
}

Symbol::~Symbol()
{
}

const Token &Symbol::GetToken(void) const
{
	return t;
}

const int Symbol::GetAddress(void)
{
	return address;
}

void Symbol::SetAddress(const int a)
{
	address = a;
}

ostream &Symbol::Print(ostream &out) const
{
	out << this->GetToken();
	return out;
}

}			// namespace SML
