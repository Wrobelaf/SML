/*
	operand.cpp

	Implementation of Class holding the operand part of SML instruction words.

*/

#include "../StdAfx.h"
#include "instructions.h"
#include "operand.h"
#include <math.h>


extern unsigned int nines_complement (unsigned int);
extern int ndigits (unsigned int);

namespace SML {

Operand::Operand(unsigned int pop) :
op(pop % Instruction::OPERATOR_SHIFT),
dbug(false)
{
}

Operand::~Operand()
{
}

unsigned int Operand::operator*(void) const
{
	return op;
}

Operand &Operand::operator++()
{
	op++;
	return *this;
}

Operand Operand::operator++(int)
{
	Operand temp = *this;
	op++;
	return temp;
}

int Operand::GetSignedValue(void) const
{
	// Convert to a signed operand, 9's complement

	int signedop = op;
	if ((signedop / (Instruction::OPERATOR_SHIFT/10)) == 9)
	{
		signedop = nines_complement (signedop);
		signedop = -signedop;
	}
	return signedop;
}

void Operand::MakeAsSignedValue(void)
{
	// Convert operand to a signed value

	op = nines_complement (op);

	// Pad the number out to the width of the operand with 9's.

	int ndig = ndigits(op);
	for (int i = ndig ; i < ndigits(Instruction::OPERATOR_SHIFT-1) ; i++)
	{
		op += (int)(9.0*pow(10,i));
	}
}

void Operand::SetOperand(const unsigned int new_op)
{
	op = new_op;
}

ostream &operator<<(ostream &out, const Operand &ms)
{
	out.width(ndigits(Instruction::OPERATOR_SHIFT-1));
	out.fill('0');
	out << right << *ms;
	return out;
}

bool Operand::debug(void) const
{
	return dbug;
}

bool Operand::debug(const bool pb)
{
	bool old = dbug;
	dbug = pb;
	return old;
}

}			// End namespace SML
