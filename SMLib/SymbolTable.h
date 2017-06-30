/*
	SymbolTable.h

	The Symbol table.

*/

#ifndef __SymbolTable_
#define __SymbolTable_

#include "Main.h"
#include "Symbol.h"

namespace SML {

class Main;

class SymbolTable
{
public:

	SymbolTable(void);
	~SymbolTable();

	void Add(Symbol *);
	vector<Symbol*>::iterator Find(Symbol *);
	const vector<Symbol*>& GetSymbolTable(void) const;
	static SymbolTable* Instance(void);
	void SetHarness(Main *);
	Main* GetHarness(void);
	friend ostream &operator<<(ostream &, const SymbolTable &);

private:

	Main* main;
	vector<Symbol*> st;

};

}			// end namespace SML

#endif		// __SymbolTable_
