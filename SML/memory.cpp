/*
	memory.cpp

	Class holding the memory of the SML machine

*/

#include "../StdAfx.h"
#include "../SMLib/common_sml_header.h"
#include "../SMLib/instructions.h"
#include "memory.h"

extern int NUM_INVALID;
extern int NUM_REAL;

extern unsigned int GetNum (const char * const,double &);

namespace SML {

memory::memory () :
dumpAmount(0)
{
	// The program must always end with an INVALID instruction to indicate its end,
	// And if it runs it with crash out neatly.

	data.clear();
	data.insert(data.begin(),Instruction::OPERATOR_SHIFT,0);
	data[0]= static_cast<int> (Instruction::INVALID*Instruction::OPERATOR_SHIFT);
}

memory::memory (istream & in) :
dumpAmount(0)
{
	// The program must always end with an INVALID instruction to indicate its end,
	// And if it runs it with crash out neatly.

	bool eos = false;
	int location = 0;
	stringstream m;

	data.clear();
	data.insert(data.begin(),Instruction::OPERATOR_SHIFT,0);

	do {

		string s;

		getline(in,s);

		if (!in.fail())
		{
			double d;
			unsigned int bw = GetNum(s.data(),d);

			if (bw & NUM_INVALID)
			{
				m << "Unrecognised number on program load..";
				throw runtime_error (m.str());
			}

			if (bw & NUM_REAL)
			{
				m << "Invalid real number encountered on program load.";
				throw runtime_error (m.str());
			}

			int word = static_cast<int>(d);

			if (word == EOS)
			{
				eos = true;
			}
			else
			{
				Instruction inst (word);

				if (*inst == Instruction::INVALID)
				{
					m << "INVALID INSTRUCTION " << word << " at location '" << location << "'";
					throw runtime_error (m.str());
				}

				data[location++] = word;
			}
		}
		else
		{
			m << "Stream input failour at location '" << location << "'";
			throw runtime_error (m.str());
		}

	} while (!eos);

	data[location]= static_cast<int> (Instruction::INVALID*Instruction::OPERATOR_SHIFT);
}

memory::memory (istream & in,ostream & out) :
dumpAmount(0)
{
	// Read program in from console, so chivvy the user on with comments.
	// The program must always end with an INVALID instruction to indicate its end,

	bool eos = false;
	int location = 0;

	data.clear();
	data.insert(data.begin(),Instruction::OPERATOR_SHIFT,0);

	out << "*** Welcome to Simpleton! ***" << endl;
	out << "*** Please enter your program one instruction ***" << endl;
	out << "*** (or data word) at a time. ***" << endl;
	out << "*** Finish input by typing in " << EOS << " ****" << endl << endl;

	do {

		out.fill ('0');
		out.width (2);

		out << location << " ? " << flush;

		string s;

		getline(in,s);

		if (!in.fail())
		{
			double d;
			unsigned int bw = GetNum(s.data(),d);

			if (bw & NUM_INVALID)
			{
				out << "Unrecognised number on program load." << endl;
				in.clear();
				in.ignore(INT_MAX,'\n');
			}
			else if (bw & NUM_REAL)
			{
				out << "Invalid real number encountered on program load." << endl;
				in.clear();
				in.ignore(INT_MAX,'\n');
			}
			else
			{
				int word = static_cast<int>(d);

				if (word == EOS)
				{
					eos = true;
				}
				else
				{
					Instruction inst (word);

					if (*inst == Instruction::INVALID)
					{
						out << "Invalid Instruction!" << endl;
					}
					else
					{
						data[location++] = word;
					}
				}
			}
		}
		else
		{
			out << "Input stream has encountered an error, retry." << endl;
			in.clear();
			in.ignore(INT_MAX,'\n');
		}

	} while (!eos);

	// Make sure we mark the end of program.

	data[location] = static_cast<int> (Instruction::INVALID*Instruction::OPERATOR_SHIFT);
}

memory::~memory ()
{
	data.empty();
}

double memory::operator[] (const Operand& idx) const
{
	// For l-value
	if ((*idx < 0) || (*idx >= data.size()))
	{
		stringstream m;
		m << "Array sub-script error " << *idx << " (max allowed=" << data.size() << ")";
		throw runtime_error (m.str());
	}
	return data.at(*idx);
}

void memory::SetMemory (const Operand& idx,const double v)
{
	if ((*idx < 0) || (*idx >= data.size()))
	{
		stringstream m;
		m << "Array sub-script error " << *idx << " (max allowed=" << data.size() << ")";
		throw runtime_error (m.str());
	}
	data.at(*idx) = v;
}

void memory::SetDumpAmount (const int amo)
{
	dumpAmount = amo;
}

const int memory::GetDumpAmount (void) const
{
	return dumpAmount;
}

ostream &operator<<(ostream &out, const memory &mem)
{
	// First dump the program.  Dump until we get the terminating INVALID instruction.

	int i,j;

	for (i = 0; Instruction(static_cast<int>(mem[i])) != Instruction::INVALID ; i++)
	{
		Instruction inst(static_cast<int>(mem[i]));
		Operand     op  (static_cast<int>(mem[i]));

		out << inst << "  " << op << endl;
	}

	// Now dump the memory but line it up on '8' address boundary.

	if (++i % 8)
	{
		j = (i/8)*8;

		out.width(3);
		out.fill('0');
		out << j << ' '; 		// Output the address

		for (j=0 ; j < (i%8) ; j++)
		{
			out.width(9);
			out.fill(' ');
			out << ' ';
		}
	}


	for (j = i; j < i+mem.GetDumpAmount() ; j++)
	{
		if ((j % 8) == 0)
		{
			out << endl;
			out.width(3);
			out.fill('0');
			out << hex << j << ' ';
		}

		out.width(8);
		out.fill('0');
		out << right << hex << (int) mem[j] << " ";
	}

	// Now dump the memory.

	return out;
}

}		// End namespace SML
