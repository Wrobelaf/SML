/*
	SymbolConstant.cpp

	A SymbolConstant table entry.

*/

#include "../StdAfx.h"
#include "SymbolTable.h"
#include "SymbolConstant.h"

extern int normalise (double,unsigned int &,bool &,bool &);

namespace SML {


SymbolConstant::SymbolConstant(const Token &pt) :
Symbol(pt)
{
}

SymbolConstant::~SymbolConstant()
{
}

bool SymbolConstant::operator==(const Symbol& itm) const
{
	bool res = false;
	const SymbolConstant* pc = dynamic_cast<const SymbolConstant*>(&itm);

	if (pc)
		res = (this->GetToken() == itm.GetToken());

	return res;

}

SymbolConstant& SymbolConstant::Find(const Token &t,bool* found)
{
	// Find Constant in symbol table, If not found then generate code to create a constant in memory.
	// All constants have a memory location(s) allocated to them.

	SymbolConstant *v = new SymbolConstant(t);
	vector<Symbol*>::iterator vi = SymbolTable::Instance()->Find(v);

	if (vi == SymbolTable::Instance()->GetSymbolTable().end())			// If not already in symbol table
	{
		if (found)
			*found = false;
		SymbolTable::Instance()->GetHarness()->ProcessConstant(v);
		return *v;
	}
	if (found)
		*found = true;
	return static_cast<SymbolConstant&>(**vi);
}

}		// Namespace SML
