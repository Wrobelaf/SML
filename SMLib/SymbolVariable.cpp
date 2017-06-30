/*
	SymbolVariable.cpp

	A SymbolVariable table entry.

*/

#include "../StdAfx.h"
#include "SymbolTable.h"
#include "SymbolVariable.h"

namespace SML {

SymbolVariable::SymbolVariable(const Token & pt) :
Symbol(pt),
assigned_to(Token(Token::INVALID))
{
}

SymbolVariable::~SymbolVariable()
{
}

bool SymbolVariable::operator==(const Symbol &itm) const
{
	bool res = false;
	const SymbolVariable* pv = dynamic_cast<const SymbolVariable*>(&itm);

	if (pv)
		res = (this->GetToken() == itm.GetToken());
	return res;
}

const string &SymbolVariable::GetString(void) const
{
	return this->GetToken().GetString();
}

ostream &SymbolVariable::Print(ostream &out) const
{
	Symbol::Print(out);
	out << assigned_to;
	return out;
}

SymbolVariable& SymbolVariable::Find(const Token &t,bool* found)
{
	SymbolVariable *v = new SymbolVariable(t);
	vector<Symbol*>::iterator vi = SymbolTable::Instance()->Find(v);

	if (vi == SymbolTable::Instance()->GetSymbolTable().end())			// If not already in symbol table
	{
		if (found)
			*found = false;
		SymbolTable::Instance()->GetHarness()->ProcessVariable(v);
		return *v;
	}
	if (found)
		*found = true;
	return static_cast<SymbolVariable&>(**vi);
}

const Token& SymbolVariable::GetAssignedTo(void) const
{
	return assigned_to;
}

void SymbolVariable::SetAssignedTo(const Token& t)
{
	assigned_to = t;
}

}		// end namespace SML
