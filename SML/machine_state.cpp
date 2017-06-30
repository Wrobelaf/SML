/*
	machine.h

	Class that runs the program in 'memory' against the 'machine state'

*/

#include <math.h>
#include "machine_state.h"
#include "../SMLib/instructions.h"
#include "../SMLib/common_sml_header.h"

namespace SML {

machine_state::machine_state () :
accumulator(0),
pc(0),
debug(false),
free_base_add(Instruction::OPERATOR_SHIFT),
next_free_idx(0)
{
}

machine_state::~machine_state ()
{
}

int machine_state::operator*(void) const
{
	// De-referencing gets the pc state.
	return pc;
}

const int machine_state::operator=(int rv)
{
	// Assignment operator just copies over the PC not the accumulator. Dodgy?
	pc = rv;
	return pc;
}

const machine_state &machine_state::operator=(const machine_state &rv)
{
	// Standard copy constructor
	if (&rv != this)
	{
		accumulator = rv.accumulator;
		pc = rv.pc;
		debug = rv.debug;
	}
	return *this;
}

double machine_state::GetAccumulator(void) const
{
	return accumulator;
}

void machine_state::SetAccumulator(const double lv)
{
	accumulator = lv;
}

bool machine_state::GetDebugState(void) const
{
	return debug;
}

void machine_state::SetDebugState(bool b)
{
	debug = b;
}

const bool machine_state::operator==(const int r)
{
	return (fabs(this->accumulator-(double)r) < (double)(1.0/ROUND_TO));
}

const bool machine_state::operator==(const double r)
{
	return (fabs(this->accumulator-r) < (double)(1.0/ROUND_TO));
}

const bool machine_state::operator<(const int r)
{
	return (this->accumulator<(double)r);
}

const bool machine_state::operator<(const double r)
{
	return (this->accumulator<r);
}

machine_state & machine_state::operator++()							// pre increment
{
	++pc;
	return *this;
}

machine_state machine_state::operator++(int)						// post increment
{
	machine_state temp = *this;
	pc++;
	return temp;
}

const int machine_state::GetFreeAdd(void)
{
	return (--next_free_idx)+free_base_add;
}

void machine_state::SetFreeAdd (const int add)
{
	next_free_idx = add;
}

void machine_state::SetFreeBase(const int add)
{
	free_base_add = add;
	SetFreeAdd (0);
}

ostream& operator<<(ostream &out, const machine_state &ms)
{
	out << "Accumulator= " << ms.GetAccumulator() << ", PC = " << *ms;
	return out;
}

}		// End SML namespace

