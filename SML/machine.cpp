/*
    machine.cpp

    Class that runs the program in 'memory' against the 'machine state'

*/

#include "../StdAfx.h"
#include "machine.h"
#include "machine_state.h"
#include "../SMLib/instructions.h"
#include "../SMLib/operand.h"
#include <math.h>

extern int NUM_INVALID;
extern unsigned int GetNum (const char * const,double &);

namespace SML {

machine::machine (memory &pmem,machine_state &pstate) :
mem(pmem),
state(pstate)
{
}

machine::machine (memory *pmem,machine_state &pstate) :
mem(*pmem),
state(pstate)
{
}

machine::~machine ()
{
}

int machine::Push (const double itm)
{
    int add = state.GetFreeAdd();           // Will advance TOS
    mem.SetMemory (add,itm);
    return add;
}

double machine::Pop (void)
{
    int TOS = state.GetFreeAdd()+1;         // Will advance TOS
    double d = mem[TOS];
    state.SetFreeAdd (TOS+1);               // Remove itm from TOS
    return d;
}

void machine::Execute(ostream & out)
{
    // Execute 'word' on machine 'state' placing any results in 'mem'.
    // Note any input comes from the keyboard, but output can be routed to a file.
    // Return 'true' to continue processing, to halt return 'false'.

    bool halt = 0;
    stringstream m;

    do
    {
        Instruction inst(static_cast<int>(mem[*state]));
        Operand     op  (static_cast<int>(mem[*state]));

        switch (*inst)
        {
            case Instruction::NOOP:
            {
                state++;
                break;
            }
            case Instruction::HALT:
            {
                halt = true;
                break;
            }
            case Instruction::SET_HEAP_ADD:         // Setup instruction to indicate where free space starts.
            {
                state.SetFreeBase (*op);
                state++;
                break;
            }
            case Instruction::READ:
            {
                // cin >> memory[op].

                char itm[100];

                cout << endl << "(Number) ? ";
                cin.getline(itm,100);

                if (!cin.fail())
                {
                    double d;

                    if (GetNum(itm,d) & NUM_INVALID)
                    {
                        cerr << endl << "Unrecognised number, try again.";
                        cin.clear();
                        cin.ignore(INT_MAX,'\n');
                    }
                    else
                    {
                        mem.SetMemory (op,d);
                        state++;                            // next PC
                    }
                }
                else
                {
                    cerr << endl << "Input failed, try again.";
                    cin.clear();
                    cin.ignore(INT_MAX,'\n');
                    // Loop round on the same PC re-doing this LOAD function.
                }
                break;
            }
            case Instruction::READ_STRING:
            {
                // Rread string into free memory and assign the address to its length (its first
                // location) to 'op'
                // 1st address is length of string. Each char held in one memory location.

                vector<char> vc;
                bool eos = false;
                char c;

                cout << endl << "(String) ? ";

                do
                {
                    cin.get(c);

                    if (!cin.fail())
                    {
                        if (c == '\n')
                        {
                            vector<char>::reverse_iterator i=vc.rbegin();

                            while (i != vc.rend())
                            {
                                Push (*i);
                                i++;
                            }
                            mem.SetMemory (op,Push(vc.size()));         // First memory location is the length.
                            eos = true;
                            state++;        // next PC.
                        }
                        else
                        {
                            vc.push_back(c);
                        }
                    }
                    else
                    {
                        cerr << endl << "String input failed, try again.";
                        cin.clear();
                        cin.ignore(INT_MAX,'\n');
                        eos = true;
                        // Loop round on the same PC re-doing this LOAD function.
                    }
                } while (!eos);
                break;
            }
            case Instruction::WRITE:
            {
                // out << memory[op].

                out.width(15);
                out.precision(7);
                out << mem[op];
                state++;
                break;
            }
            case Instruction::WRITE_LITERAL:
            {
                // out << op. Last 7 bits of the operand. So newlines can be output

                out << (char) *op;
                state++;
                break;
            }
            case Instruction::WRITE_STRING:
            {
                // out << mem[mem[op]]  where the first memory location is the length of string.
                // The memory location in the instruciton is the address to the start of the string.

                int add = static_cast <int>(mem[op]);
                int len = (int) mem[add];

                while (len--)
                    out << (char) mem[++add];
                state++;
                break;
            }
            case Instruction::CMP_STRING:
            {
                // Compare the string addressed in the accumulator with the string addressed by the operand.
                // exit with accumulator = 1   acc > op
                //           accumulator = 0   acc = op
                //           accumulator = -1  acc < op

                int op_len = static_cast <int> (mem[op++]);
                int acc_len = static_cast <int> (mem[(int)state.GetAccumulator()]);
                int cmp_len = (acc_len < op_len) ? acc_len : op_len;
                int res = 0;

                for (int i = 1 ; i <= cmp_len ; i++)
                {
                    int o_op = static_cast <int> (mem[op++]);
                    int a_op = static_cast <int> (mem[(int)state.GetAccumulator()+i]);
                    if (a_op < o_op)
                    {
                        res = -1;
                        break;
                    }
                    if (a_op > o_op)
                    {
                        res = 1;
                        break;
                    }
                }
                // If identical at end of compare then compare the lengths.
                if (!res)
                {
                    if (acc_len < op_len)
                    {
                        res = -1;
                    } else if (acc_len > op_len)
                    {
                        res = 1;
                    }
                }
                state.SetAccumulator(res);
                state++;
                break;
            }
            case Instruction::INDIRECT_CMP_STRING:
            {
                // Compare the string addressed in the accumulator with the string addressed by the address in
                // location specified by the operand.
                // exit with accumulator = 1   acc > op
                //           accumulator = 0   acc = op
                //           accumulator = -1  acc < op

                int op_add = static_cast <int> (mem[op]);
                int op_len = static_cast <int> (mem[op_add++]);
                int acc_len = static_cast <int> (mem[(int)state.GetAccumulator()]);
                int cmp_len = (acc_len < op_len) ? acc_len : op_len;
                int res = 0;

                for (int i = 1 ; i <= cmp_len ; i++)
                {
                    int o_op = static_cast <int> (mem[op_add++]);
                    int a_op = static_cast <int> (mem[(int)state.GetAccumulator()+i]);
                    if (a_op < o_op)
                    {
                        res = -1;
                        break;
                    }
                    if (a_op > o_op)
                    {
                        res = 1;
                        break;
                    }
                }
                // If identical at end of compare then compare the lengths.
                if (!res)
                {
                    if (acc_len < op_len)
                    {
                        res = -1;
                    } else if (acc_len > op_len)
                    {
                        res = 1;
                    }
                }
                state.SetAccumulator(res);
                state++;
                break;
            }
            case Instruction::LOAD:
            {
                // accumulator=mem[op]

                state.SetAccumulator(mem[op]);
                state++;
                break;
            }
            case Instruction::STORE:
            {
                // mem[op]=accumulator

                mem.SetMemory (op,state.GetAccumulator());
                state++;
                break;
            }
            case Instruction::LOAD_LITERAL:
            {
                // accumulator=OP, here OP is a signed value.

                state.SetAccumulator(op.GetSignedValue());
                state++;
                break;
            }
            case Instruction::ADD:
            {
                // accumulator+=mem[op]

                state.SetAccumulator(state.GetAccumulator()+mem[op]);
                state++;
                break;
            }
            case Instruction::ADD_LITERAL:
            {
                // accumulator+=op   where op is a signed value

                state.SetAccumulator(state.GetAccumulator()+op.GetSignedValue());
                state++;
                break;
            }
            case Instruction::SUBTRACT:
            {
                // accumulator+=mem[op]

                state.SetAccumulator(state.GetAccumulator()-mem[op]);
                state++;
                break;
            }
            case Instruction::DIVIDE:
            {
                // accumulator+=mem[op]

                if (mem[op] == 0)
                {
                    m << "Zero divide at address" << *state << endl;
                    throw runtime_error (m.str());
                }
                state.SetAccumulator(state.GetAccumulator()/mem[op]);
                state++;
                break;
            }
            case Instruction::MULTIPLY:
            {
                // accumulator*=mem[op]

                state.SetAccumulator(state.GetAccumulator()*mem[op]);
                state++;
                break;
            }
            case Instruction::MULTIPLY_LITERAL:
            {
                // accumulator*=op   op is a signed value

                state.SetAccumulator(state.GetAccumulator()*op.GetSignedValue());
                state++;
                break;
            }
            case Instruction::MODULUS:
            {
                // accumulator*=mem[op]

                if (mem[op] == 0)
                {
                    m << "Zero modulus at address" << *state << endl;
                    throw runtime_error (m.str());
                }
                state.SetAccumulator(((long)state.GetAccumulator())%(long)mem[op]);
                state++;
                break;
            }
            case Instruction::POWER:
            {
                // accumulator=accumulator**mem[op]

                state.SetAccumulator(pow(state.GetAccumulator(),mem[op]));
                state++;
                break;
            }
            case Instruction::SHIFT:
            {
                // accumulator=accumulator * 10^operand   (where 'operand' is a signed value)
                // e.g. if Acc = 17 then shift -1 results in 1.7
                // Note this is shift literal, not yet implemented shift by memory location.

                state.SetAccumulator(state.GetAccumulator()*pow(10,op.GetSignedValue()));
                state++;
                break;
            }
            case Instruction::BRANCH:
            {
                state = *op;
                break;
            }
            case Instruction::BRANCH_NEG:
            {
                if (state < 0)
                {
                    state = *op;
                }
                else
                {
                    state++;
                }
                break;
            }
            case Instruction::BRANCH_ZERO:
            {
                if (state == 0)
                {
                    state = *op;
                }
                else
                {
                    state++;
                }
                break;
            }
            case Instruction::SML_DEBUG:
            {
                state.SetDebugState((*op) ? true : false);      // 01 switch on, 00 switch off.
                mem.SetDumpAmount(*op);
                state++;
                break;
            }
            default:;
            case Instruction::INVALID:
            {
                stringstream m;
                m << "Invalid instruction encountered at runtime.";
                throw runtime_error (m.str());
            }
        }

        if (state.GetDebugState())
        {
            out << state << endl;
            out << mem << endl;
        }
    }
    while (!halt);
}

}		// End of namespace SML
