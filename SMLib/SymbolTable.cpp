/*
	SymbolTable.cpp

	The Symbol table.

*/

#include "../StdAfx.h"
#include "SymbolTable.h"

namespace SML {

SymbolTable *ST=0;					// Only a single instance of this is required.

SymbolTable::SymbolTable(void)
{
}

SymbolTable::~SymbolTable()
{
	vector<Symbol*>::iterator i;
	for (i=st.begin();i<st.end();i++)
	{
		delete *i;
	}

	st.clear();

	delete ST;
}

SymbolTable* SymbolTable::Instance(void)
{
	if (!ST)
	{
		ST = new SymbolTable();
	}
	return ST;
}

void SymbolTable::SetHarness(Main* pmain)
{
	main = pmain;
}

Main* SymbolTable::GetHarness(void)
{
	return main;
}

void SymbolTable::Add(Symbol *i)
{
	st.push_back(i);
}

vector<Symbol*>::iterator SymbolTable::Find(Symbol *itm)
{
	vector<Symbol*>::iterator i;
	for (i=st.begin();i<st.end();i++)
	{
		if (**i == *itm)
			return i;
	}
	return i;

//	return find(st.begin(),st.end(),*itm);
}


const vector<Symbol*>& SymbolTable::GetSymbolTable(void) const
{
	return st;
}

ostream &operator<<(ostream & out, const SymbolTable & s)
{
	// Dump the Symbol table

	vector<Symbol*>::const_iterator i;

	for (i=s.GetSymbolTable().begin();i<s.GetSymbolTable().end();i++)
	//for (i=ST.GetSymbolTable().begin();i<ST.GetSymbolTable().end();i++)
	{
		(*i)->Print(out) << endl;
	}
	return out;
}

}		// end namespace SML
