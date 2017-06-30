/*
	Main.h

	Class holding the Main of the SML machine

*/

#ifndef __Main_
#define __Main_

#include "../StdAfx.h"
#include "SymbolConstant.h"
#include "SymbolLine.h"
#include "SymbolTable.h"
#include "SymbolVariable.h"
#include "Token.h"

namespace SML {

class Main
{
public:

	Main (int,char **);
	virtual ~Main ();

	int Run(void);
	virtual void SetOptions (bool)=0;
	virtual void Print (ostream &)=0;
	virtual int  Pass1 (vector<Token>&,string&)=0;
	virtual int  Pass2 (ostream &)=0;
	virtual void ProcessConstant(SymbolConstant *)=0;
	virtual void ProcessLine(SymbolLine *,bool *,bool)=0;
	virtual void ProcessVariable(SymbolVariable *)=0;
	const vector<Token>& GetTokens(void) const;
	friend ostream &operator<<(ostream &, Main &);

protected:


private:

	void Emit (Token*&,string&,int&,double&,int&,unsigned int&);
	int Run (ostream &,istream &);
	int Run (ostream &,bool);
	bool Tokenise(int &,string &);

	int argc;
	char ** argv;

	vector<Token> tokens;

};

}			// End namespace SML

#endif		// __Main_

