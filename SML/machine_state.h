/*
	machine_state.h

	Class that maintains the 'state' of the SML computer.

*/

#ifndef __MACHINE_STATE_
#define __MACHINE_STATE_

#include "../StdAfx.h"

namespace SML {

class machine_state
{
public:

	machine_state ();
	~machine_state ();
	int operator*(void) const;

	const int operator=(int);								// Copy just PC
	const machine_state &operator=(const machine_state &);	// standard copy constructor.

	const bool operator==(const int);
	const bool operator==(const double);
	const bool operator<(const int);
	const bool operator<(const double);
	machine_state & operator++();							// pre increment
	machine_state operator++(int);							// post increment

	double  GetAccumulator(void) const;
	void SetAccumulator(const double);

	bool GetDebugState(void) const;
	void SetDebugState(bool);

	const int GetFreeAdd (void);
	void SetFreeAdd (const int);
	void SetFreeBase(const int);
	friend ostream& operator<<(ostream &, const machine_state &);

private:

	double accumulator;
	int pc;
	int free_base_add;
	int next_free_idx;
	bool debug;
};

}			// End namespace SML

#endif		// __MACHINE_STATE_

