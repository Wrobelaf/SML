/*
	machine.h

	Class that runs the program in 'memory' against the 'machine state'
*/

#ifndef __MACHINE_
#define __MACHINE_

#include "memory.h"
#include "machine_state.h"

namespace SML {

class machine
{
public:

	machine (memory &,machine_state &);
	machine (memory *,machine_state &);
	void Execute (ostream &);
	~machine ();

private:

	int Push(const double);
	double Pop (void);
	memory & mem;
	machine_state & state;
};

}			// End namespace SML

#endif		// __MACHINE_

