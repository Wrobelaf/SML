/*
	SymbolLine.cpp

	A SymbolLine table entry.

*/

#include "../StdAfx.h"
#include "SymbolLine.h"
#include "SymbolTable.h"

namespace SML {

SymbolLine::SymbolLine(const Token &pt) :
Symbol(pt)
{
}

SymbolLine::~SymbolLine()
{
}

bool SymbolLine::operator==(const Symbol &itm) const
{
	bool res = false;
	const SymbolLine* pl = dynamic_cast<const SymbolLine*>(&itm);

	if (pl)
	   res = (this->GetToken().GetNumber() == itm.GetToken().GetNumber());
	return res;
}

const unsigned int SymbolLine::GetLineNo(void) const
{
	return static_cast<unsigned int> (this->GetToken().GetNumber());
}

SymbolLine& SymbolLine::Find(const Token& t,bool* found)
{
	SymbolLine *l = new SymbolLine(t);
	vector<Symbol*>::iterator li = SymbolTable::Instance()->Find(l);

	if (li == SymbolTable::Instance()->GetSymbolTable().end())			// If target line no. not in the symbol table
	{
		SymbolTable::Instance()->GetHarness()->ProcessLine(l,found,false);
		return *l;
	}
	SymbolTable::Instance()->GetHarness()->ProcessLine(static_cast<SymbolLine*>(*li),found,true);
	return static_cast<SymbolLine&>(**li);
}

}		// End namespace SML
