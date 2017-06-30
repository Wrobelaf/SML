/*
    instructions.cpp

    Class holding a mapping of number to instruction for the SML machine

*/

#include "../StdAfx.h"
#include "instructions.h"

namespace SML {

const int Instruction::OPERATOR_SHIFT = 1000;
const int Instruction::MAX_POSITIVE_NUM = OPERATOR_SHIFT-(OPERATOR_SHIFT/10)-1;
const int Instruction::MAX_NEGATIVE_NUM = -((OPERATOR_SHIFT/10)-1);

const Instruction::v_inst Instruction::valid_inst[] = { {READ,         "READ"},
                                                        {WRITE,        "WRITE"},
                                                        {WRITE_LITERAL,"WRITE_LITERAL"},
                                                        {READ_STRING,  "READ_STRING"},
                                                        {WRITE_STRING, "WRITE_STRING"},
                                                        {LOAD,         "LOAD"},
                                                        {STORE,        "STORE"},
                                                        {LOAD_LITERAL, "LOAD_LITERAL"},
                                                        {ADD,          "ADD"},
                                                        {SUBTRACT,     "SUBTRACT"},
                                                        {DIVIDE,       "DIVIDE"},
                                                        {MULTIPLY,     "MULTIPLY"},
                                                        {MODULUS,      "MODULUS"},
                                                        {POWER,        "POWER"},
                                                        {SHIFT,        "SHIFT"},
                                                        {ADD_LITERAL,  "ADD_LITERAL"},
                                                        {MULTIPLY_LITERAL,     "MULT_LITERAL"},
                                                        {BRANCH,       "BRANCH"},
                                                        {BRANCH_NEG,   "BRANCH_NEG"},
                                                        {BRANCH_ZERO,  "BRANCH_ZERO"},
                                                        {HALT,         "HALT"},
                                                        {CMP_STRING,   "CMP_STRING"},
                                                        {INDIRECT_CMP_STRING,   "ICMP_STRING"},
                                                        {NOOP,         "NOOP"},
                                                        {SML_DEBUG,    "SML_DEBUG"},
                                                        {SET_HEAP_ADD, "SET_HEAP_ADD"},
                                                        {INVALID,      "INVALID"} };

const int Instruction::NUM_INSTRUCTIONS = sizeof(Instruction::valid_inst)/sizeof(Instruction::valid_inst[0]);


Instruction::Instruction(int pi) :
xx(0),
dbug(false)
{
    instructions i = static_cast<instructions>(pi/OPERATOR_SHIFT);

    for (int j = 0 ; j < NUM_INSTRUCTIONS ; j++)
    {
        if (valid_inst[j].inst == i)
        {
            xx = &valid_inst[j];
            break;
        }
    }
    if (!xx)
        xx = &valid_inst[NUM_INSTRUCTIONS-1];           // Address the INVALID instruction
}

Instruction::~Instruction()
{
}

bool Instruction::operator==(const Instruction &rhs) const
{
    return (xx->id == rhs.xx->id);
}

bool Instruction::operator!=(const Instruction &rhs) const
{
    return (xx->id != rhs.xx->id);
}

Instruction::instructions Instruction::operator*(void) const
{
    return xx->inst;
}

const char * const Instruction::GetId(void) const
{
    return xx->id;
}

ostream &operator<<(ostream &out, const Instruction &ms)
{
    if (ms.debug())
    {
        out.width(14);
        out.fill(' ');
        out << left << ms.GetId();
    }
    else
    {
        out.width(2);
        out.fill('0');
        out << static_cast<int>(*ms);
    }
    return out;
}

bool Instruction::debug(void) const
{
    return dbug;
}

bool Instruction::debug(const bool pb)
{
    bool old = dbug;
    dbug = pb;
    return old;
}

}			// End namespace SML
